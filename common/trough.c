/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Common trough logic
 *
 * The trough device is present on all games.  The logic here handles
 * variations such as different number of switches, and different types
 * of kicker coils.
 *
 * The key action performed by the trough callbacks is to update the
 * number of "live balls".  When a ball enters/exits the trough, this
 * count is adjusted.  When live balls goes to zero, this creates the
 * end of ball condition.  When it goes up, it indicates that a ball
 * was successfully added to play.
 */

#include <freewpc.h>

/* For games with an outhole switch.
 *
 * The outhole switch is simple; just trigger the kicker
 * to move the balls into the trough.
 */

#ifdef MACHINE_OUTHOLE_SWITCH
static void handle_outhole (void)
{
	while (switch_poll (SW_OUTHOLE))
	{
		/* TODO : For TZ, this keeps a ball on the outhole even
		when it could be moved to far left trough,
		which is not part of the trough device. */
		if (device_full_p (device_entry (DEVNO_TROUGH)))
		{
			task_sleep_sec (1);
		}
		else
		{
			sol_pulse (SOL_OUTHOLE);
			task_sleep_sec (2);
		}
	}
	task_exit ();
}
#endif


CALLSET_ENTRY (trough, sw_outhole)
{
#ifdef MACHINE_OUTHOLE_SWITCH
	task_create_gid1 (GID_OUTHOLE_HANDLER, handle_outhole);
#endif /* MACHINE_OUTHOLE_SWITCH */
}


CALLSET_ENTRY (trough, dev_trough_enter)
{
	/* Audit center drains (outlanes are audited in their respective
	 * switches) */
	if (!event_did_follow (any_outlane, center_drain))
	{
		audit_increment (&system_audits.center_drains);
	}

	/* Note that there is one less ball in play now */
	device_remove_live ();
}


CALLSET_ENTRY (trough, dev_trough_kick_success)
{
	/* Note that there is one more ball in play now */
	device_add_live ();
}
