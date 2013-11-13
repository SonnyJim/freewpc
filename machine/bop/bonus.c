#include <freewpc.h>

#define TOTAL_COMBOS (rightramp_combos + loop_combos)

#define NUM_BONUS_ITEMS 3

extern U8 rightramp_combos;
extern U8 loop_combos;
extern U8 rightramps;
extern U8 total_body_parts;


score_t total_bonus;
score_t bonus_scored;
/*
typedef struct {
	const char *score_name[16];
	const char *score_text[6];
	const score_id_t *score_amount;
	U8 multiplier_var;
} bonus_struct_t;

const struct bonus_struct_t bonus_scores[NUM_BONUS_ITEMS] = {
	{ "LOOP COMBOS", "250K", SC_250K, loop_combos },
	{ "RAMP COMBOS", "250K", SC_250K, rightramp_combos },
	{ "BODY PARTS", "250K", SC_250K, total_body_parts }
};
	
static void bonus_score_add (score_id_t score_amount, U8 multiplier)
{
	score_zero (bonus_scored);
	score_add (bonus_scored, score_table[score_amount]);
	score_mul (bonus_scored, multiplier);
	score_add (total_bonus, bonus_scored);
}
*/
static void bonus_check_and_score (void)
{
	/*
	score_zero (total_bonus);
	
	U8 i;

	for (i = 1; i <= NUM_BONUS_ITEMS; i++)
	{
		if (bonus_scores[i].multiplier_var > 0)
		{
			bonus_score_add (bonus_scores[i].score_amount, bonus_scores[i].multiplier_var);
			seg_alloc_clean ();
			sprintf ("%d %s", bonus_scores[i].multiplier_var, bonus_scores[i].score_text);
			seg_write_row_center (0, sprintf_buffer);
			sprintf ("%d X %s", bonus_scores[i].multiplier_var, bonus_scores[i].score_text);
			seg_write_row_center (1, sprintf_buffer);
			seg_show ();
			task_sleep_sec (1);
		
			seg_alloc_clean ();
			sprintf ("%d %s", bonus_scores[i].multiplier_var, bonus_scores[i].score_text);
			seg_write_row_center (0, sprintf_buffer);
			sprintf_score (bonus_scored);
			seg_write_row_center (1, sprintf_buffer);
			seg_show ();
			task_sleep_sec (1);
		}
	}
	*/
	score_zero (bonus_scored);
	score_zero (total_bonus);
	if (TOTAL_COMBOS > 0 )
	{
		score_add (bonus_scored, score_table[SC_250K]);
		score_mul (bonus_scored, TOTAL_COMBOS);
		score_add (total_bonus, bonus_scored);


		seg_alloc_clean ();
		psprintf ("1 COMBO","%d COMBOS", TOTAL_COMBOS);
		seg_write_row_center (0, sprintf_buffer);
		sprintf ("%d X 250K", TOTAL_COMBOS);
		seg_write_row_center (1, sprintf_buffer);
		seg_show ();
		task_sleep_sec (1);
		
		seg_alloc_clean ();
		psprintf ("1 COMBO","%d COMBOS", TOTAL_COMBOS);
		seg_write_row_center (0, sprintf_buffer);
		sprintf_score (bonus_scored);
		seg_write_row_center (1, sprintf_buffer);
		seg_show ();
		task_sleep_sec (1);

	}
	if (total_body_parts > 0)
	{
		score_zero (bonus_scored);
		score_add (bonus_scored, score_table[SC_250K]);
		score_mul (bonus_scored, total_body_parts);
		score_add (total_bonus, bonus_scored);

		seg_alloc_clean ();
		psprintf ("1 BODY PART", "%d BODY PARTS", total_body_parts);
		seg_write_row_center (0, sprintf_buffer);
		sprintf ("%d X 250K", total_body_parts);
		seg_write_row_center (1, sprintf_buffer);
		seg_show ();
		task_sleep_sec (1);
		
		seg_alloc_clean ();
		psprintf ("1 BODY PART", "%d BODY PARTS", total_body_parts);
		seg_write_row_center (0, sprintf_buffer);
		sprintf_score (bonus_scored);
		seg_write_row_center (1, sprintf_buffer);
		seg_show ();
		task_sleep_sec (1);

	}

	if (rightramps > 0)
	{
		score_zero (bonus_scored);
		score_add (bonus_scored, score_table[SC_50K]);
		score_mul (bonus_scored, rightramps);
		score_add (total_bonus, bonus_scored);

		seg_alloc_clean ();
		psprintf ("1 HEARTBEAT", "%d HEARTBEATS", rightramps);
		seg_write_row_center (0, sprintf_buffer);
		sprintf ("%d X 50K", rightramps);
		seg_write_row_center (1, sprintf_buffer);
		seg_show ();
		task_sleep_sec (1);
		
		seg_alloc_clean ();
		psprintf ("1 HEARTBEAT", "%d HEARTBEATS", rightramps);
		seg_write_row_center (0, sprintf_buffer);
		sprintf_score (bonus_scored);
		seg_write_row_center (1, sprintf_buffer);
		seg_show ();
		task_sleep_sec (1);

	}
	
	seg_alloc_clean ();
	seg_write_row_center (0, "TOTAL BONUS");
	sprintf_score (total_bonus);
	seg_write_row_center (1, sprintf_buffer);
	seg_show ();
	task_sleep_sec (1);

	score_long (total_bonus);
}

void bonus_deff (void)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "BONUS");
	seg_show ();
	task_sleep_sec (2);
	bonus_check_and_score ();
	deff_exit ();
}

CALLSET_ENTRY (trivial, bonus)
{
	deff_start (DEFF_BONUS);
	//leff_start (LEFF_BONUS);
	task_sleep_sec (1);
	while (deff_get_active () == DEFF_BONUS)
		task_sleep (TIME_66MS);
	//leff_stop (LEFF_BONUS);
}

