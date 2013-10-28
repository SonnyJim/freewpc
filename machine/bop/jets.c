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
U8 jets_hit;
__local__ U8 jets_level;
U8 jets_sound_index;

sound_code_t jet_sounds[5][3] = {
	{ SND_POWER_UP1_1, SND_POWER_UP1_2, SND_POWER_UP1_3 }, 
	{ SND_POWER_UP3_1, SND_POWER_UP3_2, SND_POWER_UP3_3 },
	{ SND_POWER_UP4_1, SND_POWER_UP4_2, SND_POWER_UP4_3 },
	{ SND_POWER_UP5_1, SND_POWER_UP5_2, SND_POWER_UP5_3 },
	{ SND_POWER_UP6_1, SND_POWER_UP6_2, SND_POWER_UP6_3 }
};

free_timer_id_t timer_jets_level_up;
score_t total_jets_score;

const score_id_t jets_level_up_scores[] =  { SC_100K, SC_200K, SC_300K, SC_500K, SC_1M };

//const score_id_t jets_level_up_score_table[] = { SC_100K, SC_200K, SC_300K, SC_500K, SC_1M };

/* How much each jet scores per level */
const score_id_t jets_hit_scores[] = {
	SC_1K, SC_10K, SC_25K, SC_50K, SC_100K
};

const struct generic_ladder jets_level_up_score_rule = {
	5,
	jets_hit_scores,
	&jets_level,
};


const char jets_hit_text[][4] = {
	"1K", "10K", "25K", "50K", "100K"
};

/* How many jets are needed to level up,
 * jet count is reset on level up */
const U8 jets_needed[] = {20, 30, 50, 60, 100};

void jet_hit_deff (void)
{
	seg_alloc_clean ();
	if (free_timer_test (timer_jets_level_up))
	{
		sprintf ("JETS NOW WORTH", jets_level);
		seg_write_row_center (0, sprintf_buffer);
		sprintf ("%s EACH HIT", jets_hit_text[jets_level]);
		seg_write_row_center (1, sprintf_buffer);
	}
	else
	{
		sprintf ("%d JETS", (jets_needed[jets_level] - jets_hit));
		seg_write_row_center (0, sprintf_buffer);
		sprintf ("TILL LEVEL UP"); 
		seg_write_row_center (1, sprintf_buffer);
	}
	seg_show ();
	task_sleep (TIME_500MS);
	deff_exit ();
}

CALLSET_ENTRY (jets, jet_hit)
{
	if (!in_live_game)
		return;
	//score_copy (temp_score, current_score);
	flasher_pulse (FLASH_RIGHT_HELMET);
	/* Make a sound, based on level */
	sound_send (jet_sounds[jets_level][jets_sound_index]);
	jets_sound_index++;
	if (jets_sound_index > 2)
		jets_sound_index = 0;

	/* Score the hit, based on level */	
	generic_ladder_score (&jets_level_up_score_rule);
	score_add (total_jets_score, score_table[jets_hit_scores[jets_level]]);
	bounded_increment (jets_hit, 255);

	/* Level up if needed */
	if (jets_hit >= jets_needed[jets_level])
	{
		generic_ladder_score_and_advance (&jets_level_up_score_rule);
		jets_hit = 0;
		free_timer_restart (timer_jets_level_up, TIME_2S);
	}
	
	if ((score_compare (total_jets_score, score_table[SC_250K]) >= 0) && !flag_test (FLAG_250K_JETS))
		callset_invoke (wheel_award_jets);
	deff_restart (DEFF_JET_HIT);
}

CALLSET_ENTRY (jets, start_player)
{
	jets_level = 0;
}

CALLSET_ENTRY (jets, start_ball)
{
	generic_ladder_reset (&jets_level_up_score_rule);	
	jets_sound_index = 0;
	jets_hit = 0;
	score_zero (total_jets_score);
}

CALLSET_ENTRY (jet, status_report)
{
	status_page_init ();
	sprintf ("JET LEVEL %d", jets_level);
	font_render_string_center (&font_mono5, 64, 11, sprintf_buffer);
	sprintf ("%d JETS", jets_hit);
	font_render_string_center (&font_mono5, 64, 21, sprintf_buffer);
	status_page_complete ();

	status_page_init ();
	sprintf ("TOTAL JET SCORE");
	font_render_string_center (&font_mono5, 64, 11, sprintf_buffer);
	sprintf_score (total_jets_score);
	font_render_string_center (&font_mono5, 64, 21, sprintf_buffer);
	status_page_complete ();
}
