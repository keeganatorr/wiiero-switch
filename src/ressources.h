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

#ifndef RESSOURCE_H
#define RESSOURCE_H
#include "common.h"
#include <SDL/SDL.h>


#define SPRITEDIR "sprites"
//#define SPRITEDIR "./sprites"

/* PLAYERS */
#define PL_PATH                      SPRITEDIR"/players/"
#define DEFAULT_PWSKIN_ANIM_STEPS     3
#define DEFAULT_PWSKIN               PL_PATH "worms.bmp"
#define DEFAULT_PWFLESH_ANIM_STEPS    4
#define DEFAULT_PWFLESH              PL_PATH "beef.bmp"


/* MAP */
#define MAP_PATH                     SPRITEDIR"/maps/"
#define DEFAULT_BREAKS               MAP_PATH "breakable.bmp"
#define DEFAULT_LITTLE_ROCKS         MAP_PATH "lsrocks.bmp"
#define DEFAULT_BIG_ROCKS            MAP_PATH "bsrocks.bmp"

/* SPLASH */
#define SPASH_PATH                   SPRITEDIR"/splash/"
#define DEFAULT_SPLASH               SPASH_PATH "load.bmp"


/* FX */
#define FX_PATH                      SPRITEDIR"/fx/"
#define DEFAULT_GUNFIRE_ANIM_START    3
#define DEFAULT_GUNFIRE_ANIM_SLEEP    2
#define DEFAULT_GUNFIRE_ANIM_STEPS    4
#define DEFAULT_GUNFIRE              FX_PATH "gunfire.bmp"
#define DEFAULT_SHELL_ANIM_STEPS      4
#define DEFAULT_SHELL                FX_PATH "shell.bmp"
#define DEFAULT_MAX_EXPLOSION        19
#define DEFAULT_EXPLOSION_ANIM_STEPS 20
#define DEFAULT_EXPLOSION            FX_PATH "explosion.bmp"
#define DEFAULT_EXPLOSION_BLUE       FX_PATH "blue_explosion.bmp"
#define DEFAULT_NUKE_SMOKE_ANIM_STEPS 4
#define DEFAULT_NUKE_SMOKE           FX_PATH "nuke_smoke.bmp"
#define DEFAULT_FLASH_ANIM_STEPS      4
#define DEFAULT_ZIMM_FLASH           FX_PATH "zimm_flash.bmp"
#define DEFAULT_TELEPORT_ANIM_STEPS   7
#define DEFAULT_TELEPORT             FX_PATH "teleport2.bmp"
#define DEFAULT_SMOKE_ANIM_STEPS      5
#define DEFAULT_SMOKE                FX_PATH "smoke.bmp"
#define DEFAULT_REACTOR_PARTICLE_STEPS 10

/* WEAPONS */
#define WP_PATH                      SPRITEDIR"/weapons/"
#define DEFAULT_CANNON_ANIM_STEPS     4
#define DEFAULT_CANNON               WP_PATH "cannon.bmp"
#define DEFAULT_GRASS_HOPPER         WP_PATH "grasshopper.bmp"
#define DEFAULT_GAUSS                WP_PATH "gauss.bmp"
#define DEFAULT_MINE_ANIM_STEPS       4
#define DEFAULT_MINE                 WP_PATH "mine.bmp"
#define DEFAULT_JUMP_MINE            WP_PATH "jmine.bmp"
#define DEFAULT_CLUSTER_ANIM_STEPS    4
#define DEFAULT_CLUSTER              WP_PATH "cluster.bmp"
#define DEFAULT_GRENADE_ANIM_STEPS    4
#define DEFAULT_GRENADE              WP_PATH "grenade.bmp"
#define DEFAULT_RED_GRENADE          WP_PATH "red_grenade.bmp"
#define DEFAULT_GAZ_GRENADE          WP_PATH "gaz_grenade.bmp"
#define DEFAULT_HOLY_GRENADE_ANIM_STEPS    8
#define DEFAULT_HOLY_GRENADE         WP_PATH "holy_grenade.bmp"
#define DEFAULT_BIG_NUKE             WP_PATH "big_nuke.bmp"
#define DEFAULT_MINI_NUKE            WP_PATH "mini_nuke.bmp"
#define DEFAULT_NUKE_PIECE           WP_PATH "nuke_piece.bmp"
#define DEFAULT_FLAMER_ANIM_STEPS     6
#define DEFAULT_FLAMER               WP_PATH "flamer.bmp"
#define DEFAULT_BLUE_FLAMER          WP_PATH "blue_flamer.bmp"
#define DEFAULT_BOUNCY_LARPA_ANIM_STEPS 4
#define DEFAULT_BOUNCY_LARPA         WP_PATH "bouncy_larpa.bmp"
#define DEFAULT_BOUNCY_MINE_ANIM_STEPS 4
#define DEFAULT_BOUNCY_MINE          WP_PATH "bmine.bmp"
#define DEFAULT_CHIQUITA_ANIM_STEPS   4
#define DEFAULT_CHIQUITA             WP_PATH "chiquita.bmp"
#define DEFAULT_CHIQUITA_ANIM_STEPS 4
#define DEFAULT_CHIQUITA_PIECE       WP_PATH "chiquita_piece.bmp"
#define DEFAULT_CRACKLER             WP_PATH "crackler.bmp"
#define DEFAULT_DART                 WP_PATH "dart.bmp"
#define DEFAULT_DIRTBALL             WP_PATH "dirtball.bmp"
#define DEFAULT_DOOMSDAY             WP_PATH "doomsday.bmp"
#define DEFAULT_EXPLOSIVES_ANIM_STEPS 4
#define DEFAULT_EXPLOSIVES           WP_PATH "explosives.bmp"
#define DEFAULT_FLOAT_MINE_ANIM_STEPS   4
#define DEFAULT_FLOAT_MINE           WP_PATH "float_mine.bmp"
#define DEFAULT_LARPA                WP_PATH "larpa.bmp"
#define DEFAULT_BULLET_MATRIX_STEPS 4
#define DEFAULT_MISSILE              WP_PATH "missile.bmp"
#define DEFAULT_NAPALM_ANIM_STEPS    8
#define DEFAULT_NAPALM               WP_PATH "napalm.bmp"
#define DEFAULT_SPIKEBALL_ANIM_STEPS 4
#define DEFAULT_SPIKEBALL            WP_PATH "spikeball.bmp"
#define DEFAULT_ZIMM                 WP_PATH "zimm.bmp"
#define DEFAULT_PINBALL              WP_PATH "pinball.bmp"


/* TOOLS */
#define OTHER_PATH                   SPRITEDIR"/others/"
#define DEFAULT_LFONT                OTHER_PATH "littlefont.bmp"
#define DEFAULT_MFONT                OTHER_PATH "medfont.bmp"
#define DEFAULT_BFONT                OTHER_PATH "bigfont.bmp"
#define DEFAULT_HOUSE                OTHER_PATH "house.bmp"
#define DEFAULT_HOOK                 OTHER_PATH "hook.bmp"
#define DEFAULT_HEALTH               OTHER_PATH "health.bmp"
#define DEFAULT_FLAG                 OTHER_PATH "flag.bmp"
#define DEFAULT_BONUS                OTHER_PATH "bonus.bmp"


typedef struct{
  /* players */
  SDL_Surface * worms1_skin;
  SDL_Surface * worms2_skin;
  SDL_Surface * worms_iron;
  SDL_Surface * worms1_flesh;
  SDL_Surface * worms2_flesh;
  /* map */
  SDL_Surface * breakable;
  SDL_Surface * lrocks;
  SDL_Surface * brocks;
  /* fx */
  SDL_Surface * gunfire_skin;
  SDL_Surface * blue_explosion;
  SDL_Surface * explosion;  
  SDL_Surface * shell;
  SDL_Surface * nuke_smoke;
  SDL_Surface * zimm_flash;
  SDL_Surface * teleport;
  SDL_Surface * smoke;
  
  /* weapons */
  SDL_Surface * cannon; 
  SDL_Surface * grass_hopper;
  SDL_Surface * mine;
  SDL_Surface * jmine;
  SDL_Surface * grenade;
  SDL_Surface * gauss; 
  SDL_Surface * cluster; 
  SDL_Surface * red_grenade;
  SDL_Surface * gaz_grenade;
  SDL_Surface * big_nuke;
  SDL_Surface * mini_nuke;
  SDL_Surface * nuke_piece;
  SDL_Surface * flamer;
  SDL_Surface * blue_flamer;
  SDL_Surface * bouncy_larpa;
  SDL_Surface * bouncy_mine;
  SDL_Surface * chiquita;
  SDL_Surface * chiquita_piece;
  SDL_Surface * crackler;
  SDL_Surface * dart;
  SDL_Surface * dirtball;
  SDL_Surface * doomsday;
  SDL_Surface * explosives;
  SDL_Surface * float_mine;
  SDL_Surface * holy_grenade;
  SDL_Surface * larpa;
  SDL_Surface * missile;
  SDL_Surface * napalm;
  SDL_Surface * spikeball;
  SDL_Surface * zimm;
  SDL_Surface * pinball;
  
  /* Others */
  SDL_Surface * house_p1;
  SDL_Surface * house_p2;
  SDL_Surface * hook;
  SDL_Surface * health;
  SDL_Surface * flag_p1;
  SDL_Surface * flag_p2;
  SDL_Surface * bonus;
  
}ressources_t;


ressources_t* load_ressource();
void shift_ressource_colors(SDL_Surface * s,Uint8 sr,Uint8 sg,Uint8 sb);
void release_ressource(ressources_t* r);
#endif
