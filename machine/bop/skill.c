/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
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

score_t skill_score;

free_timer_id_t timer_shooter_ignore_trough_kick;

/* Strange order as 25K is treated as last */
const sound_code_t skill_sounds[] = {
	SND_SKILL_SHOT_2, SND_SKILL_SHOT_3, 
	SND_SKILL_SHOT_4, SND_SKILL_SHOT_5, SND_SKILL_SHOT_1
};

void skill_missed_deff (void)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "SKILL SHOT");
	seg_write_row_center (1, "MISSED");
	seg_sched_transition (&seg_trans_rtl);
	seg_show ();
	task_sleep_sec (2);
	deff_exit ();
}

void skill_shot_deff (void)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "SKILL SHOT");
	sprintf_score (skill_score);
	seg_write_row_center (1, sprintf_buffer);
	seg_sched_transition (&seg_trans_rtl);
	seg_show ();
	task_sleep_sec (2);
	deff_exit ();
}

CALLSET_ENTRY (skill, enable_skill_shot)
{
	flag_on (FLAG_SKILL_SHOT_ENABLED);
	//Turn off GI
	if (!flag_test (FLAG_50K_SS_MADE) 
			&& !flag_test (FLAG_75K_SS_MADE)
			&& !flag_test (FLAG_100K_SS_MADE)
			&& !flag_test (FLAG_200K_SS_MADE)
			&& !flag_test (FLAG_25K_SS_MADE))
	{
		leff_start (LEFF_SKILL_STROBE);
	}
	else
	{
		if (!flag_test (FLAG_50K_SS_MADE))
				lamp_flash_on (LM_SKILL_SHOT_50K);
		if (!flag_test (FLAG_75K_SS_MADE))
				lamp_flash_on (LM_SKILL_SHOT_75K);
		if (!flag_test (FLAG_100K_SS_MADE))
				lamp_flash_on (LM_SKILL_SHOT_100K);
		if (!flag_test (FLAG_200K_SS_MADE))
				lamp_flash_on (LM_SKILL_SHOT_200K);
		if (!flag_test (FLAG_25K_SS_MADE))
				lamp_flash_on (LM_SKILL_SHOT_25K);
	}
				
}

CALLSET_ENTRY (skill, disable_skill_shot)
{
	flag_off (FLAG_SKILL_SHOT_ENABLED);
	leff_stop (LEFF_SKILL_STROBE);
	lamplist_apply (LAMPLIST_SKILL_SHOT, lamp_flash_off);
}

void shooter_flasher_task (void)
{
	U8 n;
	for (n = 0; n < 6; n++)
	{
		flasher_pulse (FLASH_SKILL_SHOT);
		task_sleep (TIME_166MS);
	}
	task_exit ();
}

void lamp_flash_task (lampnum_t lamp)
{
	lamp_flash_on (lamplist_index (LAMPLIST_SKILL_SHOT, lamp));
	task_sleep_sec (3);
	lamp_flash_off (lamplist_index (LAMPLIST_SKILL_SHOT, lamp));
	task_exit ();
}

void award_skill_switch (U8 sw)
{
	//Debounce to stop skill shot being rewarded again
		//After a misfire from the kicker
	if (!task_find_gid (GID_SKILL_DEBOUNCE)
		 && flag_test (FLAG_SKILL_SHOT_ENABLED))
	{
		flag_off (FLAG_SKILL_MISSED);
		callset_invoke (disable_skill_shot);
		timer_restart_free (GID_SKILL_DEBOUNCE, TIME_10S);
		score_zero (skill_score);
		switch (sw)
		{
			case 0:
				if (!flag_test (FLAG_50K_SS_MADE))
				{
					score_add (skill_score, score_table[SC_50K]);
					flag_on (FLAG_50K_SS_MADE);
					callset_invoke (wheel_award_fifty);
				}
				else
					flag_on (FLAG_SKILL_MISSED);
				break;
			case 1:
				if (!flag_test (FLAG_75K_SS_MADE))
				{
					score_add (skill_score, score_table[SC_75K]);
					flag_on (FLAG_75K_SS_MADE);
				}
				else
					flag_on (FLAG_SKILL_MISSED);
				break;
			case 2:
				if (!flag_test (FLAG_100K_SS_MADE))
				{
					score_add (skill_score, score_table[SC_100K]);
					flag_on (FLAG_100K_SS_MADE);
					callset_invoke (wheel_award_hundred);
				}
				else
					flag_on (FLAG_SKILL_MISSED);
				break;
			case 3:
				if (!flag_test (FLAG_200K_SS_MADE))
				{
					score_add (skill_score, score_table[SC_200K]);
					sound_send (SND_YAHOO);
					flag_off (FLAG_200K_SS_MADE);
				}
				else
					flag_on (FLAG_SKILL_MISSED);
				break;
			case 4:
				if (!flag_test (FLAG_25K_SS_MADE))
				{
					score_add (skill_score, score_table[SC_25K]);
					flag_on (FLAG_25K_SS_MADE);
				}
				else
					flag_on (FLAG_SKILL_MISSED);
				break;
			default:
				break;
		}
		score_long (skill_score);
		
		if (flag_test (FLAG_SKILL_MISSED))
		{
			deff_start (DEFF_SKILL_MISSED);
			sound_send (SND_WIPEOUT1);
		}
		else
		{
			sound_send (skill_sounds[sw]);
			deff_start (DEFF_SKILL_SHOT);
			lamp_flash_task (sw);
		}
	}
}

CALLSET_ENTRY (skill, ball_search)
{
	//if (switch_poll_logical (SW_SKILL_SHOT_50K))
		sol_request (SOL_SSHOT_KICKER);
}

CALLSET_ENTRY (skill, start_ball)
{
	callset_invoke (enable_skill_shot);
	task_kill_gid (GID_SKILL_DEBOUNCE);
}

CALLSET_ENTRY (skill, dev_trough_kick_attempt)
{
//	free_timer_restart (timer_shooter_ignore_trough_kick, TIME_1S);
}

CALLSET_ENTRY (skill, sw_shooter)
{
	/*
	if (flag_test (FLAG_SKILL_SHOT_ENABLED) && !free_timer_test (timer_shooter_ignore_trough_kick))
	{
		sound_send (SND_SKILL_SHOT_LAUNCH);
		task_recreate_gid (GID_SHOOTER_FLASHER, shooter_flasher_task);
	}
	*/
}

static void skill_shot_kicker_bug_task (void)
{
	task_sleep_sec (2);
	sol_request (SOL_SSHOT_KICKER);
	task_exit ();
}

/* Switch handlers */
CALLSET_ENTRY (skill, sw_skill_shot_50k)
{
	//TODO Container bug?
	task_create_gid (GID_SSHOT_KICKER_BUG, skill_shot_kicker_bug_task);
	award_skill_switch (0);	

}

CALLSET_ENTRY (skill, sw_skill_shot_75k)
{
	award_skill_switch (1);	
}

CALLSET_ENTRY (skill, sw_skill_shot_100k)
{
	award_skill_switch (2);	
}

CALLSET_ENTRY (skill, sw_skill_shot_200k)
{
	award_skill_switch (3);	
}

CALLSET_ENTRY (skill, sw_skill_shot_25k)
{
	award_skill_switch (4);	
}
