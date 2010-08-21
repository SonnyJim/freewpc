/*
 * Copyright 2006, 2007, 2008, 2009, 2010 by Brian Dominy <brian@oddchange.com>
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

/* amode.c */
/* autofire.c */
__machine__ void autofire_catch(void);
__machine__ void autofire_open_for_trough(void);
__machine__ void autofire_add_ball(void);

/* magnet.c */
#define MAG_LEFT 0
#define MAG_UPPER_RIGHT 1
#define MAG_RIGHT 2

void magnet_enable_catch (U8 magnet);
void magnet_enable_catch_and_hold (U8 magnet, U8 secs);
void magnet_enable_catch_and_throw (U8 magnet);
void magnet_disable_catch (U8 magnet);
void magnet_reset (void);
