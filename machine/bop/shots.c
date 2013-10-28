/*
 * Copyright 2013 by Ewan Meadows (sonny_jim@hotmail.com)
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

free_timer_id_t 	timer_l_ramp_entered;
free_timer_id_t 	timer_l_ramp_rollback;
free_timer_id_t 	timer_r_ramp_entered;
free_timer_id_t 	timer_r_ramp_rollback;
free_timer_id_t 	timer_head_entered;
free_timer_id_t 	timer_mpf_entered;
free_timer_id_t 	timer_heartbeat_to_skill;	
free_timer_id_t 	timer_r_loop_bottom_entered;
free_timer_id_t 	timer_r_loop_top_entered;	

CALLSET_ENTRY (shots, sw_left_ramp_enter)
{
	if (free_timer_test (timer_l_ramp_rollback))
	{
		//Ball didn't make it up the ramp
		callset_invoke (l_ramp_rollback);
	}
	else
	{
		//Start a timer to detect a rollback
		free_timer_restart (timer_l_ramp_rollback, TIME_3S);
		callset_invoke (l_ramp_entered);
	}
}

//Head logic doesn't really need an entry in shots
CALLSET_ENTRY (shots, sw_enter_head)
{
	free_timer_stop (timer_l_ramp_entered);
	free_timer_restart (timer_head_entered, TIME_4S);
}

CALLSET_ENTRY (shots, sw_right_ramp_enter)
{
	if (free_timer_test (timer_r_ramp_rollback))
	{
		//Ball has rolled back down after an attempt
		callset_invoke (r_ramp_rollback);
	}
	else
	{
		//Start a timer to detect a rollbac
		free_timer_restart (timer_r_ramp_rollback, TIME_2S);
		free_timer_restart (timer_r_ramp_entered, TIME_3S);
		callset_invoke (r_ramp_entered);
	}
}

CALLSET_ENTRY (shots, sw_right_ramp_made)
{
	if (free_timer_test (timer_r_ramp_entered))
	{
		callset_invoke (r_ramp_made);
	}
}

CALLSET_ENTRY (shots, sw_enter_mpf)
{
	//Start a timer for the exit switches
	free_timer_restart (timer_mpf_entered, TIME_4S);
}

CALLSET_ENTRY (shots, sw_mpf_exit_left)
{
	if (free_timer_test (timer_r_ramp_entered))
	{
		//We made it up the heartbeat ramp
		callset_invoke (r_ramp_entered);
	}
	callset_invoke (light_lane);
	/*
	else if (free_timer_test (timer_mpf_entered))
	{	
		//We got here from the minipf
	}
	*/
	//Ball should end up in the right inlane
}

CALLSET_ENTRY (shots, sw_mpf_exit_right)
{	/*
	if (free_timer_test (timer_mpf_entered))
	{
		//We got here from the helmet
	}
	
	else
	*/
	callset_invoke (enable_skill_shot);
//	free_timer_restart (timer_heartbeat_to_skill, TIME_5S);
}

CALLSET_ENTRY (shots, sw_shooter_lane)
{
	/*
	if (free_timer_test (timer_heartbeat_to_skill))
	{
		//We got here from the MPF
		callset_invoke (enable_skill_shot);
	}
	*/
}

CALLSET_ENTRY (shots, sw_right_loop_top)
{/*
	if (free_timer_test (timer_r_loop_bottom_entered))
	{
		//We got here from the bottom entrance
		callset_invoke (right_loop_completed);
	}
	else
	{
		free_timer_start (timer_r_loop_top_entered, TIME_3S);
	}*/
}	

CALLSET_ENTRY (shots, sw_right_loop_bottom)
{
	callset_invoke (right_loop_completed);
}
