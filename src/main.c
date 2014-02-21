#include <pebble.h>

// ### CONFIG ###
#define SCREEN_WIDTH	144
#define SCREEN_HEIGHT	168

#define LEVEL_D			0
#define AGE_D			0
#define HAPPINESS_D		100
#define TIME_LEFT_D		30
#define HEALTH_D		100
#define TIME_ALIVE_D	0
// ###

enum {
	KEY_AGE = 91,
	KEY_LEVEL = 94,
	KEY_HEALTH = 93,
	KEY_HAPPINESS = 92,
	KEY_TOGROW = 95,
	KEY_TOTALTIME = 96
};

typedef struct USER
{
	int level;
	int happiness;
	int health;
	int age;
	int timeleft;
	int timealive;
}User;
User pet;

// ### Predeclare Functions ###
static void deinit();
static void build_clock();
static void build_stats();
static void saveGame();
// ###

static Window *windowMain;
static Window *windowStats;
static Layer *layer_main;
static Layer *layer_stats;
static Layer *layers[2];
static Layer *line_layer;

static TextLayer *text_date_layer;
static TextLayer *text_time_layer;
static TextLayer *text_age;
static TextLayer *text_happy;
static TextLayer *text_health;
static TextLayer *text_level;
static TextLayer *text_time;

static GRect bounds;
static GBitmap *egg[2];
static GBitmap *stage1[7];
static GBitmap *stage2[7];
static GBitmap *stage3[5];
static GBitmap *stage4[7];
static GBitmap *stage5[7];
static GRect rows[5];
static GBitmap *poo;

static AppTimer *timer;

static bool bounce = false;
static bool petReady = false;
static bool clockMode = true;
static bool isPoop = false;

static short eggVariant = 0;
static short stage1Variant = 0;
static short stage2Variant = 0;
static short stage3Variant = 0;
static short stage4Variant = 0;
static short stage5Variant = 0;

static int nStep = 0;
static short pX, pY;
/// ### END DECLARES ###

void accel_tap_handler(AccelAxisType axis, int32_t direction) {	isPoop = false; }

void setupPet()
{
  switch(pet.level)
	{
	case 0:
		egg[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_EGG_UP);
		egg[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_EGG_DOWN);
		break;
	case 1:
		stage1[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE1_0);
		stage1[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE1_1);
		stage1[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE1_2);
		stage1[3] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE1_3);
		stage1[4] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE1_4);
		stage1[5] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE1_5);
		stage1[6] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE1_6);
		break;
	case 2:
		break;
	case 3:
		stage3[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE3_1_0);
		stage3[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE3_1_1);
		stage3[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE3_1_2);
		stage3[3] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE3_1_3);
		stage3[4] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE3_1_4);
		break;
	case 4:
		break;
	case 5:
		break;
	default:
		break;
	}
	petReady = true;
}

void out_sent_handler(DictionaryIterator *sent, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Outgoing delivered!");
}
void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Outgoing failed!");
}
void in_received_handler(DictionaryIterator *received, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Incoming Starting...");

	vibes_short_pulse();
		
	Tuple *age_tuple = dict_find(received, KEY_AGE);
	if (age_tuple) { pet.age = age_tuple->value->int32; }

	Tuple *level_tuple = dict_find(received, KEY_LEVEL);
	if (level_tuple) { pet.level=level_tuple->value->int32; }

	Tuple *happy_tuple = dict_find(received, KEY_HAPPINESS);
	if (happy_tuple) { pet.happiness=happy_tuple->value->int32; }

	Tuple *health_tuple = dict_find(received, KEY_HEALTH);
	if (health_tuple) { pet.health=health_tuple->value->int32; }

	Tuple *tleft_tuple = dict_find(received, KEY_TOGROW);
	if (tleft_tuple) { pet.timeleft=tleft_tuple->value->int32; }

	Tuple *ttotal_tuple = dict_find(received, KEY_TOTALTIME);
	if (ttotal_tuple) { pet.timealive=ttotal_tuple->value->int32; }
	
	app_log(APP_LOG_LEVEL_DEBUG, "Incoming Done.", 112, "%d %d", pet.timeleft, pet.timeleft);

    setupPet();s
//	app_log(APP_LOG_LEVEL_DEBUG, "main.c", 108, "%d %d %d", nGreen, nRed, nBlue);
}
void in_dropped_handler(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Incoming failed!");
	vibes_long_pulse();
}

static void saveGame()
{
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Saving Game!");
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	Tuplet t_age = TupletInteger(KEY_AGE, pet.age);
	Tuplet t_level = TupletInteger(KEY_LEVEL, pet.level);
	Tuplet t_health = TupletInteger(KEY_HEALTH, pet.health);
	Tuplet t_happiness = TupletInteger(KEY_HAPPINESS, pet.happiness);
	Tuplet t_tleft = TupletInteger(KEY_TOGROW, pet.timeleft);
	Tuplet t_ttotal = TupletInteger(KEY_TOTALTIME, pet.timealive);
	dict_write_tuplet(iter, &t_age);
	dict_write_tuplet(iter, &t_level);
	dict_write_tuplet(iter, &t_health);
	dict_write_tuplet(iter, &t_happiness);
	dict_write_tuplet(iter, &t_tleft);
	dict_write_tuplet(iter, &t_ttotal);
	app_message_outbox_send();
	
}

void levelUp()
{
	static const uint32_t const segments[] = { 200, 100, 200, 100, 200 };
	VibePattern pat = {
		.durations = segments,
		.num_segments = ARRAY_LENGTH(segments),
	};
	vibes_enqueue_custom_pattern(pat);

	switch(pet.level)
	{
	case 0:
		pet.timeleft = 1440; // one day
		pet.level++;
		break;
	case 1:
		pet.timeleft = 2880; // two days
		pet.level++;
		break;
	case 2:
		pet.timeleft = 4320; // three days
		pet.level++;
		break;
	case 3:
		pet.timeleft = 5760;
		pet.level++;
		break;
	case 4:
		pet.timeleft = 7200;
		pet.level++;
		break;
	case 5:
		pet.timeleft = 8640;
		pet.level++;
		break;
	default:
		break;
	}

	switch(pet.level)
	{
	case 1:
		gbitmap_destroy(egg[0]);
		gbitmap_destroy(egg[1]);
		stage1[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE1_0);
		stage1[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE1_1);
		stage1[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE1_2);
		stage1[3] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE1_3);
		stage1[4] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE1_4);
		stage1[5] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE1_5);
		stage1[6] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE1_6);
		break;
	case 2:
		break;
	case 3:
		gbitmap_destroy(stage1[0]);
		gbitmap_destroy(stage1[1]);
		gbitmap_destroy(stage1[2]);
		gbitmap_destroy(stage1[3]);
		gbitmap_destroy(stage1[4]);
		gbitmap_destroy(stage1[5]);
		gbitmap_destroy(stage1[6]);
		stage3[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE3_1_0);
		stage3[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE3_1_1);
		stage3[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE3_1_2);
		stage3[3] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE3_1_3);
		stage3[4] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAGE3_1_4);
		break;
	case 4:
		break;
	case 5:
		break;
	default:
		break;
	}

	saveGame();
}

void line_layer_update_callback(Layer *layer, GContext* ctx) {
	if (clockMode)
	{
		graphics_context_set_fill_color(ctx, GColorBlack);
		graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
	}
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (clockMode)
	{
		window_stack_remove(windowMain, true);
		build_stats();
		window_stack_push(windowStats, true);
		clockMode = false;
		tick_timer_service_unsubscribe();
	} else {
		window_stack_remove(windowStats, true);
		build_clock();
		clockMode = true;
	}
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	//UP
	// Use this space for mini games
	levelUp(); //Debug
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	//DOWN
	// Use this space for mini games
	//pet.timeleft--; //Debug
	//saveGame();
	pX = (rand()%(SCREEN_WIDTH-10)+10);
	pY = (rand()%((SCREEN_HEIGHT-20)-100)+100);
	isPoop = true;
}

static void back_click_handler(ClickRecognizerRef recognizer, void *context) {
	//Exit/Back
	deinit();
	//saveGame(); -- Normal

}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
	if (clockMode)
	{
		static char time_text[] = "00:00";
		static char date_text[] = "Xxxxxxxxx 00";

		char *time_format;

		strftime(date_text, sizeof(date_text), "%B %e", tick_time);
		text_layer_set_text(text_date_layer, date_text);


		if (clock_is_24h_style()) {
			time_format = "%R";
		} else {
			time_format = "%I:%M";
		}

		strftime(time_text, sizeof(time_text), time_format, tick_time);

		if (!clock_is_24h_style() && (time_text[0] == '0')) {
			memmove(time_text, &time_text[1], sizeof(time_text) - 1);
		}

		text_layer_set_text(text_time_layer, time_text);

		if (units_changed & MINUTE_UNIT)
		{
			pet.timeleft--;
			if (pet.timeleft <= 0) levelUp();
			if (isPoop) {
				pet.happiness--;
			} else {
				if (pet.level >= 1) {
				int decide = (rand()%(20-1)+1);
				if (decide >= 19) { 
					pX = (rand()%(SCREEN_WIDTH-10)+10);
					pY = (rand()%((SCREEN_HEIGHT-20)-100)+100);
					isPoop = true; 
					vibes_short_pulse();
				} else {
					if (pet.happiness != 100) { pet.happiness++; }
					
				}
				}
			}
		}

		if (units_changed & HOUR_UNIT)
		{
			pet.timealive++;
				if (isPoop) {
					pet.health--;
				} else {
					if (pet.health != 100) { pet.health++; }
				}
			if (pet.timealive % 24)
				pet.age++;
			saveGame();
		}

	}
}

static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
	window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
}

static void timer_callback(void *context) {
	if (clockMode)
	layer_mark_dirty(layers[0]);
	else
	layer_mark_dirty(layers[1]);

	const uint32_t timeout_ms = 2000;
	timer = app_timer_register(timeout_ms, timer_callback, NULL);
}

static void layer_update_callback(Layer *me, GContext* ctx) {
	if (clockMode)
	{
        if (petReady)
        {
				switch(pet.level)
				{
				case 0:
					if (bounce)
					{
						GRect bounds = egg[0]->bounds;
						graphics_draw_bitmap_in_rect(ctx, egg[0], (GRect) { .origin = { 50, 100 }, .size = bounds.size });
						bounce = false;
					}	else {
						GRect bounds = egg[1]->bounds;
						graphics_draw_bitmap_in_rect(ctx, egg[1], (GRect) { .origin = { 50, 100 }, .size = bounds.size });
						bounce = true;
					}
					break;
				case 1:
					if (nStep == 0)
					{
						GRect bounds = stage1[0]->bounds;
						graphics_draw_bitmap_in_rect(ctx, stage1[0], (GRect) { .origin = { 10, 103 }, .size = bounds.size });
						nStep = 1;
					} else if (nStep == 1) {
						GRect bounds = stage1[1]->bounds;
						graphics_draw_bitmap_in_rect(ctx, stage1[1], (GRect) { .origin = { 10, 103 }, .size = bounds.size });
						nStep = 2;
					} else if (nStep == 2) {
						GRect bounds = stage1[2]->bounds;
						graphics_draw_bitmap_in_rect(ctx, stage1[2], (GRect) { .origin = { 10, 103 }, .size = bounds.size });
						nStep = 3;
					} else if (nStep == 3) {
						GRect bounds = stage1[3]->bounds;
						graphics_draw_bitmap_in_rect(ctx, stage1[3], (GRect) { .origin = { 10, 103 }, .size = bounds.size });
						nStep = 4;
					} else if (nStep == 4) {
						GRect bounds = stage1[4]->bounds;
						graphics_draw_bitmap_in_rect(ctx, stage1[4], (GRect) { .origin = { 10, 103 }, .size = bounds.size });
						nStep = 5;
					} else if (nStep == 5) {
						GRect bounds = stage1[5]->bounds;
						graphics_draw_bitmap_in_rect(ctx, stage1[5], (GRect) { .origin = { 10, 103 }, .size = bounds.size });
						nStep = 6;
					} else if (nStep == 6) {
						GRect bounds = stage1[2]->bounds;
						graphics_draw_bitmap_in_rect(ctx, stage1[2], (GRect) { .origin = { 10, 103 }, .size = bounds.size });
						nStep = 7;
					} else if (nStep == 7) {
						GRect bounds = stage1[1]->bounds;
						graphics_draw_bitmap_in_rect(ctx, stage1[1], (GRect) { .origin = { 10, 103 }, .size = bounds.size });
						nStep = 8;
					} else if (nStep == 8) {
						GRect bounds = stage1[6]->bounds;
						graphics_draw_bitmap_in_rect(ctx, stage1[6], (GRect) { .origin = { 10, 103 }, .size = bounds.size });
						nStep = 0;
					}
					break;
				case 2:
					break;
				case 3:
					if (nStep == 0)
					{
						GRect bounds = stage3[0]->bounds;
						graphics_draw_bitmap_in_rect(ctx, stage3[0], (GRect) { .origin = { 10, 103 }, .size = bounds.size });
						nStep = 1;
					} else if (nStep == 1) {
						GRect bounds = stage3[1]->bounds;
						graphics_draw_bitmap_in_rect(ctx, stage3[1], (GRect) { .origin = { 10, 103 }, .size = bounds.size });
						nStep = 2;
					} else if (nStep == 2) {
						GRect bounds = stage3[2]->bounds;
						graphics_draw_bitmap_in_rect(ctx, stage3[2], (GRect) { .origin = { 10, 103 }, .size = bounds.size });
						nStep = 3;
					} else if (nStep == 3) {
						GRect bounds = stage3[3]->bounds;
						graphics_draw_bitmap_in_rect(ctx, stage3[3], (GRect) { .origin = { 10, 103 }, .size = bounds.size });
						nStep = 4;
					} else if (nStep == 4) {
						GRect bounds = stage3[4]->bounds;
						graphics_draw_bitmap_in_rect(ctx, stage3[4], (GRect) { .origin = { 10, 103 }, .size = bounds.size });
						nStep = 5;
					} else if (nStep == 5) {
						GRect bounds = stage3[3]->bounds;
						graphics_draw_bitmap_in_rect(ctx, stage3[3], (GRect) { .origin = { 10, 103 }, .size = bounds.size });
						nStep = 6;
					} else if (nStep == 6) {
						GRect bounds = stage3[2]->bounds;
						graphics_draw_bitmap_in_rect(ctx, stage3[2], (GRect) { .origin = { 10, 103 }, .size = bounds.size });
						nStep = 7;
					} else if (nStep == 7) {
						GRect bounds = stage3[1]->bounds;
						graphics_draw_bitmap_in_rect(ctx, stage3[1], (GRect) { .origin = { 10, 103 }, .size = bounds.size });
						nStep = 0;
					}
					break;
				case 4:
					break;
				case 5:
					break;
				default:
					break;
				}
		
	if (isPoop) 
	{
		GRect bounds = poo->bounds;
		graphics_draw_bitmap_in_rect(ctx, poo, (GRect) { .origin = { pX, pY }, .size = bounds.size });
	}
	} else {
		static char dlevel[10];
		static char dage[13];
		static char dhealth[13];
		static char dhappy[16];
		static char dtime[15];
		static int rTime = 0;
		if (pet.timeleft > 60) { rTime = (pet.timeleft / 60); }
		
		//snprintf(displaystr, 10, "%s%s", totalProgress, "hrs");
		snprintf(dlevel, sizeof(dlevel), "%s: %d", "Level", pet.level);
		text_layer_set_text(text_level, dlevel);
		snprintf(dage, sizeof(dage), "%s: %d days", "Age", pet.age);
		text_layer_set_text(text_age, dage);
		snprintf(dhealth, sizeof(dhealth), "%s: %d%%", "Health", pet.health);
		text_layer_set_text(text_health, dhealth);
		snprintf(dhappy, sizeof(dhappy), "%s: %d%%", "Happiness", pet.happiness);
		text_layer_set_text(text_happy, dhappy);
		(rTime != 0) ? snprintf(dtime, sizeof(dtime), "%s: %dh", "To Grow", rTime) : snprintf(dtime, sizeof(dtime), "%s: %dm", "To Grow", pet.timeleft);
		text_layer_set_text(text_time, dtime);
	}
}

static void build_clock()
{
	windowMain = window_create();
	window_set_fullscreen(windowMain, true);
	window_set_click_config_provider(windowMain, click_config_provider);
	window_stack_push(windowMain, true);

	layer_main = window_get_root_layer(windowMain);
	bounds = layer_get_frame(layer_main);

	layers[0] = layer_create(bounds);
	layer_set_update_proc(layers[0], layer_update_callback);
	layer_add_child(layer_main, layers[0]);
	
	text_date_layer = text_layer_create(GRect(0, 0, SCREEN_WIDTH, 168-68));
	text_time_layer = text_layer_create(GRect(0, 25, SCREEN_WIDTH, 168-92)); //30

	text_layer_set_text_color(text_date_layer, GColorBlack);
	text_layer_set_background_color(text_date_layer, GColorClear);
	text_layer_set_font(text_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(text_date_layer, GTextAlignmentCenter);
	layer_add_child(layer_main, text_layer_get_layer(text_date_layer));

	text_layer_set_text_color(text_time_layer, GColorBlack);
	text_layer_set_background_color(text_time_layer, GColorClear);
	text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
	layer_add_child(layer_main, text_layer_get_layer(text_time_layer));

	GRect line_frame = GRect(10, 75, 120, 2); // Left, Top, Width, Height
	line_layer = layer_create(line_frame);
	layer_set_update_proc(line_layer, line_layer_update_callback);
	layer_add_child(layer_main, line_layer);
	
	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void build_stats()
{
	windowStats = window_create();
	window_set_fullscreen(windowStats, true);
	window_set_click_config_provider(windowStats, click_config_provider);
	//window_stack_push(windowStats, true);

	layer_stats = window_get_root_layer(windowStats);
	bounds = layer_get_frame(layer_stats);

	layers[1] = layer_create(bounds);
	layer_set_update_proc(layers[1], layer_update_callback);
	layer_add_child(layer_stats, layers[1]);

	text_age = text_layer_create(rows[0]);
	text_layer_set_text(text_age, "Loading...");
	text_layer_set_font(text_age, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(text_age, GTextAlignmentLeft);
	layer_add_child(layer_stats, text_layer_get_layer(text_age));

	text_happy = text_layer_create(rows[1]);
	text_layer_set_text(text_happy, "Loading...");
	text_layer_set_font(text_happy, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(text_happy, GTextAlignmentLeft);
	layer_add_child(layer_stats, text_layer_get_layer(text_happy));

	text_health = text_layer_create(rows[2]);
	text_layer_set_text(text_health, "Loading...");
	text_layer_set_font(text_health, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(text_health, GTextAlignmentLeft);
	layer_add_child(layer_stats, text_layer_get_layer(text_health));

	text_level = text_layer_create(rows[3]);
	text_layer_set_text(text_level, "Loading...");
	text_layer_set_font(text_level, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(text_level, GTextAlignmentLeft);
	layer_add_child(layer_stats, text_layer_get_layer(text_level));

	text_time = text_layer_create(rows[4]);
	text_layer_set_text(text_time, "Loading...");
	text_layer_set_font(text_time, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(text_time, GTextAlignmentLeft);
	layer_add_child(layer_stats, text_layer_get_layer(text_time));
}

static void init() {
	pet.level = LEVEL_D;
	pet.happiness = HAPPINESS_D;
	pet.health = HEALTH_D;
	pet.age = AGE_D;
	pet.timeleft = TIME_LEFT_D;
	pet.timealive = TIME_ALIVE_D;

	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);

	app_message_open(124, 256);

	rows[0] = GRect(5, 0, SCREEN_WIDTH, 30);
	rows[1] = GRect(5, 30, SCREEN_WIDTH, 30);
	rows[2] = GRect(5, 60, SCREEN_WIDTH, 30);
	rows[3] = GRect(5, 90, SCREEN_WIDTH, 30);
	rows[4] = GRect(5, 120, SCREEN_WIDTH, 30);

  	poo = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_POO);
	
	build_stats();
	build_clock();

	const uint32_t timeout_ms = 2000;
	accel_tap_service_subscribe(&accel_tap_handler);
	timer = app_timer_register(timeout_ms, timer_callback, NULL);
}

static void deinit() {
    accel_tap_service_unsubscribe();
	tick_timer_service_unsubscribe();

	text_layer_destroy(text_age);
	text_layer_destroy(text_happy);
	text_layer_destroy(text_health);
	text_layer_destroy(text_level);
	text_layer_destroy(text_time);
	text_layer_destroy(text_date_layer);
	text_layer_destroy(text_time_layer);

	layer_destroy(line_layer);
	layer_destroy(layers[0]);
	layer_destroy(layers[1]);

	window_stack_pop_all(false);
	window_destroy(windowMain);
	window_destroy(windowStats);
	switch(pet.level)
	{
	case 0:
		gbitmap_destroy(egg[0]);
		gbitmap_destroy(egg[1]);
	case 1:
		gbitmap_destroy(stage1[0]);
		gbitmap_destroy(stage1[1]);
		gbitmap_destroy(stage1[2]);
		gbitmap_destroy(stage1[3]);
		gbitmap_destroy(stage1[4]);
		gbitmap_destroy(stage1[5]);
		gbitmap_destroy(stage1[6]);
		break;
	case 2:
		break;
	case 3:
		gbitmap_destroy(stage3[0]);
		gbitmap_destroy(stage3[1]);
		gbitmap_destroy(stage3[2]);
		gbitmap_destroy(stage3[3]);
		gbitmap_destroy(stage3[4]);
		break;
	case 4:
		break;
	case 5:
		break;
	}
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
