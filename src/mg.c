
#include "math.h"
#include "mWin.h"
#include "base.h"
#include "mInput.h"
#include "mRend.h"
#include "mTex.h"
#include "mqoi.h"
#include "mSound.h"
#include "mAlloc.h"
#include "mProfiler.h"
#include "mui.h"

extern muiState m;

typedef enum {
	MG_MENU_START=1,
	MG_MENU_OPT,
	MG_MENU_PLAY,
	MG_MENU_HOWTO,
}mgMenuState;

static mTex tex_atlas;
static mSound pew_sound;
static mSound dead_sound;
static mSound enemy_dead_sound;
static mSound enemy_pew_sound;
static mSound powerup_sound;
static mSound menu_sound;
static mgMenuState menu_state = MG_MENU_START;
static int val = 6;
static int arcademode = 0;
static b32 game_paused = 0;
static b32 godmode = 0;
static u64 start_time = 0;
static u64 wave_timer = 0;
static u64 wave_counter = 0;
//---------PROJECTILE---------
typedef enum{
	MG_PLAYER_BULLET = 1,
	MG_ENEMY_BULLET,
	MG_POWERUP,
	MG_EXPLOSION,
}mgProjectileType;
typedef struct {
	v2 pos;
	u32 size;
	f32 speed;
	u64 spawn_time;
	mgProjectileType type;
}mgProjectile;
mgProjectile *projectiles = NULL;
void mg_projectile_update(mgProjectile *p, f32 dt){
	switch(p->type){
		case MG_PLAYER_BULLET:
			p->pos.y -= p->speed * dt;
			break;
		case MG_ENEMY_BULLET:
			p->pos.y += p->speed * dt;
			break;
		case MG_POWERUP:
			p->pos.y += p->speed * dt;
			break;
		case MG_EXPLOSION:
			if ((int)(mtime_sec(4*mtime_now())) % 2 == 0){
				p->pos.y += p->speed * dt;
			}
			break;
		default:
			break;
	}
}
void mg_projectile_render(mgProjectile *p){
	switch(p->type){
		case (MG_POWERUP):{
			if ((int)(mtime_sec(2*mtime_now())) % 2 == 0)
				mtex_render(&tex_atlas, (mRect){16*8,0,16,16}, (mRect){p->pos.x - p->size/2,p->pos.y - p->size/2,p->size,p->size});
			else
				mtex_render(&tex_atlas, (mRect){16*9,0,16,16}, (mRect){p->pos.x - p->size/2,p->pos.y - p->size/2,p->size,p->size});
		}break;
		case (MG_EXPLOSION):{
			if ((int)(mtime_sec(4*mtime_now() - 4*p->spawn_time)) % 2 == 0)
				mtex_render(&tex_atlas, (mRect){16*8,16*0,16,16}, (mRect){p->pos.x - p->size/2,p->pos.y - p->size/2,p->size,p->size});
			else
				mtex_render(&tex_atlas, (mRect){16*7,16*0,16,16}, (mRect){p->pos.x - p->size/2,p->pos.y - p->size/2,p->size,p->size});
		}break;
		case (MG_PLAYER_BULLET):{
				if ((int)(mtime_sec(2*mtime_now())) % 2 == 0)
					mtex_render(&tex_atlas, (mRect){16*9,16*15,16,16}, (mRect){p->pos.x - p->size/2,p->pos.y - p->size/2,p->size,p->size});
				else
					mtex_render(&tex_atlas, (mRect){16*10,16*15,16,16}, (mRect){p->pos.x - p->size/2,p->pos.y - p->size/2,p->size,p->size});
		}break;
		case (MG_ENEMY_BULLET):{
				if ((int)(mtime_sec(2*mtime_now())) % 2 == 0)
					mtex_render(&tex_atlas, (mRect){16*9,16*15,16,16}, (mRect){p->pos.x - p->size/2,p->pos.y - p->size/2,p->size,p->size});
				else
					mtex_render(&tex_atlas, (mRect){16*10,16*15,16,16}, (mRect){p->pos.x - p->size/2,p->pos.y - p->size/2,p->size,p->size});
		}break;
		default:
			break;
	}
	
}

void mg_projectiles_update(f32 dt){
	for (int i = 0; i < da_len(projectiles); ++i){
		mg_projectile_update(&projectiles[i], dt);
		f32 del_threshold = 10;
		if (projectiles[i].pos.y < (0 - del_threshold) || projectiles[i].pos.y > (600 + del_threshold)){
			da_del(projectiles, i);
		}
		if (mtime_sec(mtime_now() - projectiles[i].spawn_time) > 0.5 && projectiles[i].type == MG_EXPLOSION){
			da_del(projectiles, i);
		}
	}
}
void mg_projectiles_render(void){
	for (int i = 0; i < da_len(projectiles); ++i){
		mg_projectile_render(&projectiles[i]);
	}
}
//----------------------------


//------------SHIP------------
typedef struct{
	v2 pos;
	u32 size; //in pixels
	u64 last_shot_time; //to calc when it can reshoot
	f32 speed;

	f32 speed_multiplier;
	b32 dir;
}mgShip;
M_RESULT mg_ship_create(mgShip *s){
	s->pos = (v2){300,570};
	s->size = 40;
	s->last_shot_time = 0x0;
	s->speed = 200;
	s->speed_multiplier;
	s->dir = 0;
	return M_OK;
}
b32 mg_ship_isect(mgShip *s);
void mg_ship_update(mgShip *s, f32 dt){
	if (s->pos.x > 600){
		s->pos.x = 0 + s->size / 2;
	}
	if (s->pos.x < 0){
		s->pos.x = 600 - s->size / 2;
	}
	if (mkey_down(MK_A)){
		s->pos.x -= s->speed_multiplier * s->speed * dt;
		s->dir = 0;
	}
	if (mkey_down(MK_D)){
		s->pos.x += s->speed_multiplier * s->speed * dt;
		s->dir = 1;
	}
	if (mkey_down(MK_A) || mkey_down(MK_D)){
		s->speed_multiplier += ((s->speed) * dt) / 20;
	}else{
		s->speed_multiplier -= ((s->speed) * dt) / 10;
	}
	s->speed_multiplier = MAX(1, MIN(s->speed_multiplier, 8));

	//s->pos.x = minput_get_mouse_pos().x;

	if (mkey_pressed(MK_W) && mtime_sec(mtime_now()-s->last_shot_time) > 0.1){
		msound_play(&pew_sound);
		s->last_shot_time = mtime_now();
		mgProjectile p = (mgProjectile){(v2){s->pos.x, s->pos.y+10},30,300,mtime_now(),MG_PLAYER_BULLET};
		da_push(projectiles, p);
	}
	mg_ship_isect(s);
}
void mg_ship_render(mgShip *s){
	if ((int)(mtime_sec(2*mtime_now())) % 2 == 0)
		mtex_render(&tex_atlas, (mRect){16*15,0,16,16}, (mRect){s->pos.x - s->size/2,s->pos.y - s->size/2,s->size,s->size});
	else
		mtex_render(&tex_atlas, (mRect){16*10,16*2,16,16}, (mRect){s->pos.x - s->size/2,s->pos.y - s->size/2,s->size,s->size});
	
}
static mgShip ship;
//----------------------------

//------------ENEMY------------
typedef struct{
	v2 prev_pos;
	v2 pos;
	u32 size; //in pixels
	u64 last_shot_time; //to calc when it can reshoot
	f32 speed;

	v2 start_pos;
	v2 end_pos;

	f32 speed_multiplier;
	b32 dir;
}mgEnemy;
M_RESULT mg_enemy_create(mgEnemy *s){
	s->pos = (v2){200,280};
	s->prev_pos = s->pos;
	s->size = 30;
	s->last_shot_time = 0x0;
	s->speed = 240;
	s->speed_multiplier;
	s->dir = 0;
	return M_OK;
}
void mg_enemy_update(mgEnemy *s, f32 dt){
	if ((int)(mtime_sec(4*mtime_now())) % 2 == 0){
		s->prev_pos = s->pos;
		s->pos.y += s->speed * dt;
		s->pos.y = MAX(s->start_pos.y, MIN(s->pos.y, s->end_pos.y));
	}
	RND_SEED(mtime_now());
	f32 p = 10;
	if (mtime_sec(mtime_now()) > 5.0)p--;
	if (mtime_sec(mtime_now()) > 15.0)p--;
	if (mtime_sec(mtime_now()) > 35.0)p--;
	if (mtime_sec(mtime_now()) > 60.0)p--;
	if (mtime_sec(mtime_now()) > 120.0)p--;
	if (dt > 0.0001 && RND() % (u32)(p * dt * 1000000) == 0){
		mgProjectile p = (mgProjectile){(v2){s->pos.x, s->pos.y},30,300,mtime_now(),MG_ENEMY_BULLET};
		msound_play(&enemy_pew_sound);
		da_push(projectiles, p);
	}
}
void mg_enemy_render(mgEnemy *s){
	if ((int)(mtime_sec(2*mtime_now())) % 2 == 0)
		mtex_render(&tex_atlas, (mRect){16,0,16,16}, (mRect){s->pos.x - s->size/2,s->pos.y - s->size/2,s->size,s->size});
	else
		mtex_render(&tex_atlas, (mRect){0,16*4,16,16}, (mRect){s->pos.x - s->size/2,s->pos.y - s->size/2,s->size,s->size});
}
static mgEnemy *enemies = NULL;
b32 mg_circle_isect(f32 xa, f32 ya, f32 xc, f32 yc, f32 r){
   return ((xa-xc)*(xa-xc) + (ya-yc)*(ya-yc)) < r*r;
}
b32 mg_enemy_bullet_isect(mgEnemy *e){
	for (u32 i = 0; i < da_len(projectiles); ++i){
		if (projectiles[i].type == MG_PLAYER_BULLET &&  mg_circle_isect(e->pos.x,e->pos.y, projectiles[i].pos.x, projectiles[i].pos.y, 20)){
			da_del(projectiles, i);
			return TRUE;
		}
	}
	return FALSE;
}
b32 mg_ship_isect(mgShip *s){
	for (u32 i = 0; i < da_len(projectiles); ++i){
		if (!godmode && projectiles[i].type == MG_ENEMY_BULLET &&  mg_circle_isect(s->pos.x,s->pos.y, projectiles[i].pos.x, projectiles[i].pos.y, 20)){
			menu_state = MG_MENU_START;
			msound_play(&dead_sound);
			da_free(projectiles);
			da_free(enemies);
			return TRUE;
		}else if (projectiles[i].type == MG_POWERUP &&  mg_circle_isect(s->pos.x,s->pos.y, projectiles[i].pos.x, projectiles[i].pos.y, 20)){
			for (int i = 0; i < da_len(enemies); ++i){
				f32 exp_speed = (enemies[i].pos.y - enemies[i].prev_pos.y < 0.1) ? 0 : 200;
				mgProjectile p = (mgProjectile){(v2){enemies[i].pos.x, enemies[i].pos.y},27,exp_speed,mtime_now(),MG_EXPLOSION};
				da_push(projectiles, p);
			}
			da_free(enemies);
			da_del(projectiles, i);
			msound_play(&powerup_sound);
			return TRUE;
		}
	}
	return FALSE;
}
typedef enum {
	MG_ENEMY_FORMATION_PI=0,
	MG_ENEMY_FORMATION_OW,
	MG_ENEMY_FORMATION_LAMBDA,
	MG_ENEMY_FORMATION_V,
	MG_ENEMY_FORMATION_COUNT,
}MG_ENEMY_WAVE_FORMATION;

char *fbitmaps[MG_ENEMY_FORMATION_COUNT] = {
	"1001100110011111",
	"1111100110011111",
	"0000011010011001",
	"0000100110010110",
};
void mg_spawn_enemy_wave(MG_ENEMY_WAVE_FORMATION f, char sym){
	RND_SEED(mtime_now());
	f32 pos_x = (f32)100 + 200 * (RND() % 3);//(3 + RND() % 3) * 100;
	for (u32 i = 0; i < 4; ++i){
		for (u32 j = 0; j < 4; ++j){
			if (fbitmaps[f][i * 4 + j] == sym){
				//RND_SEED(mtime_now());
				mgEnemy e;
				mg_enemy_create(&e);
				e.speed = 200;
				//e.pos.x = RND() % 600;
				e.pos.x = pos_x - 2 * e.size + j * e.size;
				e.pos.y = 0 - (f32)e.size * i;
				e.start_pos = e.pos;
				e.end_pos = (v2){e.pos.x, e.pos.y + 300};
				da_push(enemies, e);
			}
		}
	}
}
void mg_enemies_update(f32 dt){
	wave_timer = mtime_now() - start_time;
	int c = 5;
	if (mtime_sec(mtime_now()) > 5.0)c++;
	if (mtime_sec(mtime_now()) > 15.0)c++;
	if (mtime_sec(mtime_now()) > 35.0)c++;
	b32 new_wave = (wave_counter*(u64)c == (u64)(mtime_sec(wave_timer)));
	if (new_wave){
		wave_counter++;
	}
	if (mkey_pressed(MK_Q) || new_wave){
		RND_SEED(mtime_now());
		mg_spawn_enemy_wave(RND() % MG_ENEMY_FORMATION_COUNT, '0' + RND() % 2);
	}
	for (u32 i = 0; i < da_len(enemies); ++i){
		mg_enemy_update(&enemies[i], dt);
		if (mg_enemy_bullet_isect(&enemies[i])){
			//spawn animation for dead enemy
			//msound_play(&snd);
			f32 exp_speed = (enemies[i].pos.y - enemies[i].prev_pos.y < 0.1) ? 0 : 200;
			mgProjectile p = (mgProjectile){(v2){enemies[i].pos.x, enemies[i].pos.y},27,exp_speed,mtime_now(),MG_EXPLOSION};
			da_push(projectiles, p);
			RND_SEED(mtime_now());
			if (RND() % 5 == 0){
				mgProjectile p = (mgProjectile){(v2){enemies[i].pos.x, enemies[i].pos.y},15,300,mtime_now(),MG_POWERUP};
				da_push(projectiles, p);
			}
			msound_play(&enemy_dead_sound);
			da_del(enemies, i); //TODO does that change the for loop though??? investigate!!!!!
		}
	}
}
void mg_enemies_render(){
	for (u32 i = 0; i < da_len(enemies); ++i){
		mg_enemy_render(&enemies[i]);
	}
}
//-----------------------------

void mg_draw_ui(void){
	switch(menu_state){
		case(MG_MENU_START):
			{
				mui_panel_begin(&m, (iv2){300 - m.style.button_size_x/2,200});
				if (mui_button(&m,__LINE__, "PLAY")){
					menu_state = MG_MENU_PLAY;	
					msound_play(&menu_sound);
					start_time = mtime_now();
					wave_counter = 0;
				}
				if (mui_button(&m,__LINE__, "HOWTO")){
					menu_state = MG_MENU_HOWTO;	
					msound_play(&menu_sound);
				}
				if (mui_button(&m,__LINE__, "OPTIONS")){
					menu_state = MG_MENU_OPT;	
					msound_play(&menu_sound);
				}
				mui_panel_end(&m);
			}break;
			case(MG_MENU_OPT):
			{
				mui_panel_begin(&m, (iv2){300 - m.style.button_size_x/2,200});
				static int val2 = 7;
				mui_slider(&m,__LINE__, "VOLUME", &val2, 0, 10);
				if (mui_checkbox(&m,__LINE__, "ARCADE", &arcademode)){	
					msound_play(&menu_sound);
				}
				if (mui_button(&m,__LINE__, "BACK")){
					menu_state = MG_MENU_START;	
					msound_play(&menu_sound);
				}
				mui_panel_end(&m);
			}break;
			case(MG_MENU_HOWTO):
			{
				mui_panel_begin(&m, (iv2){300 - m.style.button_size_x/2,200});
				mui_label(&m, __LINE__, "MOVE:AD");
				mui_label(&m, __LINE__, "SHOOT:W");
				if (mui_button(&m,__LINE__, "BACK")){
					menu_state = MG_MENU_START;	
					msound_play(&menu_sound);
				}
				mui_panel_end(&m);
			}break;
			case(MG_MENU_PLAY):
			{
				mui_panel_begin(&m, (iv2){300 - 3*(m.style.button_size_x+5)/2,5});
				mui_layout_push(&m, MUI_HORIZONTAL_LAYOUT);
				mui_checkbox(&m, __LINE__, "GODMODE", &godmode);
				mui_checkbox(&m, __LINE__, "PAUSE", &game_paused);
				if(mui_button(&m, __LINE__, "RESTART")){
					menu_state = MG_MENU_START;
					da_free(enemies);
					da_free(projectiles);	
					msound_play(&menu_sound);
				}
				mui_layout_pop(&m);
				mui_panel_end(&m);
			}break;
		default:
			break;
	}
}
void mg_init(void){
	mtime_init(); //TODO delete ASAP
	msound_load(&(mSoundDesc){"../assets/pew.wav"}, &pew_sound);
	msound_load(&(mSoundDesc){"../assets/dead.wav"}, &dead_sound);
	msound_load(&(mSoundDesc){"../assets/enemy_pew.wav"}, &enemy_pew_sound);
	msound_load(&(mSoundDesc){"../assets/POWERUP.wav"}, &powerup_sound);
	msound_load(&(mSoundDesc){"../assets/menu.wav"}, &menu_sound);
	msound_load(&(mSoundDesc){"../assets/enemy_dead.wav"}, &enemy_dead_sound);
	tex_atlas = *((mTex*)(m.texture_atlas)); //already initialized via mui, no need to copy
	mg_ship_create(&ship);
}
void mg_update(f32 dt){
	dt = (game_paused) ? 0 : dt;
	if (menu_state == MG_MENU_PLAY){
		if (mkey_pressed(MK_S)){
			RND_SEED(mtime_now());
			mgEnemy e;
			mg_enemy_create(&e);
			e.speed = 100;
			e.pos.x = RND() % 600;
			e.start_pos = e.pos;
			e.end_pos = (v2){e.pos.x, e.pos.y + 100};
			da_push(enemies, e);
		}
		mg_ship_update(&ship, dt);
		mg_enemies_update(dt);
		mg_projectiles_update(dt);
	}
}
void mg_render(void){
	if (menu_state == MG_MENU_PLAY){
		mg_ship_render(&ship);
		mg_enemies_render();
		mg_projectiles_render();
	}
	mg_draw_ui();
}