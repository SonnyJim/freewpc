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

/**
 * \file
 * \brief Render full-sized DMD frames.
 *
 * This module provides an API for drawing full-screen 128x32 4-color images
 * to the dot matrix that have been compressed into the FreeWPC Image Format
 * (FIF).  The fiftool utility can be used to generate FIFs from PGMs, which
 * are supported by most graphics programs.  The key feature of the FIF is
 * simple compression, since WPC provides limited space and has limited
 * processing cycles.
 */

#include <freewpc.h>
#include <xbmprog.h>


U8 frame_repeat_count;

U8 frame_repeat_value;


static __attribute__((noinline))
const U8 *frame_copy_raw (const U8 *framedata)
{
	dmd_copy_page (dmd_low_buffer, (const dmd_buffer_t)framedata);
	return framedata + (dmd_high_buffer - dmd_low_buffer);
}


static __attribute__((noinline))
const U8 *frame_copy_rle (const U8 *framedata)
{
	register U8 *dbuf = dmd_low_buffer;
	register U8 c;

	do {
		c = *framedata++;
		if (c == XBMPROG_RLE_SKIP)
		{
			/* The 'skip' flag indicates an RLE sequence where
			the data byte is assumed to be zero.  The zero byte 
			is not present in the stream.  The zero case occurs 
			frequently, and is thus given special treatment. */
			frame_repeat_count = *framedata++;

			while (frame_repeat_count >= 4)
			{
				*dbuf++ = 0;
				*dbuf++ = 0;
				*dbuf++ = 0;
				*dbuf++ = 0;
				frame_repeat_count -= 4;
			}

			while (frame_repeat_count != 0)
			{
				*dbuf++ = 0;
				frame_repeat_count--;
			}
		}
		else if (c == XBMPROG_RLE_REPEAT)
		{
			/* The 'repeat' flag is the usual RLE case and can
			support a sequence of any byte value. */
			frame_repeat_value = *framedata++; /* data */
			frame_repeat_count = *framedata++; /* count */
			/* TODO - use word copies if possible */
			do {
				*dbuf++ = frame_repeat_value;
			} while (--frame_repeat_count != 0);
		}
		else
			/* Unrecognized flags are interpreted as literals.
			Note that a literal value that matches a flag value
			above will need to be encoded as an RLE sequence of
			1, since no escape character is defined. */
			*dbuf++ = c;
	} while (unlikely (dbuf < dmd_high_buffer));
	return framedata;
}


static __attribute__((noinline))
const U8 *frame_xor_rle (const U8 *framedata)
{
	register U8 *dbuf = dmd_low_buffer;
	register U8 c;

	do {
		c = *framedata++;
		if (c == XBMPROG_RLE_SKIP)
		{
			dbuf += *framedata++;
		}
		else if (c == XBMPROG_RLE_REPEAT)
		{
			frame_repeat_value = *framedata++; /* data */
			frame_repeat_count = *framedata++; /* count */
			do {
				*dbuf++ ^= frame_repeat_value;
				--frame_repeat_value;
			} while (frame_repeat_value != 0);
		}
		else
			*dbuf++ ^= c;
	} while (unlikely (dbuf < dmd_high_buffer));
	return framedata;
}


/** An internal function to decompress a single bitplane
 * into the low-mapped page buffer. */
static const U8 *dmd_decompress_bitplane (const U8 *framedata)
{
	U8 method;

	/* TODO - remove this and force all callers of the
	function to do the appropriate save/restore. */
	wpc_push_page (PRG_PAGE);

	/* The first byte of a compressed bitplane is a 'method', which
	says the overlap manner in which the image has been
	compressed. */
	method = *framedata++;
	switch (method)
	{
		case XBMPROG_METHOD_RAW:
			/* In the 'raw' method, no compression was done at
			all.  The following 512 bytes are copied verbatim to
			the display buffer. */
			framedata = frame_copy_raw (framedata);
			break;

		case XBMPROG_METHOD_RLE:
			/* In the 'run-length encoding (RLE)' method,
			certain long sequences of the same byte are replaced
			by a flag, the byte, and a count. */
			framedata = frame_copy_rle (framedata);
			break;

		case XBMPROG_METHOD_RLE_DELTA:
			/* The RLE delta method is almost identical to the
			RLE method above, but the input stream is overlaid on
			top of the existing image data, using XOR operations
			instead of simple assignment.  This is useful for animations
			in which a subsequent frame is quite similar to its
			predecessor. */
			framedata = frame_xor_rle (framedata);
			break;
	}

	wpc_pop_page ();
	return framedata;
}


/** Draws a FreeWPC formatted image (FIF), which is just a
 * compressed XBMPROG.  The image header says whether or not
 * it is 1-bit or 2-bit. */
const U8 *dmd_draw_fif1 (const U8 *fif)
{
	U8 depth;

#ifdef CONFIG_UI
	extern void ui_clear_dmd_text (int);
	ui_clear_dmd_text (0);
#endif

	/* The first byte of the FIF format is the depth, which
	 * indicates if the frame has 2 or 4 colors. */
	wpc_push_page (PRG_PAGE);
	depth = *fif++;
	wpc_pop_page ();

	/* Draw the frame(s) */
	fif = dmd_decompress_bitplane (fif);
	if (depth == 2)
	{
		task_yield ();
		dmd_flip_low_high ();
		fif = dmd_decompress_bitplane (fif);
		dmd_flip_low_high ();
	}
	return fif;
}


struct faf
{
	U8 depth;
	U8 len;
	U8 *fifs[0];
};

/** Execute a FreeWPC animation, which is a series of
consecutive FIFs.  An animation is a sequence of FIFs,
with a leading header. */
void dmd_draw_faf1 (struct faf *faf, task_ticks_t delay)
{
	U8 n;
	wpc_push_page (PRG_PAGE);
	for (n=0; n < faf->len; n++)
		dmd_draw_fif1 (faf->fifs[n]);
	wpc_pop_page ();
}

