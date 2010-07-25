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
__local__ U8 jets_hit;
__local__ U8 jets_level;
U8 jets_sound_index;

U8 jet_sounds[5][3] = {
	{ SND_POWER_UP1_1, SND_POWER_UP1_2, SND_POWER_UP1_3 }, 
	{ SND_POWER_UP3_1, SND_POWER_UP3_2, SND_POWER_UP3_3 },
	{ SND_POWER_UP4_1, SND_POWER_UP4_2, SND_POWER_UP4_3 },
	{ SND_POWER_UP5_1, SND_POWER_UP5_2, SND_POWER_UP5_3 },
	{ SND_POWER_UP6_1, SND_POWER_UP6_2, SND_POWER_UP6_3 }
};

/* How much is earned for going up a level */
const score_id_t jets_level_up_scores[] = {
	SC_100K, SC_200K, SC_300K, SC_500K, SC_1M
};

const struct generic_ladder jets_level_up_score_rule = {
	5,
	jets_level_up_scores,
	&jets_level,
};

/* How much each jet scores per level */
const score_id_t jets_hit_scores[] = {
	SC_1K, SC_10K, SC_25K, SC_50K, SC_100K
};

/* How many jets are needed to level up,
 * jet count is reset on level up */
const U8 jets_needed[] = {
	10, 20, 30, 40, 50
};

CALLSET_ENTRY (jets, jet_hit)
{
//	if (!in_live_game)
//		return;
	/* Make a sound, based on level */
	sound_send (jet_sounds[jets_level][jets_sound_index]);
	jets_sound_index++;
	if (jets_sound_index > 2)
		jets_sound_index = 0;

	/* Score the hit, based on level */	
	score (jets_hit_scores[jets_level]);
	bounded_increment (jets_hit, 255);

	/* Level up if needed */
	if (jets_hit >= jets_needed[jets_level])
	{
		generic_ladder_score_and_advance (&jets_level_up_score_rule);
		jets_hit = 0;
		//deff_start (JETS_LEVEL_UP);
	}
	else
	{
		//deff_start (JETS_HIT);
	}
}

CALLSET_ENTRY (jets, start_ball)
{
	jets_sound_index = 0;
}
