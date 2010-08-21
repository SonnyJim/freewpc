#include <freewpc.h>

CALLSET_ENTRY (camera, sw_camera)
{
	device_switch_can_follow (camera, slot, TIME_3S);
	event_can_follow (camera_or_piano, slot_prox, TIME_4S);
	if (event_did_follow (gumball_exit, camera) 
		|| event_did_follow (dead_end, camera))
	{
		/* Do nothing */
	}
	else
	{
		/* Camera was hit */
	}
}
