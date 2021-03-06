/*
 * Copyright 2008, 2009, 2010 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <freewpc.h>
#include <simulation.h>

#define COIL_MAX 5

struct sim_coil_state
{
	unsigned int on;
	unsigned int pos;
	unsigned int scheduled;
	unsigned int devno;
	unsigned int kick_disabled;
	unsigned int tick;
} coil_states[64];


unsigned int coil_pulse_count;


/**
 * Simulate a device kicking a pinball.
 */
void sim_coil_kick (struct sim_coil_state *c)
{
	extern device_properties_t device_properties_table[];
	unsigned int solno = c - coil_states;
	int devno;

	simlog (SLC_DEBUG, "Kick solenoid %d", solno);

	/* If it's the outhole kicker, simulate the ball being
	moved off the outhole into the trough */
#if defined(MACHINE_OUTHOLE_SWITCH) && defined(DEVNO_TROUGH)
	if (solno == SOL_OUTHOLE &&
		linux_switch_poll_logical (MACHINE_OUTHOLE_SWITCH))
	{
		/* Simulate kicking the ball off the outhole into the trough */
		simlog (SLC_DEBUG, "Outhole kick");
		sim_switch_toggle (MACHINE_OUTHOLE_SWITCH);
		sim_switch_toggle (device_properties_table[DEVNO_TROUGH].sw[0]);
	}
#endif

	/* See if it's attached to a device.  Then find the first
	switch that is active, and deactivate it, simulating the
	removal of one ball from the device.  (This does not map
	to reality, where lots of switch closures would occur, but
	it does produce the intended effect.) */
	for (devno = 0; devno < NUM_DEVICES; devno++)
	{
		const device_properties_t *props = &device_properties_table[devno];
		if (props->sol == solno)
		{
			int n;
			for (n = 0; n < props->sw_count; n++)
			{
				if (linux_switch_poll_logical (props->sw[n]))
				{
					simlog (SLC_DEBUG, "Device %d release", devno);
					sim_switch_toggle (props->sw[n]);

					/* Where does the ball go from here?
					Normally device kickout leads to unknown areas of
					the playfield.
					The shooter switch could be handled though. */
#if defined(DEVNO_TROUGH) && defined(MACHINE_SHOOTER_SWITCH)
					if (devno == DEVNO_TROUGH)
					{
						sim_switch_toggle (MACHINE_SHOOTER_SWITCH);
					}
#endif
					break;
				}
			}

			/* If no balls are in the device, then nothing happens. */
			break;
		}
	}
}


/**
 * Update the state machine for a coil.
 *
 * The complexity here is that software can pulse the solenoid rapidly
 * between on and off; the goal is to detect when a single 'kick' operation
 * has occurred.  That requires ignoring brief off-periods and only
 * considering that it is been on a majority of the time in the recent past.
 *
 * Also, once a kick has occurred, it is not considered to kick again until
 * it has returned to the resting position (in reality, to allow another ball
 * to enter the kicking area).
 *
 * This function uses periodic callbacks to keep the solenoid state updated
 * even when no writes are emitted from the CPU; the power driver board
 * latches state.
 */
void sim_coil_update (struct sim_coil_state *c)
{
	if (c->on && c->pos < COIL_MAX)
	{
		c->pos++;
		if (c->pos == COIL_MAX && !c->kick_disabled)
		{
			sim_coil_kick (c);
			c->kick_disabled++;
		}
	}
	else if (!c->on && c->pos > 0)
	{
		c->pos--;
		if (c->pos == 0)
			c->kick_disabled = 0;
	}

	if (c->pos != 0)
		sim_time_register (8, FALSE, (time_handler_t)sim_coil_update, c);
	else
	{
		c->scheduled = 0;
		coil_pulse_count--;
	}

	c->tick++;
}


/**
 * Called when the CPU board writes to a solenoid signal.
 * coil identifies the signal; on is nonzero for active voltage.
 */
void sim_coil_change (unsigned int coil, unsigned int on)
{
	struct sim_coil_state *c = coil_states + coil;

	on = !!on;
	if (c->on != on)
	{
		c->on = on;
		if (!c->scheduled)
		{
			sim_time_register (8, FALSE, (time_handler_t)sim_coil_update, c);
			c->scheduled = 1;
			coil_pulse_count++;
		}
	}
}

void sim_coil_init (void)
{
	memset (coil_states, 0, sizeof (coil_states));
	coil_pulse_count = 0;
}
