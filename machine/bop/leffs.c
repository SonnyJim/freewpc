
#include <freewpc.h>

static void pf_strobe_up_subtask (void)
{
	for (;;)
		lamplist_apply (LAMPLIST_TOP_TO_BOTTOM, leff_toggle);
}

void strobe_up_leff (void)
{
	lamplist_set_apply_delay (TIME_16MS);
	leff_create_peer (pf_strobe_up_subtask);
	task_sleep (TIME_100MS);
	leff_create_peer (pf_strobe_up_subtask);
	task_sleep_sec (1);
	task_kill_peers ();
	leff_exit ();
}


void jets_completed_leff (void)
{
	leff_on (lamplist_index (LAMPLIST_JETS_ENTER, 0));

	for (;;)
	{
		lamplist_rotate_previous (LAMPLIST_JETS_ENTER, matrix_lookup (LMX_EFFECT1_LAMPS));
		task_sleep (TIME_33MS);
	}
}

void skill_strobe_leff (void)
{
	for (;;)
	{
		leff_on (LM_SKILL_SHOT_50K);
		task_sleep (TIME_100MS);
		leff_off (LM_SKILL_SHOT_50K);
		leff_on (LM_SKILL_SHOT_75K);
		task_sleep (TIME_100MS);
		leff_off (LM_SKILL_SHOT_75K);
		leff_on (LM_SKILL_SHOT_100K);
		task_sleep (TIME_100MS);
		leff_off (LM_SKILL_SHOT_100K);
		leff_on (LM_SKILL_SHOT_200K);
		task_sleep (TIME_100MS);
		leff_off (LM_SKILL_SHOT_200K);
		leff_on (LM_SKILL_SHOT_25K);
		task_sleep (TIME_100MS);
		leff_off (LM_SKILL_SHOT_25K);
	}
}

static void amode_cycle_leff1 (void)
{
	for (;;)
	{

		lamplist_rotate_next (LAMPLIST_BIG_WHEEL,
			matrix_lookup (LMX_EFFECT1_LAMPS));

		lamplist_rotate_previous (LAMPLIST_SMALL_WHEEL,
			matrix_lookup (LMX_EFFECT1_LAMPS));

		lamplist_rotate_next (LAMPLIST_LEFT_LOOPS,
			matrix_lookup (LMX_EFFECT1_LAMPS));

		lamplist_rotate_previous (LAMPLIST_RIGHT_LOOPS,
			matrix_lookup (LMX_EFFECT1_LAMPS));

		lamplist_rotate_previous (LAMPLIST_RIGHT_RAMPS,
			matrix_lookup (LMX_EFFECT1_LAMPS));

		lamplist_rotate_next (LAMPLIST_JETS_ENTER,
			matrix_lookup (LMX_EFFECT1_LAMPS));

		lamplist_rotate_next (LAMPLIST_SKILL_SHOT,
			matrix_lookup (LMX_EFFECT1_LAMPS));

		lamplist_rotate_next (LAMPLIST_MPF,
			matrix_lookup (LMX_EFFECT1_LAMPS));

		lamplist_rotate_next (LAMPLIST_LOWER_PF,
			matrix_lookup (LMX_EFFECT1_LAMPS));

		lamplist_rotate_next (LAMPLIST_LEFT_RAMPS,
			matrix_lookup (LMX_EFFECT1_LAMPS));
		task_sleep (TIME_166MS);
	}
}

inline static void amode_cycle (void)
{
	leff_on (lamplist_index (LAMPLIST_SMALL_WHEEL, 0));
	leff_on (lamplist_index (LAMPLIST_BIG_WHEEL, 0));
	leff_on (lamplist_index (LAMPLIST_LEFT_LOOPS, 0));
	leff_on (lamplist_index (LAMPLIST_RIGHT_LOOPS, 0));
	leff_on (lamplist_index (LAMPLIST_RIGHT_RAMPS, 0));
	leff_on (lamplist_index (LAMPLIST_JETS_ENTER, 0));
	leff_on (lamplist_index (LAMPLIST_SKILL_SHOT, 0));
	leff_on (lamplist_index (LAMPLIST_MPF, 0));
	leff_on (lamplist_index (LAMPLIST_LOWER_PF, 0));
	leff_on (lamplist_index (LAMPLIST_LEFT_RAMPS, 0));

	leff_create_peer (amode_cycle_leff1);
}

void amode_leff (void)
{
	amode_cycle ();
	while (1)
	{
		task_sleep_sec (4);
	}
}
