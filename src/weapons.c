/*
 *  __      _ .__  .__
 * /  \    / \(__) (__)  ____ ______  ____
 * \   \/\/  /|  | |  |_/ __ \\_  _ \/  _ \ 
 *  \       //   |/   |\  ___/ |  |\(  <_> )
 *   \__/\_/ \__ \\__ \ \___  >|__|  \____/ 
 *              \/   \/     \/ By BScrk
 * Copyright (C) 2008-2009 Luca Benevolo <wiiero@free.fr>
 *
 * Original game concept (Liero) : Joosa Riekkinen in 1998
 * Wiiero is Liero game clone developed for Wii platform.
 * Wiiero is also avaiable on PSP and PC (linux/Windows) platform.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "weapons.h"
#include "tools.h"
#include "surface_manip.h"
#include "camera.h"
#include "player.h" 
#include "object.h" 
#include "dynamic_objs.h"
#include "fast_trig.h"
#include "font.h"
#include "sound_engine.h"
#include "lang.h"
#define DEBUG_ELEMENT_POS   put_pix_color(c->cam_surface,o->pos_x-c->map_x,o->pos_y-c->map_y,255,0,0);

typedef struct{
  Uint8  show_bonus_name;
  Uint8  reload_percent;
}weapon_opt_t;


static weapon_opt_t weapon_round_option;
extern player_score_t game_score[NB_PLAYERS];
extern Uint8 transparent_r_value;
extern Uint8 transparent_g_value;
extern Uint8 transparent_b_value;
extern Uint8 mud_particle;


void weapon_set_options(Uint8 show_bonus_name,Uint8 reload_percent){
  weapon_round_option.show_bonus_name = show_bonus_name;
  weapon_round_option.reload_percent  = reload_percent;
}


typedef bullet_t* (*creation_callback)(weapon_id w_id,player_t *p,int acc_x,int acc_y);
bullet_t* init_with_skin( weapon_id w_id,int posx,int posy,int accx,int accy,SDL_Surface * skin,void* p_origin);
  
/* *** BULLET CALLBACKS  *** */
bullet_t* create_bullet(weapon_id w_id,player_t *p,int acc_x,int acc_y);
bullet_t* create_mg_bullet(weapon_id w_id,player_t *p,int acc_x,int acc_y); // minigun
int bullet_collision_cb(SDL_Surface* ground,int pos_x,int pos_y,int acc_x,int acc_y);
void bullet_blit_cb(camera_t* c,void* obj,void* userdata);
void bullet_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* null,void* null2, void* userdata);
/* *** SHOOTGUNS CALLBACKS *** */
bullet_t* create_shootgun_bullet(weapon_id w_id,player_t *p,int acc_x,int acc_y);
bullet_t* create_super_shootgun_bullet(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void shootgun_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata);
/* *** RIFLE CALLBACK *** */
void rifle_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata);
/* *** CANNON *** */
bullet_t* create_cannonball(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void cannonball_blit_cb(camera_t* c,void* obj,void* userdata);


/* *** GRASS HOPPER *** */
void grass_hopper_blit_cb(camera_t* c,void* obj,void* userdata);
bullet_t* create_grass_hopper(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void grass_hopper_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata);
void grass_hopper_update(void* bullet,void *p1,void *p2, void* userdata);


/* *** MINE *** */
void mine_blit_cb(camera_t* c,void* obj,void* userdata);
bullet_t* create_mine(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void mine_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata);
void ceiling_mine_update(void* bullet,void *p1,void *p2, void* userdata);

/* *** SPIKE TIME BOMB *** */
void spike_tbomb_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata);

/* *** BOUNCY MINE *** */
void bouncy_mine_blit_cb(camera_t* c,void* obj,void* userdata);
bullet_t* create_bouncy_mine(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void bouncy_mine_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata);
                                 
/* *** FLOAT MINE *** */
void float_mine_blit_cb(camera_t* c,void* obj,void* userdata);
bullet_t* create_float_mine(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void float_mine_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata);
void float_mine_update_cb(void* bullet,void *p1,void *p2, void* userdata);

/* *** NINJA HOOK *** */
bullet_t* create_ninja_hook(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void ninja_hook_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata);
void ninja_hook_blit_cb(camera_t* c,void* obj,void* userdata);
void ninja_hook_update_cb(void* bullet,void *p1,void *p2, void* userdata);


/* *** GRENADE *** */
void grenade_blit_cb(camera_t* c,void* obj,void* userdata);
bullet_t* create_grenade(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void grenade_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                            , void* p1,void* p2, void* userdata);
      
/* *** GAUSS *** */
void gauss_blit_cb(camera_t* c,void* obj,void* userdata);
bullet_t* create_gauss(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void gauss_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                            , void* p1,void* p2, void* userdata);
    
/* *** CLUSTER BOMB *** */
void cluster_bomb_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 , void* p1,void* p2, void* userdata);

/* *** CLUSTER PIECE *** */
void cluster_piece_blit_cb(camera_t* c,void* obj,void* userdata);
void cluster_piece_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata);

/* *** ZIMM *** */
void zimm_blit_cb(camera_t* c,void* obj,void* userdata);
bullet_t* create_zimm(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void zimm_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata);

/* *** HOLY_GRENADE *** */
void holy_grenade_blit_cb(camera_t* c,void* obj,void* userdata);
bullet_t* create_holy_grenade(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void holy_grenade_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata);

/* *** GRASSHOPPER grenade *** */
void gh_grenade_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                               , void* p1,void* p2, void* userdata);

/* *** AN-M14 Grenade *** */
bullet_t* create_red_grenade(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void an_m14_grenade_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                   , void* p1,void* p2, void* userdata);

/* *** GAZ Grenade *** */
bullet_t* create_white_grenade(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void gaz_grenade_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                           , void* p1,void* p2, void* userdata);
void gaz_grenade_update(void* bullet,void *p1,void *p2, void* userdata);

/* *** FLAME & FIRE *** */
void fire_blit_cb(camera_t* c,void* obj,void* userdata);
void flame_blit_cb(camera_t* c,void* obj,void* userdata);

bullet_t* create_flame(weapon_id w_id,player_t *p,int acc_x,int acc_y);
bullet_t* create_blue_flame(weapon_id w_id,player_t *p,int acc_x,int acc_y);

void flame_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata);
void fire_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata); 
void fire_update(void* bullet,void *p1,void *p2, void* userdata);
 
 
/* *** GAZ *** */
bullet_t* create_gaz(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void gaz_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata); 
void gaz_blit_cb(camera_t* c,void* obj,void* userdata);
void gaz_update(void* bullet,void *p1,void *p2, void* userdata);

/* *** SPIKE *** */
void spike_blit_cb(camera_t* c,void* obj,void* userdata);
bullet_t* create_spike(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void spike_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata);
 
/* *** CHIQUITA BOMB *** */ 
void chiquita_blit_cb(camera_t* c,void* obj,void* userdata);
bullet_t* create_chiquita(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void chiquita_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata);
void chiquita_piece_blit_cb(camera_t* c,void* obj,void* userdata);
void chiquita_piece_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata);
void sticky_chiquita_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata);
/* *** Blaster *** */
void blaster_blit_cb(camera_t* c,void* obj,void* userdata);
bullet_t* create_blaster(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void blaster_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata); 

/* *** RB Rampage *** */
void rb_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata); 
 
/* *** HELLRAIDER *** */
void hellraider_blit_cb(camera_t* c,void* obj,void* userdata);
bullet_t* create_hellraider(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void hellraider_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata);

/* *** NAPALM *** */
void nflame_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata);
bullet_t* create_napalm(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void napalm_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata);
void napalm_blit_cb(camera_t* c,void* obj,void* userdata);

/* *** LASER *** */
void laser_blit_cb(camera_t* c,void* obj,void* userdata);
bullet_t* create_laser(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void laser_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata);
void laser_update(void* bullet,void *p1,void *p2, void* userdata);

/* *** EXPLOSIVES *** */
bullet_t* create_explosives(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void explosives_blit_cb(camera_t* c,void* bullet,void* userdata);
void explosive_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata);
                             
/* *** BOUNCY LARPA *** */
bullet_t* create_bouncy_larpa(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void bouncy_larpa_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata);
void bouncy_larpa_blit_cb(camera_t* c,void* bullet,void* userdata);

/* *** LARPA *** */
bullet_t* create_larpa(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void larpa_update(void* bullet,void *p1,void *p2, void* userdata);

/* *** NUKE LARPA *** */
void nuke_larpa_update(void* bullet,void *p1,void *p2, void* userdata);

/* *** CTRL NUKE LARPA *** */
void cnuke_larpa_update(void* bullet,void *p1,void *p2, void* userdata);

/* *** HEALTH GIFT *** */
bullet_t* create_health_gift(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void health_gift_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata);
void health_gift_blit_cb(camera_t* c,void* bullet,void* userdata);

/* *** CRACKLER *** */
bullet_t* create_crackler(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void crackler_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata);
void crackler_blit_cb(camera_t* c,void* bullet,void* userdata);
void crackler_update(void* bullet,void *p1,void *p2, void* userdata);

/* *** MINI NUKE *** */
bullet_t* create_mini_nuke(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void mini_nuke_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                              , void* p1,void* p2, void* userdata);
void mini_nuke_blit_cb(camera_t* c,void* bullet,void* userdata);

/* *** BIG NUKE *** */
bullet_t* create_big_nuke(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void big_nuke_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                              , void* p1,void* p2, void* userdata);
void big_nuke_blit_cb(camera_t* c,void* bullet,void* userdata);


/* *** NUKE PIECE *** */
bullet_t* create_nuke_piece(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void nuke_piece_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                              , void* p1,void* p2, void* userdata);
void nuke_piece_blit_cb(camera_t* c,void* bullet,void* userdata);

/* *** VERMI NUKE*** */
void vermi_nuke_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                              , void* p1,void* p2, void* userdata);
                              
/* *** DIRTBALL *** */
bullet_t* create_dirtball(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void dirtball_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                              , void* p1,void* p2, void* userdata);
void dirtball_blit_cb(camera_t* c,void* bullet,void* userdata);

/* *** PLASMA CANNON *** */
bullet_t* create_plasma_cannon(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void plasma_cannon_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                              , void* p1,void* p2, void* userdata);
void plasma_cannon_blit_cb(camera_t* c,void* bullet,void* userdata);

/* *** BAZOOKA *** */
bullet_t* create_bazooka(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void bazooka_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                              , void* p1,void* p2, void* userdata);
void bazooka_blit_cb(camera_t* c,void* bullet,void* userdata);
void wsprite_blit_cb(camera_t* c,void* bullet,void* userdata);

/* *** DOOMSDAY *** */
bullet_t* create_doomsday(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void doomsday_blit_cb(camera_t* c,void* bullet,void* userdata);

/* *** MINI ROCKET *** */
void minirocket_blit_cb(camera_t* c,void* bullet,void* userdata);
bullet_t* create_minirocket(weapon_id w_id,player_t *p,int acc_x,int acc_y);

/* *** VERMIFUGE *** */
bullet_t* create_vermifuge(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void vermifuge_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata);

/* *** JUMPING MINE *** */
bullet_t* create_jumping_mine(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void jumping_mine_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata);
void jmine_activ_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata);

/* *** JUMPING MINE *** */
void trap_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata);

/* *** KAMIKASE *** */
bullet_t* create_kamikase(weapon_id w_id,player_t *p,int acc_x,int acc_y);

/* *** GHIFTS *** */ 
bullet_t* create_bonus_gift(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void bonus_gift_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                              , void* p1,void* p2, void* userdata);
void bonus_gift_blit_cb(camera_t* c,void* bullet,void* userdata);

/* *** SPORE & Viral CASE *** */ 
bullet_t* create_spore(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void spore_blit_cb(camera_t* c,void* bullet,void* userdata);
bullet_t* create_viral_case(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void viral_case_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                           , void* p1,void* p2, void* userdata);
void viral_case_blit_cb(camera_t* c,void* bullet,void* userdata);
void viral_case_update(void* bullet,void *p1,void *p2, void* userdata);

/* *** BOOBY *** */ 
void  booby_trap_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                           , void* p1,void* p2, void* userdata);
bullet_t* create_booby_trap(weapon_id w_id,player_t *p,int acc_x,int acc_y);

/* *** MEDIKIT *** */
bullet_t* create_medikit(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void medikit_blit_cb(camera_t* c,void* bullet,void* userdata);

/* *** SHIELD *** */
bullet_t* create_shield(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void shield_blit_cb(camera_t* c,void* bullet,void* userdata);

/* *** PLAYERS HOUSES *** */
void house_p1_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                           , void* p1,void* p2, void* userdata);
void house_p2_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                           , void* p1,void* p2, void* userdata);
void house_blit_cb(camera_t* c,void* bullet,void* userdata);


/* *** PLAYERS FLAGS *** */
void flag_p1_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                           , void* p1,void* p2, void* userdata);
void flag_p2_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                           , void* p1,void* p2, void* userdata);
void flag_blit_cb(camera_t* c,void* bullet,void* userdata);


/* *** PINBALL *** */
bullet_t* create_pinball(weapon_id w_id,player_t *p,int acc_x,int acc_y);
void  pinball_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             , void* p1,void* p2, void* userdata);

/* *** MISSILE *** */
void missile_update(void* bullet,void *p1,void *p2, void* userdata);
void missile_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                              , void* p1,void* p2, void* userdata);

/* DEFAULT WEAPON DATA */
typedef struct{
  Uint8               damages;           // default damages
  Uint8               range;             // default range
  Uint16              timeout;           // default timeout
  Uint8               explo_size;        // default explosion size
  Uint8               cadency;           // default cadency
  Uint8               ammo;              // default ammo
  Uint8               resist;            // default resistancy
  float               velocity;          // default velocity
  Uint16              recharge_time;     // default rechage time
  Uint8               info;              // default information bitset
  creation_callback   create_cb;         // default creation callback  
  test_coll_callback  test_collision_cb; // default collision test callback
  on_coll_callback    on_collision_cb;   // default on collision callback
  blit_callback       blit_cb;           // default blit callback 
  update_callback     special_cb;        // default special callback 
  sengine_sound_id_t  fire_sound;        // default fire sound
  sengine_sound_id_t  explo_sound;       // default explosion sound
}weapon_data_t;


weapon_data_t std_weap_info[MAX_REAL_WEAPONS]= {
/*                            Da Ra   To Ex  Ca  Am Re Ve   Rt CR TC OC BC SP*/  
/* -WEAPON_HANDGUN        */ {11, 5,   0, 4,  8, 15, 0,1.0,25*3,NO_KICK,create_bullet,bullet_collision_cb,bullet_on_collision_cb,bullet_blit_cb,0l,WIIERO_SOUND_GUN,WIIERO_SOUND_NONE},
/* -WEAPON_UZI            */ { 2, 5,   0, 4,  2, 15, 0,0.8,25*3,NO_KICK,create_bullet,bullet_collision_cb,bullet_on_collision_cb,bullet_blit_cb,0l,WIIERO_SOUND_LITTLE_AUTO,WIIERO_SOUND_NONE},
/* -WEAPON_CHAINGUN       */ { 3, 5,   0, 5,  3, 50, 0,0.8,25*4,0,create_bullet,bullet_collision_cb,bullet_on_collision_cb,bullet_blit_cb,0l,WIIERO_SOUND_BIG_AUTO,WIIERO_SOUND_NONE},
/* -WEAPON_SHOOTGUN       */ { 2, 5,   0, 5, 20,  5, 0,0.8,25*3,0,create_shootgun_bullet,bullet_collision_cb,shootgun_on_collision_cb,bullet_blit_cb,0l,WIIERO_SOUND_SHOOTGUN,WIIERO_SOUND_NONE},
/* -WEAPON_SUPER_SHOOTGUN */ { 4, 5,   0, 7, 20,  2, 0,0.8,25*6,0,create_super_shootgun_bullet,bullet_collision_cb,shootgun_on_collision_cb,bullet_blit_cb,0l,WIIERO_SOUND_SHOOTGUN,WIIERO_SOUND_NONE},
/* -WEAPON_MINIGUN        */ { 2, 5,   0, 6,  1, 70, 0,0.4,25*7,0,create_mg_bullet,bullet_collision_cb,bullet_on_collision_cb,bullet_blit_cb,0l,WIIERO_SOUND_MINIGUN,WIIERO_SOUND_NONE},
/* -WEAPON_CANNON         */ {20, 5,   0,12,  5,  6, 4,0.5,25*6,0,create_cannonball,bullet_collision_cb,bullet_on_collision_cb,cannonball_blit_cb,0l,WIIERO_SOUND_CANNON,WIIERO_SOUND_LITTLE_EXPLO},
/* -WEAPON_BAZOOKA        */ {45, 5,   0,12, 25,  3, 1,0.7,25*7,0,create_bazooka,bullet_collision_cb,bazooka_on_collision_cb,bazooka_blit_cb,0l,WIIERO_SOUND_BAZOOK,WIIERO_SOUND_LITTLE_EXPLO}, // 6 particles (5)
/* -WEAPON_DOOMSDAY       */ {15, 6,   0, 8,  3,  8, 0,0.5,25*7,0,create_doomsday,bullet_collision_cb,bullet_on_collision_cb,doomsday_blit_cb,0l,WIIERO_SOUND_BAZOOK,WIIERO_SOUND_LITTLE_EXPLO},
/* -WEAPON_WINCHESTER     */ {15, 5,   0, 8, 10,  9, 0,1.5,25*5,0,create_bullet,bullet_collision_cb,bullet_on_collision_cb,bullet_blit_cb,0l,WIIERO_SOUND_MAGNUM,WIIERO_SOUND_NONE},
/* -WEAPON_LARPA          */ {15, 5,   0, 5,  7,  5, 0,0.4,25*5,NO_KICK,create_larpa,bullet_collision_cb,bullet_on_collision_cb,wsprite_blit_cb,larpa_update,WIIERO_SOUND_LARPA,WIIERO_SOUND_NONE},
/* -WEAPON_BOUNCY_LARPA   */ {15, 6,25*8, 4,  7,  5, 1,0.4,25*5,NO_KICK,create_bouncy_larpa,bullet_collision_cb,bouncy_larpa_on_collision_cb,bouncy_larpa_blit_cb,larpa_update,WIIERO_SOUND_LARPA,WIIERO_SOUND_NONE},
/* -WEAPON_ZIMM           */ {49, 5,25*8, 5, 25,  2, 0,0.8,25*7,NO_KICK|NO_SHELL|NO_GUNFIRE,create_zimm,bullet_collision_cb,zimm_on_collision_cb,zimm_blit_cb,0l,WIIERO_SOUND_ZIMM,WIIERO_SOUND_NONE},
/* -WEAPON_RIFLE          */ {30, 5,   0,14, 30,  2, 0,4.0,25*5,0,create_bullet,bullet_collision_cb,rifle_on_collision_cb,bullet_blit_cb,0l,WIIERO_SOUND_RIFLE,WIIERO_SOUND_NONE},
/* -WEAPON_BLASTER        */ {10, 6,   0,10, 25,  3, 5,0.7,25*4,0,create_blaster,bullet_collision_cb,blaster_on_collision_cb,blaster_blit_cb,0l,WIIERO_SOUND_BLAST,WIIERO_SOUND_LITTLE_EXPLO},   //15 particles (5)
/* -WEAPON_GAUSS_GUN      */ {15, 7,   0,14,  1,  1, 0,1.9,25*5,0,create_gauss,bullet_collision_cb,gauss_on_collision_cb,gauss_blit_cb,0l,WIIERO_SOUND_MAGNUM,WIIERO_SOUND_LITTLE_EXPLO},   //10 particles (10) 
/* -WEAPON_FLAMER         */ { 1, 5,  25, 0,  1,200, 0,0.2,25*4,NO_KICK|NO_SHELL,create_flame,bullet_collision_cb,flame_on_collision_cb,flame_blit_cb,0l,WIIERO_SOUND_FLAMMER,WIIERO_SOUND_NONE},
/* -WEAPON_MINI_ROCKER    */ {15, 6,   0, 5,  3, 10, 0,0.5,25*5,NO_KICK,create_minirocket,bullet_collision_cb,bullet_on_collision_cb,minirocket_blit_cb,0l,WIIERO_SOUND_BAZOOK,WIIERO_SOUND_NONE},
/*  WEAPON_MISSILE        */ {45, 5,   0,12,  1,  1, 0,0.5,25*5,NO_KICK,create_bazooka,bullet_collision_cb,missile_on_collision_cb,bazooka_blit_cb,missile_update,WIIERO_SOUND_BAZOOK,WIIERO_SOUND_LITTLE_EXPLO},
/* -WEAPON_RB_RAMPAGE     */ { 5, 5, 250, 2,  1, 30, 2,0.5,25*4,NO_KICK,create_mg_bullet,bullet_collision_cb,rb_on_collision_cb,bullet_blit_cb,0l,WIIERO_SOUND_LITTLE_AUTO,WIIERO_SOUND_NONE}, 
/* -WEAPON_HELLRAIDER     */ { 5, 6,  50,10,  1,  1, 3,0.3,25*9,0,create_hellraider,bullet_collision_cb,hellraider_on_collision_cb,hellraider_blit_cb,0l,WIIERO_SOUND_HELLRAIDER,WIIERO_SOUND_LITTLE_EXPLO}, // 6 cannonballs (10) tm 25
/* -WEAPON_LASER          */ { 1, 4,   0, 3,  1,100, 0,4.0,25*6,NO_KICK|NO_SHELL,create_laser,bullet_collision_cb,laser_on_collision_cb,laser_blit_cb,laser_update,WIIERO_SOUND_LASER,WIIERO_SOUND_NONE},
/* -WEAPON_GRASSHAPPER    */ {10, 5,25*9, 6,  1,  1, 4,0.3,25*4,NO_SHELL,create_grass_hopper,bullet_collision_cb,grass_hopper_on_collision_cb,grass_hopper_blit_cb,grass_hopper_update,WIIERO_SOUND_GRENADE,WIIERO_SOUND_NONE},
/*                            Da Ra   To Ex  Ca Am  Re Ve   Rt CR TC OC BC */
/* -WEAPON_BOOBY_TRAP     */ {30,10,   0,10,  1,  1, 4,0.3,25*4,NO_KICK|NO_SHELL|NO_GUNFIRE,create_booby_trap,bullet_collision_cb,booby_trap_on_collision_cb,bonus_gift_blit_cb,0l,WIIERO_SOUND_GRENADE,WIIERO_SOUND_LITTLE_EXPLO},
/* -WEAPON_MINE           */ {20,10,3000,19,  1,  1, 4,0.2,25*3,NO_KICK|NO_SHELL|NO_GUNFIRE,create_mine,bullet_collision_cb,mine_on_collision_cb,mine_blit_cb,0l,WIIERO_SOUND_GRENADE,WIIERO_SOUND_LITTLE_EXPLO}, // 5 particles (5) 
/* -WEAPON_BOUNCY_MINE    */ {20,10,2000, 8,  1,  1, 3,0.3,25*4,NO_KICK|NO_SHELL|NO_GUNFIRE,create_bouncy_mine,bullet_collision_cb,bouncy_mine_on_collision_cb,bouncy_mine_blit_cb,0l,WIIERO_SOUND_LAUNCHER,WIIERO_SOUND_LITTLE_EXPLO}, // 5 particles (5)  
/* -WEAPON_FLOAT_MINE     */ {20, 6,3000, 8,  7,  2, 0,0.3,25*4,NO_KICK|NO_SHELL|NO_GUNFIRE,create_float_mine,bullet_collision_cb,float_mine_on_collision_cb,float_mine_blit_cb,float_mine_update_cb,WIIERO_SOUND_GRENADE,WIIERO_SOUND_LITTLE_EXPLO}, // 5 particles (5)  
/* -WEAPON_SPIKEBALLS     */ {10, 6,25*8, 0,  0,  1, 4,0.3,25*6,NO_KICK|NO_SHELL|NO_GUNFIRE,create_spike,bullet_collision_cb,spike_on_collision_cb,spike_blit_cb,0l,WIIERO_SOUND_GRENADE,WIIERO_SOUND_NONE}, // 16 iron
/*                            Da Ra   To Ex  Ca  Am Re Ve   Rt CR TC OC BC */
/* -WEAPON_CRACKLER       */ { 3, 6, 250, 4,  1,  1, 1,0.3,25*7,NO_KICK|NO_SHELL,create_crackler,bullet_collision_cb,crackler_on_collision_cb,crackler_blit_cb,crackler_update,WIIERO_SOUND_LAUNCHER,WIIERO_SOUND_NONE},
/* -WEAPON_GRENADE        */ { 5, 0,25*2,10, 20,  2, 2,0.3,25*3,NO_KICK|NO_SHELL|NO_GUNFIRE,create_grenade,bullet_collision_cb,grenade_on_collision_cb,grenade_blit_cb,0l,WIIERO_SOUND_GRENADE,WIIERO_SOUND_LITTLE_EXPLO}, // 50 particles (5) 
/* -WEAPON_CLUSTER_BOMB   */ {11, 6,25*2,12,  1,  1, 2,0.3,25*6,NO_KICK|NO_SHELL|NO_GUNFIRE,create_grenade,bullet_collision_cb,cluster_bomb_on_collision_cb,grenade_blit_cb,0l,WIIERO_SOUND_GRENADE,WIIERO_SOUND_LITTLE_EXPLO}, // 20 sharp fragments (11)
/* -WEAPON_MINI_NUKE      */ { 0, 0,25*2,17, 10,  2, 2,0.5,25*6,NO_KICK|NO_SHELL|NO_GUNFIRE,create_mini_nuke,bullet_collision_cb,mini_nuke_on_collision_cb,mini_nuke_blit_cb,0l,WIIERO_SOUND_GRENADE,WIIERO_SOUND_CLIP}, //  10 sharp fragments (6)
/* -WEAPON_BIG_NUKE       */ { 0, 0,25*3,17,  1,  1, 2,0.5,25*9,NO_KICK|NO_SHELL|NO_GUNFIRE,create_big_nuke,bullet_collision_cb,big_nuke_on_collision_cb,big_nuke_blit_cb,0l,WIIERO_SOUND_GRENADE,WIIERO_SOUND_CLIP}, //  50 sharp fragments (6)
/* -WEAPON_CHIQUITA_BOMB  */ {20, 6,25*5,19,  1,  1, 2,0.4, 250,NO_KICK|NO_SHELL|NO_GUNFIRE,create_chiquita,bullet_collision_cb,chiquita_on_collision_cb,chiquita_blit_cb,0l,WIIERO_SOUND_GRENADE,WIIERO_SOUND_BIG_EXPLO}, //  22 sharp fragments (15)
/* -WEAPON_EXPLOSIVES     */ {15,10,  40,17,  1,  1, 3,0.3,25*2,NO_KICK|NO_SHELL|NO_GUNFIRE,create_explosives,bullet_collision_cb,explosive_on_collision_cb,explosives_blit_cb,0l,WIIERO_SOUND_GRENADE,WIIERO_SOUND_LITTLE_EXPLO},
/* -WEAPON_NAPALM         */ { 0, 0,   0,10,  1,  1, 3,0.4,25*4,NO_KICK|NO_SHELL|NO_GUNFIRE,create_napalm,bullet_collision_cb,napalm_on_collision_cb,napalm_blit_cb,0l,WIIERO_SOUND_GRENADE,WIIERO_SOUND_BURN}, //  16 flames trail (3)
/* -WEAPON_DIRTBALL       */ { 0, 0,25*2, 0, 10,  5, 2,0.8,25*3,NO_KICK|NO_SHELL|NO_GUNFIRE,create_dirtball,bullet_collision_cb,dirtball_on_collision_cb,dirtball_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_POP},

/* XTRA Weapons */
/*                            Da Ra   To Ex  Ca  Am Re Ve   Rt CR TC OC BC */
/* -WEAPON_HOLY_GRENADE   */ {25, 6,25*3,19,  1,  1, 3,0.4,25*8,NO_KICK|NO_SHELL|NO_GUNFIRE,create_holy_grenade,bullet_collision_cb,holy_grenade_on_collision_cb,holy_grenade_blit_cb,0l,WIIERO_SOUND_GRENADE,WIIERO_SOUND_BIG_EXPLO},
/* -WEAPON_SUPER_CANNON   */ {20, 5,   0,12,  5,  6, 4,0.5,25*6,0,create_cannonball,bullet_collision_cb,rifle_on_collision_cb,cannonball_blit_cb,0l,WIIERO_SOUND_CANNON,WIIERO_SOUND_LITTLE_EXPLO},
/* -WEAPON_CEILING_MINE   */ {20,10,3000,19,  1,  1, 0,0.1,25*3,NO_KICK|NO_SHELL|NO_GUNFIRE,create_mine,bullet_collision_cb,mine_on_collision_cb,mine_blit_cb,ceiling_mine_update,WIIERO_SOUND_NONE,WIIERO_SOUND_LITTLE_EXPLO},
/* -WEAPON_GH_GRENADE     */ { 5, 0,25*3,10,  8,  2, 2,0.3,25*6,NO_KICK|NO_SHELL|NO_GUNFIRE,create_grenade,bullet_collision_cb,gh_grenade_on_collision_cb,grenade_blit_cb,0l,WIIERO_SOUND_GRENADE,WIIERO_SOUND_LITTLE_EXPLO},
/* -WEAPON_BLUE_FLAMER    */ { 2, 5,  25, 0,  1,200, 0,0.2,25*5,NO_KICK|NO_SHELL,create_blue_flame,bullet_collision_cb,flame_on_collision_cb,flame_blit_cb,0l,WIIERO_SOUND_FLAMMER,WIIERO_SOUND_NONE},
/* -WEAPON_AN_M14_GRENADE */ {30, 0,25*3,10, 40,  2, 2,0.3,25*6,NO_KICK|NO_SHELL|NO_GUNFIRE,create_red_grenade,bullet_collision_cb,an_m14_grenade_on_collision_cb,grenade_blit_cb,0l,WIIERO_SOUND_GRENADE,WIIERO_SOUND_LONG_BURN},
/* -WEAPON_GAZ_GRENADE    */ {30, 0, 250,10, 40,  2, 2,0.3,25*6,NO_KICK|NO_SHELL|NO_GUNFIRE,create_white_grenade,bullet_collision_cb,gaz_grenade_on_collision_cb,grenade_blit_cb,gaz_grenade_update,WIIERO_SOUND_GRENADE,WIIERO_SOUND_NONE},
/* -WEAPON_PLASMA_CANNON  */ {30,11,   0, 1,  1,  1, 1,1.2,25*4,0,create_plasma_cannon,bullet_collision_cb,plasma_cannon_on_collision_cb,plasma_cannon_blit_cb,0l,WIIERO_SOUND_ELEC,WIIERO_SOUND_LONG_BURN},
/* -WEAPON_NUKE_LARPA     */ {15, 5,   0, 5, 20,  4, 0,0.5,25*8,NO_KICK,create_larpa,bullet_collision_cb,bullet_on_collision_cb,wsprite_blit_cb,nuke_larpa_update,WIIERO_SOUND_LARPA,WIIERO_SOUND_NONE},
/* -WEAPON_CTRL_NUKE_LARPA*/ {15, 5,25*8, 5,  1,  1, 0,0.5,25*8,NO_KICK,create_larpa,bullet_collision_cb,missile_on_collision_cb,wsprite_blit_cb,cnuke_larpa_update,WIIERO_SOUND_LARPA,WIIERO_SOUND_NONE},
/* -WEAPON_SC_LAUNCHER    */ {10, 6,25*5,19,  1,  1, 2,0.8, 250,0,create_chiquita,bullet_collision_cb,sticky_chiquita_on_collision_cb,chiquita_blit_cb,0l,WIIERO_SOUND_LAUNCHER,WIIERO_SOUND_BIG_EXPLO}, //  22 sharp fragments (15)
/* -WEAPON_VERMIFUGE      */ { 1, 5,3000, 1,  3, 10, 4,0.2,25*5,NO_KICK|NO_SHELL|NO_GUNFIRE,create_vermifuge,bullet_collision_cb,vermifuge_on_collision_cb,bullet_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_NONE},
/* -WEAPON_JUMPING_MINE   */ { 0,13,3000,19,  1,  1, 4,0.2,25*4,NO_KICK|NO_SHELL|NO_GUNFIRE,create_jumping_mine,bullet_collision_cb,jumping_mine_on_collision_cb,mine_blit_cb,0l,WIIERO_SOUND_GRENADE,WIIERO_SOUND_BIPBIP}, // 5 particles (5) 
/* -WEAPON_TRAP           */ { 0,15,3000, 0, 20,  2, 2,0.9,25*4,NO_KICK|NO_SHELL|NO_GUNFIRE,create_dirtball,bullet_collision_cb,trap_on_collision_cb,dirtball_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_NONE},
/* -WEAPON_KAMIKASE       */ {40,40,   0,19,  1,  1, 0,  0, 250,NO_KICK|NO_GUNFIRE,create_kamikase,bullet_collision_cb,sticky_chiquita_on_collision_cb,bullet_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_BIG_EXPLO},
/* -WEAPON_SPIKE_TIME_BOMB*/ { 0, 0,1000,19,  1,  1, 0,0.2,25*4,NO_KICK|NO_SHELL|NO_GUNFIRE,create_mine,bullet_collision_cb,spike_tbomb_on_collision_cb,mine_blit_cb,ceiling_mine_update,WIIERO_SOUND_NONE,WIIERO_SOUND_NONE},
/* -WEAPON_VIRAL_CASE     */ {30,11, 300, 1,  1,  1, 5,0.3,25*7,NO_KICK|NO_SHELL|NO_GUNFIRE,create_viral_case,bullet_collision_cb,viral_case_on_collision_cb,viral_case_blit_cb,viral_case_update,WIIERO_SOUND_GRENADE,WIIERO_SOUND_NONE},
/* -WEAPON_VERMI_NUKE     */ { 0, 0,25*2,17, 10,  1, 2,0.5,25*6,NO_KICK|NO_SHELL|NO_GUNFIRE,create_mini_nuke,bullet_collision_cb,vermi_nuke_on_collision_cb,mini_nuke_blit_cb,0l,WIIERO_SOUND_GRENADE,WIIERO_SOUND_CLIP}, //  10 sharp fragments (6)
/* -WEAPON_MEDIKIT        */ {30, 0,   9, 0, 10,  1, 0,0.0, 600,NO_KICK|NO_SHELL|NO_GUNFIRE,create_medikit,0l,0l,medikit_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_NONE},
/* -WEAPON_SHIELD         */ { 0, 0, 600, 0,  1,  1, 0,0.0, 800,NO_KICK|NO_SHELL|NO_GUNFIRE,create_shield ,0l,0l, shield_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_NONE},
/* -WEAPON_PINBALL        */ { 0, 5,8000, 0,  2, 20, 1,0.3,25*5,NO_KICK|NO_SHELL|NO_GUNFIRE,create_pinball,bullet_collision_cb,pinball_on_collision_cb,gauss_blit_cb,0l,WIIERO_SOUND_LAUNCHER,WIIERO_SOUND_LITTLE_EXPLO}, // 5 particles (5)  

/* Hidden Weapons */
/*                            Da Ra   To Ex  Ca  Am Re Ve   Rt CR TC OC BC */
/* -WEAPON_CLUSTER_PIECE  */ {11, 0,   0, 4,  0,  0, 2,0.0,   0,0,0l,bullet_collision_cb,cluster_piece_on_collision_cb,cluster_piece_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_LITTLE_EXPLO},
/* -WEAPON_FIRE           */ { 1, 5, 250, 0,  1,  1, 1,0.2,  25,0,create_flame,bullet_collision_cb,fire_on_collision_cb,fire_blit_cb,fire_update,WIIERO_SOUND_NONE,WIIERO_SOUND_NONE},
/* -WEAPON_BLEU_FIRE      */ { 2, 5, 250, 0,  1,  1, 1,0.2,  25,0,create_blue_flame,bullet_collision_cb,fire_on_collision_cb,fire_blit_cb,fire_update,WIIERO_SOUND_NONE,WIIERO_SOUND_NONE},
/* -WEAPON_NINJA_HOOK     */ { 0, 3,   0, 0,  1,  1, 4,1.0,   1,NO_KICK|NO_SHELL|NO_GUNFIRE,create_ninja_hook,bullet_collision_cb,ninja_hook_collision_cb,ninja_hook_blit_cb,ninja_hook_update_cb,WIIERO_SOUND_NONE,WIIERO_SOUND_NONE},
/* -WEAPON_CHIQUITA_PIECE */ {15,10,25*9,17,  0,  0, 2,0.0,   0,0,0l,bullet_collision_cb,chiquita_piece_on_collision_cb,chiquita_piece_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_LITTLE_EXPLO},
/* -WEAPON_GAZ_PARTICLE   */ { 2, 2, 300, 0,  1,  1, 0,0.2,   0,0,create_gaz,bullet_collision_cb,gaz_on_collision_cb,gaz_blit_cb,gaz_update,WIIERO_SOUND_NONE,WIIERO_SOUND_NONE},
/* -WEAPON_NAPALM_FLAME   */ { 1,10,  20, 0,  1,  1, 1,0.3,   0,0,create_hellraider,bullet_collision_cb,nflame_on_collision_cb,hellraider_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_NONE},
/* -WEAPON_HEALTH_GIFT    */ {33,10,1500, 0,  1,  1, 4,0.3,   0,0,create_health_gift,bullet_collision_cb,health_gift_on_collision_cb,health_gift_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_NONE},
/* -WEAPON_BONUS_GIFT     */ { 0,10,1500, 0,  1,  1, 4,0.3,   0,0,create_bonus_gift,bullet_collision_cb,bonus_gift_on_collision_cb,bonus_gift_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_NONE},
/* -WEAPON_NUKE_PIECE     */ { 6, 0,25*5,19,  1,  1, 3,0.3,   0,0,create_nuke_piece,bullet_collision_cb,nuke_piece_on_collision_cb,nuke_piece_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_LITTLE_EXPLO},
/* -WEAPON_JMINE_ACTIV    */ {15, 5,  10,19,  1,  1, 4,0.2,25*3,NO_KICK|NO_SHELL|NO_GUNFIRE,0l,bullet_collision_cb,jmine_activ_on_collision_cb,mine_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_LITTLE_EXPLO}, // 5 particles (5) 
/* -WEAPON_SPORE_PARTICULE*/ { 2, 4, 300, 0,  1,  1, 1,1.2,   0,0,create_spore,bullet_collision_cb,gaz_on_collision_cb,spore_blit_cb,gaz_update,WIIERO_SOUND_NONE,WIIERO_SOUND_NONE},
/* -WEAPON_BOOBY_BONUS    */ {15,10,3000,17,  1,  1, 4,0.3,25*2,NO_KICK|NO_SHELL|NO_GUNFIRE,0l,bullet_collision_cb,booby_trap_on_collision_cb ,bonus_gift_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_LITTLE_EXPLO},
/* -WEAPON_BOOBY_HEALTH   */ {15,10,3000,17,  1,  1, 4,0.3,25*2,NO_KICK|NO_SHELL|NO_GUNFIRE,0l,bullet_collision_cb,booby_trap_on_collision_cb,health_gift_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_LITTLE_EXPLO},
/* -WEAPON_P1_HOUSE       */ { 0, 6,   0, 0,  0,  0, 4,0.3,   0,NO_KICK|NO_SHELL|NO_GUNFIRE,0l,bullet_collision_cb,house_p1_on_collision_cb,house_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_NONE},
/* -WEAPON_P2_HOUSE       */ { 0, 6,   0, 0,  0,  0, 4,0.3,   0,NO_KICK|NO_SHELL|NO_GUNFIRE,0l,bullet_collision_cb,house_p2_on_collision_cb,house_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_NONE},
/* -WEAPON_P1_FLAG        */ { 0, 5,   0, 0,  0,  0, 4,0.3,   0,NO_KICK|NO_SHELL|NO_GUNFIRE,0l,bullet_collision_cb,flag_p1_on_collision_cb,flag_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_NONE},
/* -WEAPON_P2_FLAG        */ { 0, 5,   0, 0,  0,  0, 4,0.3,   0,NO_KICK|NO_SHELL|NO_GUNFIRE,0l,bullet_collision_cb,flag_p2_on_collision_cb,flag_blit_cb,0l,WIIERO_SOUND_NONE,WIIERO_SOUND_NONE}
}; 

/*
typedef struct{
  int damages;           // default damages
  int range;             // default range
  int timeout;           // default timeout
  int cadency;           // default cadency
  int ammo;              // default ammo
  int resist;            // default resistancy
  int velocity;          // default velocity
  int recharge_time;     // default rechage time
  weapon_id id;
}weapon_t;
*/
/*
static const char weapon_name[MAX_WEAPONS_AND_XTRA][32] = {
  "handgun"
, "uzi"
, "chaingun"
, "shotgun"
, "super shotgun"
, "minigun"
, "cannon"
, "bazooka"
, "doomsday"
, "winchester"
, "larpa"
, "bouncy larpa"
, "zimm"
, ".50 rifle"
, "blaster"
, "gauss gun"
, "flamer"
, "mini rocket"
, "missile"
, "rb bampage"
, "hellraider"
, "laser"
, "grasshopper"
, "booby trap"
, "mine"
, "bouncy mine"
, "float mine"
, "spikeballs"
, "crackler"
, "grenade"
, "cluster bomb"
, "mini nuke"
, "big nuke"
, "chiquita bomb"
, "explosives"
, "napalm"
, "dirtball"
// Xtra 
, "holy grenade"
, "super cannon"
, "ceiling mine"
, "ghopper grenade"
, "blue flammer"
, "an-m14 grenade"
, "gas grenade"
, "plasma cannon"
, "nuke larpa"
, "ctrl nuke larpa"
, "chiquita gun"
, "vermifuge"
, "jumping mine"
, "trap"
, "kamikaze"
, "spike time bomb"
, "bug case"
, "vermi nuke"
, "medikit"
, "iron worm"
, "pinball"
};
*/

char* weapon_get_name(weapon_id w_id){
  return (char*)wiiero_label[WIIERO_LANG_WEAPON_NAMES_FIRST_ID+w_id];
}

weapon_t* weapon_init(weapon_id w_id){
  weapon_t* w = secure_malloc(sizeof(weapon_t));
  w->damages = std_weap_info[w_id].damages;
  w->range = std_weap_info[w_id].range;
  w->timeout = std_weap_info[w_id].timeout;
  w->cadency_step = std_weap_info[w_id].cadency;
  w->ammo = std_weap_info[w_id].ammo;
  w->resist = std_weap_info[w_id].resist;
  w->velocity = std_weap_info[w_id].velocity;
  w->recharge_step = std_weap_info[w_id].recharge_time;
  w->explo_size = std_weap_info[w_id].explo_size;
  w->id = w_id;
  w->last_bullet = 0l;
  return w;
}

int weapon_get_max_ammo(weapon_t* w){
  return std_weap_info[w->id].ammo;
}

int weapon_get_nb_recharge_steps(weapon_t* w){
  return std_weap_info[w->id].recharge_time * weapon_round_option.reload_percent / 100;
}

int weapon_can_shoot(weapon_t* w){
  ASSERT( w );
  ASSERT( (w->ammo!=0xff) );
  /* NO AMMO */
  if(w->ammo == 0)
    return 0;
  /* CADENCY WAIT */
  if(w->cadency_step < std_weap_info[w->id].cadency)
    return 0;
  //printf("SHOOT : ammo %d\n",w->ammo);
  w->ammo--;
  if(w->ammo == 0)
    w->recharge_step = 0;

  w->cadency_step = 0;
  return 1;
}

int weapon_update(weapon_t* w){
  ASSERT( w )
  int is_loading =  weapon_reload(w);
  if(w->cadency_step < std_weap_info[w->id].cadency){
    w->cadency_step++;
    //printf("cadency %d\n",w->cadency_step);
  }
  return is_loading;
}

int weapon_reload(weapon_t* w){
  ASSERT( w )
  /* REALOAD */
  int reload_time_per_ammo = ( weapon_get_nb_recharge_steps(w) / std_weap_info[w->id].ammo);
  if(w->ammo != 0)
    return 0;
  //printf("reload %d/%d\n",w->recharge_step,std_weap_info[w->id].recharge_time);
  w->recharge_step = w->recharge_step + 1 ;
  if (w->recharge_step % (( reload_time_per_ammo > 8)? reload_time_per_ammo : 8) ==0)
    sengine_play(WIIERO_SOUND_RELOAD,SENGINE_PLAY_ACTION);
  if(w->recharge_step >= weapon_get_nb_recharge_steps(w))
      w->ammo = std_weap_info[w->id].ammo;
  return 1;
}

Uint8 weapon_shoot(weapon_t* w,void* p,int acc_x,int acc_y){
  ASSERT(std_weap_info[w->id].create_cb)
  w->last_bullet = std_weap_info[w->id].create_cb(w->id,p,acc_x,acc_y);
  if(!(std_weap_info[w->id].info & NO_KICK)){
    ((player_t*)p)->worms.acc_x -=  (acc_x * std_weap_info[w->id].velocity)/4 ;
    ((player_t*)p)->worms.acc_y -=  (acc_y * std_weap_info[w->id].velocity)/4 ;  
  }
  sengine_play(std_weap_info[w->id].fire_sound,SENGINE_PLAY_ACTION);
  return std_weap_info[w->id].info; 
}

int weapon_check_all_collisions( bullet_t* bullet,SDL_Surface* ground,SDL_Surface* statics
                               , void* p1, void* p2, void* userdata){
  ASSERT(bullet);
  ASSERT(ground);
  ASSERT(statics);
  ASSERT(p1);
  ASSERT(p2);
  int coll=0;
  int i = 0;
  int last_x = bullet->obj.pos_x;
  int last_y = bullet->obj.pos_y; 

  if(!bullet->obj.test_collision_cb) return 0;
  if(bullet->obj.acc_x || bullet->obj.acc_y){

    DBG("pos_x %d , pos_y %d\n",bullet->obj.pos_x, bullet->obj.pos_y);
    DBG("acc_x %d , acc_y %d\n",bullet->obj.acc_x, bullet->obj.acc_y);

    while(i <= MOVE_DECOMPO){
      int x,y;
      x = bullet->obj.pos_x + ((i * bullet->obj.acc_x) / MOVE_DECOMPO);
      y = bullet->obj.pos_y + ((i * bullet->obj.acc_y) / MOVE_DECOMPO);

      if((x!=last_x) || (y!=last_y)){
        /* Check collisions */
        DBG("test %d (%d) %d (%d)\n",x,((i * bullet->obj.acc_x) / MOVE_DECOMPO)
                                      ,y,((i * bullet->obj.acc_y) / MOVE_DECOMPO));

        if((((player_t*)p1)->worms_status & STATUS_ALIVE)
            &&( x - bullet->range <= ((player_t*)p1)->worms.pos_x)
            &&( x + bullet->range >= ((player_t*)p1)->worms.pos_x)
            &&( y - bullet->range <= ((player_t*)p1)->worms.pos_y-3)
            &&( y + bullet->range >= ((player_t*)p1)->worms.pos_y-3)){
              ((player_t*)p1)->worms_status |= STATUS_SHOT;
              coll = 1;
        }
        if((((player_t*)p2)->worms_status & STATUS_ALIVE)
            &&( x - bullet->range <= ((player_t*)p2)->worms.pos_x)
            &&( x + bullet->range >= ((player_t*)p2)->worms.pos_x)
            &&( y - bullet->range <= ((player_t*)p2)->worms.pos_y-3)
            &&( y + bullet->range >= ((player_t*)p2)->worms.pos_y-3)){
              ((player_t*)p2)->worms_status |= STATUS_SHOT;
              coll = 1;
        }
        coll += bullet->obj.test_collision_cb(ground, x , y,bullet->obj.acc_x,bullet->obj.acc_y); 
        coll += bullet->obj.test_collision_cb(statics, x , y,bullet->obj.acc_x,bullet->obj.acc_y);

        if(coll!=0){
           if(bullet->obj.on_collision_cb)
              bullet->obj.on_collision_cb(bullet,last_x,last_y,x,y,p1,p2,userdata);

           ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
           ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
           return coll;
        }
        last_x = x;
        last_y = y;
      }
      i++;
    }
  }else{
        coll = 0;
        /* don't move */
        if((((player_t*)p1)->worms_status & STATUS_ALIVE)
            &&( last_x - bullet->range <= ((player_t*)p1)->worms.pos_x)
            &&( last_x + bullet->range >= ((player_t*)p1)->worms.pos_x)
            &&( last_y - bullet->range <= ((player_t*)p1)->worms.pos_y-3)
            &&( last_y + bullet->range >= ((player_t*)p1)->worms.pos_y-3)){
              ((player_t*)p1)->worms_status |= STATUS_SHOT;
              coll = 1;
        }
        if((((player_t*)p2)->worms_status & STATUS_ALIVE)
            &&( last_x - bullet->range <= ((player_t*)p2)->worms.pos_x)
            &&( last_x + bullet->range >= ((player_t*)p2)->worms.pos_x)
            &&( last_y - bullet->range <= ((player_t*)p2)->worms.pos_y-3)
            &&( last_y + bullet->range >= ((player_t*)p2)->worms.pos_y-3)){
              ((player_t*)p2)->worms_status |= STATUS_SHOT;
              coll = 1;
        } 
        if(coll){
          bullet->obj.on_collision_cb(bullet,last_x,last_y,last_x,last_y,p1,p2,userdata);
          ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
          ((player_t*)p2)->worms_status &= ~STATUS_SHOT;    
          return coll;
        }
  }
  DBG("%d-%d\n",last_x,last_y);
  bullet->obj.pos_x = last_x;
  bullet->obj.pos_y = last_y;
  return coll;
}

static __inline__ void weapon_apply_physics(bullet_t* bullet,SDL_Surface* ground,SDL_Surface* statics,void* p1, void* p2,void* userdata){
  ASSERT(bullet);
  ASSERT(ground);
  ASSERT(statics);
  ASSERT(p1);
  ASSERT(p2);
  if(std_weap_info[bullet->w_id].timeout){
    if(bullet->timeout == 0){
      if(std_weap_info[bullet->w_id].on_collision_cb)
        std_weap_info[bullet->w_id].on_collision_cb(bullet,bullet->obj.pos_x,bullet->obj.pos_y,bullet->obj.pos_x,bullet->obj.pos_y,p1,p2,ground);
      bullet->obj.remove_flag = 1;
      return;
    }else{
      bullet->timeout--;
    }
  }else{
    if(bullet->timeout){
       bullet->timeout--;
      if(bullet->timeout == 0){
        bullet_on_collision_cb(bullet,bullet->obj.pos_x,bullet->obj.pos_y,bullet->obj.pos_x,bullet->obj.pos_y,p1,p2,ground);
        bullet->obj.remove_flag = 1;
        return;
      }
    }
  }
  DBG(" - APPLY GRAV\n");
  apply_gravity(&(bullet->obj));
  DBG(" - CHECK COLL\n");
  weapon_check_all_collisions(bullet,ground,statics,p1,p2,userdata);
}


bullet_list_t* weapon_init_bullet_list(){
  return secure_malloc(sizeof(bullet_list_t));
}

bullet_t* weapon_add_bullet_to_list(bullet_list_t* l,bullet_t* bullet){
  ASSERT(l);
  bullet_cell_t* cell = 0l;
    
  if(l->len >= MAX_SUPPORTED_BULLETS){
    bullet->obj.remove_flag = 1;
  }
  cell = secure_malloc(sizeof(bullet_cell_t));
  cell->bullet = bullet;

  if(l->head)
    cell->next = l->head;
  else
    cell->next = 0L;

  l->head = cell;
  l->len++;
  return bullet;
}

void clean_bullets_list(bullet_list_t* l){
  ASSERT(l);
  bullet_cell_t* cell = l->head; 
  bullet_cell_t* cell_to_remove = 0l;
  while(cell){
    if(cell->bullet){
      secure_free(cell->bullet);
      cell->bullet = 0L;
      cell_to_remove = cell;
      cell = cell->next;
      secure_free(cell_to_remove);
      l->len--;
    }
  }
  l->head = 0l;
}

void proceed_bullets(bullet_list_t* l,SDL_Surface* ground,SDL_Surface* statics
                     , void* p1, void* p2,void* coll_cb_userdata){
  ASSERT(l);
  ASSERT(ground);
  ASSERT(statics);
  ASSERT(p1);
  ASSERT(p2);
  bullet_cell_t* last_cell = 0l;
  bullet_cell_t* cell_to_remove = 0l;
  bullet_cell_t* cell = l->head;

  if(!cell)
    return;

  while(cell){
    if(cell->bullet->obj.remove_flag){
      sengine_play(std_weap_info[cell->bullet->w_id].explo_sound,SENGINE_PLAY_ACTION);
      secure_free(cell->bullet);
      cell->bullet = 0L;
      /* update head */
      if(l->head == cell)
        l->head = cell->next;
      if(last_cell)
        last_cell->next = cell->next;
      cell_to_remove = cell;
      cell = cell->next;
      secure_free(cell_to_remove);
      l->len--;
    }else{
      if(cell->bullet->obj.update_cb)
        cell->bullet->obj.update_cb(cell->bullet,p1,p2,ground);
      weapon_apply_physics(cell->bullet,ground,statics,p1,p2,coll_cb_userdata);
      last_cell=cell;
      cell = cell->next;
    }
  }
}


void blit_bullets(bullet_list_t* l,camera_t* cam1,camera_t* cam2){
  ASSERT(l);
  ASSERT(cam1);
  ASSERT(cam2);
  bullet_cell_t* cell = l->head;
  if(!cell)
    return;
  while(cell){
    if(cell->bullet->obj.blit_cb){

      switch(cell->bullet->w_id){
      case WEAPON_LASER:
        {
          player_t* p = ((player_t*)cell->bullet->p_origin);
          if(p->weapon_slots[p->selected_weapon]->id == WEAPON_LASER ){
              // laser in use 
              // blit last
              cell->bullet->obj.blit_cb(cam1,cell->bullet,l);
              cell->bullet->obj.blit_cb(cam2,cell->bullet,l);
          }
          break;
        }
        case WEAPON_NINJA:
          cell->bullet->obj.blit_cb(cam1,cell->bullet,l);
          cell->bullet->obj.blit_cb(cam2,cell->bullet,l);
          break;
        default:
          if((cell->bullet->obj.pos_x >= cam1->map_x)
           &&(cell->bullet->obj.pos_x < (cam1->map_x + cam1->w))
           &&(cell->bullet->obj.pos_y >= cam1->map_y)
           &&(cell->bullet->obj.pos_y < (cam1->map_y + cam1->h)))
              cell->bullet->obj.blit_cb(cam1,cell->bullet,l);

          if((cell->bullet->obj.pos_x >= cam2->map_x)
           &&(cell->bullet->obj.pos_x < (cam2->map_x + cam2->w))
           &&(cell->bullet->obj.pos_y >= cam2->map_y)
           &&(cell->bullet->obj.pos_y < (cam2->map_y + cam2->h)))
             cell->bullet->obj.blit_cb(cam2,cell->bullet,l);
          break;
      }
    }
    cell = cell->next;
  }
} 

void create_gift(bullet_list_t* l,ressources_t* r,int x,int y,int xtra_weap){
  switch(rand()%2){
    case 0:  /*HEALTH GIFT*/
      weapon_add_bullet_to_list( l, init_with_skin( WEAPON_HEALTH_GIFT
                                                  , x, y
                                                  , 0, 0
                                                  , r->health,0l));
      break;
    case 1:  /*BONUS GIFT*/
      weapon_add_bullet_to_list( l , init_with_skin( WEAPON_BONUS_GIFT
                                                   , x, y
                                                   , 0, 0
                                                   , r->bonus,0l)
                               )->damages = (xtra_weap) 
                                            ? rand()%MAX_WEAPONS_AND_XTRA 
                                            : rand()%MAX_WEAPONS;
      break;
  }
}

void set_player_house(bullet_list_t* l,ressources_t* r,int playerid,void* layer){
  ASSERT(layer)
  ASSERT(layer)
  ASSERT(r)
  int x=0, y=0;
  get_empty_layer_position(&x,&y,(SDL_Surface*)layer);
  weapon_add_bullet_to_list(l,init_with_skin(((playerid == PLAYER_1) 
                                             ? WEAPON_P1_HOUSE: WEAPON_P2_HOUSE)
                                            ,x, y, 0, 0
                                            ,((playerid == PLAYER_1) 
                                             ? r->house_p1: r->house_p2)
                                            ,0l));
}

void set_player_flag(bullet_list_t* l,ressources_t* r,int playerid,void* layer){
  ASSERT(l)
  ASSERT(layer)
  ASSERT(r)
  int x=0, y=0;
  get_empty_layer_position(&x,&y,(SDL_Surface*)layer);
  weapon_add_bullet_to_list(l,init_with_skin(((playerid == PLAYER_1) 
                                             ? WEAPON_P1_FLAG: WEAPON_P2_FLAG)
                                            ,x, y, 0, 0
                                            ,((playerid == PLAYER_1) 
                                             ? r->flag_p1: r->flag_p2)
                                            ,0l));
}

void drop_player_flag(bullet_list_t* l,ressources_t* r,int playerid,int x,int y){
  ASSERT(r)
  int acc_x= rand()%7-3;
  int acc_y= - rand()%3 - 1;
  weapon_add_bullet_to_list(l,init_with_skin(((playerid == PLAYER_1) 
                                             ? WEAPON_P1_FLAG: WEAPON_P2_FLAG)
                                            ,x, y, acc_x, acc_y
                                            ,((playerid == PLAYER_1) 
                                             ? r->flag_p1: r->flag_p2)
                                            ,0l));
}

/* * * * * * * * * * * * * BULLET SPRITE GET POSITION * * * * * * * * * * * * */
typedef struct{
  Uint8 x;
  Uint8 y;
}sprite_position_t;

static __inline__ int weapon_sprite_get_x_pos(side_t side,int angle){
  if(angle > 75)  return 2;
  if(angle < -75) return 0;  
  if(side == LEFT_SIDE){
    if(angle < -10) return 0;
    else            return 1;
  }else{
    if(angle > 10) return 2;
    else           return 3;
  }
}

static __inline__ sprite_position_t weapon_sprite_get_pos(side_t side,int angle){
  sprite_position_t position = {0,0};
  int new_angle = fast_abs(angle);
  //printf("side:%d angle:%d\n",side,angle);
  position.x = weapon_sprite_get_x_pos(side,angle);
  
  switch(position.x){
    case 0:
    case 2:
      if(new_angle > 75)      position.y=0;
      else if(new_angle > 50) position.y=1;
      else if(new_angle > 30) position.y=2;
      else if(new_angle > 10) position.y=3;
      break;
    case 1:
    case 3:
      if(new_angle > 75)      position.y=0;
      else if(new_angle > 50) position.y=3;
      else if(new_angle > 30) position.y=2;
      else if(new_angle > 10) position.y=1;
      break;

  }
  //printf("sprite position (%dx%d)\n",position.x,position.y);
  return position;
}

void wsprite_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);                                 
  sprite_position_t position = weapon_sprite_get_pos(o->side,o->angle);
  skin_offset.x = ( o->skin->w / DEFAULT_BULLET_MATRIX_STEPS) * position.x; 
  skin_offset.y = ( o->skin->w / DEFAULT_BULLET_MATRIX_STEPS) * position.y;   
  skin_offset.w = o->skin->w / DEFAULT_BULLET_MATRIX_STEPS; 
  skin_offset.h = o->skin->h / DEFAULT_BULLET_MATRIX_STEPS;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  camera_blit_surface_on(c,o->skin, &skin_offset, &camera_offset); 
}

/******************************************************************************/


/* * * * * * * * * * * * *  STD SKINNED BULLET  * * * * * * * * * * * * * * * */

bullet_t* init_with_skin( weapon_id w_id,int posx,int posy,int accx,int accy,SDL_Surface * skin,void* p_origin){
  bullet_t* bullet = secure_malloc(sizeof(bullet_t));
  bullet->damages = std_weap_info[w_id].damages;
  bullet->range = std_weap_info[w_id].range;
  bullet->timeout = std_weap_info[w_id].timeout;
  bullet->explo_size = std_weap_info[w_id].explo_size;
  bullet->obj.pos_x = posx;
  bullet->obj.pos_y = posy;
  bullet->obj.gravit=0;
  bullet->obj.side = NONE_SIDE;
  bullet->obj.skin = skin;
  bullet->obj.animation_pos = 0;
  bullet->obj.animation_sleep = 0;
  bullet->w_id  = w_id;
  bullet->obj.resist = std_weap_info[w_id].resist;
  bullet->obj.acc_x = accx;
  bullet->obj.acc_y = accy; 
  bullet->obj.test_collision_cb = std_weap_info[w_id].test_collision_cb;
  bullet->obj.on_collision_cb   = std_weap_info[w_id].on_collision_cb; 
  bullet->obj.blit_cb           = std_weap_info[w_id].blit_cb; 
  bullet->obj.update_cb         = std_weap_info[w_id].special_cb;
  bullet->p_origin =  p_origin;
  return bullet;
}

/* * * * * * * * * * * * * * * *  PIECES * * * * * * * * * * * * * * * * * * */

bullet_t* init_pieces(int posx,int posy,int accx,int accy, Uint8 r, Uint8 g, Uint8 b,int timeout,int damages,void * p_origin){
  bullet_t* pieces = secure_malloc(sizeof(bullet_t));
  pieces->damages = damages;
  pieces->range = 6;
  pieces->timeout = timeout;
  pieces->explo_size = 4;
  pieces->obj.pos_x = posx;
  pieces->obj.pos_y = posy;
  pieces->obj.gravit=0;
  pieces->obj.side = NONE_SIDE;
  pieces->obj.w = 1;
  pieces->obj.h = 1; 
  pieces->obj.r = r;
  pieces->obj.g = g;
  pieces->obj.b = b;
  pieces->w_id  = WEAPON_HANDGUN;  //use Handgun callbacks  
  pieces->obj.resist = 3;
  pieces->obj.acc_x = accx;
  pieces->obj.acc_y = accy;
  pieces->obj.test_collision_cb = bullet_collision_cb;
  pieces->obj.on_collision_cb   = bullet_on_collision_cb;
  pieces->obj.blit_cb           = bullet_blit_cb;
  pieces->obj.update_cb         = 0l;
  pieces->p_origin =  p_origin;
  return pieces;
}

/* * * * * * * * * * * * * * * EXPLOSIVE PIECES * * * * * * * * * * * * * * * */

void explosive_piece_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                    , void* p1,void* p2, void* userdata){
  int i;
  bullet_on_collision_cb(bullet, lastx, lasty, colx, coly,p1, p2,  userdata);

  for(i=0;i<4;i++){
    weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                   , init_pieces( colx + (rand() % 7) - 3
                                , coly + (rand() % 7) - 3 
                                , (rand() % 9) - 4
                                , (rand() % 7) - 4
                                , ((bullet_t*)bullet)->obj.r
                                , ((bullet_t*)bullet)->obj.g
                                , ((bullet_t*)bullet)->obj.b,30*2,2
                                , ((bullet_t*)bullet)->p_origin));
  }
}


bullet_t* init_explos_pieces( int posx,int posy,int accx,int accy
                            , Uint8 r, Uint8 g, Uint8 b,Uint8 timeout,int damages, void * p_origin){
  bullet_t* pieces = secure_malloc(sizeof(bullet_t));
  pieces->damages = damages;
  pieces->range = 6;
  pieces->timeout = timeout;
  pieces->explo_size = 6;
  pieces->obj.pos_x = posx;
  pieces->obj.pos_y = posy;
  pieces->obj.gravit=0;
  pieces->obj.side = NONE_SIDE;
  pieces->obj.w = 1;
  pieces->obj.h = 1; 
  pieces->obj.r = r;
  pieces->obj.g = g;
  pieces->obj.b = b;
  pieces->w_id  = WEAPON_HANDGUN;  //use Handgun callbacks  
  pieces->obj.resist = 2;
  pieces->obj.acc_x = accx;
  pieces->obj.acc_y = accy;
  pieces->obj.test_collision_cb = bullet_collision_cb;
  pieces->obj.on_collision_cb   = explosive_piece_on_collision_cb;
  pieces->obj.blit_cb           = bullet_blit_cb;
  pieces->obj.update_cb         = 0l;
  pieces->p_origin =  p_origin;
  return pieces;
}
/* * * * * * * * * * * * * * * * * BULLET * * * * * * * * * * * * * * * * * */

bullet_t* init_bullet( weapon_id w_id,int posx,int posy,int accx,int accy
                     , Uint8 r, Uint8 g, Uint8 b,void * p_origin){
  bullet_t* bullet = secure_malloc(sizeof(bullet_t));
  bullet->damages = std_weap_info[w_id].damages;
  bullet->range = std_weap_info[w_id].range;
  bullet->timeout = std_weap_info[w_id].timeout;
  bullet->explo_size = std_weap_info[w_id].explo_size;
  bullet->obj.pos_x = posx;
  bullet->obj.pos_y = posy;
  bullet->obj.gravit=0;
  bullet->obj.side = NONE_SIDE;
  bullet->obj.w = 1;
  bullet->obj.h = 1; 
  bullet->obj.r = r;
  bullet->obj.g = g;
  bullet->obj.b = b;
  bullet->w_id  = w_id;  
  bullet->obj.resist = std_weap_info[w_id].resist;
  bullet->obj.acc_x = accx;
  bullet->obj.acc_y = accy; 
  //printf("final acc : %dx%d\n",accx,accy);
  bullet->obj.test_collision_cb = std_weap_info[w_id].test_collision_cb;
  bullet->obj.on_collision_cb   = std_weap_info[w_id].on_collision_cb; 
  bullet->obj.blit_cb           = std_weap_info[w_id].blit_cb;
  bullet->obj.update_cb         = std_weap_info[w_id].special_cb;
  bullet->p_origin =  p_origin;
  return bullet;
}

bullet_t* create_bullet(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  //printf("tmp acc : %dx%d (velocity %f)\n",acc_x,acc_y, std_weap_info[w_id].velocity);
  return weapon_add_bullet_to_list( p->bullet_list_link
                           , init_bullet( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , 0xba,0xac,0x5e,p));
}

bullet_t* create_mg_bullet(weapon_id w_id,player_t *p,int acc_x,int acc_y){

  return weapon_add_bullet_to_list( p->bullet_list_link
                           , init_bullet( w_id
                                   , p->reticle_x + ((rand() % 5) - 2)
                                   , p->reticle_y + ((rand() % 5) - 2)
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , 0xc7,0xc7,0xc7,p));
                                   
}

int bullet_collision_cb(SDL_Surface* ground,int pos_x,int pos_y,int acc_x,int acc_y){
  ASSERT(ground);
  Uint8 r,g,b;
  if((pos_x>=ground->w) || (pos_x<0) || (pos_y>=ground->h) || (pos_y<0))
    return 1;
  get_pix_color(ground,pos_x,pos_y,&r,&g,&b);
  if(!((r==transparent_r_value)&&(g ==transparent_g_value)&&(b ==transparent_b_value)))
    return 1;
  return 0;
}

void bullet_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect particle;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  particle.x = o->pos_x - c->map_x;
  particle.y = o->pos_y - c->map_y;
  particle.w = o->w; 
  particle.h = o->h;
  camera_fillrect(c,&particle,SDL_MapRGB(c->cam_surface->format,o->r,o->g,o->b));
}

static __inline__ void bullet_explode(obj_list_t* dynamic_list,int x,int y, ressources_t* r,int explo_size){
  int it=0;
  object_add_to_list(dynamic_list, create_explosion(x,y,r,explo_size));
  if( mud_particle)  
    for(it=0;it<5;it++)
      object_add_to_list(dynamic_list,create_mud_particle( x+rand()%9-4
                                                         , y+rand()%9-4
                                                         , rand()%5-2
                                                         , rand()%5-2));
}

void bullet_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata){
  ASSERT(bullet)
  ASSERT(userdata)
  ASSERT(p1)
  ASSERT(p2)
  SDL_Surface* ground = (SDL_Surface*) userdata;
  bullet_t* b = (bullet_t*)bullet;
  colx = (colx < 0)? 0 : colx ;
  coly = (coly < 0)? 0 : coly ;
  colx = (colx >= ground->w) ? ground->w-1 : colx ;
  coly = (coly >= ground->h) ? ground->h-1 : coly ;
  drow_circle(ground,colx,coly,b->range+b->damages/15,transparent_r_value,transparent_g_value,transparent_b_value);
  b->obj.remove_flag = 1;
  bullet_explode( ((player_t*)p1)->dynamic_list_link,colx,coly
                , ((player_t*)p1)->r,b->explo_size);

   if(((player_t*)p1)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);

   if(((player_t*)p2)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata); 

  ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
  ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
  b->obj.pos_x=colx;
  b->obj.pos_y=coly;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * * * FLAME * * * * * * * * * * * * * * * * * */


bullet_t* create_flame(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                    ,init_with_skin( w_id
                                   , p->reticle_x + ((rand()%5) - 2)
                                   , p->reticle_y + ((rand()%5) - 2)
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->flamer,p));
}

bullet_t* create_blue_flame(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                    ,init_with_skin( w_id
                                   , p->reticle_x + ((rand()%5) - 2)
                                   , p->reticle_y + ((rand()%5) - 2)
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->blue_flamer,p));
}

void flame_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);                                 
  skin_offset.x = ( o->skin->w / DEFAULT_FLAMER_ANIM_STEPS) * (o->animation_pos/4); 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_FLAMER_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  o->animation_pos += 1;
  if((o->animation_pos/4) >= DEFAULT_FLAMER_ANIM_STEPS)
    o->remove_flag = 1;
}

void fire_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);                                
  skin_offset.x = ( o->skin->w / DEFAULT_FLAMER_ANIM_STEPS) * (o->animation_pos/4); 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_FLAMER_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  o->animation_pos += 1;
  if((o->animation_pos/4) >= DEFAULT_FLAMER_ANIM_STEPS)
    o->animation_pos = 4;
}


void flame_on_collision_cb( void* bullet,int lastx,int lasty,int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  ASSERT(bullet)
  ASSERT(userdata)
  ASSERT(p1)
  ASSERT(p2)
  SDL_Surface* ground = (SDL_Surface*) userdata;
  bullet_t* b = (bullet_t*)bullet;   
  colx = (colx < 0)? 0 : colx ;
  coly = (coly < 0)? 0 : coly ;
  colx = (colx >= ground->w) ? ground->w-1 : colx ;  
  coly = (coly >= ground->h) ? ground->h-1 : coly ;
  drow_circle(ground,colx,coly,b->range,transparent_r_value,transparent_g_value,transparent_b_value);
   if(((player_t*)p1)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);

   if(((player_t*)p2)->worms_status & STATUS_SHOT) 
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata); 

  ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
  ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
}

void fire_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  ASSERT(bullet)
  ASSERT(userdata)
  ASSERT(p1)
  ASSERT(p2)
  SDL_Surface* ground = (SDL_Surface*) userdata;
  bullet_t* b = (bullet_t*)bullet;   
  colx = (colx < 0)? 0 : colx ;
  coly = (coly < 0)? 0 : coly ;
  colx = (colx >= ground->w) ? ground->w-1 : colx ;  
  coly = (coly >= ground->h) ? ground->h-1 : coly ;
  put_pix_color(ground, colx, coly,transparent_r_value,transparent_g_value,transparent_b_value);
  put_pix_color(ground, colx-1, coly-1,transparent_r_value,transparent_g_value,transparent_b_value);
  put_pix_color(ground, colx+1, coly-1,transparent_r_value,transparent_g_value,transparent_b_value);

   if(((player_t*)p1)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,0,0,userdata);

   if(((player_t*)p2)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,0,0,userdata); 

  ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
  ((player_t*)p2)->worms_status &= ~STATUS_SHOT;  
  b->obj.acc_y = 0;
  b->obj.acc_x = 0;  
}


void fire_update(void* bullet,void *p1,void *p2, void* userdata){
  bullet_t* b = (bullet_t*)bullet;
  if(b->obj.acc_x == 0)
    b->obj.acc_x = (rand()%41 - 20)/20 ;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * SPORE PARTICULE * * * * * * * * * * * * * * */
bullet_t*create_spore(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                           , init_bullet( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , 0x1D,0x85,0x57,p));
}

void spore_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect particle;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  particle.x = o->pos_x - c->map_x;
  particle.y = o->pos_y - c->map_y;
  particle.w = o->w; 
  particle.h = o->h;
  o->r = (o->r + (rand()%21-10));
  o->r = (o->r < 0x1D) ? 0x1D : o->r;
  o->r = (o->r > 0xD4) ? 0xD4 : o->r;
  o->b = (o->b + (rand()%21-10));
  o->b = (o->b < 0x57) ? 0x57 : o->b;
  o->b = (o->b > 0xA4) ? 0xA4 : o->b;
  camera_fillrect(c,&particle,SDL_MapRGB(c->cam_surface->format,o->r,o->g,o->b));
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * GAZ PARTICULE * * * * * * * * * * * * * * * */
bullet_t* create_gaz(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                  ,init_with_skin( w_id
                                 , p->reticle_x+ ((rand()%5) - 2)
                                 , p->reticle_y+ ((rand()%5) - 2)
                                 , 0
                                 , 0
                                 , p->r->smoke,p)); 
}

void gaz_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  ASSERT(bullet)
  ASSERT(userdata)
  ASSERT(p1)
  ASSERT(p2)
  bullet_t* b = (bullet_t*)bullet;    
  if(rand()%3 == 2){
    if(((player_t*)p1)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,0,0,userdata); 
    if(((player_t*)p2)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,0,0,userdata);
  }  
  ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
  ((player_t*)p2)->worms_status &= ~STATUS_SHOT;  
  b->obj.acc_y = 0;
  b->obj.acc_x = 0;
}

void gaz_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);                                
  skin_offset.x = ( o->skin->w / DEFAULT_SMOKE_ANIM_STEPS) * (o->animation_pos/4); 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_SMOKE_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  o->animation_pos += 1;
  if((o->animation_pos/4) >= DEFAULT_SMOKE_ANIM_STEPS)
    o->animation_pos = 4; 
}

void gaz_update(void* bullet,void *p1,void *p2, void* userdata){
  bullet_t* b = (bullet_t*)bullet;
  b->obj.acc_x += (rand()%3 ) - 1 ;
  b->obj.acc_y += (rand()%3 ) - 1 ;
  b->obj.acc_x = (b->obj.acc_x >  4) ?  4 : b->obj.acc_x;
  b->obj.acc_x = (b->obj.acc_x < -4) ? -4 : b->obj.acc_x;
  b->obj.acc_y = (b->obj.acc_y >  4) ?  4 : b->obj.acc_y;
  b->obj.acc_y = (b->obj.acc_y < -4) ? -4 : b->obj.acc_y; 
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * * * RIFLE * * * * * * * * * * * * * * * * * */
void rifle_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata){
  int i;                         
  bullet_on_collision_cb(bullet, lastx, lasty, colx, coly,p1, p2,  userdata);
                            
  for(i=0;i<8;i++){
    weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                    , init_explos_pieces( colx + (rand() % 5) - 2
                                        , coly + (rand() % 5) - 2
                                        , (rand() % 7) - 3
                                        , (rand() % 7) - 3
                                        , 0x9f,0x9e,0x5a,6,2
                                        , ((bullet_t*)bullet)->p_origin));
  }
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * * VERMIFUGE * * * * * * * * * * * * * * * * */
bullet_t* create_vermifuge(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                             , init_bullet( w_id
                                , p->reticle_x + (rand() % 11) - 5
                                , p->reticle_y + (rand() % 11) - 5 
                                , acc_x * std_weap_info[w_id].velocity
                                , acc_y * std_weap_info[w_id].velocity
                                , 0x21 - (rand() % 21), 0xFF - (rand() % 21)
                                , 0x21 - (rand() % 21) ,p));
}

void vermifuge_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata){
  bullet_t *b =  (bullet_t*) bullet;
  if((((player_t*)p1)->worms_status & STATUS_SHOT)
     ||(((player_t*)p2)->worms_status & STATUS_SHOT)){
    /* HIT !*/
    if(rand()%3==0)
      return;
    if(((player_t*)p1)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);

    if(((player_t*)p2)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata); 

    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT; 
    object_add_to_list( ((player_t*)b->p_origin)->dynamic_list_link
                       , create_nuke_smoke( b->obj.pos_x + (rand() % 7) - 3
                                          , b->obj.pos_y + (rand() % 7) - 3
                                          , ((player_t*)b->p_origin)->r));
  }else{
    b->obj.acc_x = 0;
    b->obj.acc_y = 0;
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * * SHOOTGUNS * * * * * * * * * * * * * * * * */

bullet_t* create_shootgun_bullet(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  int i;
  bullet_t* last;
  for(i=0;i<8;i++){
    last = weapon_add_bullet_to_list( p->bullet_list_link
                             , init_bullet( w_id
                                , p->reticle_x + (rand() % 7) - 3
                                , p->reticle_y + (rand() % 7) - 3 
                                , acc_x * std_weap_info[w_id].velocity + ((rand() % 3) - 1)
                                , acc_y * std_weap_info[w_id].velocity + ((rand() % 3) - 1)
                                , 0x9d,0x9d,0x9d,p));
  }
  return last;
}



bullet_t* create_super_shootgun_bullet(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  int i;
  bullet_t* last;
  for(i=0;i<15;i++){
    last = weapon_add_bullet_to_list( p->bullet_list_link
                   , init_bullet( w_id
                                , p->reticle_x + (rand() % 9) - 4
                                , p->reticle_y + (rand() % 9) - 4 
                                , acc_x * std_weap_info[w_id].velocity + ((rand() % 3) - 1)
                                , acc_y * std_weap_info[w_id].velocity + ((rand() % 3) - 1)
                                , 0xd5,0xd5,0xd5,p));
  }
  return last;
}

void shootgun_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata){
   bullet_t* bull = (bullet_t*) bullet;
   bullet_on_collision_cb(bullet, lastx, lasty, colx, coly,p1, p2,  userdata); 
   weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                , init_pieces( colx + (rand() % 7) - 3
                                    , coly + (rand() % 7) - 3 
                                    , bull->obj.acc_x + (rand() % 9) - 4
                                    , bull->obj.acc_y + (rand() % 7) - 4
                                    , 0xd5,0xd5,0xd5,5,4,((bullet_t*)bullet)->p_origin));                          
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * * BAZOOKA * * * * * * * * * * * * * * * * */
bullet_t* create_bazooka(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  bullet_t* bullet = weapon_add_bullet_to_list( p->bullet_list_link
                    ,init_with_skin( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->missile,p));
  bullet->obj.angle = p->worms.angle;
  bullet->obj.side = p->worms.side;
  return bullet;
}

void bazooka_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                              , void* p1,void* p2, void* userdata){
  int i=0;
  bullet_on_collision_cb(bullet, lastx, lasty, colx, coly,p1, p2,  userdata);
  for(i=0;i<4;i++)
   weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                , init_explos_pieces( colx + (rand() % 7) - 3
                                                        , coly + (rand() % 7) - 3 
                                                        , (rand() % 9) - 4
                                                        , (rand() % 7) - 4
                                                        , 0x84,0x84,0x84,5,4,((bullet_t*)bullet)->p_origin));                                
}

void bazooka_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  sprite_position_t position = weapon_sprite_get_pos(o->side,o->angle);
  skin_offset.x = ( o->skin->w / DEFAULT_BULLET_MATRIX_STEPS) * position.x; 
  skin_offset.y = ( o->skin->w / DEFAULT_BULLET_MATRIX_STEPS) * position.y;   
  skin_offset.w = o->skin->w / DEFAULT_BULLET_MATRIX_STEPS; 
  skin_offset.h = o->skin->h / DEFAULT_BULLET_MATRIX_STEPS;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  object_add_to_list( ((player_t*)b->p_origin)->dynamic_list_link
                    , create_smoke( b->obj.pos_x - (b->obj.acc_x) + rand()%3-2
                                  , b->obj.pos_y - (b->obj.acc_y) + rand()%3-2
                                  , ((player_t*)b->p_origin)->r));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * DOOMSDAY * * * * * * * * * * * * * * * * */
bullet_t* create_doomsday(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  bullet_t* bullet = weapon_add_bullet_to_list( p->bullet_list_link
                    ,init_with_skin( w_id
                                   , p->reticle_x + ((rand() % 9) - 4)
                                   , p->reticle_y + ((rand() % 9) - 4)
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->doomsday,p));
  bullet->obj.angle = p->worms.angle;
  bullet->obj.side = p->worms.side;
  bullet = weapon_add_bullet_to_list( p->bullet_list_link
                    ,init_with_skin( w_id
                                   , p->reticle_x + ((rand() % 9) - 4)
                                   , p->reticle_y + ((rand() % 9) - 4)
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->doomsday,p));
  bullet->obj.angle = p->worms.angle;
  bullet->obj.side = p->worms.side;  
  return bullet;
}

void doomsday_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  int i=0;  
  sprite_position_t position = weapon_sprite_get_pos(o->side,o->angle);
  skin_offset.x = ( o->skin->w / DEFAULT_BULLET_MATRIX_STEPS) * position.x; 
  skin_offset.y = ( o->skin->w / DEFAULT_BULLET_MATRIX_STEPS) * position.y;   
  skin_offset.w = o->skin->w / DEFAULT_BULLET_MATRIX_STEPS; 
  skin_offset.h = o->skin->h / DEFAULT_BULLET_MATRIX_STEPS;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset);
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  for(i=0;i<2;i++)  
  object_add_to_list( ((player_t*)b->p_origin)->dynamic_list_link
                    , create_reactor_particle( b->obj.pos_x - (b->obj.acc_x/2) + ((rand() % 5) - 2)
                                     , b->obj.pos_y - (b->obj.acc_y/2) + ((rand() % 5) - 2)));
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * MINI ROCKET * * * * * * * * * * * * * * * * */
bullet_t* create_minirocket(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  bullet_t* bullet = weapon_add_bullet_to_list( p->bullet_list_link
                    ,init_with_skin( w_id
                                   , p->reticle_x + ((rand() % 3) - 1)
                                   , p->reticle_y + ((rand() % 3) - 1)
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->larpa,p));
  bullet->obj.angle = p->worms.angle;
  bullet->obj.side = p->worms.side;
  return bullet;
}

void minirocket_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);       
  sprite_position_t position = weapon_sprite_get_pos(o->side,o->angle);
  skin_offset.x = ( o->skin->w / DEFAULT_BULLET_MATRIX_STEPS) * position.x; 
  skin_offset.y = ( o->skin->w / DEFAULT_BULLET_MATRIX_STEPS) * position.y;   
  skin_offset.w = o->skin->w / DEFAULT_BULLET_MATRIX_STEPS; 
  skin_offset.h = o->skin->h / DEFAULT_BULLET_MATRIX_STEPS;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset);
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  object_add_to_list( ((player_t*)b->p_origin)->dynamic_list_link
                    , create_reactor_particle( b->obj.pos_x - (b->obj.acc_x/2) + ((rand() % 3) - 1)
                                             , b->obj.pos_y - (b->obj.acc_y/2) + ((rand() % 3) - 1))); 
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * CANNON * * * * * * * * * * * * * * * * */
void cannonball_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);                                 
  skin_offset.x = ( o->skin->w / DEFAULT_CANNON_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_CANNON_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  o->animation_pos = (o->animation_pos+1)% DEFAULT_CANNON_ANIM_STEPS;
}


bullet_t* create_cannonball(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                    ,init_with_skin( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->cannon,p));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 
/* * * * * * * * * * * * * * *  GRASS HOPPER * * * * * * * * * * * * * * * * */
void grass_hopper_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);                                 
  skin_offset.x = 0; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
}


bullet_t* create_grass_hopper(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  bullet_t * b = weapon_add_bullet_to_list( p->bullet_list_link
                     ,init_with_skin(w_id
                         , p->reticle_x
                         , p->reticle_y
                         , acc_x * std_weap_info[w_id].velocity
                         , acc_y * std_weap_info[w_id].velocity
                         , p->r->grass_hopper,p));
  b->obj.update_cb = 0l; // grass_hopper become erratic and unpredictable after the first explosion :-)
  return b;
}

void grass_hopper_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  ASSERT(bullet)
  bullet_t * b = (bullet_t *) bullet;
  bullet_on_collision_cb(bullet, lastx, lasty, colx, coly,p1, p2,  userdata);               						  
  weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
		                       , init_explos_pieces( colx + (rand() % 7) - 3
							                                 , coly + (rand() % 7) - 3 
							                                 , (rand() % 9) - 4
							                                 , (rand() % 7) - 4
							                                 , 0x34,0x98,0x34,2,4
							                                 , b->p_origin));				  
  b->obj.remove_flag = 0;
  b->obj.acc_x = (-b->obj.acc_x) + (rand()%3 - 1);
  if(b->obj.acc_x > 8 ) b->obj.acc_x = 8;
  b->obj.acc_y = (-b->obj.acc_y) + (rand()%3 - 1);
  if(b->obj.acc_y > 8 ) b->obj.acc_y = 8;
  
  if(!b->obj.update_cb)
    b->obj.update_cb = std_weap_info[b->w_id].special_cb;
}

void grass_hopper_update(void* bullet,void *p1,void *p2, void* userdata){
   ASSERT(bullet)
   bullet_t * b = (bullet_t *) bullet;
   
   if(rand()%8 == 7){
     grass_hopper_on_collision_cb(bullet,b->obj.pos_x,b->obj.pos_y,b->obj.pos_x,b->obj.pos_y,p1,p2,userdata);
   }
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 
/* * * * * * * * * * * * * * * * *  MINE * * * * * * * * * * * * * * * * * * */

void mine_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);                                
  skin_offset.x = ( o->skin->w / DEFAULT_MINE_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_MINE_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  if(o->acc_y || o->acc_x )
     o->animation_pos = (o->animation_pos+1)% DEFAULT_MINE_ANIM_STEPS;
  
}

bullet_t* create_mine(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                           ,init_with_skin( w_id
                                 , p->reticle_x
                                 , p->reticle_y
                                 , acc_x * std_weap_info[w_id].velocity
                                 , acc_y * std_weap_info[w_id].velocity
                                 , p->r->mine,p));
}


void mine_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;

  if(  (((player_t*)p1)->worms_status & STATUS_SHOT)
     ||(((player_t*)p2)->worms_status & STATUS_SHOT)
     || (b->timeout == 0))
    {
  /* BOOM !*/        
    int i;
    SDL_Surface* ground = (SDL_Surface*) userdata;  
    colx = (colx < 0)? 0 : colx ;
    coly = (coly < 0)? 0 : coly ;
    colx = (colx >= ground->w) ? ground->w-1 : colx ;
    coly = (coly >= ground->h) ? ground->h-1 : coly ;
    drow_circle(ground,colx,coly,b->range+b->damages/15,transparent_r_value,transparent_g_value,transparent_b_value);
    b->obj.remove_flag = 1;
    bullet_explode( ((player_t*)p1)->dynamic_list_link,colx,coly
                , ((player_t*)p1)->r,b->explo_size);

    if(((player_t*)p1)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);

    if(((player_t*)p2)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata); 

    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT;

    for(i=0;i<10;i++)
      weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                              , init_pieces( colx + (rand() % 7) - 3
                                           , coly + (rand() % 7) - 3
                                           , (rand() % 9) - 4
                                           , -(rand() % 5)
                                           , 0xd5,0xd5,0xd5,0,2,b->p_origin));
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(9*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (9*b->obj.acc_x/10);
    b->obj.acc_y = -(b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}

void spike_tbomb_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if(b->timeout == 0){
  /* BOOM !*/
    int i;
    SDL_Surface* ground = (SDL_Surface*) userdata;  
    colx = (colx < 0)? 0 : colx ;
    coly = (coly < 0)? 0 : coly ;
    colx = (colx >= ground->w) ? ground->w-1 : colx ;
    coly = (coly >= ground->h) ? ground->h-1 : coly ;
    drow_circle(ground,colx,coly,b->range+b->damages/15,transparent_r_value,transparent_g_value,transparent_b_value);
    b->obj.remove_flag = 1;
    bullet_explode( ((player_t*)p1)->dynamic_list_link,colx,coly
                  , ((player_t*)p1)->r,b->explo_size);

    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
    for(i=0;i<40;i++)
      weapon_add_bullet_to_list( ((player_t*)b->p_origin)->bullet_list_link
          ,init_with_skin(WEAPON_SPIKEBALLS
                         , b->obj.pos_x + ((rand()%21) - 10)
                         , b->obj.pos_y + (rand()%5) + 2
                         , ((rand()%11) - 5)
                         , (rand()%2)
                         , ((player_t*)b->p_origin)->r->spikeball,b->p_origin));
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(9*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (9*b->obj.acc_x/10);
    b->obj.acc_y = -(b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}

void ceiling_mine_update(void* bullet,void *p1,void *p2, void* userdata){
  if(((bullet_t *) bullet)->obj.acc_y > -4)
  ((bullet_t *) bullet)->obj.acc_y -= 1;
}


bullet_t* create_jumping_mine(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                           ,init_with_skin( w_id
                                 , (p->reticle_x + (((p->reticle_x - p->worms.pos_x)) /2))
                                 , (p->reticle_y + (((p->reticle_y - p->worms.pos_y)) /2))
                                 , acc_x * std_weap_info[w_id].velocity
                                 , acc_y * std_weap_info[w_id].velocity
                                 , p->r->jmine,p));
}


void jmine_activ_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  int i;

  if(b->timeout != 0){
    b->obj.pos_x=colx;
    b->obj.pos_y=coly; 
    return;
  }
  SDL_Surface* ground = (SDL_Surface*) userdata; 
  colx = (colx < 0)? 0 : colx ;
  coly = (coly < 0)? 0 : coly ;
  colx = (colx >= ground->w) ? ground->w-1 : colx ;
  coly = (coly >= ground->h) ? ground->h-1 : coly ;
  drow_circle(ground,colx,coly,b->range+b->damages/15,transparent_r_value,transparent_g_value,transparent_b_value);
  b->obj.remove_flag = 1;
  bullet_explode( ((player_t*)p1)->dynamic_list_link,colx,coly
                , ((player_t*)p1)->r,b->explo_size);

  for(i=0;i<20;i++)
  weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
    , init_explos_pieces( colx + (rand() % 5) - 2
                 , coly + (rand() % 5) - 2
                 , (rand() % 11) - 5
                 , -(rand() % 4)
                 , 0x5e,0x5e,0x5e,0,2,b->p_origin));
}

void jumping_mine_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;

  if(  (((player_t*)p1)->worms_status & STATUS_SHOT)
     ||(((player_t*)p2)->worms_status & STATUS_SHOT)
     || (b->timeout == 0))
    {
  /* BOOM !*/
    b->obj.remove_flag = 1;
    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
              weapon_add_bullet_to_list( ((player_t*)b->p_origin)->bullet_list_link
                          ,init_with_skin( WEAPON_JMINE_ACTIV
                                , b->obj.pos_x
                                , b->obj.pos_y-2
                                , 0
                                , -2
                                , ((player_t*)b->p_origin)->r->jmine,b->p_origin));

  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(9*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (9*b->obj.acc_x/10);
    b->obj.acc_y = -(b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }                     
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void bouncy_mine_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);                                
  skin_offset.x = ( o->skin->w / DEFAULT_BOUNCY_MINE_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_BOUNCY_MINE_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  if(o->acc_y || o->acc_x )
     o->animation_pos = (o->animation_pos+1)% DEFAULT_BOUNCY_MINE_ANIM_STEPS; 
}

bullet_t* create_bouncy_mine(weapon_id w_id,player_t *p,int acc_x,int acc_y){
   return weapon_add_bullet_to_list( p->bullet_list_link
                           ,init_with_skin( w_id
                                 , p->reticle_x
                                 , p->reticle_y
                                 , acc_x * std_weap_info[w_id].velocity
                                 , acc_y * std_weap_info[w_id].velocity
                                 , p->r->bouncy_mine,p)); 
}

void bouncy_mine_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
 ASSERT(bullet)
  bullet_t * b = (bullet_t *) bullet;

  if( ( b->timeout == 0 )
    ||(((player_t*)p1)->worms_status & STATUS_SHOT)
    ||(((player_t*)p2)->worms_status & STATUS_SHOT)){

    /* BOOM !*/
    int i;
    SDL_Surface* ground = (SDL_Surface*) userdata;  
    colx = (colx < 0)? 0 : colx ;
    coly = (coly < 0)? 0 : coly ;
    colx = (colx >= ground->w) ? ground->w-1 : colx ;
    coly = (coly >= ground->h) ? ground->h-1 : coly ;
    drow_circle(ground,colx,coly,b->range+b->damages/15,transparent_r_value,transparent_g_value,transparent_b_value);
    b->obj.remove_flag = 1;
    bullet_explode( ((player_t*)p1)->dynamic_list_link,colx,coly
                  , ((player_t*)p1)->r,b->explo_size);

    if(((player_t*)p1)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);

    if(((player_t*)p2)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata); 

    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT;

    for(i=0;i<5;i++)      
      weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                  , init_pieces( colx + (rand() % 7) - 3
                                , coly + (rand() % 7) - 3
                                    , (rand() % 9) - 4
                                    , -(rand() % 5)
                              , 0xd5,0xd5,0xd5,0,2,b->p_origin));
    for(i=0;i<5;i++)      
      weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                  , init_pieces( colx + (rand() % 7) - 3
                              , coly + (rand() % 7) - 3
                                  , (rand() % 9) - 4
                                  , (rand() % 5)
                            , 0xd5,0xd5,0xd5,0,2,b->p_origin));
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -b->obj.acc_x;
    else 
      b->obj.acc_x = b->obj.acc_x;
    b->obj.acc_y = -b->obj.acc_y + (rand()%21/10 - 1);
    b->obj.acc_x += (rand()%21/10 - 1);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
 }
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * *  Pinball * * * * * * * * * * * * * * * * */
bullet_t* create_pinball(weapon_id w_id,player_t *p,int acc_x,int acc_y){
   return weapon_add_bullet_to_list( p->bullet_list_link
                           ,init_with_skin( w_id
                                 , p->reticle_x
                                 , p->reticle_y
                                 , acc_x * std_weap_info[w_id].velocity
                                 , acc_y * std_weap_info[w_id].velocity
                                 , p->r->pinball,p)); 
}

void pinball_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
 ASSERT(bullet)
  bullet_t * b = (bullet_t *) bullet;
  if( b->timeout == 0 )
    b->obj.remove_flag = 1;

  if((((player_t*)p1)->worms_status & STATUS_SHOT)
     ||(((player_t*)p2)->worms_status & STATUS_SHOT)){

    if(((player_t*)p1)->worms_status & STATUS_SHOT){
      ((player_t*)p1)->worms.acc_x += 2*b->obj.acc_x/3;
      ((player_t*)p1)->worms.acc_y += 2*b->obj.acc_y/3;
    }
    if(((player_t*)p2)->worms_status & STATUS_SHOT){
      ((player_t*)p2)->worms.acc_x += 2*b->obj.acc_x/3;
      ((player_t*)p2)->worms.acc_y += 2*b->obj.acc_y/3;
    }
    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
  }

  if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
    b->obj.acc_x = -b->obj.acc_x;
  else 
    b->obj.acc_x = b->obj.acc_x;
  b->obj.acc_y = -b->obj.acc_y + (rand()%21/10 - 1);
  b->obj.acc_x += (rand()%21/10 - 1);
  b->obj.pos_x=lastx;
  b->obj.pos_y=lasty; 
  b->obj.acc_x = b->obj.acc_x % 10;
  b->obj.acc_y = b->obj.acc_y % 10;
 }
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * *  FLOAT MINE * * * * * * * * * * * * * * * * */
void float_mine_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);                                
  skin_offset.x = ( o->skin->w / DEFAULT_FLOAT_MINE_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_FLOAT_MINE_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  o->animation_pos = (o->animation_pos+rand()%5/4) % DEFAULT_FLOAT_MINE_ANIM_STEPS; 
}

bullet_t* create_float_mine(weapon_id w_id,player_t *p,int acc_x,int acc_y){
   return weapon_add_bullet_to_list( p->bullet_list_link
                         ,init_with_skin( w_id
                               , p->reticle_x
                               , p->reticle_y
                               , acc_x * std_weap_info[w_id].velocity
                               , acc_y * std_weap_info[w_id].velocity
                               , p->r->float_mine,p)); 
}
  
void float_mine_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  ASSERT(bullet)
  bullet_t * b = (bullet_t *) bullet;
  if( ( b->timeout == 0 )
    ||(((player_t*)p1)->worms_status & STATUS_SHOT)
    ||(((player_t*)p2)->worms_status & STATUS_SHOT)){

    /* BOOM !*/
    int i;
    SDL_Surface* ground = (SDL_Surface*) userdata;  
    colx = (colx < 0)? 0 : colx ;
    coly = (coly < 0)? 0 : coly ;
    colx = (colx >= ground->w) ? ground->w-1 : colx ;
    coly = (coly >= ground->h) ? ground->h-1 : coly ;
    drow_circle(ground,colx,coly,b->range+b->damages/15,transparent_r_value,transparent_g_value,transparent_b_value);
    b->obj.remove_flag = 1;
    bullet_explode( ((player_t*)p1)->dynamic_list_link,colx,coly
                  , ((player_t*)p1)->r,b->explo_size);

    if(((player_t*)p1)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);

    if(((player_t*)p2)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata); 

    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT;

    for(i=0;i<8;i++)
      weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                               , init_pieces( colx + (rand() % 7) - 3
                                            , coly + (rand() % 7) - 3
                                            , (rand() % 9) - 4
                                            , -(rand() % 5)
                                            , 0xad,0x40,0x08,0,2,b->p_origin));
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -b->obj.acc_x;
    else 
      b->obj.acc_x = b->obj.acc_x;
    b->obj.acc_y = -b->obj.acc_y + (rand()%21/10 - 1);
    b->obj.acc_x += (rand()%21/10 - 1);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}

void float_mine_update_cb(void* bullet,void *p1,void *p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  b->obj.acc_y = b->obj.acc_y/(rand()%2+1);
  b->obj.acc_x = b->obj.acc_x/(rand()%2+1);
  
  if(rand()%100 ==0)
    b->obj.acc_y = b->obj.acc_y + ((rand()%3) - 1);
  if(rand()%100 ==0)
    b->obj.acc_x = b->obj.acc_x + ((rand()%3) - 1);
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 
/* * * * * * * * * * * * * * * * *  Explosives * * * * * * * * * * * * * * * * */
bullet_t* create_explosives(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  bullet_t* last;
  int i;
  for(i=0;i<3;i++)
  last =  weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id
                                   , p->reticle_x + (rand() % 7) - 3
                                   , p->reticle_y + (rand() % 7) - 3
                                   , acc_x * std_weap_info[w_id].velocity + (rand() % 3) - 1
                                   , acc_y * std_weap_info[w_id].velocity + (rand() % 3) - 1
                                   , p->r->explosives,p));
    return last;
}

void explosive_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                            , void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if( b->timeout == 0 
     || ((((player_t*)p1)->worms_status & STATUS_SHOT) 
        || (((player_t*)p2)->worms_status & STATUS_SHOT)) ){
    /* BOOM !*/
    SDL_Surface* ground = (SDL_Surface*) userdata;  
    colx = (colx < 0)? 0 : colx ;
    coly = (coly < 0)? 0 : coly ;
    colx = (colx >= ground->w) ? ground->w-1 : colx ;  
    coly = (coly >= ground->h) ? ground->h-1 : coly ;
    drow_circle(ground,colx,coly,b->range+b->damages/15,transparent_r_value,transparent_g_value,transparent_b_value);
    b->obj.remove_flag = 1;
    bullet_explode( ((player_t*)p1)->dynamic_list_link,colx,coly
                  , ((player_t*)p1)->r,b->explo_size);

    if(((player_t*)p1)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,0,0,userdata);

    if(((player_t*)p2)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,0,0,userdata); 

    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(4*b->obj.acc_x/5);
    else 
      b->obj.acc_x = (4*b->obj.acc_x/5);
    b->obj.acc_y = -(2*b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}


void explosives_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);                                
  skin_offset.x = ( o->skin->w / DEFAULT_EXPLOSIVES_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_EXPLOSIVES_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  if(o->acc_y || o->acc_x )
     o->animation_pos = (o->animation_pos+1)% DEFAULT_EXPLOSIVES_ANIM_STEPS;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * *  grenade * * * * * * * * * * * * * * * * * */
void grenade_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);                                
  skin_offset.x = ( o->skin->w / DEFAULT_GRENADE_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_GRENADE_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  if(o->acc_y || o->acc_x )
     o->animation_pos = (o->animation_pos+1)% DEFAULT_GRENADE_ANIM_STEPS;
  
}

bullet_t* create_grenade(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->grenade,p));
}


void grenade_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                            , void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if( b->timeout == 0 ){
    /* BOOM !*/
    int i;
    SDL_Surface* ground = (SDL_Surface*) userdata;  
    colx = (colx < 0)? 0 : colx ;
    coly = (coly < 0)? 0 : coly ;
    colx = (colx >= ground->w) ? ground->w-1 : colx ;  
    coly = (coly >= ground->h) ? ground->h-1 : coly ;
    drow_circle(ground,colx,coly,b->range+b->damages/15,transparent_r_value,transparent_g_value,transparent_b_value);
    b->obj.remove_flag = 1;
    bullet_explode( ((player_t*)p1)->dynamic_list_link,colx,coly
                  , ((player_t*)p1)->r,b->explo_size);

    if(((player_t*)p1)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,0,0,userdata);

    if(((player_t*)p2)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,0,0,userdata); 

    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
    for(i=0;i<50;i++)
    weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                             , init_pieces( colx + (rand() % 11) - 5
                                          , coly + (rand() % 11) - 5 
                                          , (rand() % 11) - 5
                                          , -(rand() % 5)
                             , 0xd5,0xd5,0xd5,0,3,b->p_origin));
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(9*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (9*b->obj.acc_x/10);
    b->obj.acc_y = -(2*b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}

/* * * * * * * * * * * * * * *  ClusterBomb * * * * * * * * * * * * * * * * */
void cluster_piece_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  skin_offset.x = ( o->skin->w / DEFAULT_CLUSTER_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_CLUSTER_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  o->animation_pos = (o->animation_pos+1)% DEFAULT_CLUSTER_ANIM_STEPS;
}

void cluster_piece_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata){
   int i;
   bullet_on_collision_cb(bullet, lastx, lasty, colx, coly,p1, p2,  userdata);

    for(i=0;i<6;i++)
      weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
         , init_explos_pieces( colx + (rand() % 7) - 3
                              , coly + (rand() % 7) - 3 
                              , (rand() % 9) - 4
                              , -(rand() % 5)
                              , 0x58,0x70,0x00,8,4,((bullet_t*)bullet)->p_origin));
}

void cluster_bomb_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                            , void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if( ( b->timeout == 0 )
    ||(((player_t*)p1)->worms_status & STATUS_SHOT)
    ||(((player_t*)p2)->worms_status & STATUS_SHOT)){
    /* BOOM !*/
    int i;
    SDL_Surface* ground = (SDL_Surface*) userdata;  
    colx = (colx < 0)? 0 : colx ;
    coly = (coly < 0)? 0 : coly ;
    colx = (colx >= ground->w) ? ground->w-1 : colx ;
    coly = (coly >= ground->h) ? ground->h-1 : coly ;
    drow_circle(ground,colx,coly,b->range+b->damages/15,transparent_r_value,transparent_g_value,transparent_b_value);
    b->obj.remove_flag = 1;
    bullet_explode( ((player_t*)p1)->dynamic_list_link,colx,coly
                  , ((player_t*)p1)->r,b->explo_size);

    if(((player_t*)p1)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);

    if(((player_t*)p2)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata); 

    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT;

    for(i=0;i<8;i++)
      weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
               , init_with_skin(WEAPON_CLUSTER_PIECE
                       , colx +(rand() % 9) - 4
                       , coly +(rand() % 5) - 2
                       , (rand() % 9) - 4
                       , -(rand() % 4)
                       , ((player_t*)p1)->r->cluster,b->p_origin)); 
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(9*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (9*b->obj.acc_x/10);
    b->obj.acc_y = -(2*b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * NUKE PIECE * * * * * * * * * * * * * * * * * */
bullet_t* create_nuke_piece(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->nuke_piece,p));
}

void nuke_piece_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                              , void* p1,void* p2, void* userdata){
 bullet_t * b = (bullet_t *) bullet;
  if( b->timeout == 0 ){
    int i;
    bullet_on_collision_cb(bullet, lastx, lasty, colx, coly,p1, p2,  userdata);
    b->obj.remove_flag = 1;
    for(i=0;i<4;i++)
      weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
         , init_explos_pieces( colx + (rand() % 7) - 3
                              , coly + (rand() % 7) - 3 
                              , (rand() % 9) - 4
                              , (rand() % 9) - 4
                              , 0x88,0x7c,0x00,rand()%5,4,((bullet_t*)bullet)->p_origin));  
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(9*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (9*b->obj.acc_x/10);
    b->obj.acc_y = -(2*b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}

/* * * * * * * * * * * * * * * * MINI NUKE * * * * * * * * * * * * * * * * * */
void nuke_piece_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  skin_offset.x = 0; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;

  object_add_to_list( ((player_t*)b->p_origin)->dynamic_list_link
                  , create_nuke_smoke( b->obj.pos_x - (b->obj.acc_x/2)
                                , b->obj.pos_y - (b->obj.acc_y/2) 
                                , ((player_t*)b->p_origin)->r));
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);

} 

/* * * * * * * * * * * * * * * * MINI NUKE * * * * * * * * * * * * * * * * * */
bullet_t* create_mini_nuke(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->mini_nuke,p));
}

void mini_nuke_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                              , void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if( b->timeout == 0 ){
    int i;
    bullet_on_collision_cb(bullet, lastx, lasty, colx, coly,p1, p2,userdata);
    b->obj.remove_flag = 1;
    for(i=0;i<25;i++){
      bullet_t * piece =  weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                               , init_with_skin( WEAPON_NUKE_PIECE
                                               , colx + (rand() % 13) - 6
                                               , coly - 2 - (rand() % 9)
                                               , (rand() % 9) - 4
                                               , -(rand() % 5)
                                               , ((player_t*)p1)->r->nuke_piece
                                               , b->p_origin));
      piece->timeout += (rand()%21) - 10;
    }
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(9*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (9*b->obj.acc_x/10);
    b->obj.acc_y = -(2*b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty;
  }
}

void mini_nuke_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  skin_offset.x = 0;
  skin_offset.y = 0;
  skin_offset.w = o->skin->w; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * BIG NUKE * * * * * * * * * * * * * * * * * * */
bullet_t* create_big_nuke(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                 , init_with_skin( w_id
                                 , p->reticle_x
                                 , p->reticle_y
                                 , acc_x * std_weap_info[w_id].velocity
                                 , acc_y * std_weap_info[w_id].velocity
                                 , p->r->big_nuke,p));
}

void big_nuke_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                              , void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if( b->timeout == 0 ){
    int i;
    bullet_on_collision_cb(bullet, lastx, lasty, colx, coly,p1, p2,userdata);
    b->obj.remove_flag = 1;
    for(i=0;i<80;i++){
     bullet_t * piece = weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                               , init_with_skin( WEAPON_NUKE_PIECE
                                               , colx + (rand() % 17) - 8
                                               , coly - 2 - (rand() % 9)
                                               , (rand() % 17) - 8
                                               , -(rand() % 8)
                                               , ((player_t*)p1)->r->nuke_piece
                                               , b->p_origin));
        piece->timeout += (rand()%21) - 10;
    }
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(9*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (9*b->obj.acc_x/10);
    b->obj.acc_y = -(2*b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  } 
}

void big_nuke_blit_cb(camera_t* c,void* bullet,void* userdata){
  mini_nuke_blit_cb(c,bullet,userdata);
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * VERMI NUKE * * * * * * * * * * * * * * * * * */
void vermi_nuke_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                              , void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if( b->timeout == 0 ){
    int i;                         
    bullet_on_collision_cb(bullet, lastx, lasty, colx, coly,p1, p2,userdata);
    b->obj.remove_flag = 1;                        
    for(i=0;i<30;i++){
      bullet_t * piece =  weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                               , init_bullet( WEAPON_VERMIFUGE
                                            , colx + (rand() % 17) - 8
                                            , coly - 2 - (rand() % 9)
                                            , (rand() % 17) - 8
                                            , -(rand() % 8)
                                            , 0x21 - (rand() % 41)
											, 0xFF - (rand() % 41)
                                            , 0x21 - (rand() % 41) 
											,b->p_origin));
      piece->timeout += (rand()%21) - 10; 
    }
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(9*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (9*b->obj.acc_x/10);
    b->obj.acc_y = -(2*b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * * GAUSS * * * * * * * * * * * * * * * * * * */
bullet_t* create_gauss(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->gauss,p));
}


void gauss_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  skin_offset.x = 0;
  skin_offset.y = 0;
  skin_offset.w = o->skin->w; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
}

void gauss_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                            , void* p1,void* p2, void* userdata){
  int i,j;
  bullet_t * b = (bullet_t *) bullet;
  for(i=0;i<4;i++){
    if(i<3){
      bullet_on_collision_cb(bullet, 0, 0, colx+(i*b->obj.acc_x/3), coly+(i*b->obj.acc_y/3),p1, p2,  userdata);
    }else{
      drow_circle((SDL_Surface*)userdata, colx+(i*b->obj.acc_x/3)
                                        , coly+(i*b->obj.acc_y/3)
                                        , b->range+3+b->damages/15
                                        ,transparent_r_value,transparent_g_value,transparent_b_value);
      bullet_explode( ((player_t*)p1)->dynamic_list_link
                    ,colx+(i*b->obj.acc_x/3)
                    ,coly+(i*b->obj.acc_y/3)
                    , ((player_t*)p1)->r,b->explo_size+3);

    }
    for(j=0;j<4;j++){ 
      weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
           , init_explos_pieces(  colx+(i*b->obj.acc_x/3) + (rand() % 7) - 3
                                , coly+(i*b->obj.acc_y/3) + (rand() % 7) - 3 
                                , (rand() % 3) - 1
                                , (rand() % 3) - 1
                                , 0xd5,0xd5,0xd5,3,4,b->p_origin)); 
    }
  }
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * PLAYERS HOUSES * * * * * * * * * * * * * * * * */
void house_p1_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                           , void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  b->obj.acc_x = 0;
  b->obj.acc_y = 0;
  b->obj.pos_x=lastx;
  b->obj.pos_y=lasty;
  if((((player_t*)p1)->worms_status & STATUS_SHOT )
     &&(((player_t*)p1)->worms_status & STATUS_HAVE_FLAG)){
      ((player_t*)p1)->worms_status &= ~STATUS_HAVE_FLAG;
      game_score[PLAYER_1].nb_flags++;
      set_player_flag( ((player_t*)p1)->bullet_list_link,((player_t*)p1)->r,PLAYER_1, (SDL_Surface*)userdata);
  }
  ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
  ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
}

void house_p2_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                           , void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  b->obj.acc_x = 0;
  b->obj.acc_y = 0;
  b->obj.pos_x=lastx;
  b->obj.pos_y=lasty;
  if((((player_t*)p2)->worms_status & STATUS_SHOT )
     &&(((player_t*)p2)->worms_status & STATUS_HAVE_FLAG)){
      ((player_t*)p2)->worms_status &= ~STATUS_HAVE_FLAG;
      game_score[PLAYER_2].nb_flags++;
      set_player_flag( ((player_t*)p2)->bullet_list_link,((player_t*)p2)->r,PLAYER_2, (SDL_Surface*)userdata);
  }
  ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
  ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
}

void house_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  skin_offset.x = 0;
  skin_offset.y = 0;
  skin_offset.w = o->skin->w; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h  + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * PLAYERS FLAGS * * * * * * * * * * * * * * * * */
void flag_p1_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                           , void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  b->obj.acc_x = 0;
  b->obj.acc_y = 0;
  b->obj.pos_x=lastx;
  b->obj.pos_y=lasty;
  if((((player_t*)p1)->worms_status & STATUS_SHOT )
     && !(((player_t*)p2)->worms_status & STATUS_HAVE_FLAG)){
    ((player_t*)p1)->worms_status |= STATUS_HAVE_FLAG;
    b->obj.remove_flag = 1;
  }
  ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
  ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
}

void flag_p2_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                           , void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  b->obj.acc_x = 0;
  b->obj.acc_y = 0;
  b->obj.pos_x=lastx;
  b->obj.pos_y=lasty;
  if((((player_t*)p2)->worms_status & STATUS_SHOT )
     && !(((player_t*)p1)->worms_status & STATUS_HAVE_FLAG)){
    ((player_t*)p2)->worms_status |= STATUS_HAVE_FLAG;
    b->obj.remove_flag = 1;
  }
  ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
  ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
}

void flag_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  skin_offset.x = 0;
  skin_offset.y = 0;
  skin_offset.w = o->skin->w; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  DEBUG_ELEMENT_POS
} 
/* * * * * * * * * * * * * * * * * ZIMM * * * * * * * * * * * * * * * * * * * */
void zimm_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  skin_offset.x = 0;
  skin_offset.y = 0;
  skin_offset.w = o->skin->w; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
} 


bullet_t* create_zimm(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                     ,init_with_skin(w_id
                         , p->reticle_x
                         , p->reticle_y
                         , acc_x * std_weap_info[w_id].velocity
                         , acc_y * std_weap_info[w_id].velocity
                         , p->r->zimm,p));
}

void zimm_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  ASSERT(bullet)
  bullet_t * b = (bullet_t *) bullet;

  if( ( b->timeout == 0 )
    ||(((player_t*)p1)->worms_status & STATUS_SHOT)
    ||(((player_t*)p2)->worms_status & STATUS_SHOT)){

    b->obj.remove_flag = 1;
    object_add_to_list( ((player_t*)p1)->dynamic_list_link
                      , create_flash(colx,coly,((player_t*)p1)->r));

     if(((player_t*)p1)->worms_status & STATUS_SHOT)
       player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);

     if(((player_t*)p2)->worms_status & STATUS_SHOT)
       player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);

     ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
     ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -b->obj.acc_x;
    else 
      b->obj.acc_x = b->obj.acc_x;
    b->obj.acc_y = -b->obj.acc_y + (rand()%21/10 - 1) ;
    b->obj.acc_x += (rand()%21/10 - 1) ;
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * HOLY_GRENADE * * * * * * * * * * * * * * * * * */
bullet_t* create_holy_grenade(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->holy_grenade,p));
}

void holy_grenade_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  skin_offset.x = ( o->skin->w / DEFAULT_HOLY_GRENADE_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_HOLY_GRENADE_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  if(o->acc_y || o->acc_x )
     o->animation_pos = (o->animation_pos+1)% DEFAULT_HOLY_GRENADE_ANIM_STEPS;
  
}

void holy_grenade_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                            , void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if( b->timeout == 0 ){
    /* BOOM !*/
    int i;
    SDL_Surface* ground = (SDL_Surface*) userdata; 
    colx = (colx < 0)? 0 : colx ;
    coly = (coly < 0)? 0 : coly ;
    colx = (colx >= ground->w) ? ground->w-1 : colx ;
    coly = (coly >= ground->h) ? ground->h-1 : coly ;
    drow_circle(ground,colx,coly,b->range+b->damages,transparent_r_value,transparent_g_value,transparent_b_value);
    b->obj.remove_flag = 1;
    bullet_explode( ((player_t*)p1)->dynamic_list_link,colx,coly
                  , ((player_t*)p1)->r,b->explo_size);

    object_add_to_list( ((player_t*)p1)->dynamic_list_link
                    , create_explosion(colx,coly-5,((player_t*)p1)->r,b->explo_size));
    object_add_to_list( ((player_t*)p1)->dynamic_list_link
                    , create_explosion(colx-4,coly+4,((player_t*)p1)->r,b->explo_size));
    object_add_to_list( ((player_t*)p1)->dynamic_list_link
                    , create_explosion(colx+4,coly+4,((player_t*)p1)->r,b->explo_size));
    
    if(((player_t*)p1)->worms_status & STATUS_SHOT)                 
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);
                        
    if(((player_t*)p2)->worms_status & STATUS_SHOT)                 
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata); 
                  
    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT;         
    for(i=0;i<20;i++){        
      weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
		    , init_explos_pieces( colx + (rand() % 11) - 5
                            , coly + (rand() % 11) - 5 
                            , (rand() % 21) - 10
                            , (rand() % 21) - 10
                            , 0xd5,0xd5,0xd5,0,5,b->p_origin));
      weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
		    , init_explos_pieces( colx + (rand() % 11) - 5
                            , coly-5 + (rand() % 11) - 5 
                            , (rand() % 21) - 10
                            , (rand() % 21) - 10
                            , 0xd5,0xd5,0xd5,0,5,b->p_origin));
      weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
		    , init_explos_pieces( colx-4 + (rand() % 11) - 5
                            , coly+4 + (rand() % 11) - 5 
                            , (rand() % 21) - 10
                            , (rand() % 21) - 10
                            , 0xd5,0xd5,0xd5,0,5,b->p_origin));
      weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
		    , init_explos_pieces( colx-4 + (rand() % 11) - 5
                            , coly+4 + (rand() % 11) - 5 
                            , (rand() % 21) - 10
                            , (rand() % 21) - 10
                            , 0xd5,0xd5,0xd5,0,5,b->p_origin));
    }
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(9*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (9*b->obj.acc_x/10);
    b->obj.acc_y = -(4*b->obj.acc_y/5);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }                         
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * GRASS HOPPER GRENADE * * * * * * * * * * * * * * * */
void gh_grenade_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                            , void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if( b->timeout == 0 ){
    /* BOOM !*/        
    int i;
    SDL_Surface* ground = (SDL_Surface*) userdata;  
    colx = (colx < 0)? 0 : colx ;
    coly = (coly < 0)? 0 : coly ;
    colx = (colx >= ground->w) ? ground->w-1 : colx ;  
    coly = (coly >= ground->h) ? ground->h-1 : coly ;                              
    b->obj.remove_flag = 1;
    bullet_explode( ((player_t*)p1)->dynamic_list_link,colx,coly
                  , ((player_t*)p1)->r,b->explo_size);
                    
    if(((player_t*)p1)->worms_status & STATUS_SHOT)                 
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);
                        
    if(((player_t*)p2)->worms_status & STATUS_SHOT)                 
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata); 
                  
    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
    
      for(i=0;i<3;i++)
        weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                     ,init_with_skin(WEAPON_GRASSHAPPER
                         , colx + rand() %15 - 7
                         , coly + rand() %15 - 7
                         , rand() %5 - 2
                         , rand() %5 - 2
                         , ((player_t*)p1)->r->grass_hopper,b->p_origin));
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(9*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (9*b->obj.acc_x/10);
    b->obj.acc_y = -(2*b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }                         
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * PLASMA CAMMON * * * * * * * * * * * * * * * * * */

bullet_t* create_plasma_cannon(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                   ,init_with_skin(w_id
                       , p->reticle_x
                       , p->reticle_y
                       , acc_x * std_weap_info[w_id].velocity
                       , acc_y * std_weap_info[w_id].velocity
                       , p->r->zimm,p));
}

void plasma_cannon_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                              , void* p1,void* p2, void* userdata){
  int i;                         
  bullet_t * b = (bullet_t *) bullet;   
  bullet_on_collision_cb(bullet, lastx, lasty, colx, coly,p1, p2,  userdata);                                
  for(i=0;i<30;i++){
    bullet_t * flam = weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                  ,init_with_skin( WEAPON_BLUE_FIRE
                         , colx + rand() %17 - 8
                         , coly + rand() %17 - 8
                         , rand() %15 - 7
                         , rand() %10 - 7
                         , ((player_t*)p1)->r->blue_flamer,b->p_origin));
    flam->timeout += rand()%21-10;
  }                             
}

void plasma_cannon_blit_cb(camera_t* c,void* bullet,void* userdata){
    SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);                              
  skin_offset.x = 0; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  object_add_to_list( ((player_t*)b->p_origin)->dynamic_list_link
                    , create_flash( b->obj.pos_x - (b->obj.acc_x/2)
                                  , b->obj.pos_y - (b->obj.acc_y/2) 
                                  , ((player_t*)b->p_origin)->r));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * AN-M14 * * * * * * * * * * * * * * * * * * * */
bullet_t* create_red_grenade(weapon_id w_id,player_t *p,int acc_x,int acc_y){
   return weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->red_grenade,p)); 
}

void an_m14_grenade_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                            , void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if( b->timeout == 0 ){
    /* BOOM !*/
    int i;
    SDL_Surface* ground = (SDL_Surface*) userdata;
    colx = (colx < 0)? 0 : colx ;
    coly = (coly < 0)? 0 : coly ;
    colx = (colx >= ground->w) ? ground->w-1 : colx ;
    coly = (coly >= ground->h) ? ground->h-1 : coly ;
    b->obj.remove_flag = 1;
    object_add_to_list( ((player_t*)p1)->dynamic_list_link
                    , create_explosion(colx,coly,((player_t*)p1)->r,b->explo_size));
    drow_circle(ground,colx,coly,b->range+b->damages/10,transparent_r_value,transparent_g_value,transparent_b_value);

    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT;

      for(i=0;i<200;i++){
        bullet_t * flam = weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                  ,init_with_skin( WEAPON_FIRE
                         , colx + rand() %17 - 8
                         , coly + rand() %17 - 8
                         , rand() %15 - 7
                         , rand() %10 - 7
                         , ((player_t*)p1)->r->flamer,b->p_origin));
        flam->timeout += rand()%21-10;
      }

  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(9*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (9*b->obj.acc_x/10);
    b->obj.acc_y = -(2*b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}

/* * * * * * * * * * * * * * * * * GAZ Grenade * * * * * * * * * * * * * * * * */
bullet_t* create_viral_case(weapon_id w_id,player_t *p,int acc_x,int acc_y){
   return weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->bonus,p)); 
}

void viral_case_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                           , void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if( b->timeout == 0 ){
    /* BOOM !*/                                    
    b->obj.remove_flag = 1;     
    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT; 
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(9*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (9*b->obj.acc_x/10);
    b->obj.acc_y = -(2*b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }  
}

void viral_case_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);                                
  skin_offset.x = 0; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
}

void viral_case_update(void* bullet,void *p1,void *p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if(b->timeout < 25*8){
      weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                ,init_bullet( WEAPON_SPORE_PARTICULE
                       , b->obj.pos_x + rand() %7 - 3
                       , b->obj.pos_y - rand() %5
                       , 0
                       , 0
                       , 0x1D,0x85,0x57,b->p_origin));
}else{
    if(b->timeout == 25*8)
      sengine_play(WIIERO_SOUND_BEE,SENGINE_PLAY_ACTION);
  }
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * GAZ Grenade * * * * * * * * * * * * * * * * */
bullet_t* create_white_grenade(weapon_id w_id,player_t *p,int acc_x,int acc_y){
   return weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->gaz_grenade,p)); 
}
void gaz_grenade_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                           , void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if( b->timeout == 0 ){
    /* BOOM !*/
    b->obj.remove_flag = 1;     
    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT; 
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(9*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (9*b->obj.acc_x/10);
    b->obj.acc_y = -(2*b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}

void gaz_grenade_update(void* bullet,void *p1,void *p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if(b->timeout < 25*8){
    int i=0;
    for(i=0;i<2;i++)
      weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                ,init_with_skin( WEAPON_GAZ_PARTICLE
                       , b->obj.pos_x + rand() %3 - 1
                       , b->obj.pos_y + rand() %3 - 1
                       , 0
                       , 0
                       , ((player_t*)p1)->r->smoke,b->p_origin));
  }else{
    if(b->timeout == 25*8)
      sengine_play(WIIERO_SOUND_GAZ,SENGINE_PLAY_ACTION);
  }
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * * SPIKE * * * * * * * * * * * * * * * * * * * */
void spike_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);  
  skin_offset.x = ( o->skin->w / DEFAULT_SPIKEBALL_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_SPIKEBALL_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  if(o->acc_y || o->acc_x )
    o->animation_pos = (o->animation_pos+1)% DEFAULT_SPIKEBALL_ANIM_STEPS;
}

bullet_t* create_spike(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  int i;
  bullet_t* last;
  for(i=0;i<10;i++)
    last = weapon_add_bullet_to_list( p->bullet_list_link
                     ,init_with_skin(w_id
                         , p->reticle_x+ ((rand()%7) - 3)
                         , p->reticle_y+ ((rand()%7) - 3)
                         , acc_x * std_weap_info[w_id].velocity + ((rand()%3) - 1)
                         , acc_y * std_weap_info[w_id].velocity + ((rand()%3) - 1)
                         , p->r->spikeball,p));
  return last;
}

void spike_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if( (((player_t*)p1)->worms_status & STATUS_SHOT)
    ||(((player_t*)p2)->worms_status & STATUS_SHOT)){
    /* SPLASH :-) !*/                         
    b->obj.remove_flag = 1;
                    
    if(((player_t*)p1)->worms_status & STATUS_SHOT)                 
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);
                        
    if(((player_t*)p2)->worms_status & STATUS_SHOT)                 
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata); 
                  
    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT;         
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(4*b->obj.acc_x/5);
    else 
      b->obj.acc_x = (4*b->obj.acc_x/5);
    b->obj.acc_y = -(2*b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * CHIQUITA * * * * * * * * * * * * * * * * * * */
bullet_t* create_chiquita(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->chiquita,p));
}

void chiquita_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  skin_offset.x = ( o->skin->w / DEFAULT_CHIQUITA_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_CHIQUITA_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  if(  (o->acc_y || o->acc_x ) 
     &&((fast_abs(o->acc_y) != 1) && (fast_abs(o->acc_x)  != 1)))
     o->animation_pos = (o->animation_pos+1)% DEFAULT_CHIQUITA_ANIM_STEPS;  
}

void chiquita_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if( b->timeout == 0 ){
    /* BOOM !*/
    int i;
    SDL_Surface* ground = (SDL_Surface*) userdata;  
    colx = (colx < 0)? 0 : colx ;
    coly = (coly < 0)? 0 : coly ;
    colx = (colx >= ground->w) ? ground->w-1 : colx ;
    coly = (coly >= ground->h) ? ground->h-1 : coly ;
    b->obj.remove_flag = 1;
    bullet_explode( ((player_t*)p1)->dynamic_list_link,colx,coly
                  , ((player_t*)p1)->r,b->explo_size);

    if(((player_t*)p1)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);

    if(((player_t*)p2)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata); 

    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT;

      for(i=0;i<20;i++)
        weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                     ,init_with_skin(WEAPON_CHIQUITA_PIECE
                         ,colx + (rand() % 21) - 10
                         , coly + (rand() % 21) - 10
                         , (rand() % 21) - 10
                         , (rand() % 21) - 15
                         , ((player_t*)p1)->r->chiquita_piece,b->p_origin));
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(9*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (9*b->obj.acc_x/10);
    b->obj.acc_y = -(2*b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}

void chiquita_piece_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  skin_offset.x = ( o->skin->w / DEFAULT_CHIQUITA_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_CHIQUITA_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  if(o->acc_y || o->acc_x ) 
     o->animation_pos = (o->animation_pos+1)% DEFAULT_CHIQUITA_ANIM_STEPS;  

}

void chiquita_piece_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  int i;
  bullet_on_collision_cb(bullet, lastx, lasty, colx, coly,p1, p2,  userdata);

  for(i=0;i<3;i++){
    weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
        , init_pieces( colx + (rand() % 7) - 3, coly + (rand() % 7) - 3 
                            , (rand() % 9) - 4, -(rand() % 5)
                            , 0xc8,0xc0,0x30,0,4,((bullet_t*)bullet)->p_origin));
    weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
        , init_pieces( colx + (rand() % 7) - 3, coly + (rand() % 7) - 3 
                            , (rand() % 9) - 4, -(rand() % 5)
                            , 0xa0,0x98,0x28,0,4,((bullet_t*)bullet)->p_origin));
  }
}


void sticky_chiquita_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if( b->timeout == 0 ){
    /* BOOM !*/ 
    int i;
    SDL_Surface* ground = (SDL_Surface*) userdata;
    colx = (colx < 0)? 0 : colx ;
    coly = (coly < 0)? 0 : coly ;
    colx = (colx >= ground->w) ? ground->w-1 : colx ;
    coly = (coly >= ground->h) ? ground->h-1 : coly ;
    b->obj.remove_flag = 1;
    bullet_explode( ((player_t*)p1)->dynamic_list_link,colx,coly
                  , ((player_t*)p1)->r,b->explo_size);

    if(((player_t*)p1)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);

    if(((player_t*)p2)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata); 

    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT;

      for(i=0;i<15;i++)
        weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                     ,init_with_skin(WEAPON_CHIQUITA_PIECE
                         ,colx + (rand() % 11) - 5
                         , coly + (rand() % 11) - 5 
                         , (rand() % 21) - 10
                         , (rand() % 21) - 15
                         , ((player_t*)p1)->r->chiquita_piece,b->p_origin));
  }else{
    b->obj.acc_x = (b->obj.acc_x>0)?1:-1;
    b->obj.acc_y = (b->obj.acc_y>0)?1:-1;
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
    b->obj.gravit = 0;
  }
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */






/* * * * * * * * * * * * * * * * * RB Rampage * * * * * * * * * * * * * * * * * * * */

void rb_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if( (((player_t*)p1)->worms_status & STATUS_SHOT)
    ||(((player_t*)p2)->worms_status & STATUS_SHOT)){
    /* SPLASH :-) !*/
    b->obj.remove_flag = 1;
    bullet_explode( ((player_t*)p1)->dynamic_list_link,colx,coly
                  , ((player_t*)p1)->r,b->explo_size); 
    if(((player_t*)p1)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);

    if(((player_t*)p2)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata); 

    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(9*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (9*b->obj.acc_x/10);
    b->obj.acc_y = -(9*b->obj.acc_y/10);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */



/* * * * * * * * * * * * * * * * * HELLRAIDER * * * * * * * * * * * * * * * * * * * */
void hellraider_blit_cb(camera_t* c,void* bullet,void* userdata){
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  object_add_to_list( ((player_t*)b->p_origin)->dynamic_list_link
                    , create_explosion( b->obj.pos_x - (b->obj.acc_x/2)
                                  , b->obj.pos_y - (b->obj.acc_y/2) 
                                  , ((player_t*)b->p_origin)->r,10));

}

bullet_t* create_hellraider(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->gauss,p));
}

void hellraider_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  ASSERT(bullet)
  ASSERT(userdata)
  ASSERT(p1)
  ASSERT(p2)
  SDL_Surface* ground = (SDL_Surface*) userdata;
  bullet_t* b = (bullet_t*)bullet;
  colx = (colx < 0)? 0 : colx ;
  coly = (coly < 0)? 0 : coly ;
  colx = (colx >= ground->w) ? ground->w-1 : colx ;
  coly = (coly >= ground->h) ? ground->h-1 : coly ;
  drow_circle(ground,colx,coly,b->range+b->damages/15,transparent_r_value,transparent_g_value,transparent_b_value);

   if(((player_t*)p1)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);

   if(((player_t*)p2)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata); 

  ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
  ((player_t*)p2)->worms_status &= ~STATUS_SHOT;

  if(( b->timeout % 10 == 0) && (b->obj.acc_y>0))
    b->obj.acc_y = -(b->obj.acc_y)-1;

  if( b->timeout == 0 ){
    /* BOOM !*/
    int i;
    b->obj.remove_flag = 1;
    drow_circle(ground,colx,coly,b->damages,transparent_r_value,transparent_g_value,transparent_b_value); 
    bullet_explode( ((player_t*)p1)->dynamic_list_link,colx,coly
                  , ((player_t*)p1)->r,b->explo_size);

    for(i=0;i<8;i++)
      weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                   ,init_with_skin(WEAPON_CANNON
                       ,colx + (rand() % 11) - 5
                       , coly + (rand() % 11) - 5 
                       , (rand() % 11) - 5
                       , (rand() % 11) - 5
                       , ((player_t*)b->p_origin)->r->cannon,b->p_origin));
    for(i=0;i<20;i++)
      weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                                , init_explos_pieces( colx + (rand() % 11) - 5
                                                    , coly + (rand() % 11) - 5 
                                                    , (rand() % 21) - 10
                                                    , (rand() % 21) - 10
                                                    , 0xEC,0xD4,0x00,0,5,b->p_origin));
  }  
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * * NAPALM * * * * * * * * * * * * * * * * * * * */
/* napalm flame */
void nflame_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  ASSERT(bullet)
  ASSERT(userdata)
  ASSERT(p1)
  ASSERT(p2)
  SDL_Surface* ground = (SDL_Surface*) userdata;
  bullet_t* b = (bullet_t*)bullet;
  colx = (colx < 0)? 0 : colx ;
  coly = (coly < 0)? 0 : coly ;
  colx = (colx >= ground->w) ? ground->w-1 : colx ;
  coly = (coly >= ground->h) ? ground->h-1 : coly ;
  drow_circle(ground,colx,coly,b->range+b->damages/15,transparent_r_value,transparent_g_value,transparent_b_value);
   if(((player_t*)p1)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,0,0,userdata);

   if(((player_t*)p2)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,0,0,userdata); 

  ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
  ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
  b->obj.pos_x = colx;
  b->obj.pos_y = coly;
}


bullet_t* create_napalm(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->napalm,p));
}

void napalm_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata){
  ASSERT(bullet)
  ASSERT(userdata)
  ASSERT(p1)
  ASSERT(p2)
  SDL_Surface* ground = (SDL_Surface*) userdata;
  int i;
  bullet_t* b = (bullet_t*)bullet;
  colx = (colx < 0)? 0 : colx ;
  coly = (coly < 0)? 0 : coly ;
  colx = (colx >= ground->w) ? ground->w-1 : colx ;
  coly = (coly >= ground->h) ? ground->h-1 : coly ;
  for(i=0;i<10;i++)
    weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                 ,init_with_skin(WEAPON_NAPALM_FLAME
                     ,colx + (rand() % 11) - 5
                     , coly + (rand() % 11) - 5 
                     , (rand() % 5) - 2
                     , (rand() % 5) - 3
                     , ((player_t*)p1)->r->cannon,b->p_origin)); 
  b->obj.remove_flag = 1;
}

void napalm_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  skin_offset.x = ( o->skin->w / DEFAULT_NAPALM_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;
  skin_offset.w = o->skin->w / DEFAULT_NAPALM_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  if(o->acc_y || o->acc_x )
     o->animation_pos = (o->animation_pos+1)% DEFAULT_NAPALM_ANIM_STEPS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */



void  booby_trap_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                           , void* p1,void* p2, void* userdata){
  bullet_t *b = (bullet_t *) bullet;
  if((b->timeout == 0 )
     ||(((player_t*)p1)->worms_status & STATUS_SHOT)
     ||(((player_t*)p2)->worms_status & STATUS_SHOT)){
     bullet_on_collision_cb(bullet,lastx, lasty, colx, coly,p1,p2,userdata);
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(7*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (7*b->obj.acc_x/10);
    b->obj.acc_y = -(2*b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}

bullet_t* create_booby_trap(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  if(rand()%2==0){
  return weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( WEAPON_BOOBY_HEALTH
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->health,p));
  }else{
  bullet_t* b = (weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( WEAPON_BOOBY_BONUS
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->bonus,p)));
    b->damages = rand() % MAX_WEAPONS_AND_XTRA;
    return b;
  }
}  


/* * * * * * * * * * * * * * * * * GIFT BOX * * * * * * * * * * * * * * * * * */
bullet_t* create_bonus_gift(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  bullet_t* b = weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->bonus,p));
  b->damages = rand()%MAX_WEAPONS_AND_XTRA;
  return b;
}

void bonus_gift_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                              , void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if( b->timeout == 0 )
    b->obj.remove_flag = 1;

  if(((player_t*)p1)->worms_status & STATUS_SHOT){
     secure_free(((player_t*)p1)->weapon_slots[((player_t*)p1)->selected_weapon]);
     ((player_t*)p1)->weapon_slots[((player_t*)p1)->selected_weapon] = weapon_init(b->damages);
    b->obj.remove_flag = 1;
    sengine_play(WIIERO_SOUND_GIFT,SENGINE_PLAY_ACTION);
  }
  
  if(((player_t*)p2)->worms_status & STATUS_SHOT){
     secure_free(((player_t*)p2)->weapon_slots[((player_t*)p2)->selected_weapon]);
     ((player_t*)p2)->weapon_slots[((player_t*)p2)->selected_weapon] = weapon_init(b->damages);
    b->obj.remove_flag = 1;
    sengine_play(WIIERO_SOUND_GIFT,SENGINE_PLAY_ACTION);
  }

  ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
  ((player_t*)p2)->worms_status &= ~STATUS_SHOT;

  if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
    b->obj.acc_x = -(9*b->obj.acc_x/10);
  else 
    b->obj.acc_x = (9*b->obj.acc_x/10);
  b->obj.acc_y = -(2*b->obj.acc_y/3);
  b->obj.pos_x=lastx;
  b->obj.pos_y=lasty; 
}

void bonus_gift_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  skin_offset.x = 0;
  skin_offset.y = 0;
  skin_offset.w = o->skin->w;
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);

  if(weapon_round_option.show_bonus_name)
    font_print(c,(char*)wiiero_label[WIIERO_LANG_WEAPON_NAMES_FIRST_ID+b->damages],o->pos_x,o->pos_y-10,FONT_SMALL);  
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * HEALTH GIFT * * * * * * * * * * * * * * * * * * */
bullet_t* create_health_gift(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->health,p));
}

void health_gift_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if( b->timeout == 0 )
    b->obj.remove_flag = 1;

  if(((player_t*)p1)->worms_status & STATUS_SHOT){
    player_regen(((player_t*)p1),b->damages);
    b->obj.remove_flag = 1;
    sengine_play(WIIERO_SOUND_HEALTH,SENGINE_PLAY_ACTION);
  }

  if(((player_t*)p2)->worms_status & STATUS_SHOT){
    player_regen(((player_t*)p2),b->damages); 
    b->obj.remove_flag = 1;
    sengine_play(WIIERO_SOUND_HEALTH,SENGINE_PLAY_ACTION);
  }

  ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
  ((player_t*)p2)->worms_status &= ~STATUS_SHOT;

  if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
    b->obj.acc_x = -(9*b->obj.acc_x/10);
  else 
    b->obj.acc_x = (9*b->obj.acc_x/10);
  b->obj.acc_y = -(2*b->obj.acc_y/3);
  b->obj.pos_x=lastx;
  b->obj.pos_y=lasty;
}

void health_gift_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  skin_offset.x = 0;
  skin_offset.y = 0;
  skin_offset.w = o->skin->w; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);

}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * * DIRTBALL * * * * * * * * * * * * * * * * * */
bullet_t* create_dirtball(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                 , init_with_skin( w_id
                                 , (p->reticle_x + ((p->reticle_x - p->worms.pos_x)/2 ))
                                 , (p->reticle_y + ((p->reticle_y - p->worms.pos_y)/2 ))
                                 , acc_x * std_weap_info[w_id].velocity
                                 , acc_y * std_weap_info[w_id].velocity
                                 , p->r->dirtball,p));
}

void dirtball_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                              , void* p1,void* p2, void* userdata){

  bullet_t * b = (bullet_t *) bullet;
  if( b->timeout == 0 ){
    /* Blop !*/
    SDL_Surface* ground = (SDL_Surface*) userdata;  
    colx = (colx < 0)? 0 : colx ;
    coly = (coly < 0)? 0 : coly ;
    colx = (colx >= ground->w) ? ground->w-1 : colx ;
    coly = (coly >= ground->h) ? ground->h-1 : coly ;
    b->obj.remove_flag = 1;
    drow_colored_circle(ground,colx,coly,15);

    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(9*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (9*b->obj.acc_x/10);
    b->obj.acc_y = -(2*b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}
void dirtball_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  skin_offset.x = 0;
  skin_offset.y = 0;
  skin_offset.w = o->skin->w; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * * KAMIKASE * * * * * * * * * * * * * * * * * */
bullet_t* create_kamikase(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  ASSERT(p);
  p->worms_health = 0;
  return weapon_add_bullet_to_list( p->bullet_list_link
                           , init_bullet( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , 0
                                   , 0
                                   , 0x7d,0x42,0x06,p));
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * * TRAP * * * * * * * * * * * * * * * * * * * */
void trap_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  if(( b->timeout == 0 )
    ||((((player_t*)p1)->worms_status & STATUS_SHOT)
      ||(((player_t*)p2)->worms_status & STATUS_SHOT))){
    /* Blop !*/
    SDL_Surface* ground = (SDL_Surface*) userdata;
    colx = (colx < 0)? 0 : colx ;
    coly = (coly < 0)? 0 : coly ;
    colx = (colx >= ground->w) ? ground->w-1 : colx ;
    coly = (coly >= ground->h) ? ground->h-1 : coly ;
    drow_colored_circle(ground,colx,coly,b->range+5);
    if(rand()%10==0)
    weapon_add_bullet_to_list( ((player_t*)b->p_origin)->bullet_list_link
                   , init_with_skin( WEAPON_GRENADE
                                   , b->obj.pos_x
                                   , b->obj.pos_y
                                   , 0
                                   , 0
                                   , ((player_t*)b->p_origin)->r->grenade
                                   , ((player_t*)b->p_origin)));

    ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
    ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -(9*b->obj.acc_x/10);
    else 
      b->obj.acc_x = (9*b->obj.acc_x/10);
    b->obj.acc_y = -(2*b->obj.acc_y/3);
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * * BLASTER * * * * * * * * * * * * * * * * * * */
void blaster_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  skin_offset.x = 0;
  skin_offset.y = 0;
  skin_offset.w = o->skin->w;
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;

  object_add_to_list( ((player_t*)b->p_origin)->dynamic_list_link
                    , create_smoke( b->obj.pos_x - (b->obj.acc_x/2)
                                  , b->obj.pos_y - (b->obj.acc_y/2) 
                                  , ((player_t*)b->p_origin)->r));
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
}

bullet_t* create_blaster(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->gauss,p));
}

void blaster_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
   int i;
   bullet_on_collision_cb(bullet, lastx, lasty, colx, coly,p1, p2,  userdata);
   for(i=0;i<10;i++)
      weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
         , init_explos_pieces( colx + (rand() % 7) - 3
                              , coly + (rand() % 7) - 3 
                              , (rand() % 9) - 4
                              , (rand() % 9) - 4
                              , 0xD0,0x8C,0x00,0,2,((bullet_t*)bullet)->p_origin));
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * * LASER * * * * * * * * * * * * * * * * * * * */
void laser_blit_cb(camera_t* c,void* bullet,void* userdata){
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)

  bullet_t * b = (bullet_t *) bullet;
  player_t* p = ((player_t*)b->p_origin);
  obj_t* o = &(b->obj);  
  if( (p->weapon_slots[p->selected_weapon]->last_bullet != 0l)
    &&(bullet != p->weapon_slots[p->selected_weapon]->last_bullet))
  return;

  camera_drow_line(c
           , p->reticle_x-c->map_x
           , p->reticle_y-c->map_y
           , o->pos_x-c->map_x
           , o->pos_y-c->map_y
           , 0xff,0x00,0x00);

 p->weapon_slots[p->selected_weapon]->last_bullet = 0l;
}

bullet_t* create_laser(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
            , init_bullet( w_id
                         , p->reticle_x
                         , p->reticle_y
                         , acc_x * std_weap_info[w_id].velocity
                         , acc_y * std_weap_info[w_id].velocity
                         , 0xff,0x00,0x00,p));
}

void laser_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata){
  ASSERT(bullet)
  ASSERT(userdata)
  ASSERT(p1)
  ASSERT(p2)
  SDL_Surface* ground = (SDL_Surface*) userdata;
  bullet_t* b = (bullet_t*)bullet;
  player_t* p = ((player_t*)b->p_origin);
  colx = (colx < 0)? 0 : colx ;
  coly = (coly < 0)? 0 : coly ;
  colx = (colx >= ground->w) ? ground->w-1 : colx ;
  coly = (coly >= ground->h) ? ground->h-1 : coly ;
  drow_circle(ground,colx,coly,4,transparent_r_value,transparent_g_value,transparent_b_value);

  object_add_to_list( ((player_t*)p1)->dynamic_list_link
                    , create_explosion(colx,coly,((player_t*)p1)->r,b->explo_size));

   if(((player_t*)p1)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,0,0,userdata);

   if(((player_t*)p2)->worms_status & STATUS_SHOT)
      player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,0,0,userdata); 

  ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
  ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
  b->obj.pos_x=colx;
  b->obj.pos_y=coly;
  b->obj.remove_flag = 1;
  if(p->weapon_slots[p->selected_weapon]->last_bullet == 0l)
      p->weapon_slots[p->selected_weapon]->last_bullet = b;
}

void laser_update(void* bullet,void *p1,void *p2, void* userdata){
  bullet_t* b = (bullet_t*)bullet;
  player_t* p = ((player_t*)b->p_origin);
  p->weapon_slots[p->selected_weapon]->last_bullet = 0l;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * CRACKLER * * * * * * * * * * * * * * * * * * * */
bullet_t* create_crackler(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->crackler,p));
}
void crackler_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata){
  ASSERT(bullet)
  bullet_t * b = (bullet_t *) bullet;
  SDL_Surface* ground = (SDL_Surface*) userdata;
  if ( b->timeout == 0 ){
    b->obj.remove_flag = 1;
    drow_circle(ground,colx,coly,b->range+b->damages/15,transparent_r_value,transparent_g_value,transparent_b_value); 
    bullet_explode( ((player_t*)p1)->dynamic_list_link,colx,coly
                  , ((player_t*)p1)->r,b->explo_size);
  }
  if((((player_t*)p1)->worms_status & STATUS_SHOT)
    ||(((player_t*)p2)->worms_status & STATUS_SHOT)){
      if(((player_t*)p1)->worms_status & STATUS_SHOT)
        player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);
      if(((player_t*)p2)->worms_status & STATUS_SHOT)
        player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);
      ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
      ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
  }
  if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
    b->obj.acc_x = -b->obj.acc_x;
  else 
    b->obj.acc_x = b->obj.acc_x;
  b->obj.acc_y = -b->obj.acc_y;
  b->obj.pos_x=lastx;
  b->obj.pos_y=lasty;
}

void crackler_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  skin_offset.x = 0;
  skin_offset.y = 0;
  skin_offset.w = o->skin->w;
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
}

void crackler_update(void* bullet,void *p1,void *p2, void* userdata){
  ASSERT(bullet)
  bullet_t * b = (bullet_t *) bullet;
  static Uint8 pos=0; 
  Uint8 d_x=0;
  Uint8 d_y=0;
  /*                       (0,0)   (2,0)
   *  0 <= pos < 10  => 1      1   2
   * 10 <= pos < 20  => 2        c      
   * 20 <= pos < 30  => 3      3   4
   * 30 <= pos < 40  => 4  (0,2)   (2,2)
   */
  if((10 <= pos) && (pos < 20)){
    d_x=2;
  }else{
    if((20 <= pos) && (pos < 30)){
      d_y=2;
    }else{
      if((30 <= pos) && (pos < 40)){
        d_x=2;d_y=2;
      }
    }
  }

  pos = (pos + 2) % 40;
  weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                        , init_pieces( b->obj.pos_x + (d_x-1)*2
                                     , b->obj.pos_y + (d_y-1)*2
                                     , (d_x-1)*2 + (rand()%3-1) + b->obj.acc_x 
                                     , (d_y-1)*2 + (rand()%3-1) + b->obj.acc_y
                                     , 0xd5,0xd5,0xd5,0,2,b->p_origin));
  if(rand()%50 == 0)
     crackler_on_collision_cb(bullet,b->obj.pos_x,b->obj.pos_y,b->obj.pos_x,b->obj.pos_y,p1,p2,userdata);
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * LARPA * * * * * * * * * * * * * * * * * * * * */
bullet_t* create_larpa(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  bullet_t* bullet = weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id
                          , p->reticle_x
                          , p->reticle_y
                          , acc_x * std_weap_info[w_id].velocity
                          , acc_y * std_weap_info[w_id].velocity
                          , p->r->larpa,p));
  bullet->obj.angle = p->worms.angle;
  bullet->obj.side = p->worms.side;
  return bullet; 
}

void larpa_update(void* bullet,void *p1,void *p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  //if(rand()%2 == 0)
  weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                          , init_pieces( b->obj.pos_x + rand()%5-2
                                       , b->obj.pos_y + rand()%5-2
                                       , b->obj.acc_x /4
                                       , b->obj.acc_y /4
                                       , 0xd5,0xd5,0xd5,0,1,b->p_origin));
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * NUKE LARPA* * * * * * * * * * * * * * * * * * */
void nuke_larpa_update(void* bullet,void *p1,void *p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  //if(rand()%2 == 0)
    weapon_add_bullet_to_list( ((player_t*)p1)->bullet_list_link
                               , init_with_skin( WEAPON_NUKE_PIECE
                                               ,  b->obj.pos_x + rand()%5-2
                                               , b->obj.pos_y + rand()%5-2
                                               , b->obj.acc_x /4
                                               , b->obj.acc_y /4
                                               , ((player_t*)p1)->r->nuke_piece
                                               , b->p_origin)); 
}
/* * * * * * * * * * * * * * * CTRL NUKE LARPA* * * * * * * * * * * * * * * * */
void cnuke_larpa_update(void* bullet,void *p1,void *p2, void* userdata){
  if(rand()%2 == 0)
    nuke_larpa_update(bullet,p1,p2,userdata);
  missile_update (bullet,p1,p2,userdata);
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * BOUNCY LARPA* * * * * * * * * * * * * * * * * * */
bullet_t* create_bouncy_larpa(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id
                                   , p->reticle_x
                                   , p->reticle_y
                                   , acc_x * std_weap_info[w_id].velocity
                                   , acc_y * std_weap_info[w_id].velocity
                                   , p->r->bouncy_larpa,p));
}

void bouncy_larpa_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                             ,void* p1,void* p2, void* userdata){
  ASSERT(bullet)
  bullet_t * b = (bullet_t *) bullet;
  SDL_Surface* ground = (SDL_Surface*) userdata;
  if( ( b->timeout == 0 )
    ||(((player_t*)p1)->worms_status & STATUS_SHOT)
    ||(((player_t*)p2)->worms_status & STATUS_SHOT)){
    b->obj.remove_flag = 1;
    drow_circle(ground,colx,coly,b->range+b->damages/15,transparent_r_value,transparent_g_value,transparent_b_value); 
    bullet_explode( ((player_t*)p1)->dynamic_list_link,colx,coly
                  , ((player_t*)p1)->r,b->explo_size);

     if(((player_t*)p1)->worms_status & STATUS_SHOT)
       player_shot(((player_t*)p1),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);

     if(((player_t*)p2)->worms_status & STATUS_SHOT)
       player_shot(((player_t*)p2),((player_t*)b->p_origin)->id,b->damages,b->obj.acc_x/2,b->obj.acc_y/2,userdata);

     ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
     ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
  }else{
    if(( lastx == b->obj.pos_x) && (lasty == b->obj.pos_y) )
      b->obj.acc_x = -b->obj.acc_x;
    else 
      b->obj.acc_x = b->obj.acc_x;
    b->obj.acc_y = -b->obj.acc_y;
    b->obj.pos_x=lastx;
    b->obj.pos_y=lasty; 
  }
}

void bouncy_larpa_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  skin_offset.x = 0;
  skin_offset.y = 0;
  skin_offset.w = o->skin->w;
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * * MEDIKIT * * * * * * * * * * * * * * * * * * */
bullet_t* create_medikit(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  obj_t* o = 0l;
  bullet_t* b = weapon_add_bullet_to_list( p->bullet_list_link
                   , init_with_skin( w_id, 0, 0, 0, 0
                                   , p->r->teleport,p));
  player_regen(p,b->damages);
  o = &(b->obj); 
  o->pos_x = p->worms.pos_x;
  o->pos_y = p->worms.pos_y;
  return b;
}

void medikit_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  player_t*  p = (player_t*)(b->p_origin);
  obj_t* o = &(b->obj);
  skin_offset.x = ( o->skin->w / DEFAULT_TELEPORT_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;
  skin_offset.w = o->skin->w / DEFAULT_TELEPORT_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  o->pos_x = p->worms.pos_x;
  o->pos_y = p->worms.pos_y;
  /* compute object position into the camera */
  camera_offset.x = (p->worms).pos_x - c->map_x - skin_offset.w / 2 - 1;
  camera_offset.y = (p->worms).pos_y - c->map_y - skin_offset.h / 2 - 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  o->animation_pos++;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * * SHIELD * * * * * * * * * * * * * * * * * * */
bullet_t* create_shield(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  obj_t* o = 0l;
  bullet_t* b = weapon_add_bullet_to_list( p->bullet_list_link
                          , init_with_skin( w_id, 0 , 0, 0, 0
                                   , p->worms.skin,p));
  b->damages = p->worms_health;
  p->worms.skin = p->r->worms_iron;
  o = &(b->obj); 
  o->pos_x = p->worms.pos_x;
  o->pos_y = p->worms.pos_y;
  p->worms_status |= STATUS_PROTECTED;
  return b;
}

void shield_blit_cb(camera_t* c,void* bullet,void* userdata){
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  player_t*  p = (player_t*)(b->p_origin);
  o->pos_x = p->worms.pos_x;
  o->pos_y = p->worms.pos_y;

  if(((int)b->timeout - (b->damages-p->worms_health)) <=0)
    b->timeout = 0;
  else
    b->timeout -= (b->damages-p->worms_health);

  p->worms_health = b->damages;
  if(b->timeout<=1){
    p->worms.skin= o->skin;
    p->worms_status &= ~STATUS_PROTECTED;
  }
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * * NINJA * * * * * * * * * * * * * * * * * * * */
void missile_update(void* bullet,void *p1,void *p2, void* userdata){
  bullet_t * b = (bullet_t *) bullet;
  ASSERT(b)
  player_t* p = b->p_origin;  
  ASSERT(p)
        
        
  camera_focus_on(p->worms_camera,b->obj.pos_x,b->obj.pos_y,MAP_WIDTH,MAP_HEIGHT);
  
  /* nop if player dead ... */
  if(!(p->worms_status & STATUS_ALIVE)){ /* ... clean all flags */
    p->worms_status &= ~(STATUS_FREEZED|STATUS_UNFOCUSED|STATUS_RESET_FOCUS); 
    return;
  }
    
  /* Bullet control */     
  p->worms_status |= (STATUS_FREEZED | STATUS_UNFOCUSED);
    
  if( p->worms_action & (ACTION_LEFT | ACTION_RIGHT))
  {
    register int angle = ANGLE_PITCH*10;
    if( p->worms_action & ACTION_LEFT){
        if(b->obj.side == RIGHT_SIDE){
          if(b->obj.angle + angle > 90){
              b->obj.angle = 90 + b->obj.angle - (b->obj.angle + angle);
              b->obj.side = LEFT_SIDE;
          }else
            b->obj.angle += angle; 
        }else{
          if(b->obj.angle - angle < -90){            
              b->obj.angle = -90 - (90 + (b->obj.angle - angle));
              b->obj.side = RIGHT_SIDE;
          }else
            b->obj.angle -= angle;   
        }
    } 
    if( p->worms_action & ACTION_RIGHT){
        if(b->obj.side == LEFT_SIDE){
          if(b->obj.angle + angle > 90){
              b->obj.angle = 90 + b->obj.angle - (b->obj.angle + angle);
              b->obj.side = RIGHT_SIDE;
          }else
            b->obj.angle += angle; 
        }else{
          if(b->obj.angle - angle < -90){        
              b->obj.angle = -90 - (90 + (b->obj.angle - angle));
              b->obj.side = LEFT_SIDE;
          }else
            b->obj.angle -= angle;   
        } 
    }  
      
    /* Bullet acceleration recalc */
    b->obj.acc_y = - (RETICLE_SPACING * ftrigo_sin(b->obj.angle)) 
                     * std_weap_info[b->w_id].velocity;
    b->obj.acc_x = (((b->obj.side == LEFT_SIDE) 
                     ? - (RETICLE_SPACING * ftrigo_cos(b->obj.angle))
                     :   (RETICLE_SPACING * ftrigo_cos(b->obj.angle)))
                    * std_weap_info[b->w_id].velocity
                   );
  }
        
}

void missile_on_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                              , void* p1,void* p2, void* userdata){
  int i=0;
  player_t* p = (player_t*)(((bullet_t*)bullet)->p_origin);
  bullet_on_collision_cb(bullet, lastx, lasty, colx, coly,p1, p2,  userdata);
  for(i=0;i<4;i++)
   weapon_add_bullet_to_list(((player_t*)p1)->bullet_list_link
                            ,init_explos_pieces( colx + (rand() % 7) - 3
                                               , coly + (rand() % 7) - 3 
                                               , (rand() % 9) - 4
                                               , (rand() % 7) - 4
                                               , 0x84,0x84,0x84,5,4
                                               ,p));
  missile_update(bullet,p1,p2, userdata);
  p->worms_status &= ~STATUS_FREEZED;
  p->worms_status |= STATUS_RESET_FOCUS;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * * * * * * * * * * * * * * NINJA * * * * * * * * * * * * * * * * * * * */
bullet_t* create_ninja_hook(weapon_id w_id,player_t *p,int acc_x,int acc_y){
  return weapon_add_bullet_to_list( p->bullet_list_link
                                  ,init_with_skin(w_id
                                          , p->reticle_x
                                          , p->reticle_y
                                          , acc_x * std_weap_info[w_id].velocity 
                                          , acc_y * std_weap_info[w_id].velocity 
                                          , p->r->hook,p));
}

void ninja_hook_collision_cb( void* bullet, int lastx, int lasty, int colx, int coly
                                 ,void* p1,void* p2, void* userdata){
  /* Bloc */
  bullet_t * b = (bullet_t *) bullet;
  player_t* source = b->p_origin;
  player_t* dest = 0l;
  b->obj.pos_x = lastx;
  b->obj.pos_y = lasty;
  if(b->obj.acc_x != 0)
    b->obj.acc_x = (b->obj.acc_x<0) ? -1 : 1;
  if(b->obj.acc_y != 0)
    b->obj.acc_y = (b->obj.acc_y<0) ? -1 : 1;
  b->obj.gravit = 0;
/*
  ((player_t*)p1)->worms_status &= ~STATUS_TRICKED;
  ((player_t*)p2)->worms_status &= ~STATUS_TRICKED;*/
  if( p1 != source){
    if(((player_t*)p1)->worms_status & STATUS_SHOT){
      // player 1 is tricked
      dest = (player_t*)p1;
      dest->worms_status |= STATUS_TRICKED;
      b->obj.pos_x = dest->worms.pos_x;
      b->obj.pos_y = dest->worms.pos_y-2;
    }
  }else{
    if(((player_t*)p2)->worms_status & STATUS_SHOT){
      // player 2 is tricked
      dest = (player_t*)p2;
      dest->worms_status |= STATUS_TRICKED;
      b->obj.pos_x = dest->worms.pos_x;
      b->obj.pos_y = dest->worms.pos_y-2;
    }
  }
  ((player_t*)p1)->worms_status &= ~STATUS_SHOT;
  ((player_t*)p2)->worms_status &= ~STATUS_SHOT;
  if(dest){
    dest->worms.pos_x += (source->worms.pos_x - b->obj.pos_x ) / 20 ;
    dest->worms.pos_y += (source->worms.pos_y - b->obj.pos_y ) / 20 ;
  }
  source->worms.acc_x -= (source->worms.pos_x - b->obj.pos_x ) / source->worms_rope_len;
  source->worms.acc_y -= (source->worms.pos_y - b->obj.pos_y ) / source->worms_rope_len;
  ninja_hook_update_cb(bullet,p1,p2,0L);
}

void ninja_hook_blit_cb(camera_t* c,void* bullet,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(bullet)
  ASSERT(c)
  ASSERT(userdata)
  bullet_t * b = (bullet_t *) bullet;
  obj_t* o = &(b->obj);
  skin_offset.x = 0;
  skin_offset.y = 0;
  skin_offset.w = o->skin->w;
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  camera_drow_line(c
           , ((player_t*)(b->p_origin))->worms.pos_x-c->map_x
           , (((player_t*)(b->p_origin))->worms.pos_y-2)-c->map_y
           , o->pos_x-c->map_x
           , o->pos_y-c->map_y
           , 0xa8,0x4b,0x0d);
  
}

void ninja_hook_update_cb(void* bullet,void *p1,void *p2, void* userdata){
 /* Bloc */
  bullet_t * b = (bullet_t *) bullet;
  player_t* source = b->p_origin;
  player_t* dest = 0l;
  if( p1 != source){
    dest = (player_t*)p1;
  }else{
    dest = (player_t*)p2;
  }
  if( dest->worms_status & STATUS_TRICKED ){
    dest->worms_status |= STATUS_TRICKED;
    b->obj.pos_x = dest->worms.pos_x;
    b->obj.pos_y = dest->worms.pos_y-2;
  }
}

void ninja_hook_disconnect(void* bullet,void *p1,void *p2, void* userdata){
 /* Bloc */
  bullet_t * b = (bullet_t *) bullet;
  player_t* source = b->p_origin;
  player_t* dest = 0l;
  if( p1 != source){
    dest = (player_t*)p1;
  }else{
    dest = (player_t*)p2;
  }
  if( dest->worms_status & STATUS_TRICKED ){
    dest->worms_status &= ~STATUS_TRICKED;
  }
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
