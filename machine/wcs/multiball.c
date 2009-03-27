
#include <freewpc.h>

__local__ U8 lock_count;

__local__ U8 mb_spinner_count;


void ball_locked_deff (void)
{
	deff_exit ();
}

void multiball_lit_deff (void)
{
	deff_exit ();
}

void multiball_start_deff (void)
{
	deff_exit ();
}

void multiball_running_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 16, "MULTIBALL RUNNING");
	dmd_show_low ();
	while (1)
		task_sleep_sec (5);
}

void multiball_jackpot_deff (void)
{
	deff_exit ();
}

void multiball_super_jackpot_deff (void)
{
	deff_exit ();
}

void multiball_spinner_deff (void)
{
	deff_exit ();
}


void mb_reset (void)
{
	flag_off (FLAG_LEFT_EJECT_LOCK);
	flag_off (FLAG_RIGHT_EJECT_LOCK);
	flag_off (FLAG_MULTIBALL_LIT);
	flag_off (FLAG_MULTIBALL_RUNNING);
	flag_off (FLAG_JACKPOT_LIT);
	flag_off (FLAG_SUPER_JACKPOT_LIT);
}

void mb_light (void)
{
	flag_on (FLAG_MULTIBALL_LIT);
}

void mb_light_jackpot (void)
{
	flag_on (FLAG_JACKPOT_LIT);
}

void mb_light_super_jackpot (void)
{
	flag_on (FLAG_SUPER_JACKPOT_LIT);
}

void mb_start (void)
{
	flag_on (FLAG_MULTIBALL_RUNNING);
	mb_light_jackpot ();
	flag_off (FLAG_SUPER_JACKPOT_LIT);
	mb_spinner_count = 0;
	VOIDCALL (goal_count_lamp_update);
	if (flag_test (FLAG_LEFT_EJECT_LOCK))
		device_unlock_ball (device_entry (DEVNO_LEFT_EJECT));
	if (flag_test (FLAG_RIGHT_EJECT_LOCK))
		device_unlock_ball (device_entry (DEVNO_RIGHT_EJECT));
}

static void mb_advance_spinner (void)
{
	if (++mb_spinner_count == 16)
	{
		mb_light_super_jackpot ();
	}
}

static void mb_award_jackpot (void)
{
	if (!flag_test (FLAG_JACKPOT_LIT))
	{
		flag_off (FLAG_JACKPOT_LIT);
	}
}

static void mb_award_super_jackpot (void)
{
	if (!flag_test (FLAG_SUPER_JACKPOT_LIT))
	{
		flag_off (FLAG_SUPER_JACKPOT_LIT);
	}
}

static void mb_award_lock (U8 flag, U8 devno)
{
	if (!flag_test (flag))
	{
		flag_on (flag);
		device_lock_ball (device_entry (devno));
		sound_start (ST_SAMPLE, SND_LOCK_MAGNET, SL_4S, PRI_GAME_QUICK5);
		lock_count++;
	}
}


CALLSET_ENTRY (mb, dev_left_eject_enter)
{
	if (!flag_test (FLAG_MULTIBALL_RUNNING))
		mb_award_lock (FLAG_LEFT_EJECT_LOCK, DEVNO_LEFT_EJECT);
}

CALLSET_ENTRY (mb, dev_right_eject_enter)
{
	if (!flag_test (FLAG_MULTIBALL_RUNNING))
		mb_award_lock (FLAG_RIGHT_EJECT_LOCK, DEVNO_RIGHT_EJECT);
}

CALLSET_ENTRY (mb, dev_tv_popper_enter)
{
	if (flag_test (FLAG_MULTIBALL_LIT))
	{
		mb_start ();
	}
	else if (flag_test (FLAG_MULTIBALL_RUNNING)
		&& flag_test (FLAG_SUPER_JACKPOT_LIT))
	{
		mb_award_super_jackpot ();
	}
}

CALLSET_ENTRY (mb, left_loop_shot)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING)
		&& !flag_test (FLAG_JACKPOT_LIT))
		mb_light_jackpot ();
}

CALLSET_ENTRY (mb, goal_shot)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING)
		&& flag_test (FLAG_JACKPOT_LIT))
	{
		mb_award_jackpot ();
	}
}

CALLSET_ENTRY (mb, sw_spinner_slow)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING))
		mb_advance_spinner ();
}

CALLSET_ENTRY (mb, lamp_update)
{
	lamp_flash_if (LM_FINAL_DRAW,
		(flag_test (FLAG_MULTIBALL_LIT) || flag_test (FLAG_SUPER_JACKPOT_LIT)));
}

CALLSET_ENTRY (mb, music_refresh)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING))
	{
		music_request (MUS_MULTIBALL, PRI_GAME_MODE3);
	}
}

CALLSET_ENTRY (mb, display_update)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING))
		deff_start_bg (DEFF_MULTIBALL_RUNNING, PRI_GAME_MODE3);
}

CALLSET_ENTRY (mb, single_ball_play, end_ball)
{
	mb_reset ();
}


CALLSET_ENTRY (mb, start_player)
{
	mb_reset ();
}

