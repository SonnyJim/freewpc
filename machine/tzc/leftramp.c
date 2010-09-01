/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
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

extern U8 autofire_request_count;
extern bool autofire_busy;

static void maybe_ramp_divert (void)
{
	/* Don't divert if a ball is waiting to be fired */
	if (autofire_request_count != 0)
		return;
	if (autofire_busy)
		return;
	/* Put in condition for ramp_divert */
}

CALLSET_ENTRY (left_ramp, sw_left_ramp_enter)
{
	device_switch_can_follow (left_ramp_enter, left_ramp_exit, TIME_2S);
}

CALLSET_ENTRY (left_ramp, sw_left_ramp_exit)
{
	callset_invoke (shot_leftramp);
}
