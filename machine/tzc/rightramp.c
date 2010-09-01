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


#include <freewpc.h>
#include <bridge_open.h>

U8 right_ramps_entered;

void sw_right_ramp_enter_task (void)
{
	/* Decide whether to let the ball onto the mini-playfield,
	or dump it.  Do this once for each balls that enters the
	ramp. */
	do {
		/* Wait for the ball to get to the holder */
		task_sleep_sec (2);
		while (kickout_locks > 0)
			task_sleep (TIME_100MS);
		
		/* Drop the ball back to the playfield */
		sound_send (SND_RIGHT_RAMP_EXIT);
		bridge_open_start ();
		task_sleep (TIME_300MS);
		/* Sleep a bit longer if multiball, so we knock any following
		 * balls back to the playfield */
		if (multi_ball_play ())
			task_sleep (TIME_200MS);
		bridge_open_stop ();
	} while (--right_ramps_entered > 0);
	task_exit ();
}

CALLSET_ENTRY (right_ramp, sw_right_ramp)
{
	/* Handle all balls entering the ramp unconditionally, so that
	they are disposed of properly. */
	right_ramps_entered++;
	task_recreate_gid_while (GID_RIGHT_RAMP_ENTERED,
		sw_right_ramp_enter_task, TASK_DURATION_INF);

	/* Scoring functions only happen during a game */
	if (!in_live_game)
		return;
	callset_invoke (shot_rightramp);
	sound_send (SND_RIGHT_RAMP_DEFAULT_ENTER);
}

CALLSET_ENTRY (right_ramp, start_ball)
{
	right_ramps_entered = 0;
}

CALLSET_ENTRY (right_ramp, ball_search)
{
	/* Wait a bit in case a ball has been launched from the
	 * autofire ball_search, this stops my loose hanging ramp
	 * from hitting the ball on the way round the loop */
	task_sleep_sec (1);
	bridge_open_start ();
	task_sleep (TIME_500MS);
	bridge_open_stop ();
}
