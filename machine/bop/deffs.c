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

void i_can_speak_deff (void)
{
	sound_send (SND_FACE_ENTERED);
	seg_alloc_clean ();
	seg_write_row_center (0, "I CAN HAZ");
	seg_write_row_center (1, "A VOICE");
	seg_sched_transition (&seg_trans_ltr);
	seg_show ();
	task_sleep_sec (2);
	sound_send (SND_I_CAN_SPEAK);
	task_sleep_sec (3);
	deff_exit ();
}

void ball_locked_deff (void)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "BALL");
	seg_write_row_center (1, "LOCKED");
	seg_sched_transition (&seg_trans_ltr);
	seg_show ();
	task_sleep_sec (2);
	deff_exit ();
}

