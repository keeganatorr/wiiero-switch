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


#ifndef GAME_H
#define GAME_H

#include "common.h"
#include <SDL/SDL.h>
#include "screen.h"
#include "ressources.h"
#include "player.h"
#include "map.h"
#include "weapons.h"
#include "sound_engine.h"

#ifdef PC_MODE
#define WIIERO_VERSION "wiiero v1.3    [pc edition]"
#endif

#ifdef PSP_MODE
#define WIIERO_VERSION "wiiero v1.3    [psp edition]"
#endif

#ifdef WII_MODE
#define WIIERO_VERSION "wiiero v1.3"
#endif


#define WIIERO_STD_LIFES              10
#define WIIERO_STD_FLAGS               5
#define WIIERO_STD_TIME              300
#define WIIERO_STD_GIFT_NB             5
#define WIIERO_STD_LOADING_TIME      100
#define WIIERO_STD_BONUS_NAMES         1
#define WIIERO_STD_MINI_MAP            1
#define WIIERO_STD_MAP_REGEN           1
#define WIIERO_STD_SHADOW              1
#define WIIERO_STD_XTRA_W              1
#define WIIERO_STD_NB_ROCKS           50
#define WIIERO_NAME_LEN               10
#define WIIERO_STD_SCREEN_RESOLUTION  SCREEN_RES
#define WIIERO_STD_OPT_FILE           "config.wro"
#define WIIERO_STD_MUB                 0
#define WIIERO_STD_LANG                0


enum{
    FULL_SCREEN_CAM,
    PLAYER_1_GAME_ZONE_CAM,
    PLAYER_2_GAME_ZONE_CAM,
    GLOBAL_MINI_MAP_CAM,
    PLAYER_1_STATS_ZONE_CAM,
    PLAYER_2_STATS_ZONE_CAM,
    NB_MAX_CAMS   
};

typedef enum{
    GAME_SET_MENU,
    GAME_MENU,
    GAME_SET_OPTIONS,
    GAME_OPTIONS,
    GAME_OPTIONS_BIS,
    GAME_SET_WEAPON_SELECT,
    GAME_WEAPON_SELECT,
    GAME_SET_PAUSE,
    GAME_PAUSE,
    GAME_SET_PLAYING,
    GAME_PLAYING,
    GAME_SET_ROUND_STATS,
    GAME_ROUND_STATS,
    GAME_LOAD_CONFIG,
    GAME_SAVE_CONFIG,
    GAME_HOW_TO_PLAY,
    GAME_ABOUT,
    GAME_SET_EDIT_NAME,
    GAME_EDIT_NAME,
    GAME_EXIT
}game_status_t;


typedef struct{
  int            wiiero_frame_delay;
  /* options */
  game_mode_t    wiiero_opt_game_mode;
  Uint16         wiiero_opt_nb_lifes;
  Uint16         wiiero_opt_nb_flags;
  Uint16         wiiero_opt_got_time; 
  Uint8          wiiero_opt_gift_nb;
  Uint8          wiiero_opt_loading_time;
  Uint8          wiiero_opt_bonus_name;
  Uint8          wiiero_opt_mini_map;
  Uint8          wiiero_opt_map_regen;
  Uint8          wiiero_opt_shadow;
  Uint8          wiiero_opt_xtra_weap;
  Uint8          wiiero_opt_nb_rocks;
  screen_res_t   wiiero_opt_screen_resolution;
  /* ******* */
  game_status_t  wiiero_game_status;
  screen_t*      wiiero_screen;
  camera_t*      wiiero_cameras[NB_MAX_CAMS];
  map_t*         wiiero_map;
  player_t*      worms[NB_PLAYERS];
  obj_list_t*    wiiero_dynamic_objects;
  bullet_list_t* wiiero_bullets;
  ressources_t*  wiiero_ressources;
  Uint8          wiiero_exit; 
  Uint8          wiiero_lang;
}game_t;


extern void wiiero_init(game_t* g);
extern void wiiero_load(game_t* g);
extern int  wiiero_alive(game_t* g);
extern void wiiero_cycle(game_t* g);
extern void wiiero_quit(game_t* g);
extern int wiiero_load_config(game_t* g);
extern int wiiero_save_config(game_t* g);
#endif
