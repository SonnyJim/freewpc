/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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
#include <coin.h>
#include <highscore.h>
#include <replay.h>

/**
 * \file
 * \brief Common attract mode module
 * This file implements a basic attract mode that can be used by any game.
 */

/* Which page is to be shown */
U8 amode_page;

/* non-zero when the page has been requested to change */
U8 amode_page_changed;

/* Used to show the scores for 2 mins after a right flipper hold */
bool amode_show_scores_long;

void amode_page_change (S8 delta);


void system_amode_leff (void)
{
	for (;;)
		task_sleep_sec (1);
}


void amode_flipper_sound_debounce_timer (void)
{
	task_sleep_sec (30);
	task_exit ();
}


void amode_flipper_sound (void)
{
	if (!task_find_gid (GID_AMODE_FLIPPER_SOUND_DEBOUNCE))
	{
		task_create_gid (GID_AMODE_FLIPPER_SOUND_DEBOUNCE,
			amode_flipper_sound_debounce_timer);
#ifdef MACHINE_AMODE_FLIPPER_SOUND_CODE
		sound_send (MACHINE_AMODE_FLIPPER_SOUND_CODE);
#endif
	}
}


void amode_sleep_sec (U8 secs)
{
	if (secs > 0)
	{
		amode_page_changed = 0;
		while (secs > 0)
		{
			task_sleep (TIME_250MS);
			if (amode_page_changed)
				return;
			task_sleep (TIME_250MS);
			if (amode_page_changed)
				return;
			task_sleep (TIME_250MS);
			if (amode_page_changed)
				return;
			task_sleep (TIME_250MS);
			if (amode_page_changed)
				return;
			secs--;
		}
	}
}

void amode_page_start (void)
{
	amode_page_changed = 0;
}

void amode_page_end (U8 secs)
{
	amode_sleep_sec (secs);
	amode_page_change (1);
}


void amode_score_page (void)
{
	dmd_alloc_low_clean ();
	scores_draw ();
	dmd_show_low ();

	/* Hold the scores up for a while longer than usual
	 * in tournament mode or when triggered by a right button hold */
	if (system_config.tournament_mode == YES || amode_show_scores_long)
	{
		/* Don't allow the player to skip past the scores for 10 seconds */
		timer_restart_free (GID_AMODE_BUTTON_DEBOUNCE, TIME_10S);
		amode_show_scores_long = FALSE;
		amode_page_end (120);
	}
	else
		amode_page_end (5);
}


#if (MACHINE_DMD == 1)
void amode_logo_page (void)
{
	U16 fno;

	for (fno = IMG_FREEWPC_SMALL; fno <= IMG_FREEWPC; fno += 2)
	{
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_show2 ();
		task_sleep (TIME_66MS);
	}
	amode_sleep_sec (3);
	for (fno = IMG_FREEWPC; fno >= IMG_FREEWPC_SMALL; fno -= 2)
	{
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_show2 ();
		task_sleep (TIME_66MS);
	}
	dmd_sched_transition (&trans_bitfade_slow);
	amode_page_end (0);
}
#endif


void amode_credits_page (void)
{
	credits_draw ();
	dmd_show_low ();
	amode_page_end (3);
}


void amode_replay_page (void)
{
	if (system_config.replay_award != FREE_AWARD_OFF)
	{
		replay_draw ();
		amode_sleep_sec (3);
	}
	amode_page_end (0);
}


void amode_high_score_page (void)
{
	if (hstd_config.highest_scores == ON)
	{
		high_score_draw_gc ();
		amode_sleep_sec (3);
		high_score_draw_12 ();
		amode_sleep_sec (3);
		high_score_draw_34 ();
		amode_sleep_sec (3);
	}
	amode_page_end (0);
}


#ifdef CONFIG_RTC
void amode_date_time_page (void)
{
	if (system_config.show_date_and_time == YES)
	{
		rtc_show_date_time ();
		amode_page_end (3);
	}
}
#endif


void amode_kill_music (void)
{
	music_set (MUS_OFF);
	amode_page_end (0);
}

void (*amode_page_table[]) (void) = {
	amode_score_page,
#if (MACHINE_DMD == 1)
	amode_logo_page,
#endif
	amode_credits_page,
	amode_replay_page,
	amode_high_score_page,
#ifdef CONFIG_RTC
	amode_date_time_page,
#endif
	amode_kill_music,
};


__attribute__((noinline)) void amode_page_change (S8 delta)
{
	amode_page += delta;
	/* Check for boundary cases */
	if (amode_show_scores_long)
	{
		amode_page = 0;
	}
	else if (amode_page >= 0xF0)
	{
		amode_page = (sizeof (amode_page_table) / sizeof (void *)) - 1;
	}
	else if (amode_page >= sizeof (amode_page_table) / sizeof (void *))
	{
		amode_page = 0;
	}

	/* TODO Check for pages that should be skipped? */
	amode_page_changed = 1;
}


void amode_right_button_detect (void)
{
	/* Exit early if the player releases the button or presses the other
	 * button at the same time */
	U8 hold = TIME_5S / TIME_100MS;
	while (hold > 0)
	{
		if (!switch_poll_logical (SW_RIGHT_BUTTON)
				|| switch_poll_logical (SW_LEFT_BUTTON))
		{
			task_exit ();
		}
		task_sleep (TIME_100MS);
		hold--;
	}

#ifdef MACHINE_AMODE_FLIPPER_SOUND_CODE
	sound_send (MACHINE_AMODE_FLIPPER_SOUND_CODE);
#endif
	/* Switch to the scores page on the next page change */
	amode_show_scores_long = TRUE;
}


bool amode_check_flipper_button (void)
{
	if (deff_get_active () == DEFF_AMODE
		&& !timer_find_gid (GID_AMODE_BUTTON_DEBOUNCE))
		return TRUE;
	else
		return FALSE;
}


CALLSET_ENTRY (amode, sw_left_button)
{
	if (amode_check_flipper_button ())
	{
		amode_flipper_sound ();
		if (amode_page_changed == 0)
			amode_page_change (-1);
	}
}



CALLSET_ENTRY (amode, sw_right_button)
{
	if (amode_check_flipper_button ())
	{
		amode_flipper_sound ();
		if (amode_page_changed == 0)
			amode_page_change (1);
		amode_right_button_detect ();
	}
}


CALLSET_ENTRY (amode, init, start_game)
{
	amode_show_scores_long = FALSE;
}


__attribute__((noreturn)) void system_amode_deff (void)
{

	/* When amode is started, diagnostic are also being re-run.  Give that
	some time to finish, so that the score screen will show the credit
	dot correctly. */
	task_sleep (TIME_100MS);

	amode_page = 0;
	for (;;)
	{
#ifdef MACHINE_CUSTOM_AMODE
		if (amode_page == 1)
		{
			callset_invoke (amode_page);
		}
#endif
		amode_page_table[amode_page] ();
	}
}

