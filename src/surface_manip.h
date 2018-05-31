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

#ifndef SURFACE_MANIP_H
#define SURFACE_MANIP_H

#include "common.h"
#include <SDL/SDL.h>

#define LAYERS_OPTIM_MODE

void get_pix_color(const SDL_Surface * surface, int x, int y
                  , Uint8 * r, Uint8 * g, Uint8 * b);
void put_pix_color(SDL_Surface* surface, int x, int y, int r, int g, int b);
void drow_hline(SDL_Surface* surface,int x,int y,int w,int h,int cr, int cg, int cb);
void drow_line(SDL_Surface* surface,int x1, int y1, int x2, int y2,int cr, int cg, int cb);
void drow_circle(SDL_Surface* surface,int cx,int cy,int cr,int r,int g,int b);
void drow_colored_circle(SDL_Surface* surface,int cx,int cy,int cr);
void get_empty_layer_position(int* x, int* y,const SDL_Surface* layer);
SDL_Surface* surface_try_to_optimize(SDL_Surface* in, char* layer_name);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static __inline__ void lock_layer(SDL_Surface* s, char* surface_name){
#ifdef LAYERS_OPTIM_MODE
  if(SDL_MUSTLOCK(s))
    if(SDL_LockSurface(s) == -1)
      printf("Can't lock %s hardware surface\n",surface_name);
#endif
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static __inline__ void unlock_layer(SDL_Surface* s){
#ifdef LAYERS_OPTIM_MODE
  if(SDL_MUSTLOCK(s))
    SDL_UnlockSurface(s);
#endif
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static __inline__ void switch_values(int* x, int* y){
  int t = *x;
  *x = *y;
  *y = t;
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#endif
