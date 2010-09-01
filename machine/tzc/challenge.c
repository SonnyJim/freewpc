#include <freewpc.h>
#include <math.h>

#define NUM_CHALLENGE_SHOTS 6

U8 shots_made;

U8 challenge_shot_1;
U8 challenge_shot_2;
U8 old_challenge_shot_1;
U8 old_challenge_shot_2;
bool challenge_shot_1_active;
bool challenge_shot_2_active;


enum { 
	CS_PIANO = 0,
	CS_SLOT,
	CS_CAMERA,
	CS_LOCK,
	CS_LEFTRAMP,
	CS_RIGHTRAMP,
};

struct {
	const char *shot_name;
	U8 lamp;
} challenge_shots[] = {
	{ "PIANO", LM_PIANO_PANEL },
	{ "SLOT", LM_SLOT_MACHINE },
	{ "CAMERA", LM_CAMERA },
	{ "LOCK", LM_LOCK_ARROW },
	{ "LEFT RAMP", LM_MULTIBALL },
	{ "RIGHT RAMP", LM_RAMP_BATTLE },
};

void challenge_running_deff (void)
{
	bool flash = FALSE;
	while (in_live_game)
	{
		U8 time_minutes, time_seconds;
		time_minutes = 0;
		time_seconds = timed_game_timer;
		while (time_seconds >= 60)
		{
			time_minutes++;
			time_seconds -= 60;
		}

		dmd_alloc_low_clean ();
		font_render_string_center (&font_fixed6, 64, 7, "SHOOT");
		if (flash)
		{
			if (challenge_shot_1_active && challenge_shot_2_active)
			{
				sprintf("%s", challenge_shots[challenge_shot_1].shot_name);
				font_render_string_left (&font_var5, 8, 16, sprintf_buffer);
				sprintf("%s", challenge_shots[challenge_shot_2].shot_name);
				font_render_string_right (&font_var5, 120, 16, sprintf_buffer);
			}
			else if (challenge_shot_1_active)
			{
				sprintf("%s", challenge_shots[challenge_shot_1].shot_name);
				font_render_string_center (&font_var5, 64, 16, sprintf_buffer);
			}
			else if (challenge_shot_2_active)
			{
				sprintf("%s", challenge_shots[challenge_shot_2].shot_name);
				font_render_string_center (&font_var5, 64, 16, sprintf_buffer);
			}
			flash = FALSE;
		}
		else
		{
			flash = TRUE;
		}

		sprintf ("TIME: %d:%02d  SHOTS MADE: %d", time_minutes, time_seconds, shots_made);
		font_render_string_center (&font_var5, 64, 26, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_100MS);
		}
}

static inline void challenge_lamp_update (void)
{
	lamp_tristate_flash (challenge_shots[challenge_shot_1].lamp);
	lamp_tristate_flash (challenge_shots[challenge_shot_2].lamp);
}

static void challenge_pick_two_new_targets (void)
{
	/* Store the old shots */
	old_challenge_shot_1 = challenge_shot_1;
	old_challenge_shot_2 = challenge_shot_2;
	
	/* Pick 2 new ones */
	challenge_shot_1 = random_scaled (NUM_CHALLENGE_SHOTS);
	challenge_shot_2 = random_scaled (NUM_CHALLENGE_SHOTS);

	/* Make sure we pick two brand new shots */
	while (challenge_shot_1 == (old_challenge_shot_1 || old_challenge_shot_2))
		challenge_shot_1 = random_scaled (NUM_CHALLENGE_SHOTS);
	
	/* Make sure we don't light the same shot twice */
	while (challenge_shot_2 == (old_challenge_shot_2 || old_challenge_shot_1 || challenge_shot_1))
		challenge_shot_2 = random_scaled (NUM_CHALLENGE_SHOTS);
	
	challenge_shot_1_active = TRUE;
	challenge_shot_2_active = TRUE;
	timed_game_extend (10);
	challenge_lamp_update ();
}

static void challenge_shot_handler (U8 challenge_shot)
{
	if (challenge_shot == challenge_shot_1 && challenge_shot_1_active)
	{
		shots_made++;
		sound_send (SND_EXPLOSION_1);
		lamp_tristate_off (challenge_shots[challenge_shot_1].lamp);
		timed_game_extend (5);
		challenge_shot_1_active = FALSE;
	}
	else if (challenge_shot == challenge_shot_2 && challenge_shot_2_active)
	{
		shots_made++;
		sound_send (SND_EXPLOSION_1);
		lamp_tristate_off (challenge_shots[challenge_shot_2].lamp);
		timed_game_extend (5);
		challenge_shot_2_active = FALSE;
	}
	if (challenge_shot_1_active == FALSE && challenge_shot_2_active == FALSE)
	{
		sound_send (SND_TWILIGHT_ZONE_SHORT_SOUND);
		challenge_pick_two_new_targets ();
	}
}

CALLSET_ENTRY (challenge, display_update)
{
	if (valid_playfield && deff_get_active () != DEFF_CHALLENGE_RUNNING)
		deff_start (DEFF_CHALLENGE_RUNNING);
}

CALLSET_ENTRY (challenge, start_ball)
{
	lamplist_apply (LAMPLIST_CHALLENGE_SHOTS, lamp_off);
	challenge_shot_1 = 0;
	challenge_shot_2 = 0;
	shots_made = 0;
	challenge_shot_1_active = FALSE;
	challenge_shot_2_active = FALSE;
	challenge_pick_two_new_targets ();
}

CALLSET_ENTRY (challenge, shot_slot)
{
	challenge_shot_handler (CS_SLOT);
}

CALLSET_ENTRY (challenge, shot_piano)
{
	challenge_shot_handler (CS_PIANO);
}

CALLSET_ENTRY (challenge, shot_camera)
{
	challenge_shot_handler (CS_CAMERA);
}

CALLSET_ENTRY (challenge, shot_lock)
{
	challenge_shot_handler (CS_LOCK);
}

CALLSET_ENTRY (challenge, shot_leftramp)
{
	challenge_shot_handler (CS_LEFTRAMP);
}

CALLSET_ENTRY (challenge, shot_rightramp)
{
	challenge_shot_handler (CS_RIGHTRAMP);
}
