
#include <freewpc.h>

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

void amode_leff (void)
{
	leff_exit ();
}
