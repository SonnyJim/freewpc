#include <freewpc.h>

CALLSET_ENTRY (slot, dev_slot_enter)
{
	if (event_did_follow (dead_end, slot)
		|| event_did_follow (piano, slot)
		|| event_did_follow (camera, slot))
	{
		/* dead end was recently hit, so ignore slot */
		/* piano was recently hit, so ignore slot */
		/* camera was recently hit, so ignore slot */
	}
	else if (event_did_follow (skill_shot, slot))
	{
		/* skill switch was recently hit */
		set_valid_playfield ();
	}
	else
	{
		/* Slot was hit */
	}
}

void slot_kick_sound (void)
{
	sound_send (SND_SLOT_KICKOUT_2);
	task_exit ();
}

CALLSET_ENTRY (slot, dev_slot_kick_attempt)
{
	while (kickout_locks != 0)
		task_sleep (TIME_500MS);
	if (in_live_game)
	{
		sound_send (SND_SLOT_KICKOUT_1);
		leff_start (LEFF_SLOT_KICKOUT);
		task_sleep (TIME_500MS);
		task_create_anon (slot_kick_sound);
	}
}
