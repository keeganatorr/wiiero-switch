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

#ifndef MAP_H
#define MAP_H

#include "common.h"
#include <SDL/SDL.h>
#include "screen.h"
#include "ressources.h"

#define MAP_WIDTH  800
#define MAP_HEIGHT 500

#define MINI_MAP_UPDATE_IDLE 1000

enum{
  BACKGROUND_LAYER,
  GROUND_MAP_LAYER,
  STATICS_MAP_LAYER,
  MAX_TOTAL_LAYERS
};


typedef struct{
  SDL_Surface*  layers[MAX_TOTAL_LAYERS];
  int           last_minimap_update;
}map_t;


map_t* map_init(ressources_t* loaded_res,int nb_rocks,screen_res_t res);
void   map_release(map_t* m);
void   map_reset(map_t* m ,ressources_t* loaded_res,int nb_rocks);
void   map_clean_zone(map_t* m,int zx, int zy, int zw,int zh);
void   map_drow_cave(map_t* m,int cx, int cy, int cr);
void update_minimap( map_t* m,camera_t *cam , int p1_x, int p1_y
                   , int p2_x, int p2_y, int force);
void   fill_splash_ground(SDL_Surface *s);
#endif
