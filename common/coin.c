/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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
 * \brief Coin switch handlers
 *
 * This module processes coin switches to increment the number of credits
 * available.  It also contains functions for displaying credits to the
 * player in the right format, and for updating the start button lamp
 * to indicate that a game can be started.
 */

#include <freewpc.h>

/** The number of credits */
__nvram__ volatile U8 credit_count;

/** The number of additional units purchased, less than the value
 * of one credit */
__nvram__ volatile U8 unit_count;


void coin_reset (void)
{
	credit_count = unit_count = 0;
}


__nvram__ U8 coin_csum;
const struct area_csum coin_csum_info = {
	.area = (U8 *)&credit_count,
	.length = sizeof (credit_count) + sizeof (unit_count),
	.csum = &coin_csum,
	.reset = coin_reset,
#ifdef HAVE_PAGING
	.reset_page = COMMON_PAGE,
#endif
};


void credits_render (void)
{
#ifdef FREE_ONLY
	sprintf ("FREE ONLY");
#else
	if (price_config.free_play)
		sprintf ("FREE PLAY");
	else
	{
		if (unit_count != 0)
		{
			if (credit_count == 0)
			{
				sprintf ("%d/%d CREDIT", unit_count, price_config.units_per_credit);
			}
			else
			{
				sprintf ("%d %d/%d CREDITS",
					credit_count, unit_count, price_config.units_per_credit);
			}
		}
		else
		{
			if (credit_count == 1)
				sprintf ("%d CREDIT", credit_count);
			else
				sprintf ("%d CREDITS", credit_count);
		}
	}
#endif
}


void credits_draw (void)
{
	dmd_alloc_low_clean ();
	dmd_alloc_high ();

	credits_render ();
	font_render_string_center (&font_fixed6, 64, 9, sprintf_buffer);
	dmd_copy_low_to_high ();

	if (!has_credits_p ())
	{
		sprintf ("INSERT COINS");
	}
	else
	{
		sprintf ("PRESS START");
	}
	font_render_string_center (&font_fixed6, 64, 22, sprintf_buffer);

	deff_swap_low_high (in_live_game ? 13 : 21, 2 * TIME_100MS);
}


void credits_deff (void)
{
	credits_draw ();
	deff_delay_and_exit (TIME_100MS * 10);
}


/** Update the start button lamp.  It will flash when a new game
 * can be started, or be solid on during a game.   It will be
 * off only when there are no credits. */
void lamp_start_update (void)
{
#ifdef MACHINE_START_LAMP
	/* TODO : start button is flashing very early after reset, before
	 * a game can actually be started. */
	if (has_credits_p ())
	{
		if (!in_game)
			lamp_tristate_flash (MACHINE_START_LAMP);
		else
			lamp_tristate_on (MACHINE_START_LAMP);
	}
	else
		lamp_tristate_off (MACHINE_START_LAMP);
#endif
}


static void increment_credit_count (void)
{
	if (credit_count >= price_config.max_credits)
		return;

#ifndef FREE_ONLY
	credit_count++;
	lamp_start_update ();
#endif

#ifdef MACHINE_ADD_CREDIT_SOUND
	sound_send (MACHINE_ADD_CREDIT_SOUND);
#endif
#ifdef LEFF_FLASH_ALL
	leff_start (LEFF_FLASH_ALL);
#endif
#ifdef DEFF_CREDITS
	deff_restart (DEFF_CREDITS);
#endif
}


void add_credit (void)
{
	wpc_nvram_get ();
	increment_credit_count ();
	csum_area_update (&coin_csum_info);
	wpc_nvram_put ();
}


bool has_credits_p (void)
{
#ifndef FREE_ONLY
	if (price_config.free_play)
		return (TRUE);
	else
		return (credit_count > 0);
#else
	return (TRUE);
#endif
}


void remove_credit (void)
{
#ifndef FREE_ONLY
	if (credit_count > 0)
	{
		wpc_nvram_get ();
		credit_count--;
		csum_area_update (&coin_csum_info);
		wpc_nvram_put ();

		lamp_start_update ();
	}
#endif
}


void add_units (U8 n)
{
	if (credit_count >= price_config.max_credits)
		return;

	wpc_nvram_add (unit_count, n);
	if (unit_count >= price_config.units_per_credit)
	{
		while (unit_count >= price_config.units_per_credit)
		{
			wpc_nvram_subtract (unit_count, price_config.units_per_credit);
			add_credit ();
			audit_increment (&system_audits.paid_credits);
		}
	}
	else
	{
#ifdef MACHINE_ADD_COIN_SOUND
		sound_send (MACHINE_ADD_COIN_SOUND);
#endif
		deff_restart (DEFF_CREDITS);
	}
	csum_area_update (&coin_csum_info);
	wpc_nvram_put ();
}


void coin_insert_deff (void) __taskentry__
{
	register int8_t z = 4;
	while (--z > 0)
	{
		dmd_invert_page (dmd_low_buffer);
		task_sleep (TIME_100MS * 2);
	}
	deff_exit ();
}

static void do_coin (uint8_t slot)
{
	add_units (price_config.slot_values[slot]);
	audit_increment (&system_audits.coins_added[slot]);
}

CALLSET_ENTRY (coin, sw_left_coin)
{
	do_coin (0);
}

CALLSET_ENTRY (coin, sw_center_coin)
{
	do_coin (1);
}

CALLSET_ENTRY (coin, sw_right_coin)
{
	do_coin (2);
}

CALLSET_ENTRY (coin, sw_fourth_coin)
{
	do_coin (3);
}


void credits_clear (void)
{
	wpc_nvram_get ();
	credit_count = 0;
	unit_count = 0;
	csum_area_update (&coin_csum_info);
	wpc_nvram_put ();
}


// lamp_start_update (); at coin_init ();
