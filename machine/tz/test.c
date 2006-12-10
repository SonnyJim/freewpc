/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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


void autofire_launch_proc (void) __taskentry__
{
	extern void autofire_add_ball (void);

	autofire_add_ball ();
	task_exit ();
}

void autofire_to_gumball_proc (void) __taskentry__
{
	extern void gumball_load_from_trough (void);

	gumball_load_from_trough ();
	task_exit ();
}


void release_gumball_proc (void) __taskentry__
{
	extern void gumball_release (void);

	gumball_release ();
	task_exit ();
}
