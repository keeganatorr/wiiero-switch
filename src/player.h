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

#ifndef PLAYERS_H
#define PLAYERS_H

#include "common.h"
#include "camera.h"
#include "object.h"
#include "map.h"
#include "ressources.h"
#include "weapons.h"
#include <SDL/SDL.h>


#define CROP_SIZE 5

#define RETICLE_SPACING 15
#define ROPE_LEN_CHANGE_PITCH  3

#define DEFAULT_PLAYER_XACC       0
#define DEFAULT_PLAYER_YACC       0
#define DEFAULT_PLAYER_RESIST     5
#define DEFAULT_PLAYER_STATUS     0x00000000
#define DEFAULT_PLAYER_ACTIONS    0x0000
#define DEFAULT_PLAYER_ANIM_POS   0
#define DEFAULT_PLAYER_ANIM_SLEEP 3
#define DEFAULT_PLAYER_HEALTH     100
#define DEFAULT_PLAYER_ANGLE      0
#define DEFAULT_ROPE_LEN          30

#define WEAPON_POS_X 0
#define WEAPON_POS_Y 3

#define MIN_PLAYER_ANGLE -50
#define MAX_PLAYER_ANGLE  90


#define JUMP_POWER 7
#define PLAYER_STATS_UPDATE_IDLE 1000

typedef enum{
  PLAYER_1,
  PLAYER_2,
  NB_PLAYERS,
  GAME_DRAW = NB_PLAYERS
}player_id;


enum{
    ACTION_NONE     = 0x0000,
    ACTION_UP       = 0x0001,
    ACTION_DOWN     = 0x0002,
    ACTION_LEFT     = 0x0004,
    ACTION_RIGHT    = 0x0008,
    ACTION_JUMP     = 0x0010,
    ACTION_CHANGE   = 0x0020,
    ACTION_FIRE     = 0x0040,
    ACTION_HOOK     = 0x0080,
    ACTION_CROP     = 0x0100,
    ACTION_L_ACT    = 0x0200,
    ACTION_R_ACT    = 0x0400,
    ACTION_PAUSE    = 0x1000,
    ACTION_MENU     = 0x2000,
    ACTION_OK       = 0x4000,
    ACTION_CANCEL   = 0x8000,
};

enum{
    LOOKING_DOWN_2_4,
    LOOKING_DOWN_1_4,
    LOOKING_MIDDLE,
    LOOKING_UP_1_4, 
    LOOKING_UP_2_4,
    LOOKING_UP_3_4,
    LOOKING_UP_FULL,
    MAX_LOOKING_HEIGHTS
};

enum{
  STATUS_ALIVE          = 0x00000001,
  STATUS_TAGGED         = 0x00000002,
  STATUS_HAVE_FLAG      = 0x00000004,
  STATUS_FREEZED        = 0x00000008,
  STATUS_JUMPABLE       = 0x00000010,
  STATUS_EXPLODE        = 0x00000020,
  STATUS_FIREING        = 0x00000040,
  STATUS_SHOT           = 0x00000080,
  STATUS_STATS_UPDATE   = 0x00000100,
  STATUS_AIMING         = 0x00000200,
  STATUS_CROPING        = 0x00000400,
  STATUS_CHANGING_W     = 0x00000800,
  STATUS_TRICKED        = 0x00001000,
  STATUS_NINJA_ACTION   = 0x00002000,
  STATUS_SHOW_W         = 0x00004000,
  STATUS_SUICIDE        = 0x00008000,
  STATUS_RESETED        = 0x00010000,
  STATUS_PROTECTED      = 0x00020000,
  STATUS_UNFOCUSED      = 0x00040000,
  STATUS_RESET_FOCUS    = 0x00080000,
  STATUS_CLASSIC_ON     = 0x10000000,  
  STATUS_NUNCHUCK_ON    = 0x20000000,
  STATUS_GAMECUBE_ON    = 0x40000000,
};


typedef enum{
    GAME_DEATHMATCH_MODE,
    GAME_OF_TAG_MODE,
    GAME_CAPTURE_FLAG_MODE,
}game_mode_t;

typedef struct{
  player_id      id;
  Uint32         worms_status;
  Uint16         worms_action;
  Uint8          worms_health;
  Uint8          worms_killed;
  /*Uint8          worms_shockwave;*/
  Uint8          gunfire_anim_pos;
  int            last_stats_update;
  int            reticle_x;
  int            reticle_y;
  Uint8          reticle_pitch;
  Uint8          selected_weapon;
  obj_t          worms;
  weapon_t*      weapon_slots[WEAPON_SLOTS];
  weapon_t*      ninja_hook;
  bullet_list_t* bullet_list_link;
  obj_list_t*    dynamic_list_link;
  camera_t*      worms_camera;
  camera_t*      worms_stats_camera;
  ressources_t*  r;
  Uint16         worms_rope_len;  
}player_t;

typedef struct{
  Uint16 nb_lifes;
  Uint16 nb_frags;
  Uint16 nb_suicides;
  Uint16 nb_death;
  Uint16 nb_flags;
  Uint16 tag_time;
}player_score_t;



player_t* player_init( player_id id , camera_t* c,camera_t* sc,ressources_t* r
                     , bullet_list_t* bl, obj_list_t* dl,SDL_Surface* statics
                     , int xtra_weap);
void player_reset( player_t* p, SDL_Surface* ground, SDL_Surface* statics);
void player_reset_weapons(player_t* p,int xtra_on);
void player_focus(player_t* p);

void player_pix(player_t* p);

/*void player_move_up(player_t* p);
void player_move_down(player_t* p);*/
void player_move_left(player_t* p);
void player_move_right(player_t* p);
void player_show(player_t* p,int warding_flag);
void player_clean(player_t* p);
void player_change_rope_len(player_t* p,int len_modif);
void player_crop(player_t* p,map_t* m);
void player_launch_hook(player_t* p);
void player_remove_hook(player_t* p,player_t* other_p);
	
//void player_event_update(player_t* p,map_t* m,player_t* other_p);
void player_update( player_t* p,SDL_Surface* ground, SDL_Surface* statics);

void player_show_on_cam(player_t* p,camera_t* camera,int warding_flag);
void player_look_up(player_t* p);
void player_look_down(player_t* p);
void player_jump(player_t* p);

void player_is_aiming(player_t* p , player_t* target);

void player_change(player_t* p,int indice);
void player_fire(player_t* p);
void player_show_stats(player_t* p,game_mode_t gm);

void player_shot(player_t* p,player_id origin,Uint8 damage,int acc_x,int acc_y,void* userdata);
void player_regen(player_t* p,Uint8 health);
void player_new_position(player_t * p,SDL_Surface* ground, SDL_Surface* statics);

#endif
