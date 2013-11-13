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
#include <status.h>

U8 standup_multiplier;
U8 standup_counter;
U8 multiply_mode_timer;

struct timed_mode_ops multiply_mode = {
	DEFAULT_MODE,
	.gid = GID_MULTIPLY_MODE_RUNNING,
	.init_timer = 15,
	.prio = PRI_GAME_MODE3,
	.timer = &multiply_mode_timer,
	.deff_running = DEFF_STANDUP_MULTIPLIER,
	.grace_timer = 3,
	.pause = system_timer_pause,
};


static void flash_standup_lamps (void)
{
	lamp_flash_on (LM_LEFT_STANDUP);
	lamp_flash_on (LM_RIGHT_TOP_STANDUP);
	lamp_flash_on (LM_RIGHT_BOTTOM_STANDUP);
}

static void clear_standup_lamps (void)
{
	lamp_off (LM_LEFT_STANDUP);
	lamp_off (LM_RIGHT_BOTTOM_STANDUP);
	lamp_off (LM_RIGHT_TOP_STANDUP);
}

void standup_multiplier_deff (void)
{
	seg_alloc_clean ();
	sprintf ("PF MULTIPLIER");
	seg_write_row_center (0, sprintf_buffer);
	sprintf ("%dX", global_score_multiplier);
	seg_write_row_center (1, sprintf_buffer);
	seg_sched_transition (&seg_trans_center_out);
	seg_show ();
	task_sleep_sec (3);
	deff_exit ();
}

CALLSET_ENTRY (standups, lamp_update)
{
	if (timed_mode_running_p (&multiply_mode))
	{
		if (standup_multiplier > 0)
			lamp_on (LM_VALUES_X2);
		else if (multiply_mode_timer > 0 && multiply_mode_timer < 5)
			lamp_flash_on (LM_VALUES_X2);
	}
	else
	{
		lamp_off (LM_VALUES_X2);
		lamp_flash_off (LM_VALUES_X2);
	}
}

static void check_standup_group (void)
{
	if (lamp_test (LM_LEFT_STANDUP) && lamp_test (LM_RIGHT_TOP_STANDUP) && lamp_test (LM_RIGHT_BOTTOM_STANDUP))
	{
		sound_send (SND_HMM1);
		bounded_increment (standup_multiplier, 4);
		score_multiplier_set (standup_multiplier);
		deff_start (DEFF_STANDUP_MULTIPLIER);
		if (!timed_mode_running_p (&multiply_mode))
			timed_mode_begin (&multiply_mode);
		else
			timed_mode_add (&multiply_mode, 15);

		clear_standup_lamps ();
		
	}

	if (standup_multiplier == 4)
	{
		sound_send (SND_YES2);
		flash_standup_lamps ();
	}
}

static void score_standup (void)
{
	score_multiple (SC_25K, standup_counter);
}

static void handle_standup_switch (U8 sw)
{
	bounded_increment (standup_counter, 255);
	score_standup ();
	sound_send (SND_THUD1 + random_scaled(4));
	switch (sw)
	{
		case 0:
			lamp_on (LM_LEFT_STANDUP);
			break;
		case 1:
			lamp_on (LM_RIGHT_BOTTOM_STANDUP);
			break;
		case 2:
			lamp_on (LM_RIGHT_TOP_STANDUP);
			break;
		default:
			break;
	}
	check_standup_group ();
}

CALLSET_ENTRY (standups, start_ball)
{
	clear_standup_lamps ();
	standup_multiplier  = 1;
	standup_counter = 0;
	lamp_off (LM_VALUES_X2);
}

CALLSET_ENTRY (standups, end_ball)
{
	timed_mode_end (&multiply_mode);
}

CALLSET_ENTRY (standups, sw_left_standup)
{
	handle_standup_switch (0);
}

CALLSET_ENTRY (standups, sw_right_bottom_standup)
{
	handle_standup_switch (1);
}

CALLSET_ENTRY (standups, sw_right_top_standup)
{
	handle_standup_switch (2);
//	sound_send (SND_POWER_UP3_1);
}

CALLSET_ENTRY (standups, status_report)
{
	status_page_init ();
	sprintf ("PFIELD MULTIPLIER");
	font_render_string_center (&font_mono5, 64, 11, sprintf_buffer);
	sprintf ("%dX", global_score_multiplier);
	font_render_string_center (&font_mono5, 64, 21, sprintf_buffer);
	status_page_complete ();
}
