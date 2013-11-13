/*
 * Copyright 2006, 2007, 2008, 2010 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <freewpc.h>
#include <window.h>
#include <test.h>
#include "head.h"

void head_test_update (void)
{
	if (head_desired > 4)
		head_desired = 1;
	if (head_desired == 0)
		head_desired = 4;
	
	callset_invoke (check_head_face);
}

CALLSET_ENTRY (head_test, init)
{
}

void head_test_draw (void)
{
	seg_alloc_clean ();
	if (head_mode == HEAD_CALIBRATING)
		seg_write_row_center (0, "CALIBRATING");
	else if (switch_poll_logical (SW_FACE_POSITION))
		seg_write_row_center (0, "HEAD TEST 67CL");
	else
		seg_write_row_center (0, "HEAD TEST 67OP");
	sprintf ("%d TICKS F%d%d M%d", head_ticks, head_position, head_desired, head_mode);
	seg_write_row_center (1, sprintf_buffer);
	seg_show ();
}


void head_test_down (void)
{
	head_desired--;
	head_test_update ();
}


void head_test_up (void)
{
	head_desired++;
	head_test_update ();
}

void head_test_stop (void)
{

}

void bop_head_test_init (void)
{
}

void head_test_enter (void)
{
	callset_invoke (calibrate_head);
}

void head_test_thread (void)
{
	for (;;)
	{
		head_test_draw ();
		task_sleep (TIME_100MS);
	}
}

struct window_ops head_test_window = {
	DEFAULT_WINDOW,
	.init = bop_head_test_init,
	.up = head_test_up,
	.down = head_test_down,
	.enter = head_test_enter,
	.thread = head_test_thread,
};


struct menu head_test_item = {
	.name = "HEAD TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &head_test_window, NULL } },
};

