/*
 * Copyright 2005-2009 by Brian Dominy <brian@oddchange.com>
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
 * \brief Manage the alphanumeric segment displays.
 *
 */

#include <freewpc.h>
#include <m6809/math.h>

#define SEGCHAR_ALL       0  /* Light up all segments of the display */
#define SEGCHAR_HORIZ     1  /* Light up all horizontal segments */
#define SEGCHAR_VERT      2  /* Light up all vertical segments */
#define SEGCHAR_STROBE    3  /* Light up horizontal segments gradually */
	#define SEGCHAR_STROBE_COUNT 4
   #define SEG_STROBE0       SEG_TOP
	#define SEG_STROBE1       (SEG_STROBE0 + SEG_UPR_LEFT+SEG_UL_DIAG+SEG_VERT_TOP+SEG_UR_DIAG+SEG_UPR_RIGHT)
	#define SEG_STROBE2       (SEG_STROBE1 + SEG_MID)
	#define SEG_STROBE3       (SEG_STROBE2 + SEG_LWR_LEFT+SEG_LL_DIAG+SEG_VERT_BOT+SEG_LR_DIAG+SEG_LWR_RIGHT)


bool seg_in_transition;

seg_transition_t *seg_transition;

U8 *seg_trans_data_ptr;

/**
 * The segment lookup table.
 * Each entry here gives the segments that should be lit to form a specific
 * character.
 */
const segbits_t seg_table[] = {
	/* Characters in the low end, which are normally not displayable,
	have entries here for miscellaneous graphics characters. */
	[SEGCHAR_ALL] = 0xFFFF & SEG_COMMA & SEG_PERIOD,
	[SEGCHAR_HORIZ] = SEG_TOP+SEG_MID+SEG_BOT,
	[SEGCHAR_VERT] = SEG_LEFT+SEG_VERT+SEG_RIGHT,

	[SEGCHAR_STROBE] = SEG_STROBE0,
		SEG_STROBE1,
		SEG_STROBE2,
		SEG_STROBE3,

	/* The beginning of the ASCII printable characters */
	[' '] = 0,
	['%'] = SEG_UPR_LEFT+SEG_UR_DIAG+SEG_LL_DIAG+SEG_LWR_RIGHT,
	['('] = SEG_UR_DIAG+SEG_LR_DIAG,
	[')'] = SEG_UL_DIAG+SEG_LL_DIAG,
	['*'] = SEG_UL_DIAG+SEG_UR_DIAG+SEG_LL_DIAG+SEG_LR_DIAG+SEG_MID+SEG_VERT,
	['+'] = SEG_VERT+SEG_MID,
	['-'] = SEG_MID,
	['/'] = SEG_UR_DIAG+SEG_LL_DIAG,
   ['$'] = SEG_TOP+SEG_UPR_LEFT+SEG_MID+SEG_LWR_RIGHT+SEG_BOT+SEG_VERT,
   ['0'] = SEG_TOP+SEG_RIGHT+SEG_BOT+SEG_LEFT,
   ['1'] = SEG_RIGHT,
   ['2'] = SEG_TOP+SEG_UPR_RIGHT+SEG_MID+SEG_LWR_LEFT+SEG_BOT,
   ['3'] = SEG_TOP+SEG_MID+SEG_BOT+SEG_RIGHT,
   ['4'] = SEG_UPR_LEFT+SEG_MID+SEG_RIGHT,
   ['5'] = SEG_TOP+SEG_UPR_LEFT+SEG_MID+SEG_LWR_RIGHT+SEG_BOT,
   ['6'] = SEG_TOP+SEG_LEFT+SEG_BOT+SEG_LWR_RIGHT+SEG_MID,
   ['7'] = SEG_TOP+SEG_RIGHT,
   ['8'] = SEG_TOP+SEG_MID+SEG_BOT+SEG_LEFT+SEG_RIGHT,
   ['9'] = SEG_TOP+SEG_MID+SEG_BOT+SEG_UPR_LEFT+SEG_RIGHT,
	['.'] = SEG_PERIOD,
	[','] = SEG_COMMA,
	[':'] = SEG_PERIOD,
	['<'] = SEG_UR_DIAG+SEG_LR_DIAG,
	['>'] = SEG_UL_DIAG+SEG_LL_DIAG,
   ['A'] = SEG_LEFT+SEG_TOP+SEG_MID+SEG_RIGHT,
   ['B'] = SEG_TOP+SEG_BOT+SEG_MID_RIGHT+SEG_VERT+SEG_RIGHT,
   ['C'] = SEG_LEFT+SEG_TOP+SEG_BOT,
   ['D'] = SEG_TOP+SEG_BOT+SEG_VERT+SEG_RIGHT,
   ['E'] = SEG_LEFT+SEG_TOP+SEG_MID+SEG_BOT,
   ['F'] = SEG_LEFT+SEG_TOP+SEG_MID,
   ['G'] = SEG_TOP+SEG_LEFT+SEG_BOT+SEG_LWR_RIGHT+SEG_MID_RIGHT,
	['H'] = SEG_LEFT+SEG_RIGHT+SEG_MID,
	['I'] = SEG_TOP+SEG_VERT+SEG_BOT,
	['J'] = SEG_RIGHT+SEG_BOT+SEG_LWR_LEFT,
	['K'] = SEG_LEFT+SEG_UR_DIAG+SEG_LR_DIAG+SEG_MID_LEFT,
	['L'] = SEG_LEFT+SEG_BOT,
	['M'] = SEG_LEFT+SEG_UL_DIAG+SEG_UR_DIAG+SEG_RIGHT,
	['N'] = SEG_LEFT+SEG_UL_DIAG+SEG_LR_DIAG+SEG_RIGHT,
   ['O'] = SEG_TOP+SEG_RIGHT+SEG_BOT+SEG_LEFT,
	['P'] = SEG_LEFT+SEG_TOP+SEG_UPR_RIGHT+SEG_MID,
   ['Q'] = SEG_TOP+SEG_RIGHT+SEG_BOT+SEG_LEFT+SEG_LR_DIAG,
	['R'] = SEG_LEFT+SEG_TOP+SEG_UPR_RIGHT+SEG_MID+SEG_LR_DIAG,
   ['S'] = SEG_TOP+SEG_UPR_LEFT+SEG_MID+SEG_LWR_RIGHT+SEG_BOT,
	['T'] = SEG_TOP+SEG_VERT,
	['U'] = SEG_LEFT+SEG_BOT+SEG_RIGHT,
	['V'] = SEG_LEFT+SEG_LL_DIAG+SEG_UR_DIAG,
	['W'] = SEG_LEFT+SEG_BOT+SEG_VERT_BOT+SEG_RIGHT,
	['X'] = SEG_UL_DIAG+SEG_UR_DIAG+SEG_LL_DIAG+SEG_LR_DIAG,
	['Y'] = SEG_UL_DIAG+SEG_UR_DIAG+SEG_VERT_BOT,
   ['Z'] = SEG_TOP+SEG_UR_DIAG+SEG_LL_DIAG+SEG_BOT,
	['_'] = SEG_BOT,
};


seg_page_t seg_pages[SEG_ALLOC_PAGES+SEG_FIXED_PAGES];

__fastram__ seg_page_t *seg_visible_page;

__fastram__ seg_page_t *seg_writable_page;

seg_page_t *seg_secondary;

U8 seg_alloc_pageid;


/**
 * Handle the realtime update of the segment displays.
 *
 * This function is invoked every 1ms to latch a value
 * for one column -- 2 characters, one on top, one on bottom.
 * It thus takes 16ms to strobe the entire display.
 */
void seg_rtt (void)
{
	U8 col;
	segbits_t *valp;
	
	col = get_sys_time () & 0x0F;
	writeb (WPC_ALPHA_POS, col);

	valp = &(*seg_visible_page)[0][col];
	writew (WPC_ALPHA_ROW1, *valp);
	writew (WPC_ALPHA_ROW2, *(valp + 16));
}


void seg_alloc (void)
{
	seg_writable_page = seg_pages + seg_alloc_pageid;
	seg_secondary = seg_writable_page + 1;
	seg_alloc_pageid += 2;
	seg_alloc_pageid &= (SEG_ALLOC_PAGES-1);
}


void seg_map (U8 page)
{
	seg_writable_page = seg_pages + page;
}

void seg_flip_low_high (void)
{
	seg_page_t *tmp = seg_writable_page;
	seg_writable_page = seg_secondary;
	seg_secondary = tmp;
}

static void seg_show_from (seg_page_t *page_ptr)
{
	if (unlikely (seg_transition))
		seg_do_transition ();
	seg_visible_page = page_ptr;
}


void seg_show (void)
{
	seg_show_from (seg_writable_page);
}


void seg_show_page (U8 page)
{
	seg_show_from (seg_pages + page);
}


void seg_show_other (void)
{
	if (seg_visible_page == seg_writable_page)
		seg_show_from (seg_writable_page + 1);
	else
		seg_show_from (seg_writable_page);
}


void seg_copy_low_to_high (void)
{
	memcpy (seg_writable_page+1, seg_writable_page, sizeof (seg_page_t));
}


seg_page_t *seg_get_page_pointer (U8 page)
{
	return seg_pages + page;
}


/**
 * Translate an ASCII character into segments.
 *
 * If an invalid, high-end character is given, display it as a
 * dash to avoid table lookup.
 */
static segbits_t seg_translate_char (char c)
{
	if (likely (c <= '_'))
		return seg_table[(U8)c];
	else
		return SEG_MID;
}


/**
 * Write a single character to one alphanumeric display slot.
 *
 * SA points to the display memory.
 * C is the character to be displayed there.
 */
segbits_t *seg_write_char (segbits_t *sa, char c)
{
	if (c == '\0')
		return sa;
	else if ((c == '.' || c == ':') && (!(sa[-1] & SEG_PERIOD)))
		*--sa |= SEG_PERIOD;
	else if (c == ',')
		*--sa |= SEG_COMMA;
	else
		*sa = seg_translate_char (c);
	return sa+1;
}


static bool seg_addr_valid (void *sa)
{
	return ((sa >= (void *)seg_pages) &&
		(sa < (void *)seg_pages + sizeof (seg_pages)));
}


#if 0
void seg_write (segbits_t *addr, U16 *data, U8 len)
{
	while (len-- > 0)
		*addr++ = *data++;
}
#endif


/**
 * Write a string to the alphanumeric display, starting at the
 * given row and column.
 */
void seg_write_string (U8 row, U8 col, const char *s)
{
	segbits_t *addr;

	/* Sanity checking */
	if (row >= SEG_SECTIONS)
		return;
	if (col >= SEG_SECTION_SIZE)
		return;

	/* Get a pointer to the display memory */
	addr = &(*seg_writable_page)[row][col];

	/* For each character in the string, write it and advance the
	cursor. */
	while ((*s != '\0') && seg_addr_valid (addr))
		addr = seg_write_char (addr, *s++);
}


/**
 * Calculate the length of a string to be written to the
 * display.  This is used to determine positioning for
 * centering and right justification.
 */
U8 seg_strlen (const char *s)
{
	U8 n = 0;
	while (*s++ != '\0')
		if ((*s != '.') && (*s != ','))
			n++;
	return n;
}


/**
 * Erase the current page.
 */
void seg_erase (void)
{
	segbits_t *addr = &(*seg_writable_page)[0][0];
	memset (addr, 0, sizeof(segbits_t) * SEG_SECTIONS * SEG_SECTION_SIZE);
}


/**
 * Allocate a clean page for drawing.
 */
void seg_alloc_clean (void)
{
	seg_alloc ();
	seg_erase ();
}


/**
 * Execute a segment-style transition effect.
 */
void seg_do_transition (void)
{
	seg_page_t *seg_final_page;
	seg_page_t *tmp;
	U8 iteration;

	dbprintf ("seg_do_transition\n");

	/* Save pointer to the final page -- that which will ultimately
	be displayed.  This is the source page for all updates during
	the transition. */
	seg_final_page = seg_writable_page;

	/* Allocate a new page, kept in seg_writable_page, and initialize
	it with the current visible page.  This is the destination page
	for all updates. */
	seg_alloc ();
	memcpy (seg_writable_page, seg_visible_page, sizeof (seg_page_t));

	/* Invoke the constructor */
	if (seg_transition->init)
		seg_transition->init ();

	iteration = 0;
	while (seg_in_transition && iteration < 64)
	{
		dbprintf ("seg_trans: iteration %d\n", iteration);
		/* Delay */
		task_sleep (seg_transition->delay);

		/* Do a partial update */
		seg_in_transition = seg_transition->update (seg_final_page, iteration);
		iteration++;

		/* Make the current destination page visible, and allocate a
		new one for the next iteration.  This can be done by a swap
		of the old page and new page pointers */
		tmp = seg_visible_page;
		seg_visible_page = seg_writable_page;
		seg_writable_page = tmp;
	}
	seg_transition = NULL;
}


void seg_sched_transition (seg_transition_t *trans)
{
	seg_transition = trans;
	seg_in_transition = TRUE;
}

void seg_reset_transition (void)
{
	seg_transition = NULL;
	seg_in_transition = FALSE;
}


/**
 * Initialize the segment displays.
 */
void seg_init (void)
{
	memset (seg_pages, 0, sizeof (seg_pages));
	seg_show_page (0);
	seg_alloc_pageid = 0;
	seg_transition = NULL;
}

