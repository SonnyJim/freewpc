#include <freewpc.h>

CALLSET_ENTRY (piano, sw_piano)
{
	device_switch_can_follow (piano, slot, TIME_3S + TIME_500MS);
	callset_invoke (shot_piano);
}
