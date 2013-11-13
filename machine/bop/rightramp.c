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

score_t rightramp_score;

free_timer_id_t timer_r_ramp_combo;

U8 rightramp_level;

/* How much each jet scores per level */
const score_id_t rightramp_scores[] = {
	SC_25K, SC_50K, SC_100K
};

const char rightramp_scores_text[][4] = {
	"25K", "50K", "100K"
};


void right_ramp_deff (void)
{
	seg_alloc_clean ();
	psprintf ("1 HEARTBEAT", "%d HEARTBEATS", rightramps);
	seg_write_row_center (0, sprintf_buffer);
	sprintf ("%s", rightramp_scores_text);
	seg_write_row_center (1, sprintf_buffer);
	seg_show ();
	task_sleep_sec (2);
	deff_exit ();
}

void right_ramp_combo_deff (void)
{
	seg_alloc_clean ();
	psprintf ("1 HEART COMBO", "%d HEART COMBOS", rightramps);
	seg_write_row_center (0, sprintf_buffer);
	sprintf ("%s X 2", rightramp_scores_text);
	seg_write_row_center (1, sprintf_buffer);
	seg_show ();
	task_sleep_sec (2);
	deff_exit ();
}

CALLSET_ENTRY (rightramp, lamp_update)
{
	lamplist_apply (LAMPLIST_RIGHT_RAMPS, lamp_off);
	if (rightramp_level > 0)
		lamp_on (LM_RIGHT_RAMP_100K);
	if (rightramp_level > 1)
		lamp_on (LM_RIGHT_RAMP_500K);
	if (rightramp_level > 2)
		lamp_on (LM_RIGHT_RAMP_1M);
}

static void calc_rightramp_level (void)
{
	if (score_compare (rightramp_score, score_table[SC_1M]) >= 0)
		rightramp_level = 3;
	else if (score_compare (rightramp_score, score_table[SC_500K]) >= 0)
		rightramp_level = 2;
	else if (score_compare (rightramp_score, score_table[SC_100K]) >= 0)
		rightramp_level = 1;
	else
		rightramp_level = 0;
}

CALLSET_ENTRY (rightramp, r_ramp_rollback)
{
	sound_send (SND_RIGHT_RAMP_FAIL);
}

CALLSET_ENTRY (rightramp, r_ramp_entered)
{
	sound_send (SND_RIGHT_RAMP_ENTER);
}

CALLSET_ENTRY (rightramp, r_ramp_made)
{
	calc_rightramp_level ();
	bounded_increment (rightramps, 254);
	if (timer_find_gid (GID_R_RAMP_COMBO) )
	{
		sound_send (SND_RIGHT_RAMP_2);
		bounded_increment (rightramp_combos, 255);
		deff_start (DEFF_RIGHT_RAMP_COMBO);
		score (rightramp_scores[rightramp_level]);
		score (rightramp_scores[rightramp_level]);
		score_add (rightramp_score, score_table[rightramp_scores[rightramp_level]]);
	}
	else
	{
		score (rightramp_scores[rightramp_level]);
		score_add (rightramp_score, score_table[rightramp_scores[rightramp_level]]);
		sound_send (SND_RIGHT_RAMP_1);
		deff_start (DEFF_RIGHT_RAMP);
	}
	timer_restart_free (GID_R_RAMP_COMBO, TIME_4S);
}

CALLSET_ENTRY (rightramp, start_ball)
{
	rightramps = 0;
	rightramp_combos = 0;
	rightramp_level = 0;
	score_zero (rightramp_score);
}
