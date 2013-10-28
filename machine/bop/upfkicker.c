#include <freewpc.h>

void upfkicker_deff (void)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "BUILD ROBOT");
	sprintf ("TBC");
	seg_write_row_center (1, sprintf_buffer);
	seg_sched_transition (&seg_trans_rtl);
	seg_show ();
	task_sleep_sec (2);
	deff_exit ();
}


CALLSET_ENTRY (upfkicker, dev_upfkicker_enter)
{
	deff_start (DEFF_UPFKICKER);
}
