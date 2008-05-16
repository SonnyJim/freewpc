/*
 * Copyright 2008 by Brian Dominy <brian@oddchange.com>
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

extern __fastram__ U8 tick_count;


/**
 * Handles an invalid interrupt (NMI, SWI, etc.)
 */
__interrupt__ void wpcs_invalid_interrupt (void)
{
	fatal (1);
}


/**
 * Handles the periodic interrupt on the FIRQ.
 * This interrupt occurs at 5.5khz.
 *
 * 8-bit DAC samples are encoded at 11khz and thus 2 must be
 * sent.
 *
 * 1-bit CVSD samples are encoded at 22khz and thus 4 must be
 * sent.
 *
 * This routine only has about 350 cycles to get the job done
 * before another interrupt will occur.  Yikes!
 */
__interrupt__ void wpcs_periodic_interrupt (void)
{
	m6809_firq_save_regs ();

	tick_count++;
	host_send ();
	host_write (0xF0);
	fm_timer_restart (1);

	m6809_firq_restore_regs ();
}


/**
 * Handles an interrupt from the host, signalling that there
 * is data to be read.  This comes on the IRQ line.
 */
__interrupt__ void wpcs_host_interrupt (void)
{
	host_receive ();
}

