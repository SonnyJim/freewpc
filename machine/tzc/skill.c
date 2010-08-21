/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

CALLSET_ENTRY (skill, sw_skill_bottom)
{
	set_valid_playfield ();
}

CALLSET_ENTRY (skill, sw_shooter)
{
	/* Because the shooter switch is declared as an 'edge' switch,
	an event is generated on both transitions.  Check the current
	state of the switch to see which transition occurred. */
	if (!in_live_game)
		return;

	if (!switch_poll_logical (SW_SHOOTER))
	{
		sound_send (SND_SHOOTER_PULL);
		leff_restart (LEFF_STROBE_UP);
		timer_restart_free (GID_SHOOTER_SOUND_DEBOUNCE, TIME_3S);
	}
	else
	{
		leff_stop (LEFF_STROBE_UP);
	}
}

