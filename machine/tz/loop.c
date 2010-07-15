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
#include <sys/time.h>
/* Various loop counts, reset at the start of each ball */
U8 loops;
U8 powerball_loops;

U16 start_loop_time;
U8 loop_time;
/* Used to pass loop score to deff */
score_t loop_score;

extern __local__ U8 gumball_enable_count;
extern __local__ U8 thing_flips_enable_count;

extern void thing_flips (void);

/* Functions to stop leffs/deffs during certain game situations */
static bool can_show_loop_leff (void)
{
	if (task_find_gid (GID_SPIRALAWARD))
		return FALSE;
	else
		return TRUE;
}

#if 0
static bool can_show_loop_deff (void)
{
	if (fastlock_running ())
		return FALSE;
	else if (task_find_gid (GID_SPIRALAWARD))
		return FALSE;
	else if (free_timer_test (TIM_SPIRALAWARD_RUNNING))
		return FALSE;
	else
		return TRUE;
}
#endif
void enter_loop (void)
{
	if (in_live_game)
	{
		score (SC_1K);
		sound_send (SND_LOOP_ENTER);
	}
}

/* Loop scoring rules */
static void award_loop (void)
{
	/* loops includes powerball and spiral_loops */
	loops++;
	
	callset_invoke (award_spiral_loop);
	if (flag_test (FLAG_POWERBALL_IN_PLAY) && !multi_ball_play ())
	{
		powerball_loops++;

		if (powerball_loops < 3)
		{
			sound_send (SND_ADDAMS_FASTLOCK_STARTED);
			score_add (loop_score, score_table[SC_5M]);
			score (SC_5M);
		}
		else if (powerball_loops == 3)
		{
			sound_send (SND_LOAD_GUMBALL_NOW);
			gumball_enable_count++;
			score (SC_20M);
		}
		deff_start (DEFF_PB_LOOP);
	//	return;
	}
	else
	{
		/* Plain Old Loop */
		score_zero (loop_score);
		if (loops < 2)
		{
			score_add (loop_score, score_table[SC_100K]);
			score (SC_100K);
		}
		else if (loops < 4)
		{
			score_add (loop_score, score_table[SC_250K]);
			score (SC_250K);
		}
		else if (loops < 8)
		{
			score_add (loop_score, score_table[SC_500K]);
			score (SC_500K);
		}
		else if (loops < 10)
		{	
			score_add (loop_score, score_table[SC_750K]);
			score (SC_750K);
		}
		else
		{
			score_add (loop_score, score_table[SC_1M]);
			score (SC_1M);
			sound_send (SND_THUNDER1);
		}
		fastlock_loop_completed ();

		if (!task_find_gid (GID_SPIRALAWARD) || !task_find_gid (GID_GUMBALL))
			sound_send (SND_SPIRAL_AWARDED);
		/* Don't show deff during certain modes */
	//	if (can_show_loop_deff ())
		deff_start (DEFF_LOOP);
	}
}

static void abort_loop (void)
{
	if (in_live_game)
	{
		score (SC_1K);
		sound_send (SND_SPIRAL_SAME_SIDE_EXIT);
	}
}

static void award_left_loop (void)
{
	if (in_live_game)
	{
		event_can_follow (left_loop, hitchhiker, TIME_2S + TIME_500MS);
		event_can_follow (left_loop, camera, TIME_2S + TIME_500MS);
		if (can_show_loop_leff ())
			leff_start (LEFF_LEFT_LOOP);
		award_loop ();
	}
}

static void award_right_loop (void)
{
	if (in_live_game)
	{
		/* Lucky bounce combo */
		event_can_follow (right_loop, locked_ball, TIME_3S);
		/* 2 way combos */
		event_can_follow (right_loop, piano, TIME_1S + TIME_700MS);
		event_can_follow (right_loop, camera, TIME_4S);
		//event_can_follow (right_loop, hitchhiker, TIME_4S);
		spiralaward_right_loop_completed ();
		award_loop ();
		if (can_show_loop_leff ())
			leff_start (LEFF_RIGHT_LOOP);
	}
}

void loop_deff (void)
{
	U8 i;
	for (i = 0; i < 6; i++)
	{
		dmd_alloc_low_clean ();
		psprintf ("1 LOOP", "%d LOOPS", loops);
		font_render_string_center (&font_fixed6, 64, i, sprintf_buffer);
		
		sprintf_score (loop_score);
		font_render_string_center (&font_mono5, 64, 23 - i, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_66MS);
	}
	task_sleep_sec (1);
	deff_exit ();
}

void start_loop_speed_timer (void)
{
	start_loop_time = (get_sys_time ());
}

void stop_loop_speed_timer (void)
{
	loop_time = (get_elapsed_time (start_loop_time));
}

CALLSET_ENTRY (loop, sw_left_magnet)
{
	/* Cannot detect loops reliably during multiball */
	//if (multi_ball_play ())
	//	return;
	if (task_kill_gid (GID_LEFT_LOOP_ENTERED))
	{
		/* Left loop aborted */
		abort_loop ();
	}
	else if (task_kill_gid (GID_RIGHT_LOOP_ENTERED))
	{
		/* Right loop completed */
		//TODO Check for multiball here?
		//start_flipper_timer (); to train via player.
		stop_loop_speed_timer ();
		/* Inform thingfl.c and fastlock.c that a loop has been done. */
		//callset_invoke (thing_flips);
		//left_magnet_grab_start ();
	
		award_right_loop ();
	}
	else
	{
		/* Left loop started */
		timer_restart_free (GID_LEFT_LOOP_ENTERED, TIME_3S);
		start_loop_speed_timer ();
		enter_loop ();
	}

}

CALLSET_ENTRY (loop, sw_upper_right_magnet)
{
	/* Could autodetect present of third magnet using this switch? */
}

CALLSET_ENTRY (loop, sw_lower_right_magnet)
{

	/* Cannot detect loops reliably during multiball */
	//if (multi_ball_play ())
	//	return;

	if (event_did_follow (dev_lock_kick_attempt, right_loop))
	{
		/* Ball has just come out of lock, ignore */
		return;
	}
	else if (event_did_follow (autolaunch, right_loop))
	{
		/* Ignore right loop switch after an autolaunch */
		enter_loop ();
	}
	else if (task_kill_gid (GID_LEFT_LOOP_ENTERED))
	{
		/* Left loop completed */
		stop_loop_speed_timer ();
		award_left_loop ();
	}
	else if (task_kill_gid (GID_RIGHT_LOOP_ENTERED))
	{
		/* Right loop aborted */
		abort_loop ();
	}
	else
	{
		/* Right loop started */
		/* Inform gumball module that a ball may be approaching */
		sw_gumball_right_loop_entered ();
		timer_restart_free (GID_RIGHT_LOOP_ENTERED, TIME_3S);
		start_loop_speed_timer ();
		enter_loop ();
	}
}

CALLSET_ENTRY (loop, start_ball)
{
	/* Initialise loop counts */
	loops = 0;
	powerball_loops = 0;
	loop_time = 1;
	start_loop_time = 1;
}

CALLSET_ENTRY (loop, end_ball)
{
	timer_kill_gid (GID_LEFT_LOOP_ENTERED);
	timer_kill_gid (GID_RIGHT_LOOP_ENTERED);
}
