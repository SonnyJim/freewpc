#include <freewpc.h>

CALLSET_ENTRY (lock, dev_lock_enter)
{
	callset_invoke (shot_lock);
	sound_send (SND_ROBOT_FLICKS_GUN);
	leff_start (LEFF_LOCK);
}


CALLSET_ENTRY (lock, dev_lock_kick_attempt)
{
	while (timer_find_gid (GID_BALL_LAUNCH))
	{
		task_sleep (TIME_100MS);
	}

	sound_send (SND_LOCK_KICKOUT);
	event_can_follow (dev_lock_kick_attempt, right_loop, TIME_2S);
	/* Used to disable camera magnet grab */
	//timer_restart_free (GID_LOCK_KICKED, TIME_3S);
	//magnet_disable_catch (MAG_RIGHT);
}

