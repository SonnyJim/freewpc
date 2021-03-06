/*
 * Copyright 2006-2009 by Brian Dominy <brian@oddchange.com>
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
 * \brief Implement the extra ball buy-in feature.
 */

#include <freewpc.h>

#define BUYIN_TIMEOUT   7  /* No. of seconds until buyin times out */


/** Amount of time left to buy an extra ball during buyin */
U8 buyin_offer_timer;

/** The number of buyins each player has had */
__local__ U8 buyin_count;

/** Display effect that runs during the extra ball buyin */
void buyin_offer_deff (void)
{
#if (MACHINE_DMD == 1)
	dmd_sched_transition (&trans_bitfade_slow);
#else
	seg_sched_transition (&seg_trans_fast_center_out);
#endif
	for (;;)
	{
		dmd_alloc_low_clean ();
		dmd_draw_border (dmd_low_buffer);
		font_render_string_center (&font_term6, 64, 5, "CONTINUE GAME");
#if (MACHINE_DMD == 1)
		sprintf ("%d", buyin_offer_timer);
		font_render_string_left (&font_mono5, 4, 3, sprintf_buffer);
		font_render_string_right (&font_mono5, 123, 3, sprintf_buffer);
#else
		sprintf ("%d SECS", buyin_offer_timer);
		seg_write_row_center (1, sprintf_buffer);
#endif
		dmd_show_low ();
		task_sleep (TIME_500MS);
	}
	deff_exit ();
}


/** Called when the player requests to purchase a buyin. */
void buyin_purchase (void)
{
}


/** Offer to buy an extra ball.  This is called from the game state
 * machine when it detects that a player has played his last ball. */
void buyin_offer (void)
{
#ifdef MACHINE_BUYIN_SWITCH

	/* TODO - validate that a buyin offer is OK; check that the
	buyin limit hasn't been reached */

#ifdef MACHINE_BUYIN_LAMP
	lamp_tristate_flash (MACHINE_BUYIN_LAMP);
#endif

	buyin_offer_timer = BUYIN_TIMEOUT;
	deff_start (DEFF_BUYIN_OFFER);
	do
	{
		task_sleep_sec (1);
		buyin_offer_timer--;
	} while (buyin_offer_timer > 0);
	task_sleep_sec (1);
	deff_stop (DEFF_BUYIN_OFFER);

#ifdef MACHINE_BUYIN_LAMP
	lamp_tristate_off (MACHINE_BUYIN_LAMP);
#endif

#endif /* MACHINE_BUYIN_SWITCH */
}


CALLSET_ENTRY (buyin, start_button_handler)
{
}

CALLSET_ENTRY (buyin, start_player)
{
	buyin_count = 0;
}

