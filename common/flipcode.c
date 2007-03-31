/*
 * Copyright 2007 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief A generic flipper code recognizer
 */

#include <freewpc.h>

#ifndef MAX_FLIPCODE_CHARS
#define MAX_FLIPCODE_CHARS 4
#endif

#ifndef FLIPCODE_LIST
#define FLIPCODE_LIST { { 3, 1, 2, 3 }, { 3, 1, 1, 1 } }
#define FLIPCODE_HANDLERS { flipcode_default_1, flipcode_default_2 }

void flipcode_default_1 (void)
{
	dbprintf ("Flipper code 1\n");
}

void flipcode_default_2 (void)
{
	dbprintf ("Flipper code 2\n");
}

#endif


//#define DEBUG_FLIPCODE


U8 flipcode_chars[MAX_FLIPCODE_CHARS];

U8 flipcode_pos;

U8 flipcode_value;

const void (*flipcode_handler[]) (void) = FLIPCODE_HANDLERS;

const char flipcodes[][MAX_FLIPCODE_CHARS+1] = FLIPCODE_LIST;


void flipcode_reset (void)
{
	flipcode_pos = 0;
	flipcode_value = 0;
	task_kill_gid (GID_FLIPCODE_ACTIVE);
}

void flipcode_active_task (void)
{
#ifdef DEBUG_FLIPCODE
	dbprintf ("flipcode pos %d value %d\n",
		flipcode_pos, flipcode_value);
#endif
	task_sleep_sec (3);
	flipcode_reset ();
	task_exit ();
}

void flipcode_test (void)
{
	U8 code;
	U8 i;

	for (code=0; code < sizeof (flipcodes) / (MAX_FLIPCODE_CHARS+1); code++)
	{
#ifdef DEBUG_FLIPCODE
		dbprintf ("Testing against code %d:\n", code);
#endif
		if (flipcodes[code][0] != flipcode_pos)
		{
#ifdef DEBUG_FLIPCODE
		dbprintf ("Length was %d, wanted %d\n",
			flipcode_pos, flipcodes[code][0]);
#endif
			continue;
		}

		for (i=0; i < flipcode_pos; i++)
			if (flipcodes[code][i+1] != flipcode_chars[i])
			{
#ifdef DEBUG_FLIPCODE
				dbprintf ("Failed at position %d\n", i);
#endif
				continue;
			}

		/* Match */
#ifdef DEBUG_FLIPCODE
		dbprintf ("Pass!\n");
#endif
		(*flipcode_handler[code]) ();
		return;
	}
}

void flipcode_advance_char (void)
{
	flipcode_value++;
	task_recreate_gid (GID_FLIPCODE_ACTIVE, flipcode_active_task);
}

void flipcode_lock_char (void)
{
	if (flipcode_pos < MAX_FLIPCODE_CHARS)
	{
		flipcode_chars[flipcode_pos] = flipcode_value;
		flipcode_value = 0;
		++flipcode_pos;
		task_recreate_gid (GID_FLIPCODE_ACTIVE, flipcode_active_task);
	}
}


CALLSET_ENTRY (flipcode, sw_l_l_flipper_button)
{
	if (deff_get_active () != DEFF_AMODE)
		return;
	flipcode_advance_char ();
}

CALLSET_ENTRY (flipcode, sw_l_r_flipper_button)
{
	if (deff_get_active () != DEFF_AMODE)
		return;
	flipcode_lock_char ();
	flipcode_test ();
}


CALLSET_ENTRY (flipcode, amode_start)
{
	flipcode_reset ();
}
