#include <freewpc.h>

//Collect big wheel items by scoring
//eg Lite Jackpot is lit by collecting a jackpot
//Spin lamps when wheel has been completed


//These bools are to signify that the player has collected
/*
//Small wheel acheivements
//Must have at least 5 heartbeat ramps to unlock
*/
bool swheel_jackpot_collected;
bool swheel_eb_lit_collected;
bool swheel_fiftyk_skill_shot_collected;
bool swheel_hundredk_skill_shot_collected;
bool swheel_twofiftyk_jets_collected;
bool swheel_completed;

bool bwheel_billion_collected;
bool bwheel_eb_collected;
bool bwheel_tenmill_collected;
bool bwheel_fiftymill_collected;
bool bwheel_special_lit_collected;
bool bwheel_onemill_jets_collected;
bool bwheel_completed;

score_t body_bonus_score;

//Number of body parts waiting to be collected
U8 body_parts;
//Total collected
U8 total_body_parts;

static void clear_all_lamps (void)
{
	lamplist_apply (LAMPLIST_SMALL_WHEEL, lamp_off);
	lamplist_apply (LAMPLIST_BIG_WHEEL, lamp_off);
}

static void clear_all_awards (void)
{

	flag_off (FLAG_SW_JACKPOT);
	flag_off (FLAG_EB_LIT);
	flag_off (FLAG_50K_SKILL_SHOT);
	flag_off (FLAG_100K_SKILL_SHOT);
	flag_off (FLAG_250K_JETS);
	flag_off (FLAG_SW_COMPLETED);

	flag_off (FLAG_BILLION_LIT);
	flag_off (FLAG_EB_COLLECTED);
	flag_off (FLAG_10M_TOTAL);
	flag_off (FLAG_50M_TOTAL);
	flag_off (FLAG_SPECIAL_LIT);
	flag_off (FLAG_1M_JETS);
	flag_off (FLAG_BW_COMPLETED);

	swheel_jackpot_collected = FALSE;
	swheel_eb_lit_collected = FALSE;
	swheel_fiftyk_skill_shot_collected = FALSE;
	swheel_hundredk_skill_shot_collected = FALSE;
	swheel_twofiftyk_jets_collected = FALSE;
	swheel_completed = FALSE;

	clear_all_lamps ();

	bwheel_billion_collected = FALSE;
	bwheel_eb_collected = FALSE;
	bwheel_tenmill_collected = FALSE;
	bwheel_fiftymill_collected = FALSE;
	bwheel_special_lit_collected = FALSE;
	bwheel_onemill_jets_collected = FALSE;
	bwheel_completed = FALSE;
}

CALLSET_ENTRY (wheel, start_game)
{
	clear_all_awards ();
	body_parts = 0;
	total_body_parts = 0;
}

CALLSET_ENTRY (wheel, wheel_award_jackpot_lit)
{
	flag_on (FLAG_SW_JACKPOT);
	lamp_flash_on (LM_SW_JACKPOT_LIT);
}

CALLSET_ENTRY (wheel, wheel_award_eb_lit)
{
	flag_on (FLAG_EB_LIT);
	lamp_flash_on (LM_SW_LITE_EXTRA_BALL);
}

CALLSET_ENTRY (wheel, wheel_award_fifty)
{
	flag_on (FLAG_50K_SKILL_SHOT);
	lamp_flash_on (LM_SW_50K);
}

CALLSET_ENTRY (wheel, wheel_award_hundred)
{
	flag_on (FLAG_100K_SKILL_SHOT);
	lamp_flash_on (LM_SW_100K);
}

CALLSET_ENTRY (wheel, wheel_award_jets)
{
	flag_on (FLAG_250K_JETS);
	lamp_flash_on (LM_SW_250K);
}

void flash_left_ramp_task (void)
{
	while (1)
	{
		flasher_pulse (FLASH_LEFT_RAMP);
		task_sleep_sec (1);
	}
}

CALLSET_ENTRY (wheel, single_ball_play)
{
	task_kill_gid (GID_FLASH_LEFT_RAMP);
}

CALLSET_ENTRY (wheel, lamp_update)
{
	if (multi_ball_play ())
	{
		lamp_off (LM_SPIN_SMALL_WHEEL);
		
		if (flag_test (GLOBAL_FLAG_SUPER_JACKPOT_LIT) && !task_find_gid (GID_FLASH_LEFT_RAMP))
			task_create_gid (GID_FLASH_LEFT_RAMP, flash_left_ramp_task);
		else
			task_kill_gid (GID_FLASH_LEFT_RAMP);
		
		if (flag_test (GLOBAL_FLAG_MB_JACKPOT_LIT))
			lamp_flash_on (LM_SPIN_SMALL_WHEEL);
		else
			lamp_flash_off (LM_SPIN_SMALL_WHEEL);

	}
	else if ((flag_test (FLAG_EB_LIT) && !swheel_eb_lit_collected)
			|| (flag_test (FLAG_50K_SKILL_SHOT) && !swheel_fiftyk_skill_shot_collected)
			|| (flag_test (FLAG_100K_SKILL_SHOT)&& !swheel_hundredk_skill_shot_collected)
			|| (flag_test (FLAG_250K_JETS) && !swheel_twofiftyk_jets_collected)
			|| (flag_test (FLAG_SW_JACKPOT) && !swheel_jackpot_collected))
	{
		lamp_on (LM_SPIN_SMALL_WHEEL);
	}
	else
		lamp_off (LM_SPIN_SMALL_WHEEL);
}

static void check_swheel_award_eb (void)
{
	if (flag_test (FLAG_EB_LIT) && !swheel_eb_lit_collected)
	{
		sound_send (SND_BONUS_3 + random_scaled(5));
		swheel_eb_lit_collected = TRUE;
		lamp_flash_off (LM_SW_LITE_EXTRA_BALL);
		lamp_on (LM_SW_LITE_EXTRA_BALL);
		
		seg_alloc_clean ();
		seg_write_row_center (0, "EB LIT");
		seg_write_row_center (1, "RIGHT LEG");
		seg_sched_transition (&seg_trans_ltr);
		seg_show ();
		task_sleep_sec (1);
	}

}

static void check_swheel_award_fifty (void)
{
	if (flag_test (FLAG_50K_SKILL_SHOT) && !swheel_fiftyk_skill_shot_collected)
	{
		sound_send (SND_BONUS_3 + random_scaled(5));
		swheel_fiftyk_skill_shot_collected = TRUE;
		lamp_flash_off (LM_SW_50K);
		lamp_on (LM_SW_50K);
		
		seg_alloc_clean ();
		seg_write_row_center (0, "50K SKILL SHOT");
		seg_write_row_center (1, "LEFT FOOT");
		seg_sched_transition (&seg_trans_ltr);
		seg_show ();
		task_sleep_sec (1);
	}
}

static void check_swheel_award_hundred (void)
{
	if (flag_test (FLAG_100K_SKILL_SHOT) && !swheel_hundredk_skill_shot_collected)
	{
		sound_send (SND_BONUS_3 + random_scaled(5));
		swheel_hundredk_skill_shot_collected = TRUE;
		lamp_flash_off (LM_SW_100K);
		lamp_on (LM_SW_100K);
		
		seg_alloc_clean ();
		seg_write_row_center (0, "100K SKILL SHOT");
		seg_write_row_center (1, "RIGHT FOOT");
		seg_sched_transition (&seg_trans_ltr);
		seg_show ();
		task_sleep_sec (1);
	}
}

static void check_swheel_award_jets (void)
{
	if (flag_test (FLAG_250K_JETS) && !swheel_twofiftyk_jets_collected)
	{
		sound_send (SND_BONUS_3 + random_scaled(5));
		swheel_twofiftyk_jets_collected = TRUE;
		lamp_flash_off (LM_SW_250K);
		lamp_on (LM_SW_250K);
		
		seg_alloc_clean ();
		seg_write_row_center (0, "250K JETS");
		seg_write_row_center (1, "LEFT LEG");
		seg_sched_transition (&seg_trans_ltr);
		seg_show ();
		task_sleep_sec (1);
	}

}

static void check_swheel_award_jackpot (void)
{
	if (flag_test (FLAG_SW_JACKPOT) && !swheel_jackpot_collected)
	{
		sound_send (SND_BONUS_3 + random_scaled(5));
		swheel_jackpot_collected = TRUE;
		lamp_flash_off (LM_SW_JACKPOT_LIT);
		lamp_on (LM_SW_JACKPOT_LIT);
		
		seg_alloc_clean ();
		seg_write_row_center (0, "JACKPOT");
		seg_write_row_center (1, "RIGHT ARM");
		seg_sched_transition (&seg_trans_ltr);
		seg_show ();
		task_sleep_sec (1);
	}

}


//This is gunna get ugly
void wheel_deff (void)
{
	U8 i;
	seg_alloc_clean ();
	seg_write_row_center (0, "BODY PART");
	seg_write_row_center (1, "COLLECTED");
	seg_sched_transition (&seg_trans_fade);
	seg_show ();
	task_sleep_sec (2);

	if (!swheel_completed)
	{
		check_swheel_award_fifty ();
		check_swheel_award_hundred ();
		check_swheel_award_jets ();
		check_swheel_award_jackpot ();
		check_swheel_award_eb ();
	}
	else if (swheel_completed && !bwheel_completed)
	{
	}
	else
	{
		//wizard?
	}
	
	score_zero (body_bonus_score);
	score_add (body_bonus_score, score_table[SC_250K]);
	score_mul (body_bonus_score, body_parts);
	score_long (body_bonus_score);


	for (i = 1; i <= body_parts; i++)
	{
		sound_send (SND_BONUS_3 + random_scaled(5));
		//sound_send (SND_WHOOSH_WHEEL);
		seg_alloc_clean ();
		sprintf ("%d BODY PARTS", i);
		seg_write_row_center (0, sprintf_buffer);
		sprintf ("%d X 250K", i);
		seg_write_row_center (1, sprintf_buffer);
		seg_show ();
		task_sleep (TIME_500MS);

	}
	seg_alloc_clean ();
	sprintf ("%d BODY PARTS", body_parts);
	seg_write_row_center (0, sprintf_buffer);
	sprintf_score (body_bonus_score);
	seg_write_row_center (1, sprintf_buffer);

	task_sleep_sec (2);
	deff_exit ();
}

static inline void increment_body_parts (void)
{
	body_parts++;
	bounded_increment (total_body_parts, 255);
}

static void count_wheel_awards (void)
{
	body_parts = 0;
	if (flag_test (FLAG_EB_LIT) && !swheel_eb_lit_collected)
		increment_body_parts ();
	if (flag_test (FLAG_50K_SKILL_SHOT) && !swheel_fiftyk_skill_shot_collected)
		increment_body_parts ();
	if (flag_test (FLAG_100K_SKILL_SHOT) && !swheel_hundredk_skill_shot_collected)
		increment_body_parts ();
	if (flag_test (FLAG_250K_JETS) && !swheel_twofiftyk_jets_collected)
		increment_body_parts ();
	if (flag_test (FLAG_SW_JACKPOT) && !swheel_jackpot_collected)
		increment_body_parts ();
}

CALLSET_ENTRY (upfkicker, dev_upfkicker_enter)
{

	sound_send (SND_SCOOP_COLLECT);
	count_wheel_awards ();
	if (body_parts > 0)
		deff_start (DEFF_WHEEL);

	lamp_flash_off (LM_SPIN_SMALL_WHEEL);
	
	//Tell multiball.c a jackpot may have been awarded
	callset_invoke (award_scoop_jackpot);
}
