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


/*
   sw_face_position is closed on each face apart from the last face,
   so to find home:

   Turn the motor until we see a time delay between sw_face_position

*/

#include <freewpc.h>
#include "wire_ball_lock.h"
#include "head_motor.h"
#include "head_motor_relay.h"
#include <gate.h>

typedef enum { 
	HEAD_INIT, 
	HEAD_CALIBRATED, 
	HEAD_CALIBRATING 
} head_mode_t;

typedef enum { 
	FACE_LOST,
	FACE_1, 
	FACE_2, 
	FACE_3, 
	FACE_4 

} head_position_t;



head_position_t __local__ head_desired;
head_position_t head_position;
head_mode_t head_mode;
U8 head_ticks;
bool head_divert;

#define HEAD_TIMEOUT 	(1000 * 30) / 100 // 20 Seconds

#define HEAD4_TIMEOUT 	(1000 * 5) / 100 // 20 Seconds
#define CALIBRATION_TIMEOUT (1000 * 4) / 100 // 4 Seconds

void calibrating_head_deff (void)
{
	while (head_mode == (HEAD_CALIBRATING))
	{
		seg_alloc_clean ();
		if (switch_poll_logical (SW_FACE_POSITION))
			seg_write_row_center (0, "SWITCH CLOSED");
		else
			seg_write_row_center (0, "CALIBRATING HEAD");
		sprintf ("%d TICKS F%d M%d", head_ticks, head_position, head_mode);
		seg_write_row_center (1, sprintf_buffer);
		seg_show ();
		task_sleep (TIME_100MS);
	}
	deff_exit ();
}

inline void head_stop (void)
{	
	head_motor_stop ();
	head_motor_relay_stop ();
	task_kill_gid (GID_HEAD_TURN_LEFT);
	task_kill_gid (GID_HEAD_TURNING);
	task_kill_gid (GID_HEAD_CALIBRATING);
	task_kill_gid (GID_HEAD_TURN_RIGHT);
}

static void head_turn_left_task (void)
{
	for (head_ticks = 0; head_ticks < HEAD_TIMEOUT; head_ticks++)
	{
		head_motor_relay_start ();
		head_motor_start ();
		if (head_position == FACE_1 && head_ticks > HEAD4_TIMEOUT && head_mode != HEAD_CALIBRATING)
		{
			callset_invoke (sw_face_position);
		}

		if (head_position == head_desired)
		{
			head_stop ();
			task_exit ();
		}
		task_sleep (TIME_100MS);
	}

	//Task should have been killed by now by sw_face_position
	global_flag_on (GLOBAL_FLAG_HEAD_BROKEN);
	head_stop ();
	task_exit ();
}

static void head_turn_right_task (void)
{
	for (head_ticks = 0; head_ticks < HEAD_TIMEOUT; head_ticks++)
	{
		head_motor_start ();
		if (head_position == FACE_3 && head_ticks > HEAD4_TIMEOUT && head_mode != HEAD_CALIBRATING)
		{
			callset_invoke (sw_face_position);
		}

		if (head_position == head_desired)
		{
			head_stop ();
			task_exit ();
		}
		task_sleep (TIME_100MS);
	}

	//Task should have been killed by now
	global_flag_on (GLOBAL_FLAG_HEAD_BROKEN);
	head_stop ();
	task_exit ();
}

inline static void head_turn_left (void)
{
	if (head_mode == HEAD_CALIBRATED && !global_flag_test (GLOBAL_FLAG_HEAD_BROKEN))
	{
	//	while (task_find_gid (GID_HEAD_TURN_LEFT) || task_find_gid (GID_HEAD_TURN_RIGHT))
	//		task_sleep (TIME_500MS);

		task_create_gid (GID_HEAD_TURN_LEFT, head_turn_left_task);
	}
}

inline static void head_turn_right (void)
{
	if (head_mode == HEAD_CALIBRATED && !global_flag_test (GLOBAL_FLAG_HEAD_BROKEN))
	{
		//Turning right should always have priority
	//	while (task_find_gid (GID_HEAD_TURN_LEFT) || task_find_gid (GID_HEAD_TURN_RIGHT))
	//		task_sleep (TIME_300MS);

		task_create_gid (GID_HEAD_TURN_RIGHT, head_turn_right_task);
	}
}


void head_calibrate (void)
{
	head_stop ();
	head_mode = HEAD_CALIBRATING;
	head_position = FACE_LOST;
	head_desired = FACE_1;
	global_flag_on (GLOBAL_FLAG_HEAD_BROKEN);
	//deff_start (DEFF_CALIBRATING_HEAD);
	task_create_gid (GID_HEAD_CALIBRATING, head_turn_right_task);

	while (task_find_gid (GID_HEAD_CALIBRATING) && head_position == FACE_LOST)
	{
		task_sleep (TIME_500MS);
	}
	
	if (head_position == FACE_1)
	{
		global_flag_off (GLOBAL_FLAG_HEAD_BROKEN);
		head_mode = HEAD_CALIBRATED;
		head_stop ();
	}
}

static void head_open_gate_task (void)
{
	gate_start ();	
	task_sleep_sec (4);
	gate_stop ();
	task_exit ();
}


CALLSET_ENTRY (head, end_ball)
{
	task_kill_gid (GID_HEAD_DIVERT);
	task_kill_gid (GID_LEFT_EYE_POPPER);
	task_kill_gid (GID_RIGHT_EYE_POPPER);
}

CALLSET_ENTRY (head, init)
{
	head_ticks = 0;
	head_mode = HEAD_INIT;
	head_divert = FALSE;
	//TODO Option to not calibrate head on each powerup
}

CALLSET_ENTRY (head, head_enable_divert)
{
	head_divert = TRUE;
}

CALLSET_ENTRY (head, head_stop_divert)
{
	if (single_ball_play ())
	{
		gate_stop ();
		task_kill_gid (GID_HEAD_DIVERT);
	}
}

CALLSET_ENTRY (head, head_disable_divert)
{
	head_divert = FALSE;
	gate_stop ();
	task_kill_gid (GID_HEAD_DIVERT);
}

CALLSET_ENTRY (head, head_check_divert)
{
	if (!head_divert)
	{
		task_create_gid (GID_HEAD_DIVERT, head_open_gate_task);
	}
}


CALLSET_ENTRY (head, start_player)
{
	head_desired = FACE_1;
}

CALLSET_ENTRY (head, start_ball)
{
	callset_invoke (check_head_face);
}

CALLSET_ENTRY (head, calibrate_head)
{
	head_calibrate ();
}

CALLSET_ENTRY (head, init_complete)
{
	head_calibrate ();
	head_desired = FACE_1;
}

CALLSET_ENTRY (head, amode_start)
{
	while (head_mode == HEAD_CALIBRATING)
	{
		deff_restart (DEFF_CALIBRATING_HEAD);
		task_sleep (TIME_500MS);
	}
}

static void set_head_position_task (void)
{
	if (head_desired > head_position)
		head_turn_right ();
	else if (head_desired < head_position)
		head_turn_left ();
	//Wait for the head to stop turning
	while (head_desired != head_position)
		task_sleep (TIME_100MS);
	task_exit ();
}

CALLSET_ENTRY (head, check_head_face)
{
	if (head_desired == head_position || head_mode == HEAD_CALIBRATING)
		return;

	if (!task_find_gid (GID_HEAD_TURNING))
		task_create_gid (GID_HEAD_TURNING, set_head_position_task);
}

CALLSET_ENTRY (head, sw_face_position)
{
	if (head_mode == HEAD_CALIBRATING)
	{
		if (head_ticks > CALIBRATION_TIMEOUT)
		{
			head_stop ();
			head_position = FACE_1;
		}
	}
	else if (head_mode == HEAD_CALIBRATED)
	{
		if (head_ticks > HEAD4_TIMEOUT)
		{
			head_position = FACE_4;
		}

		else if (task_find_gid (GID_HEAD_TURN_LEFT))
		{
				head_position--;
		}
		else if (task_find_gid (GID_HEAD_TURN_RIGHT))
		{
			if (head_position == FACE_4)
				head_position = FACE_1;
			else
				head_position++;
		}

		if (head_position == head_desired)
			head_stop ();
	}
	head_ticks = 0;
}

CALLSET_ENTRY (head, sw_enter_head)
{

}

static void right_eye_popper_task (void)
{
	task_sleep_sec (10);
	if (head_position == FACE_2)
		sol_request (SOL_RIGHT_EYE);
	task_exit ();

}

static void left_eye_popper_task (void)
{
	task_sleep_sec (10);
	if (head_position == FACE_2)
		sol_request (SOL_LEFT_EYE);
	task_exit ();

}

CALLSET_ENTRY (head, dev_left_eye_enter)
{
	if (head_position != FACE_2)
	{
		head_position = FACE_LOST;
		global_flag_on (GLOBAL_FLAG_HEAD_BROKEN);
	}
	else if (global_flag_test (GLOBAL_FLAG_SUPER_JACKPOT_LIT))
	{
		if (switch_poll_logical (SW_RIGHT_EYE))
		{
			callset_invoke (score_super_jackpot);
		}
		else
			task_recreate_gid (GID_LEFT_EYE_POPPER, left_eye_popper_task);
	}
}

CALLSET_ENTRY (head, dev_left_eye_kick_attempt)
{
	while (task_find_gid (GID_LEFT_EYE_POPPER))
			task_sleep (TIME_500MS);
}

CALLSET_ENTRY (head, dev_right_eye_kick_attempt)
{
	while (task_find_gid (GID_RIGHT_EYE_POPPER))
			task_sleep (TIME_500MS);
}


CALLSET_ENTRY (head, dev_right_eye_enter)
{
	if (head_position != FACE_2)
	{
		head_position = FACE_LOST;
		global_flag_on (GLOBAL_FLAG_HEAD_BROKEN);
	}
	else if (global_flag_test (GLOBAL_FLAG_SUPER_JACKPOT_LIT))
	{
		if (switch_poll_logical (SW_LEFT_EYE))
		{
			callset_invoke (score_super_jackpot);
		}
		else
			task_recreate_gid (GID_LEFT_EYE_POPPER, right_eye_popper_task);
	}
}

CALLSET_ENTRY (head, dev_mouth_enter)
{
	if (head_position != FACE_1)
	{
		head_position = FACE_LOST;
		global_flag_on (GLOBAL_FLAG_HEAD_BROKEN);
	}
	//TODO Check if we actually want to start the deff if head is lost
	else if (in_game)
	{
		deff_start (DEFF_I_CAN_SPEAK);
		task_sleep_sec (4);
		timer_restart_free (GID_MOUTH_TO_WIREFORM, TIME_2S);
	}
}
