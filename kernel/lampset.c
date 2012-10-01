/*
 * Copyright 2010-2012 by Brian Dominy <brian@oddchange.com>
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
 * Lamp set low-level operators.  Each lamp set is just a bitmap,
 * one bit per resource (lamp/GI string/flasher).  These are
 * optimized according to the maximum number of resources we have.
 */

/* TODO - look at PINIO_NUM_LAMPS */

void lamp_set_zero (lamp_set dst)
{
	memset (dst, 0, sizeof (lamp_set));
}

void lamp_set_copy (lamp_set dst, const lamp_set src)
{
	register U16 *dst1 = (U16 *)dst;
	register U16 *src1 = (U16 *)src;

	dst1[0] = src1[0];
	dst1[1] = src1[1];
	dst1[2] = src1[2];
	dst1[3] = src1[3];
}

void lamp_set_add (lamp_set dst, const lamp_set src)
{
	register U16 *dst1 = (U16 *)dst;
	register U16 *src1 = (U16 *)src;

	dst1[0] |= src1[0];
	dst1[1] |= src1[1];
	dst1[2] |= src1[2];
	dst1[3] |= src1[3];
}

void lamp_set_subtract (lamp_set dst, const lamp_set src)
{
	register U16 *dst1 = (U16 *)dst;
	register U16 *src1 = (U16 *)src;

	dst1[0] &= ~src1[0];
	dst1[1] &= ~src1[1];
	dst1[2] &= ~src1[2];
	dst1[3] &= ~src1[3];
}

bool lamp_set_disjoint (const lamp_set a, const lamp_set b)
{
	register U16 *a1 = (U16 *)a;
	register U16 *b1 = (U16 *)b;
	register U16 x = 0;

	x |= (a1[0] & b1[0]);
	x |= (a1[1] & b1[1]);
	x |= (a1[2] & b1[2]);
	x |= (a1[3] & b1[3]);
	if (x)
		return FALSE;
	return TRUE;
}


bool lamp_set_can_be_added (const lamp_set a, const lamp_set b)
{
	register U16 *a1 = (U16 *)a;
	register U16 *b1 = (U16 *)b;
	register U16 x = 0;

	x |= (~a1[0] & b1[0]);
	x |= (~a1[1] & b1[1]);
	x |= (~a1[2] & b1[2]);
	x |= (~a1[3] & b1[3]);
	if (x)
		return FALSE;
	return TRUE;
}

