#include <lt2/common.h>
#include <lt2/window.h>
#include <lt2/log.h>
#include <lt2/async.h>
#include <lt2/time.h>
#include <lt2/debug.h>
#include <lt2/pixbuf.h>

#include <stdlib.h>

#undef X
#define X 0xFFF0F0F0,
#define _ 0xFF000000,

pixbuf_t player_image = {
	.width  = 16,
	.height = 16,
	.data   = (u32[]) {
		_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
		_ X X X X X X X X X X X X X X _
		_ X X X X X X X X X X X X X X _
		_ X X _ _ _ _ _ _ _ _ _ _ X X _
		_ X X _ _ _ _ _ _ _ _ _ _ X X _
		_ X X _ _ _ _ _ _ _ _ _ _ X X _
		_ X X _ _ _ _ _ _ _ _ _ _ X X _
		_ X X _ _ _ _ _ _ _ _ _ _ X X _
		_ X X _ _ _ _ _ _ _ _ _ _ X X _
		_ X X _ _ _ _ _ _ _ _ _ _ X X _
		_ X X _ _ _ _ _ _ _ _ _ _ X X _
		_ X X _ _ _ _ _ _ _ _ _ _ X X _
		_ X X _ _ _ _ _ _ _ _ _ _ X X _
		_ X X X X X X X X X X X X X X _
		_ X X X X X X X X X X X X X X _
		_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
	},
};

// ----- camera

i32 camera_pos_x = 0.0;
i32 camera_pos_y = 0.0;

f64 ui_scale = 1.4;

void wdraw_rect(i32 x, i32 y, i32 w, i32 h, u32 color) {
	x -= camera_pos_x;
	y -= camera_pos_y;
	draw_rect(x, y, w, h, color);
}

void wfill_rect(i32 x, i32 y, i32 w, i32 h, u32 color) {
	x -= camera_pos_x;
	y -= camera_pos_y;
	fill_rect(x, y, w, h, color);
}

INLINE
void ui_draw_rect(i32 x, i32 y, i32 w, i32 h, u32 color) {
	draw_rect(x * ui_scale, y * ui_scale, w * ui_scale, h * ui_scale, color);
}

INLINE
void ui_fill_rect(i32 x, i32 y, i32 w, i32 h, u32 color) {
	fill_rect(x * ui_scale, y * ui_scale, w * ui_scale, h * ui_scale, color);
}


// ----- input

u8 key_states[256];
u8 prev_key_states[256];
u8 button_states[16];
u8 prev_button_states[16];

b8 key_pressed(u8 key) {
	return key_states[key] && !prev_key_states[key];
}

void poll_input() {
	wevent_t events[16];
	usz event_count = poll_wevents(events, COUNT_OF(events));

	memcpy(prev_key_states,    key_states,    sizeof(key_states));
	memcpy(prev_button_states, button_states, sizeof(button_states));

	for (wevent_t* ev = events, *end = ev + event_count; ev < end; ++ev) {
		switch (ev->type) {
		case WEV_MOTION:
			break;

		case WEV_BUTTON_PRESS:
			button_states[ev->button.code] = 1;
			break;

		case WEV_BUTTON_RELEASE:
			button_states[ev->button.code] = 0;
			break;

		case WEV_KEY_PRESS:
			key_states[ev->key.code] = 1;
			break;

		case WEV_KEY_RELEASE:
			key_states[ev->key.code] = 0;
			break;

		case WEV_QUIT:
			llogf(NULL, LOG_INFO, "quit");
			exit(0);
			break;

		case WEV_RESIZE:
			llogf(NULL, LOG_INFO, "resize");
			break;
		}
	}
}

// ----- stat

enum stat_id {
	S_MAX_HP = 0,
	S_MAX_MP = 1,
	S_MAX_SP = 2,
	S_STR    = 3,
	S_DEX    = 4,
	S_CON    = 5,
	S_INT    = 6,
	S_WIS    = 7,
	S_CHA    = 8,
	S_LIFE   = 9,
	S_HEIGHT = 10,
	S_WEIGHT = 11,
	S_MORAL  = 12,

	S_COUNT,
};

ls stat_names[S_COUNT] = {
	[S_MAX_HP] = ls("Max HP"),
	[S_MAX_MP] = ls("Max MP"),
	[S_MAX_SP] = ls("Max EP"),
	[S_STR]    = ls("Strength"),
	[S_DEX]    = ls("Dexterity"),
	[S_CON]    = ls("Constitution"),
	[S_INT]    = ls("Intelligence"),
	[S_WIS]    = ls("Wisdom"),
	[S_CHA]    = ls("Charisma"),
	[S_LIFE]   = ls("Life span"),
	[S_HEIGHT] = ls("Height"),
	[S_WEIGHT] = ls("Weight"),
	[S_MORAL]  = ls("Moralistic"),
};

typedef struct stat {
	i16 avg;
	i16 dev;
	i16 sex;
} stat_t;

i16 generate_stat_val(i8 sex, stat_t* stat) {
	i16 dev  = (rand() % (stat->dev * 2)) - stat->dev;
	i16 sdev = sdev = stat->sex * sex;
	return stat->avg + dev + sdev;
}

// ----- creature

enum species_id {
	SP_HUMAN  = 0,
	SP_DWARF  = 1,
	SP_ELF    = 2,
	SP_GOBLIN = 3,

	SP_COUNT,
};

#define SPF_CAN_SPEAK (1 << 0)
#define SPF_ASEXUAL   (1 << 1)

typedef struct species {
	ls name;
	i16 child_age, grown_age, old_age;
	u16 flags;
	stat_t stats[S_COUNT];
} species_t;

species_t species[] = {
	[SP_HUMAN] = {
		.name = ls("Human"),
		.child_age = 2, .grown_age = 18, .old_age = 40,
		.flags = SPF_CAN_SPEAK,
		.stats = {
			[S_MAX_HP] = { .avg = 100, .dev = 10, .sex = -10 },
			[S_MAX_MP] = { .avg = 100, .dev = 10, .sex = +5  },
			[S_MAX_SP] = { .avg = 100, .dev = 10, .sex = -5  },
			[S_STR]    = { .avg = 20,  .dev = 10, .sex = -3  },
			[S_DEX]    = { .avg = 20,  .dev = 10, .sex = +2  },
			[S_CON]    = { .avg = 20,  .dev = 10, .sex = -1  },
			[S_INT]    = { .avg = 20,  .dev = 10 },
			[S_WIS]    = { .avg = 20,  .dev = 10 },
			[S_CHA]    = { .avg = 20,  .dev = 10, .sex = +1  },
			[S_LIFE]   = { .avg = 90,  .dev = 10, .sex = +5 },
			[S_HEIGHT] = { .avg = 178, .dev = 30, .sex = -8 },
			[S_WEIGHT] = { .avg = 70,  .dev = 15, .sex = -4  },
			[S_MORAL]  = { .avg = 20,  .dev = 15, .sex = +1  },
		},
	},

	[SP_DWARF] = {
		.name = ls("Dwarf"),
		.child_age = 3, .grown_age = 23, .old_age = 80,
		.flags = SPF_CAN_SPEAK,
		.stats = {
			[S_MAX_HP] = { .avg = 110, .dev = 15, .sex = -5  },
			[S_MAX_MP] = { .avg = 90,  .dev = 10, .sex = +5  },
			[S_MAX_SP] = { .avg = 90,  .dev = 10, .sex = -5  },
			[S_STR]    = { .avg = 25,  .dev = 10, .sex = -2  },
			[S_DEX]    = { .avg = 15,  .dev = 10, .sex = +2  },
			[S_CON]    = { .avg = 22,  .dev = 10, .sex = +1  },
			[S_INT]    = { .avg = 23,  .dev = 10 },
			[S_WIS]    = { .avg = 18,  .dev = 10 },
			[S_CHA]    = { .avg = 18,  .dev = 10, .sex = -1  },
			[S_LIFE]   = { .avg = 140, .dev = 20, .sex = +8  },
			[S_HEIGHT] = { .avg = 140, .dev = 20, .sex = -5  },
			[S_WEIGHT] = { .avg = 65,  .dev = 10, .sex = -5  },
			[S_MORAL]  = { .avg = 25,  .dev = 15, .sex = -2  },
		},
	},

	[SP_ELF] = {
		.name = ls("Elf"),
		.child_age = 3, .grown_age = 24, .old_age = 250,
		.flags = SPF_CAN_SPEAK,
		.stats = {
			[S_MAX_HP] = { .avg = 85,  .dev = 15, .sex = -10 },
			[S_MAX_MP] = { .avg = 110, .dev = 10 },
			[S_MAX_SP] = { .avg = 110, .dev = 10 },
			[S_STR]    = { .avg = 20,  .dev = 10 },
			[S_DEX]    = { .avg = 25,  .dev = 10 },
			[S_CON]    = { .avg = 15,  .dev = 10 },
			[S_INT]    = { .avg = 22,  .dev = 10 },
			[S_WIS]    = { .avg = 22,  .dev = 10 },
			[S_CHA]    = { .avg = 24,  .dev = 10 },
			[S_LIFE]   = { .avg = 300, .dev = 50, .sex = +15 },
			[S_HEIGHT] = { .avg = 173, .dev = 25, .sex = -7 },
			[S_WEIGHT] = { .avg = 65,  .dev = 10, .sex = -7  },
			[S_MORAL]  = { .avg = 20,  .dev = 13 },
		},
	},

	[SP_GOBLIN] = {
		.name = ls("Goblin"),
		.child_age = 1, .grown_age = 15, .old_age = 30,
		.flags = SPF_CAN_SPEAK,
		.stats = {
			[S_MAX_HP] = { .avg = 50,  .dev = 20, .sex = -5  },
			[S_MAX_MP] = { .avg = 80,  .dev = 10 },
			[S_MAX_SP] = { .avg = 90,  .dev = 10 },
			[S_STR]    = { .avg = 13,  .dev = 10 },
			[S_DEX]    = { .avg = 19,  .dev = 10 },
			[S_CON]    = { .avg = 14,  .dev = 10 },
			[S_INT]    = { .avg = 17,  .dev = 10 },
			[S_WIS]    = { .avg = 16,  .dev = 10 },
			[S_CHA]    = { .avg = 17,  .dev = 10 },
			[S_LIFE]   = { .avg = 65,  .dev = 6,  .sex = +8  },
			[S_HEIGHT] = { .avg = 130, .dev = 25, .sex = -6 },
			[S_WEIGHT] = { .avg = 35,  .dev = 7,  .sex = -2  },
			[S_MORAL]  = { .avg = 10,  .dev = 10 },
		},
	},
};

#define CF_PLAYER (1 << 0)
#define CF_BLIND  (1 << 1)
#define CF_DEAF   (1 << 2)
#define CF_MUTE   (1 << 3)
#define CF_DEAD   (1 << 4)
#define CF_HYBRID (1 << 5)

#define SEX_MALE   (-1)
#define SEX_NONE   (0)
#define SEX_FEMALE (+1)

ls sex_display_name(i8 sex) {
	if (sex == SEX_MALE)
		return ls("Male");
	if (sex == SEX_FEMALE)
		return ls("Female");
	return ls("");
}

typedef struct creature {
	u8 species, other_species;
	i8 sex;
	u16 pad;
	u32 flags;
	i32 pos_x, pos_y;
	i16 hp, mp, sp;
	i16 stat_vals[S_COUNT];
} creature_t;

creature_t generate_creature(i8 sex, u8 species_id) {
	if (species_id == 0xFF)
		species_id = rand() % SP_COUNT;
	species_t* sp = &species[species_id];

	if (sp->flags & SPF_ASEXUAL) {
		sex = SEX_NONE;
	}
	else if (sex == SEX_NONE) {
		if (rand() % 2 == 1)
			sex = SEX_MALE;
		else
			sex = SEX_FEMALE;
	}

	creature_t creature = {
		.species = species_id,
		.sex = sex,
	};
	for (usz i = 0; i < S_COUNT; ++i)
		creature.stat_vals[i] = generate_stat_val(sex, &sp->stats[i]);

	creature.hp = creature.stat_vals[S_MAX_HP];
	creature.mp = creature.stat_vals[S_MAX_MP];
	creature.sp = creature.stat_vals[S_MAX_SP];

	return creature;
}

void print_creature_info(creature_t* creature) {
	lprintf("{ls} {ls}\n", species[creature->species].name, sex_display_name(creature->sex));
	for (usz i = 0; i < S_COUNT; ++i)
		lprintf("{ls}: {u16}\n", stat_names[i], creature->stat_vals[i]);
}

// ----- dialogue

typedef struct dialogue_state {
	task t;
} dialogue_state;

void speak_1(dialogue_state* state) {
	co_reenter(&state->t);

	goto first;
	do {
		co_yield();
	first:
		fill_rect(100, 100, 200, 200, 0xFF000000);
	} while (!key_pressed(KEY_ENTER));

	do {
		co_yield();
		fill_rect(100, 100, 200, 200, 0xFF000088);
	} while (!key_pressed(KEY_ENTER));

	do {
		co_yield();
		fill_rect(100, 100, 200, 200, 0xFF008800);
	} while (!key_pressed(KEY_ENTER));
}

// ----- game loop

creature_t player;

void on_frame() {
	sleep_us(1000);

	fill_rect(0, 0, window_width, window_height, 0xFF1a1a1a);

	poll_input();

	if (key_pressed('R')) {
		player = generate_creature(0, 0xFF);
		print_creature_info(&player);
	}

	static u64 prev_time_ns;
	f64 cur_time_ns = time_ns();

	f64 delta = (f64)(cur_time_ns - prev_time_ns) * (1.0 / 1000000000.0);
	(void)delta;
	prev_time_ns = cur_time_ns;


	static f32 px = 200;
	static f32 py = 200;

	px += (key_states['D'] - key_states['A']) * delta * 200.0;
	py += (key_states['S'] - key_states['W']) * delta * 200.0;

	put_pixbuf(px, py, &player_image);


#define BAR_HEIGHT 13
#define BAR_WIDTH  200

#define BAR_POSX 10
#define BAR_POSY 10

	f64 hp_fac = (f64)player.hp / (f64)player.stat_vals[S_MAX_HP];
	f64 mp_fac = (f64)player.mp / (f64)player.stat_vals[S_MAX_MP];
	f64 sp_fac = (f64)player.sp / (f64)player.stat_vals[S_MAX_SP];

	ui_fill_rect(BAR_POSX, (BAR_POSY + (BAR_HEIGHT + 5) * 0), BAR_WIDTH,          BAR_HEIGHT, 0xFF440000);
	ui_fill_rect(BAR_POSX, (BAR_POSY + (BAR_HEIGHT + 5) * 0), BAR_WIDTH * hp_fac, BAR_HEIGHT, 0xFFEE0000);
	ui_draw_rect(BAR_POSX, (BAR_POSY + (BAR_HEIGHT + 5) * 0), BAR_WIDTH,          BAR_HEIGHT, 0xFF000000);

	ui_fill_rect(BAR_POSX, (BAR_POSY + (BAR_HEIGHT + 5) * 1), BAR_WIDTH,          BAR_HEIGHT, 0xFF002244);
	ui_fill_rect(BAR_POSX, (BAR_POSY + (BAR_HEIGHT + 5) * 1), BAR_WIDTH * mp_fac, BAR_HEIGHT, 0xFF0088BB);
	ui_draw_rect(BAR_POSX, (BAR_POSY + (BAR_HEIGHT + 5) * 1), BAR_WIDTH,          BAR_HEIGHT, 0xFF000000);

	ui_fill_rect(BAR_POSX, (BAR_POSY + (BAR_HEIGHT + 5) * 2), BAR_WIDTH,          BAR_HEIGHT, 0xFF443300);
	ui_fill_rect(BAR_POSX, (BAR_POSY + (BAR_HEIGHT + 5) * 2), BAR_WIDTH * sp_fac, BAR_HEIGHT, 0xFFEEAA00);
	ui_draw_rect(BAR_POSX, (BAR_POSY + (BAR_HEIGHT + 5) * 2), BAR_WIDTH,          BAR_HEIGHT, 0xFF000000);

	static dialogue_state dialogue;
	if (dialogue.t.running || key_states['E'])
		speak_1(&dialogue);

	window_present();
}

int main(int argc, char** argv) {
	default_log_sink->file.color = 1;

	set_root_frame();
	add_debug_hooks(err_warn);

	window_init(err_warn);

	for (;;)
		on_frame();
	return 0;
}

