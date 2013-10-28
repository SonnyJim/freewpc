#include <freewpc.h>

void bonus_deff (void)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "BONUS");
	seg_show ();
	task_sleep_sec (2);
	deff_exit ();
}

CALLSET_ENTRY (trivial, bonus)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "BONUS");
	seg_show ();
	task_sleep_sec (2);


}

