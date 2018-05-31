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

#ifndef COLOR_H
#define COLOR_H

#include "common.h"
#include <SDL/SDL.h>

#define DIRT_AMB     0
#define SAND_AMB     1
#define NIGTH_AMB    2
#define NB_AMBIENCES 3

#define BACK_GR  0
#define FRONT_GR 1
#define GROUNDS  2

#define CR 0
#define CG 1
#define CB 2
#define COLORS 3

#define CPALETT 6

extern void compute_trcolor(const SDL_Surface *surface);
extern void get_grounds_colors( const int ambience_id,const int ground_id
                              , const int pallet_id,Uint8* r,Uint8* g,Uint8* b);
extern void get_trcolor(Uint8* r,Uint8* g,Uint8* b);
extern void get_surface_trcolor( const SDL_Surface *surface
                                , Uint8* r,Uint8* g,Uint8* b);

#endif
