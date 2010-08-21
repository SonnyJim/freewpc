#include <freewpc.h>

CALLSET_ENTRY (deadend, sw_dead_end)
{
	device_switch_can_follow (dead_end, slot, TIME_2S + TIME_500MS);
	event_can_follow (dead_end, camera, TIME_2S);
	event_can_follow (camera_or_piano, slot_prox, TIME_5S);
}
