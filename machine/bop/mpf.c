/*
 * Copyright 2013 by Ewan Meadows (sonny_jim@hotmail.com)
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

U8 mpf_level;

const lampnum_t mpf_lamps[] = {
	LM_MPF_100K, LM_MPF_200K, LM_MPF_300K
};


CALLSET_ENTRY (mpf, start_ball)
{
	mpf_level = 0;	
}

CALLSET_ENTRY (mpf, lamp_update)
{
	lamplist_apply (LAMPLIST_MPF, lamp_off);
	lamplist_apply (LAMPLIST_MPF, lamp_flash_off);
	switch (mpf_level)
	{
		case 0:
			lamp_flash_on (LM_MPF_100K);
			break;
		case 1:
			lamp_on (LM_MPF_100K);
			lamp_flash_on (LM_MPF_200K);
			break;
		case 2:
			lamp_on (LM_MPF_100K);
			lamp_on (LM_MPF_200K);
			lamp_flash_on (LM_MPF_300K);
			break;
		case 3:
			lamp_flash_on (LM_MPF_100K);
			lamp_flash_on (LM_MPF_200K);
			lamp_flash_on (LM_MPF_300K);
			callset_invoke (head_enable_divert);
			break;
		default:
			break;
	}

}

CALLSET_ENTRY (mpf, mpf_entered)
{
	bounded_increment (mpf_level, 3);
	score_multiple (SC_100K, mpf_level);
	sound_send (MUS_MPF_ENTER);
}

CALLSET_ENTRY (mpf, sw_mpf_exit_left)
{
	sound_send (SND_MPF_EXIT);
}

CALLSET_ENTRY (mpf, sw_mpf_exit_right)
{
	sound_send (SND_MPF_EXIT);
}
