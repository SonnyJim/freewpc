/*
 * Copyright 2006-2012 by Brian Dominy <brian@oddchange.com>
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
 * \brief Lamp management functions
 *
 * Theory of operation:
 *
 * The WPC lamp circuit is setup as an 8x8 matrix.  A column strobe output
 * selects which group of 8 lamps can be on at any time.  A separate row
 * output register contains the on/off values for each of the 8 lamps in
 * that column.
 *
 * At IRQ time, the column strobe is repeatedly switched among the 8
 * columns, at a rate of every 2ms.  Thus, the effective time to redraw
 * the entire lamp matrix is 16ms.
 *
 * The lowest level function calculates the row output by overlaying several
 * different values.  There are 3 different states for each lamp: the
 * default state, the secondary state, and the lightshow state -- in that
 * order.  It is possible that there is no secondary/lightshow associated
 * with a lamp, in which case those values are not applied to the output.
 * So for each those, a lamp must be allocated/freed.
 *
 * In mathematical form, the row output is defined as:
 *
 * 	(((default & secondary_mask) | secondary) & lightshow_mask) | lightshow
 *
 * A mask bit of 0 discards the previous state of the lamp, and allows the
 * new state, either 0 or 1 to come through.
 *
 * On top of this, the default state of a lamp can be set to 'flashing'.
 * A separate bit matrix tracks all lamps that are in flashing mode.
 * A separate realtime function, which runs much more slowly, toggles the
 * default lamp states to create the flashing.
 *
 * There are many boolean functions defined for testing lamp states as well
 * as setting them.  In simple cases, the default lamps can be used as
 * boolean variables for indicating game state.
 *
 * Default lamp values are also tracked per-player.  They are guaranteed to be
 * all off at the start of a player's game, and they are saved/restored in
 * multiplayer games.  The secondary/lightshow values are transient and are not
 * saved this way.
 *
 */

#include <freewpc.h>

__fastram__ lamp_set lamp_matrix;

lamp_set lamp_flash_matrix;

__fastram__ lamp_set lamp_flash_matrix_now;

U8 bit_matrix[BITS_TO_BYTES (MAX_FLAGS)];

U8 global_bits[BITS_TO_BYTES (MAX_GLOBAL_FLAGS)];

#ifdef CONFIG_LAMP_STROBE16
typedef U16 lamp_strobe_t;
#else
typedef U8 lamp_strobe_t;
#endif
__fastram__ lamp_strobe_t lamp_strobe_mask;

__fastram__ U8 lamp_strobe_column;

__fastram__ U8 lamp_power_timer;

U8 lamp_power_level;

U16 lamp_power_idle_timer;

extern lamp_set leff_free_set;
extern lamp_set leff_data_set;


/** Initialize the lamp subsystem at startup. */
void lamp_init (void)
{
	/* Clear all lamps/flags */
	lamp_set_zero (lamp_matrix);
	lamp_set_zero (lamp_flash_matrix);
	lamp_set_zero (lamp_flash_matrix_now);
	memset (bit_matrix, 0, sizeof (bit_matrix));
	memset (global_bits, 0, sizeof (global_bits));
	lamp_strobe_mask = 0x1;
	lamp_strobe_column = 0;
	lamp_power_timer = 0;
	lamp_power_level = 0;
	lamp_power_idle_timer = 0;
}


/** Runs periodically to invert any lamps in the flashing state.
 * (This is hard realtime now; it could probably be dropped in
 * priority, though.) */
/* RTT(name=lamp_flash_rtt freq=128) */
void lamp_flash_rtt (void)
{
	U16 *lamp_matrix_words = (U16 *)lamp_flash_matrix_now;
	U16 *lamp_flash_matrix_words = (U16 *)lamp_flash_matrix;

	lamp_matrix_words[0] ^= lamp_flash_matrix_words[0];
#if (PINIO_NUM_LAMPS > 16)
	lamp_matrix_words[1] ^= lamp_flash_matrix_words[1];
#endif
#if (PINIO_NUM_LAMPS > 32)
	lamp_matrix_words[2] ^= lamp_flash_matrix_words[2];
#endif
#if (PINIO_NUM_LAMPS > 48)
	lamp_matrix_words[3] ^= lamp_flash_matrix_words[3];
#endif
#if (PINIO_NUM_LAMPS > 64)
	lamp_matrix_words[4] ^= lamp_flash_matrix_words[4];
#endif
}


/**
 * Set the lamp power saver level.
 */
void lamp_power_set (U8 level)
{
	dbprintf ("Lamp power level = %d\n", level);
	disable_interrupts ();
	lamp_power_timer = lamp_power_level = level;
	enable_interrupts ();
}


/* Basic bit manipulation routines.  These will
always use the WPC shift hardware and may not be
optimal.  The bitarray macros are preferred. */

#ifdef CONFIG_SINGLE_BIT_SET_ARRAY
U8 single_bit_set_array[8] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80 };
#endif

void bit_on (bitset matrix, U8 bit)
{
	bitarray_set (matrix, bit);
}

void bit_off (bitset matrix, U8 bit)
{
	bitarray_clear (matrix, bit);
}

void bit_toggle (bitset matrix, U8 bit)
{
	bitarray_toggle (matrix, bit);
}

bool bit_test (const_bitset matrix, U8 bit)
{
	return bitarray_test (matrix, bit);
}


/** Return nonzero if all bits in a matrix are set. */
bool bit_test_all_on (const_bitset matrix)
{
	U8 product = (matrix[0] & matrix[1]
		& matrix[2] & matrix[3]
		& matrix[4] & matrix[5]
		& matrix[6] & matrix[7]);
	return (product == 0xFF);
}


/** Return nonzero if all bits in a matrix are clear. */
bool bit_test_all_off (const_bitset matrix)
{
	U8 sum = (matrix[0] | matrix[1]
		| matrix[2] | matrix[3]
		| matrix[4] | matrix[5]
		| matrix[6] | matrix[7]);
	return (sum == 0);
}


__pure__ U8 *matrix_lookup (lamp_matrix_id_t id)
{
	switch (id)
	{
		case LMX_DEFAULT:
			return lamp_matrix;
		case LMX_FLASH:
			return lamp_flash_matrix;
		case LMX_EFFECT1_ALLOC:
		case LMX_EFFECT2_ALLOC:
			return leff_free_set;
		case LMX_EFFECT1_LAMPS:
		case LMX_EFFECT2_LAMPS:
			return leff_data_set;
	}
	fatal (ERR_INVALID_MATRIX);
}

void lamp_set_on (lamp_set lset)
{
	memset (lset, 0xFF, NUM_LAMP_COLS);
}


/*
 * Lamp manipulation routines
 *
 * These control the main lamp matrix and also the flash
 * matrix.  They can be modified at any time without the
 * need for allocation.
 *
 */
void lamp_on (lampnum_t lamp)
{
	bit_on (lamp_matrix, lamp);
}

void lamp_off (lampnum_t lamp)
{
	bit_off (lamp_matrix, lamp);
}

void lamp_toggle (lampnum_t lamp)
{
	bit_toggle (lamp_matrix, lamp);
}

bool lamp_test (lampnum_t lamp)
{
	return bit_test (lamp_matrix, lamp);
}

bool lamp_test_off (lampnum_t lamp)
{
	/* Note: gcc6809 generates inefficient code for this.
	This would be better:
	   if (rc) return (0) else return (rc);
	gcc is insisting on returning 0/1, when zero/nonzero is
	sufficient.
    */
#if 0
	bool t = bit_test (lamp_matrix, lamp);
	return t ? 0 : t;
#endif
	return !bit_test (lamp_matrix, lamp);
}


void lamp_flash_on (lampnum_t lamp)
{
	if (!bit_test (lamp_flash_matrix, lamp))
	{
		/* Enable flashing on this lamp */
		bit_on (lamp_flash_matrix, lamp);

		/* Set the initial flash state of the lamp to match that of all
		other lamps that are flashing.  If any of the flashing lamps
		are on now, then this one should be on, too.  Otherwise, leave
		it off. */
		disable_interrupts ();
		if (!bit_test_all_off (lamp_flash_matrix_now))
			bit_on (lamp_flash_matrix_now, lamp);
		else
			bit_off (lamp_flash_matrix_now, lamp);
		enable_interrupts ();
	}
}

void lamp_flash_off (lampnum_t lamp)
{
	bit_off (lamp_flash_matrix, lamp);
	bit_off (lamp_flash_matrix_now, lamp);
}

bool lamp_flash_test (lampnum_t lamp)
{
	return bit_test (lamp_flash_matrix, lamp);
}


/*
 * lamp_all_on / lamp_all_off are optimized and should be used
 * if all lamps are affected, rather than setting them one at
 * a time.
 */

void lamp_all_on (void)
{
	disable_interrupts ();
	lamp_set_zero (lamp_flash_matrix);
	enable_interrupts ();
	lamp_set_on (lamp_matrix);
}


void lamp_all_off (void)
{
	disable_interrupts ();
	lamp_set_zero (lamp_flash_matrix_now);
	lamp_set_zero (lamp_flash_matrix);
	lamp_set_zero (leff_data_set);
	enable_interrupts ();
	lamp_set_zero (lamp_matrix);
}


/*
 * The leff_ functions below are used to set/clear/toggle/test
 * bits from a lamp effect function.  Otherwise they work
 * identically to the lamp_ versions.
 */
void leff_on (lampnum_t lamp)
{
#ifdef PARANOID
	if (bit_test (leff_free_set, lamp))
	{
		dbprintf ("unallocated lamp on %d\n", lamp);
		return;
	}
#endif
	bit_on (leff_data_set, lamp);
}


void leff_off (lampnum_t lamp)
{
	bit_off (leff_data_set, lamp);
}


void leff_toggle (lampnum_t lamp)
{
#ifdef PARANOID
	if (bit_test (leff_free_set, lamp))
	{
		dbprintf ("unallocated lamp on %d\n", lamp);
		return;
	}
#endif
	bit_toggle (leff_data_set, lamp);
}


bool leff_test (lampnum_t lamp)
{
	return bit_test (leff_data_set, lamp);
}


/**
 * When attract mode begins, start the timer to reduce lamp levels.
 * The "GI POWER SAVER" adjustment controls the number of minutes
 * until this kicks in.  If set to zero, then the feature is disabled.
 */
CALLSET_ENTRY (lamp, amode_start)
{
	lamp_power_idle_timer = 60UL * system_config.gi_power_saver;
	dbprintf ("Lamp power saver set to %ld\n", lamp_power_idle_timer);
}


/**
 * When a game is started, or in test mode, cancel the power timer
 * and return to full power for the lamps.
 */
CALLSET_ENTRY (lamp, start_game, test_start)
{
	lamp_power_idle_timer = 0;
	lamp_power_set (0);
}


/**
 * Periodically update the timer and see if reduced power should be
 * enabled now.
 */
CALLSET_ENTRY (lamp, idle_every_second)
{
	if (lamp_power_idle_timer > 0) /* is the timer running? */
	{
		if (--lamp_power_idle_timer == 0) /* yes, decrement it: has it expired? */
		{
			/* Yes, set the reduced power level.  The argument says how many
			IRQs for which no lamps will be turned on.  The larger the value,
			the dimmer the lamps. */
			lamp_power_set ((system_config.power_saver_level >= 6) ? 2 : 3);
		}
	}
}

