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

/* CALLSET_SECTION (leffs, __machine2__) */

#include <freewpc.h>

extern bool mpf_active;
extern U8 mpf_round_timer;
extern U8 tsm_round_timer;
void bonus_leff (void)
{
	triac_leff_disable (TRIAC_GI_MASK);
	for (;;)
		task_sleep_sec (5);
}

void gi_cycle_leff (void)
{
	U8 i;
	triac_leff_enable (TRIAC_GI_MASK);
	for (;;)
	{
		for (i=0; i < 5; i++)
		{
			triac_leff_disable (TRIAC_GI_STRING (i));
			task_sleep (TIME_100MS);
			triac_leff_enable (TRIAC_GI_STRING (i));
		}
	}
}


void flasher_happy_leff (void)
{
	U8 i;
	for (i=0; i < 8; i++)
	{
		flasher_pulse (FLASH_CLOCK_TARGET);
		flasher_pulse (FLASH_RAMP1);
		flasher_pulse (FLASH_GUMBALL_HIGH);
		task_sleep (TIME_100MS);

		flasher_pulse (FLASH_RAMP2);
		flasher_pulse (FLASH_GUMBALL_MID);
		task_sleep (TIME_100MS);

		flasher_pulse (FLASH_GUMBALL_LOW);
		flasher_pulse (FLASH_RAMP3_POWER_PAYOFF);
		task_sleep (TIME_100MS);
	}
	leff_exit ();
}


void left_ramp_leff (void)
{
	U8 i;
	for (i=0; i < 4; i++)
	{
		flasher_pulse (FLASH_RAMP1);
		task_sleep (TIME_100MS);
		flasher_pulse (FLASH_RAMP2);
		task_sleep (TIME_100MS);
		flasher_pulse (FLASH_RAMP3_POWER_PAYOFF);
		task_sleep (TIME_100MS * 2);
	}
	leff_exit ();
}

void no_gi_leff (void)
{
	triac_leff_disable (TRIAC_GI_MASK);
	task_sleep_sec (2);
	triac_leff_enable (TRIAC_GI_MASK);
	leff_exit ();
}

void turn_on_gi_leff (void)
{
	triac_leff_enable (TRIAC_GI_MASK);
	leff_exit ();
}


void flash_all_leff (void)
{
	U8 i;

	lamplist_set_apply_delay (0);
	triac_leff_enable (TRIAC_GI_MASK);
	lamplist_apply_leff_alternating (LAMPLIST_AMODE_ALL, 0);
	for (i=0; i < 32; i++)
	{
		lamplist_apply (LAMPLIST_AMODE_ALL, leff_toggle);
		task_sleep (TIME_66MS);
	}
	lamplist_apply (LAMPLIST_AMODE_ALL, leff_on);
	task_sleep_sec (1);
	leff_exit ();
}


void slot_kickout_leff (void)
{
	U8 i;
	for (i = 0; i < 6; i++)
	{
		flasher_pulse (FLASH_RAMP3_POWER_PAYOFF);
		task_sleep (TIME_200MS);
	}
	leff_exit ();
}


void gumball_strobe_leff (void)
{
	U8 i;
	for (i = 0; i < 6 ; i++)
	{
		flasher_pulse (FLASH_GUMBALL_HIGH);
		task_sleep (TIME_100MS);
		flasher_pulse (FLASH_GUMBALL_MID);
		task_sleep (TIME_100MS);
		flasher_pulse (FLASH_GUMBALL_LOW);
		task_sleep (TIME_100MS * 2);
	}
	leff_exit ();
}


void clock_target_leff (void)
{
	U8 i;

	leff_create_peer (gi_cycle_leff);
	for (i = 0; i < 12; i++)
	{
		flasher_pulse (FLASH_CLOCK_TARGET);
		task_sleep (TIME_200MS);
	}
	task_kill_gid (task_getgid ());
	leff_exit ();
}


void game_timeout_leff (void)
{
	U8 i;
	for (i=0; i < 3; i++)
	{
		task_sleep (TIME_500MS);
		triac_leff_enable (GI_POWERFIELD+GI_CLOCK);
		task_sleep (TIME_100MS);
		triac_leff_disable (GI_POWERFIELD+GI_CLOCK);
	}
	leff_exit ();
}


void clock_round_started_leff (void)
{
	U8 i;
	for (i=0; i < 3; i++)
	{
		triac_leff_enable (GI_CLOCK);
		task_sleep (TIME_100MS);
		triac_leff_disable (GI_CLOCK);
		task_sleep (TIME_200MS);
	}
	leff_exit ();
}


void multiball_running_leff (void)
{
	leff_on (LM_GUM);
	leff_off (LM_BALL);
	leff_off (LM_LOCK1);
	leff_on (LM_LOCK2);
	for (;;)
	{
		lamplist_apply (LAMPLIST_DOOR_LOCKS_AND_GUMBALL, leff_toggle);
		task_sleep (TIME_200MS);
	}
	leff_exit ();
}

static void pf_strobe_up_subtask (void)
{
	for (;;)
		lamplist_apply (LAMPLIST_SORT1, leff_toggle);
}

void strobe_up_leff (void)
{
	lamplist_set_apply_delay (TIME_16MS);
	leff_create_peer (pf_strobe_up_subtask);
	task_sleep (TIME_200MS);
	leff_create_peer (pf_strobe_up_subtask);
	task_sleep_sec (1);
	task_kill_peers ();
	leff_exit ();
}

void multi_strobe1_subtask (void)
{ for (;;) { lamplist_set_apply_delay (TIME_33MS); lamplist_apply (LAMPLIST_SORT1, leff_toggle); task_sleep (TIME_500MS); } }
void multi_strobe2_subtask (void)
{ for (;;) { lamplist_set_apply_delay (TIME_33MS); lamplist_apply (LAMPLIST_SORT2, leff_toggle); task_sleep (TIME_500MS); } }
void multi_strobe3_subtask (void)
{ for (;;) { lamplist_set_apply_delay (TIME_33MS); lamplist_apply (LAMPLIST_SORT3, leff_toggle); task_sleep (TIME_500MS); } }

void multi_strobe_leff (void)
{
	U8 i;

	leff_create_peer (multi_strobe1_subtask);
	task_sleep (TIME_300MS);
	leff_create_peer (multi_strobe2_subtask);
	task_sleep (TIME_300MS);
	leff_create_peer (multi_strobe3_subtask);
	task_sleep (TIME_300MS);

	for (i=0; i< 2; i++)
	{
		lamplist_set_apply_delay (TIME_33MS);
		lamplist_apply (LAMPLIST_SORT4, leff_toggle);
		task_sleep (TIME_500MS);
	}
	task_kill_peers ();
	leff_exit ();
}

void door_strobe_subtask (void)
{
	lamplist_set_apply_delay (TIME_16MS);
	for (;;)
		lamplist_apply (LAMPLIST_DOOR_PANELS, leff_toggle);
}

void door_strobe_leff (void)
{
	triac_leff_disable (TRIAC_GI_MASK);
	lamplist_apply (LAMPLIST_DOOR_PANELS, leff_off);
	lamplist_set_apply_delay (TIME_33MS);
	leff_create_peer (door_strobe_subtask);
	task_sleep (TIME_100MS);
	leff_create_peer (door_strobe_subtask);
	task_sleep_sec (2);
	task_kill_peers ();
	triac_leff_enable (TRIAC_GI_MASK);
	leff_exit ();
}

void right_loop_leff (void)
{
	lamplist_set_apply_delay (TIME_16MS);
	lamplist_apply (LAMPLIST_SORT4, leff_toggle);
	lamplist_apply (LAMPLIST_SORT4, leff_toggle);
	leff_exit ();
}

void left_loop_leff (void)
{
	lamplist_set_apply_delay (TIME_16MS);
	lamplist_apply (LAMPLIST_SORT3, leff_toggle);
	lamplist_apply (LAMPLIST_SORT3, leff_toggle);
	leff_exit ();
}

void jets_active_leff (void)
{
	lamplist_set_apply_delay (TIME_100MS);
	while (tsm_round_timer > 0)
	{
		lamplist_apply (LAMPLIST_JETS, leff_toggle);
	}
	leff_exit ();
}

void circle_out_leff (void)
{
	lamplist_set_apply_delay (TIME_33MS);
	lamplist_apply (LAMPLIST_CIRCLE_OUT, leff_toggle);
	lamplist_apply (LAMPLIST_CIRCLE_OUT, leff_toggle);
	leff_exit ();
}

void color_cycle_leff (void)
{
	U8 i;
	const U8 count = 9;

	lamplist_set_apply_delay (0);

	i = count;
	lamplist_apply (LAMPLIST_RED_LAMPS, leff_on);
	do {
		lamplist_apply (LAMPLIST_RED_LAMPS, leff_toggle);
		task_sleep (TIME_100MS);
	} while (--i != 0);

	i = count;
	lamplist_apply (LAMPLIST_WHITE_LAMPS, leff_on);
	do {
		lamplist_apply (LAMPLIST_WHITE_LAMPS, leff_toggle);
		task_sleep (TIME_100MS);
	} while (--i != 0);

	i = count;
	lamplist_apply (LAMPLIST_YELLOW_LAMPS, leff_on);
	do {
		lamplist_apply (LAMPLIST_YELLOW_LAMPS, leff_toggle);
		task_sleep (TIME_100MS);
	} while (--i != 0);

	i = count;
	lamplist_apply (LAMPLIST_ORANGE_LAMPS, leff_on);
	do {
		lamplist_apply (LAMPLIST_ORANGE_LAMPS, leff_toggle);
		task_sleep (TIME_100MS);
	} while (--i != 0);

	i = count;
	lamplist_apply (LAMPLIST_AMBER_LAMPS, leff_on);
	do {
		lamplist_apply (LAMPLIST_AMBER_LAMPS, leff_toggle);
		task_sleep (TIME_100MS);
	} while (--i != 0);

	leff_exit ();
}


void lock_leff_task (void)
{
	lamplist_apply (LAMPLIST_LOCK_TEST, leff_toggle);
	task_exit ();
}


void lock_leff (void)
{
	lamplist_set_apply_delay (TIME_16MS);
	leff_create_peer (lock_leff_task);
	task_sleep (TIME_300MS);
	lamplist_apply (LAMPLIST_LOCK_TEST, leff_toggle);
	task_sleep (TIME_200MS);
	leff_exit ();
}

void mpf_active_leff (void)
{
	/*
	triac_leff_disable (GI_POWERFIELD);
	task_sleep (TIME_500MS);
	triac_leff_disable (GI_LLEFT_PLAYFIELD);
	task_sleep (TIME_500MS);
	triac_leff_disable (GI_LRIGHT_PLAYFIELD);
	task_sleep (TIME_500MS);
	triac_leff_disable (GI_CLOCK);
	task_sleep (TIME_500MS);
	triac_leff_disable (GI_MYSTERY);
	*/
	triac_leff_disable (TRIAC_GI_MASK);
	triac_leff_enable (GI_POWERFIELD);
//	triac_leff_enable (TRIAC_GI_MASK);	
	lamplist_set_apply_delay (TIME_100MS);
	while (mpf_active == TRUE)
	{
		lamplist_apply (LAMPLIST_POWERFIELD_VALUES, leff_toggle);
	}
	leff_exit ();
}

void spiralaward_leff (void)
{
	lamplist_set_apply_delay (TIME_33MS);
	while (task_find_gid (TIM_SPIRALAWARD))
	{
		lamplist_apply (LAMPLIST_SPIRAL_AWARDS, leff_toggle);
	}
	leff_exit ();
}


