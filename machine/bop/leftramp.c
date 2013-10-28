/*
 * Copyright 2010 by Ewan Meadows (sonny_jim@hotmail.com)
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
#include <gate.h>

void shuttle_launch_deff (void)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "SHUTTLE LAUNCH");
	seg_write_row_center (1, "WOOOSH");
	seg_show ();
	task_sleep_sec (2);
	deff_exit ();
}

void abort_launch_deff (void)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "ABORT");
	seg_show ();
	task_sleep (TIME_500MS);
	seg_alloc_clean ();
	seg_write_row_center (1, "ABORT");
	seg_show ();
	task_sleep (TIME_500MS);
	deff_exit ();
}

static void divert_to_mpf_task (void)
{
	gate_start ();	
	task_sleep_sec (4);
	gate_stop ();
	task_exit ();
}

void left_ramp_flasher_task (void)
{
	U8 n;
	for (n = 0; n < 6; n++)
	{
		flasher_pulse (FLASH_LEFT_RAMP);
		task_sleep (TIME_166MS);
	}
	task_exit ();
}


CALLSET_ENTRY (leftramp, l_ramp_entered)
{
	sound_send (SND_SHUTTLE_LAUNCH);
	deff_start (DEFF_SHUTTLE_LAUNCH);
	//TODO Some rules logic
	task_create_gid (GID_DIVERT_TO_MPF, divert_to_mpf_task);
	task_create_gid (GID_LEFT_RAMP_FLASHER, left_ramp_flasher_task);
}

CALLSET_ENTRY (leftramp, l_ramp_rollback)
{
	task_kill_gid (GID_RIGHT_RAMP_FLASHER);
	if (task_kill_gid (GID_DIVERT_TO_MPF))
		gate_stop ();

	deff_stop (DEFF_SHUTTLE_LAUNCH);
	deff_start (DEFF_ABORT_LAUNCH);
	sound_send (SND_ABORT_ABORT);
	gate_stop ();

}
