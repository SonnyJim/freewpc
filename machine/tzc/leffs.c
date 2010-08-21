#include <freewpc.h>

void amode_lamp_toggle_task (void)
{
	lamplist_apply (LAMPLIST_AMODE_ALL, leff_toggle);
	task_exit ();
}

U8 amode_leff_subset;

void amode_leff_subset_task (void)
{
	register U8 lamplist = amode_leff_subset;
	lamplist_set_apply_delay (TIME_100MS);
	for (;;)
		lamplist_apply (lamplist, leff_toggle);
}

void amode_leff (void)
{
	U8 i;
	triac_leff_enable (TRIAC_GI_MASK);

	for (;;)
	{
		amode_leff_subset = LAMPLIST_DOOR_PANELS_AND_HANDLE;
		leff_create_peer (amode_leff_subset_task);
		task_sleep (TIME_33MS);
	
		for (amode_leff_subset = LAMPLIST_DOOR_LOCKS_AND_GUMBALL;
			amode_leff_subset <= LAMPLIST_SPIRAL_AWARDS;
			amode_leff_subset++)
		{
			leff_create_peer (amode_leff_subset_task);
			task_sleep (TIME_33MS);
		}

		task_sleep_sec (15);
		task_kill_peers ();

		lamplist_apply_nomacro (LAMPLIST_SORT1, leff_off);
		lamplist_set_apply_delay (TIME_16MS);
		for (i=0 ; i < 10; i++)
			lamplist_apply (LAMPLIST_SORT1, leff_toggle);
	}
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
			task_sleep (TIME_66MS);
			triac_leff_enable (TRIAC_GI_STRING (i));
		}
	}
	triac_leff_enable (TRIAC_GI_MASK);
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

static void slot_kickout_subtask (void)
{
	U8 i;
	for (i = 0; i < 5; i++)
	{
		flasher_pulse (FLASH_RAMP3_POWER_PAYOFF);
		task_sleep (TIME_100MS);
	}
	task_exit ();
}

void slot_kickout_leff (void)
{
	if (multi_ball_play ())
	{
		triac_leff_enable (TRIAC_GI_MASK);
	}
	leff_create_peer (slot_kickout_subtask);
	task_sleep (TIME_500MS);
	triac_leff_enable (TRIAC_GI_MASK);
	leff_exit ();
}


void gumball_strobe_leff (void)
{
	triac_leff_disable (TRIAC_GI_MASK);
	triac_leff_enable (GI_LLEFT_PLAYFIELD+GI_POWERFIELD);
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
	triac_leff_enable (TRIAC_GI_MASK);
	leff_exit ();
}

void clock_target_leff (void)
{
	U8 i;

	leff_create_peer (gi_cycle_leff);
	for (i = 0; i < 12; i++)
	{
		flasher_pulse (FLASH_CLOCK_TARGET);
		task_sleep (TIME_100MS);
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
	triac_leff_enable (GI_POWERFIELD+GI_CLOCK);
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
	triac_leff_enable (GI_CLOCK);
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

static void pf_strobe_down_subtask (void)
{
	for (;;)
		lamplist_apply (LAMPLIST_SORT2, leff_toggle);
}

void strobe_down_leff (void)
{
	triac_leff_disable (TRIAC_GI_MASK);
	lamplist_set_apply_delay (TIME_16MS);
	leff_create_peer (pf_strobe_down_subtask);
	task_sleep (TIME_200MS);
	leff_create_peer (pf_strobe_down_subtask);
	task_sleep_sec (1);
	task_kill_peers ();
	triac_leff_enable (TRIAC_GI_MASK);
	leff_exit ();
}


static void pf_strobe_up_subtask (void)
{
	for (;;)
		lamplist_apply (LAMPLIST_SORT1, leff_toggle);
}

void strobe_up_leff (void)
{
	leff_create_peer (gi_cycle_leff);
	lamplist_set_apply_delay (TIME_16MS);
	leff_create_peer (pf_strobe_up_subtask);
	task_sleep (TIME_200MS);
	leff_create_peer (pf_strobe_up_subtask);
	task_sleep_sec (1);
	task_kill_peers ();
	leff_exit ();
}

static void rocket_leff_subtask (void)
{
	/* SORT4 = Right to left */
	for (;;)
		lamplist_apply (LAMPLIST_SORT4, leff_toggle);
}

void rocket_leff (void)
{
	U8 i;	
	if (live_balls != 1)
	{
		triac_leff_enable (TRIAC_GI_MASK);
	}

	for (i=0; i< 7; i++)
	{	
		flasher_pulse (FLASH_UR_FLIPPER);
		task_sleep (TIME_66MS);
	}
	triac_leff_enable (TRIAC_GI_MASK);
	lamplist_set_apply_delay (TIME_16MS);
	leff_create_peer (rocket_leff_subtask);
	task_sleep (TIME_66MS);
	leff_create_peer (rocket_leff_subtask);
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

static void powerball_leff_task (void)
{
	U8 i;
	for (i=0; i < 8; i++)
	{
		flasher_pulse (FLASH_GUMBALL_LOW);
		task_sleep (TIME_100MS);
		flasher_pulse (FLASH_RAMP3_POWER_PAYOFF);
		task_sleep (TIME_100MS);
		flasher_pulse (FLASH_UR_FLIPPER);
		task_sleep (TIME_100MS);
		flasher_pulse (FLASH_RAMP2);
		task_sleep (TIME_100MS);
		flasher_pulse (FLASH_GUMBALL_MID);
		task_sleep (TIME_100MS);
		flasher_pulse (FLASH_CLOCK_TARGET);
		task_sleep (TIME_100MS);
		flasher_pulse (FLASH_RAMP1);
		task_sleep (TIME_100MS);
		flasher_pulse (FLASH_GUMBALL_HIGH);
		task_sleep (TIME_100MS);
	}
	task_exit ();
}

void powerball_announce_leff (void)
{
	triac_leff_disable (TRIAC_GI_MASK);
	leff_create_peer (powerball_leff_task);
	task_sleep_sec (2);
	triac_leff_enable (TRIAC_GI_MASK);
	task_kill_peers ();
	leff_exit ();	
}

void mpf_hit_leff (void)
{
	U8 i = 5;
	do {
		triac_leff_disable (GI_POWERFIELD);
		task_sleep (TIME_66MS);
		flasher_pulse (FLASH_POWERFIELD);
		task_sleep (TIME_33MS);
		flasher_pulse (FLASH_POWERFIELD);
		task_sleep (TIME_33MS);
		flasher_pulse (FLASH_POWERFIELD);
		task_sleep (TIME_33MS);
		triac_leff_enable (GI_POWERFIELD);
		task_sleep (TIME_66MS);
		flasher_pulse (FLASH_POWERFIELD);
		task_sleep (TIME_33MS);
	} while (--i != 0);
	leff_exit ();
}

void flash_gi_leff (void)
{
	U8 i;
	for (i = 1; i < 7; i++)
	{
		triac_leff_disable (TRIAC_GI_MASK);
		task_sleep (TIME_100MS);
		triac_leff_enable (TRIAC_GI_MASK);
		task_sleep (TIME_100MS);
	}
	triac_leff_enable (TRIAC_GI_MASK);
	leff_exit ();
}
