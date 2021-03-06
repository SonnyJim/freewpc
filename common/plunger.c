/*
 * Copyright 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

/* Include autoplunger logic only when all of these machine
 * characteristics are known */
#if defined(MACHINE_LAUNCH_SWITCH) && defined(MACHINE_LAUNCH_SOLENOID) && defined(MACHINE_SHOOTER_SWITCH)
#define INCLUDE_AUTOPLUNGER
#endif

/**
 * \file
 * \brief Common plunger routines.
 */

#ifdef INCLUDE_AUTOPLUNGER
void timed_plunger_monitor (void)
{
	task_sleep_sec (7);
	VOIDCALL (plunger_sw_launch_button);
	task_exit ();
}
#endif


void shooter_clear_monitor (void)
{
	task_add_duration (TASK_DURATION_INF);
	task_sleep_sec (4); /* this could be machine-specific */
	global_flag_off (GLOBAL_FLAG_BALL_AT_PLUNGER);
	task_exit ();
}


void shooter_update (void)
{
#ifdef MACHINE_SHOOTER_SWITCH
	/* A ball seen on the shooter switch means it is definitely there.
	 * If not seen, it might be out of the plunger lane, or it may just be
	 * "on its way up".  During that interval, still consider the ball
	 * at the plunger, so delay clearing the flag.
	 */
	if (switch_poll_logical (MACHINE_SHOOTER_SWITCH))
	{
		task_kill_gid (GID_SHOOTER_CLEAR);
		global_flag_on (GLOBAL_FLAG_BALL_AT_PLUNGER);
	}
	else
	{
		task_create_gid1 (GID_SHOOTER_CLEAR, shooter_clear_monitor);
	}
#endif
}


CALLSET_ENTRY (plunger, amode_start)
{
	shooter_update ();
}

CALLSET_ENTRY (plunger, valid_playfield)
{
#ifdef INCLUDE_AUTOPLUNGER
	task_kill_gid (GID_TIMED_PLUNGER_MONITOR);
#endif
	task_kill_gid (GID_SHOOTER_CLEAR);
	global_flag_off (GLOBAL_FLAG_BALL_AT_PLUNGER);

}


CALLSET_ENTRY (plunger, sw_shooter)
{
	/* TODO - none of this logic works if the shooter switch is broken.
	 * Need to invoke this on a timer after any trough kick.
	 */
#ifdef INCLUDE_AUTOPLUNGER
	if (valid_playfield
		&& !tournament_mode_enabled
		&& !global_flag_test (GLOBAL_FLAG_COIN_DOOR_OPENED))
	{
		/* Autolaunch balls right away during a game if they land
		in the autoplunger lane. */
		/* TODO - after locking a ball, adding a new ball to the
		plunger while valid_playfield is TRUE: this will launch
		prematurely.  We need a permanent, system-defined global
		flag that says whether a trough serve should be autoplunged
		or not.  TZ is already doing this privately... */
		VOIDCALL (plunger_sw_launch_button);
	}
	else if (config_timed_plunger == ON)
	{
		/* If timed plunger is enabled, then start a timer
		to autoplunge the ball regardless of button press */
		task_create_gid1 (GID_TIMED_PLUNGER_MONITOR, timed_plunger_monitor);
	}
#endif
	shooter_update ();
}


CALLSET_ENTRY (plunger, sw_launch_button)
{
#ifdef INCLUDE_AUTOPLUNGER
	if (switch_poll (MACHINE_SHOOTER_SWITCH))
	{
		sol_request_async (MACHINE_LAUNCH_SOLENOID);
	}
#endif
}


CALLSET_ENTRY (plunger, sw_left_button)
{
#ifdef INCLUDE_AUTOPLUNGER
	if (system_config.flipper_plunger == ON)
		plunger_sw_launch_button ();
#endif
}


CALLSET_ENTRY (plunger, sw_right_button)
{
#ifdef INCLUDE_AUTOPLUNGER
	if (system_config.flipper_plunger == ON)
		plunger_sw_launch_button ();
#endif
}


