/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Entry of player's initials
 * This is used by the high score module, and could be used for
 * other initials entry like loop champions, etc.
 */


#define NUM_INITIALS_ALLOWED 3

/** The array of characters that can be entered.
 * Keep the length of this as a power of 2 (32) so that
 * the circular buffer implementation is simple.
 */
static const unsigned char initial_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ */-+&";


/* The amount of time left to enter initials */
U8 initials_enter_timer;

/* The index of the next initial to be entered */
U8 initials_index;

/* The index of the currently selected letter/symbol */
U8 initials_selection;

/* The array of initials */
U8 initials_data[NUM_INITIALS_ALLOWED+1];

/* The callback function to invoke when initials are entered OK */
void (*initials_enter_complete) (void);


void enter_initials_deff (void)
{
	U8 n;

	while (task_find_gid (GID_ENTER_INITIALS))
	{
		dmd_alloc_low_clean ();
		font_render_string_left (&font_var5, 2, 3, "ENTER INITIALS");
		font_render_string_left (&font_fixed6, 2, 15, initials_data);
		sprintf ("%12s", initial_chars + initials_selection);
		font_render_string_left (&font_mono5, 2, 26, sprintf_buffer);

		for (n = 25; n <= 31; n++)
			dmd_low_buffer[16UL * n] ^= 0xFE;

		sprintf ("%d", initials_enter_timer);
		font_render_string_right (&font_mono5, 126, 3, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_300MS);
	}
	deff_exit ();
}

void initials_stop (void)
{
	task_kill_gid (GID_ENTER_INITIALS);
	initials_enter_timer = 0;
}


void initials_running (void)
{
	task_sleep_sec (1);
	initials_enter_timer = 30;
	memset (initials_data, 0, sizeof (initials_data));
	initials_index = 0;
	initials_selection = 0;
	deff_start (DEFF_ENTER_INITIALS);

#if 1
	initials_enter_complete = null_function;
#endif

	while (initials_enter_timer > 0)
	{
		task_sleep (TIME_1S + TIME_66MS);
		initials_enter_timer--;
	}
	task_exit ();
}


void initials_enter (void)
{
	task_create_gid1 (GID_ENTER_INITIALS, initials_running);
}


CALLSET_ENTRY (initials, sw_left_button)
{
	if (initials_enter_timer)
		--initials_selection;
}


CALLSET_ENTRY (initials, sw_right_button)
{
	if (initials_enter_timer)
		++initials_selection;
}


CALLSET_ENTRY (initials, sw_start_button)
{
	if (initials_enter_timer)
	{
		initials_data[initials_index] = initial_chars[initials_selection];
		if (++initials_index == NUM_INITIALS_ALLOWED)
		{
			(*initials_enter_complete) ();
			initials_stop ();
		}
	}
}

