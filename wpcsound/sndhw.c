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

/* Taken blatantly from freewpc.h */
typedef unsigned char bcd_t, BCD;
typedef signed char int8_t, I8, S8;
typedef unsigned char uint8_t, U8;
typedef long int16_t, I16, S16;
typedef unsigned long uint16_t, U16;
typedef unsigned int INTPTR;
typedef U8 bool;
#define TRUE (1)
#define FALSE (0)
#define NULL 0UL
#define C_STRING(x)	C_STR(x)
#define C_STR(x)		#x

#include <m6809/m6809.h>
#include <queue.h>
#include <sndhw.h>
#include <sys/irq.h>

__attribute__((section("direct"))) U8 tmp;
U8 tmp2;
U8 tmp3 = 0;

const U8 *dac_sequence;

const U8 *dac_sequence_end;


extern inline void increase_volume (void)
{
	adjust_volume (0);
}

extern inline void decrease_volume (void)
{
	adjust_volume (1);
}

/**********************************************************************/

/* This is a sequence of DAC samples that produces the 
 * WPC sound board 'gong' at startup. */

const U8 dac_beep[] = { 0xFF, 0x00, 0x00, 0x7F, 0x00, 0x01, 0x7F, 0x01, 
0x01, 0x7E, 0x01, 0x02, 0x7E, 0x02, 0x02, 0x7D, 0x02, 0x03, 0x7D, 0x03, 
0x03, 0x7C, 0x03, 0x04, 0x7C, 0x04, 0x04, 0x7B, 0x04, 0x05, 0x7B, 0x05, 
0x05, 0x7A, 0x05, 0x06, 0x7A, 0x06, 0x06, 0x79, 0x06, 0x07, 0x79, 0x07, 
0x07, 0x78, 0x07, 0x08, 0x78, 0x08, 0x08, 0x77, 0x08, 0x09, 0x77, 0x09, 
0x09, 0x76, 0x09, 0x0A, 0x76, 0x0A, 0x0A, 0x75, 0x0A, 0x0B, 0x75, 0x0B, 
0x0B, 0x74, 0x0B, 0x0C, 0x74, 0x0C, 0x0C, 0x73, 0x0C, 0x0D, 0x73, 0x0D, 
0x0D, 0x72, 0x0D, 0x0E, 0x72, 0x0E, 0x0E, 0x71, 0x0E, 0x0F, 0x71, 0x0F, 
0x0F, 0x70, 0x0F, 0x10, 0x70, 0x10, 0x10, 0x6F, 0x10, 0x11, 0x6F, 0x11, 
0x11, 0x6E, 0x11, 0x12, 0x6E, 0x12, 0x12, 0x6D, 0x12, 0x13, 0x6D, 0x13, 
0x13, 0x6C, 0x13, 0x14, 0x6C, 0x14, 0x14, 0x6B, 0x14, 0x15, 0x6B, 0x15, 
0x15, 0x6A, 0x15, 0x16, 0x6A, 0x16, 0x16, 0x69, 0x16, 0x17, 0x69, 0x17, 
0x17, 0x68, 0x17, 0x18, 0x68, 0x18, 0x18, 0x67, 0x18, 0x19, 0x67, 0x19, 
0x19, 0x66, 0x19, 0x1A, 0x66, 0x1A, 0x1A, 0x65, 0x1A, 0x1B, 0x65, 0x1B, 
0x1B, 0x64, 0x1B, 0x1C, 0x64, 0x1C, 0x1C, 0x63, 0x1C, 0x1D, 0x63, 0x1D, 
0x1D, 0x62, 0x1D, 0x1E, 0x62, 0x1E, 0x1E, 0x61, 0x1E, 0x1F, 0x61, 0x1F, 
0x1F, 0x60, 0x1F, 0x20, 0x60, 0x20, 0x20, 0x5F, 0x20, 0x21, 0x5F, 0x21, 
0x21, 0x5E, 0x21, 0x22, 0x5E, 0x22, 0x22, 0x5D, 0x22, 0x23, 0x5D, 0x23, 
0x23, 0x5C, 0x23, 0x24, 0x5C, 0x24, 0x24, 0x5B, 0x24, 0x25, 0x5B, 0x25, 
0x25, 0x5A, 0x25, 0x26, 0x5A, 0x26, 0x26, 0x59, 0x26, 0x27, 0x59, 0x27, 
0x27, 0x58, 0x27, 0x28, 0x58, 0x28, 0x28, 0x57, 0x28, 0x29, 0x57, 0x29, 
0x29, 0x56, 0x29, 0x2A, 0x56, 0x2A, 0x2A, 0x55, 0x2A, 0x2B, 0x55, 0x2B, 
0x2B, 0x54, 0x2B, 0x2C, 0x54, 0x2C, 0x2C, 0x53, 0x2C, 0x2D, 0x53, 0x2D, 
0x2D, 0x52, 0x2D, 0x2E, 0x52, 0x2E, 0x2E, 0x51, 0x2E, 0x2F, 0x51, 0x2F, 
0x2F, 0x50, 0x2F, 0x30, 0x50, 0x30, 0x30, 0x4F, 0x30, 0x31, 0x4F, 0x31, 
0x31, 0x4E, 0x31, 0x32, 0x4E, 0x32, 0x32, 0x4D, 0x32, 0x33, 0x4D, 0x33, 
0x33, 0x4C, 0x33, 0x34, 0x4C, 0x34, 0x34, 0x4B, 0x34, 0x35, 0x4B, 0x35, 
0x35, 0x4A, 0x35, 0x36, 0x4A, 0x36, 0x36, 0x49, 0x36, 0x37, 0x49, 0x37, 
0x37, 0x48, 0x37, 0x38, 0x48, 0x38, 0x38, 0x47, 0x38 };


/**********************************************************************/


/* Delay for a certain number of milliseconds */
void mdelay (U8 count)
{
	do {
		mdelay1 ();
		count--;
	} while (count != 0);
}


/* Delay for a certain number of seconds */
void sdelay (U8 count)
{
	do {
		mdelay (250);
		mdelay (250);
		mdelay (250);
		mdelay (220);
	count--;
	} while (count != 0);
}


/* Change the volume by a given delta (up or down) */
void change_volume (S8 diff)
{
	if (diff < 0)
	{
		do {
			decrease_volume ();
			mdelay1 ();
			diff++;
		} while (diff != 0);
	}
	else
	{
		do {
			increase_volume ();
			mdelay1 ();
			diff--;
		} while (diff != 0);
	}
}


/* Silence the volume control */
void silence_volume (void)
{
	change_volume (-120);
}


/* Set the current volume */
void set_volume (U8 level)
{
	silence_volume ();
	change_volume (level);
}



void wpc_sound_init (void)
{
	U16 i;
	U8 reg;

	dac_sequence = dac_sequence_end = NULL;

	/* Initialize the volume control */
	set_volume (72);

	/* Initialize the FM chip */
	for (reg=0; reg <= 0xFE; reg++)
		fm_write (reg, 0);
	fm_write (0xFF, 0);

	fm_write (FM_ADDR_CLOCK_FUNCTIONS, FM_CLOCK_RESET_A+FM_CLOCK_RESET_B);
	fm_write (FM_ADDR_CLOCK_A1, 0xFD);
	fm_write (FM_ADDR_CLOCK_A2, 0x02);
	fm_write (FM_ADDR_CLOCK_FUNCTIONS, FM_CLOCK_RESET_A+FM_CLOCK_IRQ_A+FM_CLOCK_LOAD_A);
	enable_firq ();

	/* Emit the 'gong' sound.
	 * 1 sample per 1ms = 1000 samples/sec x 8 bits/sample = 8Khz */
	dac_sequence = dac_beep;
	dac_sequence_end = dac_beep + sizeof (dac_beep);
}


__attribute__((noreturn)) void wpc_sound_main_loop (void)
{
	for (;;)
	{
	}
}


/** The reset handler, called at CPU reset time */
__attribute__((naked)) __attribute__((noreturn))
void wpc_sound_reset (void)
{
	set_stack_pointer (0x1000);
	set_direct_page_pointer (0x02);

	wpc_sound_init ();
	wpc_sound_main_loop ();
}


/** Generated when the CPU board writes a value to the sound board */
__attribute__((interrupt)) void wpc_sound_irq (void)
{
	/* Read the value from the CPU and queue it for processing later. */
}


/** Generated by the FM chip when its programmable timer expires. */
__attribute__((interrupt)) void wpc_sound_firq (void)
{
	/* Save registers */
	asm ("pshs\td,x");

	/* Emit 1st byte in DAC sequence */
	if (dac_sequence)
		dac_write (*dac_sequence++);

	hard_delay (320);

	/* Emit 2nd byte in DAC sequence */
	if (dac_sequence)
	{
		dac_write (*dac_sequence++);
		if (dac_sequence >= dac_sequence_end)
		{
			dac_sequence = NULL;
		}
	}

	/* Restart the timer */
	fm_write (FM_ADDR_CLOCK_FUNCTIONS, FM_CLOCK_RESET_A+FM_CLOCK_IRQ_A+FM_CLOCK_LOAD_A);

	/* Restore registers and return */
	asm ("puls\td,x");
	return;
}

