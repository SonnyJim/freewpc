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

U8 rightramps;
U8 rightramp_combos;
free_timer_id_t timer_r_ramp_combo;

void right_ramp_deff (void)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "HEARTBEAT");
	psprintf ("1 RAMP", "%d RAMPS", rightramps);
	seg_write_row_center (1, sprintf_buffer);
	seg_show ();
	task_sleep_sec (2);
	deff_exit ();
}

void right_ramp_combo_deff (void)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "HEARTBEAT");
	psprintf ("COMBO", "%d COMBOS", rightramp_combos);
	seg_write_row_center (1, sprintf_buffer);
	seg_show ();
	task_sleep_sec (2);
	deff_exit ();
}

CALLSET_ENTRY (rightramp, r_ramp_rollback)
{
	//sound_send (SND_RIGHT_RAMP_FAIL);
}

CALLSET_ENTRY (rightramp, r_ramp_entered)
{
	//sound_send (SND_RIGHT_RAMP_ENTER);
}

CALLSET_ENTRY (rightramp, r_ramp_made)
{
	bounded_increment (rightramps, 254);
	//if (free_timer_test (timer_r_ramp_combo))
	if (timer_find_gid (GID_R_RAMP_COMBO) )
	{
		//sound_send (SND_RIGHT_RAMP_2);
		bounded_increment (rightramp_combos, 255);
		deff_start (DEFF_RIGHT_RAMP_COMBO);
	}
	else
	{
		//sound_send (SND_RIGHT_RAMP_1);
		deff_start (DEFF_RIGHT_RAMP);
	}
	timer_restart_free (GID_R_RAMP_COMBO, TIME_4S);
	//free_timer_restart (timer_r_ramp_combo, TIME_4S);
}

CALLSET_ENTRY (rightramp, start_ball)
{
	rightramps = 0;
	rightramp_combos = 0;
}
