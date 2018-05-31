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

#ifndef WEAPON_H
#define WEAPON_H

#include "common.h"
#include <SDL/SDL.h>
#include "object.h"
#include "ressources.h"


#define WEAPON_SLOTS 5
#define MAX_SUPPORTED_BULLETS 1000


enum{
  NO_KICK    = 0x01,
  NO_SHELL   = 0x02,
  NO_GUNFIRE = 0x04 
};


typedef enum{
  /* GUNS */
  WEAPON_HANDGUN,
  WEAPON_UZI,
  WEAPON_CHAINGUN,
  WEAPON_SHOOTGUN,
  WEAPON_SUPER_SHOOTGUN,
  WEAPON_MINIGUN,
  WEAPON_CANNON,
  WEAPON_BAZOOKA,
  WEAPON_DOOMSDAY,
  WEAPON_WINCHESTER,
  WEAPON_LARPA,
  WEAPON_BOUNCY_LARPA,
  WEAPON_ZIMM,
  WEAPON_RIFLE,
  WEAPON_BLASTER,
  WEAPON_GAUSS_GUN,
  WEAPON_FLAMER,
  WEAPON_MINI_ROCKER,
  WEAPON_MISSILE,
  WEAPON_RB_RAMPAGE,
  WEAPON_HELLRAIDER,
  WEAPON_LASER,
  WEAPON_GRASSHAPPER,

  /* TRAPS */
  WEAPON_BOOBY_TRAP,
  WEAPON_MINE,
  WEAPON_BOUNCY_MINE,
  WEAPON_FLOAT_MINE,
  WEAPON_SPIKEBALLS,

  /* EXPLOSIVES */
  WEAPON_CRACKLER,
  WEAPON_GRENADE,
  WEAPON_CLUSTER_BOMB,
  WEAPON_MINI_NUKE,
  WEAPON_BIG_NUKE,
  WEAPON_CHIQUITA_BOMB,
  WEAPON_EXPLOSIVES,
  WEAPON_NAPALM,

  /* OTHERS */
  WEAPON_DIRTBALL,
  MAX_WEAPONS,

  /* Xtra Weapons */
  WEAPON_HOLY_GRENADE = MAX_WEAPONS,
  WEAPON_SUPER_CANNON,
  WEAPON_CEILING_MINE,
  WEAPON_GH_GRENADE,
  WEAPON_BLUE_FLAMER,
  WEAPON_AN_M14_GRENADE,
  WEAPON_GAZ_GRENADE,
  WEAPON_PLASMA_CANNON,
  WEAPON_NUKE_LARPA,
  WEAPON_CTRL_NUKE_LARPA,
  WEAPON_SC_LAUNCHER,
  WEAPON_VERMIFUGE,
  WEAPON_JUMPING_MINE,
  WEAPON_TRAP,
  WEAPON_KAMIKASE,
  WEAPON_SPIKE_TIME_BOMB,
  WEAPON_VIRAL_CASE, 
  WEAPON_VERMI_NUKE,
  WEAPON_MEDIKIT, 
  WEAPON_SHEELD,
  WEAPON_PINBALL,
  MAX_WEAPONS_AND_XTRA,

  /* HIDDEN */
  WEAPON_CLUSTER_PIECE = MAX_WEAPONS_AND_XTRA, 
  WEAPON_FIRE,
  WEAPON_BLUE_FIRE,
  WEAPON_NINJA,
  WEAPON_CHIQUITA_PIECE,
  WEAPON_GAZ_PARTICLE,
  WEAPON_NAPALM_FLAME,
  WEAPON_HEALTH_GIFT,
  WEAPON_BONUS_GIFT,
  WEAPON_NUKE_PIECE,
  WEAPON_JMINE_ACTIV,
  WEAPON_SPORE_PARTICULE,
  WEAPON_BOOBY_BONUS,
  WEAPON_BOOBY_HEALTH,
  WEAPON_P1_HOUSE,
  WEAPON_P2_HOUSE,
  WEAPON_P1_FLAG,
  WEAPON_P2_FLAG,
  MAX_REAL_WEAPONS
}weapon_id;

typedef struct{
  Uint8     damages;
  Uint8     range;
  Uint16    timeout;
  Uint8     explo_size;
  weapon_id w_id;
  obj_t     obj;
  void*     p_origin;
}bullet_t;

typedef struct{
  Uint8  damages;           // damages
  Uint8  range;             // range
  Uint16 timeout;          // timeout
  Uint8  explo_size;        // explosion size
  Uint8  cadency_step;      // cadency
  Uint8  ammo;              // ammo
  Uint8  resist;            // resistancy
  Uint16 recharge_step;    // rechage step
  float velocity;          // velocity
  weapon_id id;
  bullet_t *last_bullet;
}weapon_t;



typedef struct bullet_cell_s{
  bullet_t* bullet;
  struct  bullet_cell_s *next;
}bullet_cell_t;

typedef struct{
  bullet_cell_t *head;
  int         len;
}bullet_list_t;

void weapon_set_options(Uint8 show_bonus_name,Uint8 reload_percent);
weapon_t* weapon_init(weapon_id w_id);

int weapon_can_shoot(weapon_t* w);
int weapon_update(weapon_t* w);
int weapon_reload(weapon_t* w);
Uint8 weapon_shoot(weapon_t* w,void* player,int acc_x,int acc_y);
int  weapon_get_max_ammo(weapon_t* w);
int  weapon_get_nb_recharge_steps(weapon_t* w);
char* weapon_get_name(weapon_id w_id);
bullet_list_t* weapon_init_bullet_list();

bullet_t* weapon_add_bullet_to_list(bullet_list_t* l,bullet_t* bullet);

void proceed_bullets(bullet_list_t* l,SDL_Surface* ground,SDL_Surface* statics
                     , void* p1, void* p2,void* coll_cb_userdata);

void blit_bullets(bullet_list_t* l,camera_t* cam1,camera_t* cam2);

void create_gift(bullet_list_t* l,ressources_t* r,int x,int y,int xtra_weap);

void ninja_hook_disconnect(void* bullet,void *p1,void *p2, void* userdata);
void clean_bullets_list(bullet_list_t* l);

void set_player_house(bullet_list_t* l,ressources_t* r,int playerid,void* layer);
void set_player_flag(bullet_list_t* l,ressources_t* r,int playerid,void* layer);
void drop_player_flag(bullet_list_t* l,ressources_t* r,int playerid,int x,int y);

#endif
