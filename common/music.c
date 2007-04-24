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

#include <freewpc.h>

/**
 * \file
 * \brief Manages which music track should be played.
 *
 * Like other effect modules (deffs and leffs), music effects are queued
 * and use priorities to determine which one gets played.  Up to MAX_TRACKS
 * can be queued at a time.  Whenever the highest priority queued effect
 * changes, the sound board is re-written with the updated music code.
 */


/** The array of all queued music tracks */
audio_track_t music_tracks[MAX_TRACKS];

/** The index into 'music_tracks' denoting the entry which is currently
the highest priority.  If no tracks are queued, this will be 0xFF. */
U8 music_track_current;


/** Recalculates the highest priority track if necessary by scanning the
entire array.  Then, update the sound board hardware with the correct
value. */
void music_update (void)
{
	U8 i;
	U8 best_prio = 0;

	if (music_track_current == 0xFF)
	{
		for (i=0; i < MAX_TRACKS; i++)
		{
			if (music_tracks[i].prio > best_prio)
				music_track_current = i;
		}
	}

	dbprintf ("Current track = (%02X, %d)\n", 
		music_tracks[music_track_current].code,
		music_tracks[music_track_current].prio);

	if (music_track_current == 0xFF)
		music_off ();
	else
		/* Note: if for some reason this code is already playing, then
		'music_set' will NOT write it to the sound board again, causing
		the track to restart. */
		music_set (music_tracks[music_track_current].code);
}


/** Find the offset into the queue where 'track' is.  Returns 0xFF if not found. */
U8 music_find (audio_track_t track)
{
	U8 i;
	for (i=0; i < MAX_TRACKS; i++)
		if ((music_tracks[i].code == track.code)
			&& (music_tracks[i].prio == track.prio))
			return i;
	return 0xFF;
}


/** Starts a new music track. */
void music_start (audio_track_t track)
{
	U8 i;
	audio_track_t zero_track = { 0, };

	dbprintf ("music_start (%02X, %d)\n", track.code, track.prio);

	i = music_find (track);
	if (i != 0xFF)
	{
		dbprintf ("warning: started track that is already queued\n");
		return;
	}

	i = music_find (zero_track);
	if (i != 0xFF)
	{
		music_tracks[i] = track;

		if ((music_track_current == 0xFF)
			|| (music_tracks[music_track_current].prio < track.prio))
		{
			music_track_current = i;
			music_update ();
		}
	}
	else
	{
		dbprintf ("error: no free music tracks\n");
	}
}


/** Stops a queued/running music track. */
void music_stop (audio_track_t track)
{
	U8 i;

	dbprintf ("music_stop (%02X, %d)\n", track.code, track.prio);
	
	i = music_find (track);
	if (i != 0xFF)
	{
		music_tracks[i].code = 0;
		music_tracks[i].prio = 0;

		if (i == music_track_current)
		{
			music_track_current = 0xFF;
			music_update ();
		}
	}
}

CALLSET_ENTRY (music, init)
{
	memset (music_tracks, 0, sizeof (music_tracks));
	music_track_current = 0xFF;
}


/** Stops all music tracks running and/or queued. */
void music_stop_all (void)
{
	music_init ();
	music_update ();
}
