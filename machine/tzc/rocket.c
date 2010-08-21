#include <freewpc.h>

static void rocket_kick_sound (void)
{
	sound_send (SND_ROCKET_KICK_DONE);
	flasher_pulse (FLASH_UR_FLIPPER);
	task_exit ();
}

CALLSET_ENTRY (rocket, dev_rocket_kick_attempt)
{
	if (in_live_game)
	{
		if (!multi_ball_play ())
			leff_start (LEFF_ROCKET);
		sound_send (SND_ROCKET_KICK_REVVING);
		task_sleep (TIME_500MS);
		task_create_gid (0, rocket_kick_sound);
	}
}
