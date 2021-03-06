/*
 * Copyright 2007, 2008, 2009, 2010 by Brian Dominy <brian@oddchange.com>
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

#include <ncurses.h>
#include <string.h>
#include <stdarg.h>
#include <simulation.h>

/* \file ui_curses.c
 * \brief A curses-based UI for the built-in WPC simulator.
 */

WINDOW *lamp_win;
WINDOW *triac_win;
WINDOW *sol_win;
WINDOW *debug_win;
WINDOW *dmd_win;
WINDOW *switch_win;
WINDOW *task_win;
WINDOW *sound_win;
WINDOW *display_win;
WINDOW *ball_tracker_win;
WINDOW *cmdline_win;


static void print_center (WINDOW *w, int x, int y, const char *format, ...)
{
	va_list ap;
	int len;
	char buf[80];

	va_start (ap, format);
	vsprintf (buf, format, ap);
	va_end (ap);

	len = strlen (buf);
	x = x - len/2;
	wmove (w, y, x);
	wprintw (w, format);
}


static WINDOW * ui_window_create (int width, int height, int x, int y, const char *title)
{
	WINDOW *w = newwin (height, width, y, x);
	if (title)
	{
		box (w, 0, 0);
		print_center (w, width/2, 0, title);
	}
	wrefresh (w);
	return w;
}


void ui_print_command (const char *cmdline)
{
	wmove (cmdline_win, 1, 1);
	wprintw (cmdline_win, "                     ");
	wmove (cmdline_win, 1, 1);
	if (*cmdline != '\0')
	{
		wprintw (cmdline_win, ":");
		wprintw (cmdline_win, cmdline);
	}
	wrefresh (cmdline_win);
}

void ui_write_debug (enum sim_log_class c, const char *format, va_list ap)
{
	if (c != SLC_DEBUG_PORT)
		wprintw (debug_win, "[SIM] ");
	vw_printw (debug_win, format, ap);
	waddch (debug_win, '\n');
	wrefresh (debug_win);
}


void ui_write_solenoid (int solno, int on_flag)
{
	int x = (solno / 8) * 3 + 2;
	int y = (solno % 8) + 1;
	wmove (sol_win, y, x);
	if (on_flag)
		wprintw (sol_win, "%02d", solno);
	else
		wprintw (sol_win, "  ");
	wrefresh (sol_win);
}


void ui_write_lamp (int lampno, int on_flag)
{
	int x = (lampno / 8) * 3 + 2;
	int y = (lampno % 8) + 1;
	wmove (lamp_win, y, x);
	if (on_flag)
		wprintw (lamp_win, "%02d", lampno);
	else
		wprintw (lamp_win, "  ");
	wrefresh (lamp_win);
}


void ui_write_triac (int triacno, int on_flag)
{
	int x = triacno + 2;
	int y = 1;
	wmove (triac_win, y, x);
	wprintw (triac_win, on_flag ? "*" : " ");
	wrefresh (triac_win);
}


void ui_write_switch (int switchno, int on_flag)
{
	int x = (switchno / 8) * 3 + 2;
	int y = (switchno % 8) + 1;
	wmove (switch_win, y, x);
	if (on_flag)
		wprintw (switch_win, "%02d", switchno);
	else
		wprintw (switch_win, "  ");
	wrefresh (switch_win);
}


void ui_write_sound_command (unsigned int x)
{
	wmove (sound_win, 1, 1);
	wprintw (sound_win, "%04X", x);
	wrefresh (sound_win);
}


void ui_write_sound_reset (void)
{
	wmove (sound_win, 1, 1);
	wprintw (sound_win, "    ");
	wrefresh (sound_win);
}


void ui_write_task (int taskno, int gid)
{
	int x = (taskno / 12) * 8 + 2;
	int y = (taskno % 12) + 1;
	wmove (task_win, y, x);
	if (gid == 0)
		wprintw (task_win, "%02d:    ", taskno);
	else
		wprintw (task_win, "%02d: %3d", taskno, gid);
	wrefresh (task_win);
}


#if (MACHINE_DMD == 1)
void ui_refresh_asciidmd (unsigned char *data)
{
	unsigned int x, y;

	for (y = 0; y < 32; y++)
	{
		wmove (display_win, y+1, 1);
		for (x = 0; x < 128; x++)
			wprintw (display_win, "%c", enhanced_pixel_ascii (*data++));
	}
	wrefresh (display_win);
}
#else
void ui_refresh_display (unsigned int x, unsigned int y, char c)
{
	wmove (display_win, y+1, x+1);
	wprintw (display_win, "%c", c);
	wrefresh (display_win);
}
#endif


void ui_update_ball_tracker (unsigned int ballno, unsigned int location)
{
	extern const char *sim_ball_location_name (unsigned int location);
	wmove (ball_tracker_win, ballno+1, 1);
	wprintw (ball_tracker_win, "%d: %-14.14s", ballno,
		sim_ball_location_name (location));
	wrefresh (ball_tracker_win);
}


void ui_init (void)
{
	initscr ();
	clear ();
	int x = 0, y = 0;

	ui_window_create (COLS, 2, x, y, " FreeWPC - Linux Simulator ");
	y += 2;

	switch_win = ui_window_create (34, 10, x, y, " Switches ");
	x += 34 + 2;

	lamp_win = ui_window_create (28, 10, x, y, " Lamps ");
	x += 28 + 2;

	sol_win = ui_window_create (20, 10, x, y, " Solenoids ");
	triac_win = ui_window_create (12, 3, x, y+10, " Triacs ");
	sound_win = ui_window_create (10, 3, x, y+13, " Sound ");
	scrollok (sound_win, 1);
	x += 20 + 2;

	task_win = ui_window_create (27, 14, x, y, " Tasks ");
	x += 27 + 2;

	ball_tracker_win = ui_window_create (24, 10, x, y, " Ball Tracker ");
	cmdline_win = ui_window_create (24, 3, x, y + 10, " Command Line ");
	x += 24 + 2;

	y += 9 + 1;
	x = 0;
	debug_win = ui_window_create (64, 40, x, y, NULL);
	scrollok (debug_win, 1);
	x += 64 + 2;
	y += 6;

#if (MACHINE_DMD == 1)
	display_win = ui_window_create (130, 34, x, y, " ASCII-Matrix ");
#else
	display_win = ui_window_create (20, 4, x, y, " Display Panel ");
#endif

}


void ui_exit (void)
{
	endwin ();
}

