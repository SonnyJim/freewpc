#include "freewpc.h"
#include "head.h"

bool can_lock_ball;
bool mb_jackpot_collected;
U8 multiball_level;
__local__ U8 balls_locked;

extern U8 mpf_level;
extern score_t total_jets_score;
extern U8 rightramp_level;
extern U8 left_loop_level;
extern U8 right_loop_level;
extern U8 total_body_parts;
score_t super_jackpot_calc;
score_t super_jackpot_value;


device_t *dev = device_entry (DEVNO_WIRE_BALL_LOCK);

void jackpot_deff (void)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "JACKPOT");
	seg_write_row_center (1, "1 MILLION");
	seg_show ();
	task_sleep_sec (2);
	deff_exit ();
}

void super_jackpot_deff (void)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "SUPER JACKPOT");
	sprintf_score (super_jackpot_value);
	seg_write_row_center (1, sprintf_buffer);
	seg_show ();
	task_sleep_sec (2);
	deff_exit ();
}


CALLSET_ENTRY (multiball, score_super_jackpot)
{
	score_long (super_jackpot_value);
	deff_start (DEFF_SUPER_JACKPOT);
	global_flag_off (GLOBAL_FLAG_SUPER_JACKPOT_LIT);
}

void super_jackpot_value_deff (void)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "SUPER JACKPOT");
	seg_write_row_center (1, "VALUE");
	seg_show ();
	task_sleep_sec (1);
	
	if (total_body_parts > 0)
	{
		score_zero (super_jackpot_calc);
		score_add (super_jackpot_calc, score_table[SC_50K]);
		score_mul (super_jackpot_calc, total_body_parts);

		seg_alloc_clean ();
		psprintf ("1 BODY PART", "%d BODY PARTS", total_body_parts);
		seg_write_row_center (0, sprintf_buffer);
		sprintf_score (super_jackpot_calc);
		seg_write_row_center (1, sprintf_buffer);
		seg_show ();
		task_sleep_sec (1);
	}

	score_zero (super_jackpot_calc);
	switch (mpf_level)
	{
		case 0:
			score_add (super_jackpot_calc, score_table[SC_100K]);
			break;
		case 1:
			score_add (super_jackpot_calc, score_table[SC_200K]);
			break;
		case 2:
			score_add (super_jackpot_calc, score_table[SC_300K]);
			break;
		case 3:
			score_add (super_jackpot_calc, score_table[SC_500K]);
			break;
		default:
			break;
	}
	seg_alloc_clean ();
	sprintf ("MPF LEVEL");
	seg_write_row_center (0, sprintf_buffer);
	sprintf_score (super_jackpot_calc);
	seg_write_row_center (1, sprintf_buffer);
	seg_show ();
	task_sleep_sec (1);
	


	score_zero (super_jackpot_calc);
	switch (rightramp_level)
	{
		case 0:
			score_add (super_jackpot_calc, score_table[SC_50K]);
			break;
		case 1:
			score_add (super_jackpot_calc, score_table[SC_100K]);
			break;
		case 2:
			score_add (super_jackpot_calc, score_table[SC_500K]);
			break;
		case 3:
			score_add (super_jackpot_calc, score_table[SC_1M]);
			break;
		default:
			break;
	}
	seg_alloc_clean ();
	sprintf ("RIGHT RAMP");
	seg_write_row_center (0, sprintf_buffer);
	sprintf_score (super_jackpot_calc);
	seg_write_row_center (1, sprintf_buffer);
	seg_show ();
	task_sleep_sec (1);
	
	score_zero (super_jackpot_calc);
	if (score_compare (total_jets_score, score_table[SC_500K]) >= 0)
		score_add (super_jackpot_calc, score_table[SC_500K]);
	else if (score_compare (total_jets_score, score_table[SC_100K]) >= 0)
		score_add (super_jackpot_calc, score_table[SC_100K]);
	else if (score_compare (total_jets_score, score_table[SC_50K]) >= 0)
		score_add (super_jackpot_calc, score_table[SC_50K]);
	else if (score_compare (total_jets_score, score_table[SC_25K]) >= 0)
		score_add (super_jackpot_calc, score_table[SC_25K]);
	
	seg_alloc_clean ();
	sprintf ("JETS");
	seg_write_row_center (0, sprintf_buffer);
	sprintf_score (super_jackpot_calc);
	seg_write_row_center (1, sprintf_buffer);
	seg_show ();
	task_sleep_sec (1);
	
	score_zero (super_jackpot_calc);
	switch (left_loop_level)
	{
		case 0:
			score_add (super_jackpot_calc, score_table[SC_25K]);
			break;
		case 1:
			score_add (super_jackpot_calc, score_table[SC_50K]);
			break;
		case 2:
			score_add (super_jackpot_calc, score_table[SC_100K]);
			break;
		case 3:
			score_add (super_jackpot_calc, score_table[SC_500K]);
			break;
		default:
			break;
	}
	
	seg_alloc_clean ();
	sprintf ("LEFT LOOP");
	seg_write_row_center (0, sprintf_buffer);
	sprintf_score (super_jackpot_calc);
	seg_write_row_center (1, sprintf_buffer);
	seg_show ();
	task_sleep_sec (1);
	
	score_zero (super_jackpot_calc);
	switch (right_loop_level)
	{
		case 0:
			score_add (super_jackpot_calc, score_table[SC_25K]);
			break;
		case 1:
			score_add (super_jackpot_calc, score_table[SC_50K]);
			break;
		case 2:
			score_add (super_jackpot_calc, score_table[SC_100K]);
			break;
		case 3:
			score_add (super_jackpot_calc, score_table[SC_500K]);
			break;
		default:
			break;
	}
	seg_alloc_clean ();
	sprintf ("RIGHT LOOP");
	seg_write_row_center (0, sprintf_buffer);
	sprintf_score (super_jackpot_calc);
	seg_write_row_center (1, sprintf_buffer);
	seg_show ();
	task_sleep_sec (1);
	
	deff_exit ();
}

static void calculate_super_jackpot_value (void)
{
	score_zero (super_jackpot_value);

	

	if (total_body_parts > 0)
	{
		score_add (super_jackpot_value, score_table[SC_50K]);
		score_mul (super_jackpot_value, total_body_parts);
	}

	switch (mpf_level)
	{
		case 0:
			score_add (super_jackpot_value, score_table[SC_100K]);
			break;
		case 1:
			score_add (super_jackpot_value, score_table[SC_200K]);
			break;
		case 2:
			score_add (super_jackpot_value, score_table[SC_300K]);
			break;
		case 3:
			score_add (super_jackpot_value, score_table[SC_500K]);
			break;
		default:
			break;
	}
	
	switch (rightramp_level)
	{
		case 0:
			score_add (super_jackpot_value, score_table[SC_50K]);
			break;
		case 1:
			score_add (super_jackpot_value, score_table[SC_100K]);
			break;
		case 2:
			score_add (super_jackpot_value, score_table[SC_500K]);
			break;
		case 3:
			score_add (super_jackpot_value, score_table[SC_1M]);
			break;
		default:
			break;
	}

	if (score_compare (total_jets_score, score_table[SC_500K]) >= 0)
		score_add (super_jackpot_value, score_table[SC_500K]);
	else if (score_compare (total_jets_score, score_table[SC_100K]) >= 0)
		score_add (super_jackpot_value, score_table[SC_100K]);
	else if (score_compare (total_jets_score, score_table[SC_50K]) >= 0)
		score_add (super_jackpot_value, score_table[SC_50K]);
	else if (score_compare (total_jets_score, score_table[SC_25K]) >= 0)
		score_add (super_jackpot_value, score_table[SC_25K]);
	
	switch (left_loop_level)
	{
		case 0:
			score_add (super_jackpot_value, score_table[SC_25K]);
			break;
		case 1:
			score_add (super_jackpot_value, score_table[SC_50K]);
			break;
		case 2:
			score_add (super_jackpot_value, score_table[SC_100K]);
			break;
		case 3:
			score_add (super_jackpot_value, score_table[SC_500K]);
			break;
		default:
			break;
	}

	switch (right_loop_level)
	{
		case 0:
			score_add (super_jackpot_value, score_table[SC_25K]);
			break;
		case 1:
			score_add (super_jackpot_value, score_table[SC_50K]);
			break;
		case 2:
			score_add (super_jackpot_value, score_table[SC_100K]);
			break;
		case 3:
			score_add (super_jackpot_value, score_table[SC_500K]);
			break;
		default:
			break;
	}


}

CALLSET_ENTRY (multiball, start_game)
{
	balls_locked = 0;
	multiball_level = 0;
}

CALLSET_ENTRY (multiball, multiball_start)
{
	global_flag_on (GLOBAL_FLAG_SUPER_JACKPOT_LIT);
	global_flag_on (GLOBAL_FLAG_MB_JACKPOT_LIT);
	callset_invoke (head_enable_divert);
	calculate_super_jackpot_value ();
	deff_start (DEFF_SUPER_JACKPOT_VALUE);
	mb_jackpot_collected = FALSE;
}

CALLSET_ENTRY (multiball, start_ball)
{
	can_lock_ball = TRUE;
}

CALLSET_ENTRY (multiball, single_ball_play)
{
	head_desired = FACE_1;
	callset_invoke (check_head_face);
}

CALLSET_ENTRY (multiball, dev_wire_ball_lock_enter)
{
	if (!in_game)
		return;
	if (can_lock_ball)
	{
		if (device_recount (dev) < 2)
			device_lock_ball (dev);
		bounded_increment (balls_locked, 2);
		
		head_desired = FACE_2;
		callset_invoke (check_head_face);
		
		deff_start (DEFF_BALL_LOCKED);
		callset_invoke (ball_locked);
	}
}

CALLSET_ENTRY (multiball, valid_playfield)
{
	if (balls_locked == 1)
	{
		callset_invoke (wheel_award_jackpot_lit);
		if (device_recount (dev) > 0)
			device_request_empty (dev);
	}
}

CALLSET_ENTRY (multiball, award_scoop_jackpot)
{
	if (multi_ball_play () && global_flag_test (GLOBAL_FLAG_MB_JACKPOT_LIT))
	{
		global_flag_off (GLOBAL_FLAG_MB_JACKPOT_LIT);
		sound_send (SND_JACKPOT);
		score (SC_1M);
		deff_start (DEFF_JACKPOT);
	}
}
