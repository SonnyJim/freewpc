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
#include <status.h>
U8 balls_served;

/* Hack to disable ballsave after first ball_serve */
CALLSET_ENTRY (config, start_ball)
{
	balls_served = 0;
}

/* Reset the count so we can have a ballsave on the next ball only */
CALLSET_ENTRY (config, mball_start)
{
	balls_served = 1;
}


CALLSET_ENTRY (config, serve_ball)
{
	balls_served++;	
}

CALLSET_ENTRY (config, ball_search)
{
	end_ball ();
}

CALLSET_ENTRY (config, status_report)
{

	status_page_init ();
	sprintf ("LIVE BALLS %d", live_balls);
	font_render_string_center (&font_mono5, 64, 11, sprintf_buffer);
	sprintf ("KICKS %d", device_entry (DEVNO_TROUGH)->kicks_needed);
	font_render_string_center (&font_mono5, 64, 21, sprintf_buffer);

	status_page_complete ();
}
