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


#include "colors.h"

#define TRANSP_R 0xff
#define TRANSP_G 0x3f
#define TRANSP_B 0x7f


/* ************************************************************************** *
 * Ground colors table                                                        */
Uint8 grounds_colors[NB_AMBIENCES][GROUNDS][CPALETT][COLORS] = {
{ {{0x50,0x23,0x00},
   {0x52,0x26,0x00},
   {0x57,0x25,0x00},
   {0x54,0x27,0x00},
   {0x5b,0x2b,0x00},
   {0x5f,0x2f,0x00}}
 ,{{0x7d,0x42,0x06},
   {0x8d,0x4d,0x0f},
   {0x90,0x4f,0x10},
   {0x92,0x52,0x12},
   {0x94,0x53,0x13},
   {0x9d,0x58,0x19}}
},
{ {{0x6f,0x43,0x05},
   {0x66,0x3e,0x04},
   {0x66,0x40,0x09},
   {0x66,0x43,0x11},
   {0x6c,0x47,0x12},
   {0x60,0x3f,0x10}}
 ,{{0xb3,0x92,0x27},
   {0x98,0x72,0x11},
   {0xaa,0x7e,0x1b},
   {0x9b,0x70,0x12},
   {0x91,0x6d,0x0c},
   {0x9a,0x84,0x27}}
},
{ {{0x50,0x23,0x00},
   {0x52,0x26,0x00},
   {0x57,0x25,0x00},
   {0x54,0x27,0x00},
   {0x5b,0x2b,0x00},
   {0x5f,0x2f,0x00}}
 ,{{0x0f,0x0d,0x07},
   {0x10,0x0f,0x0d},
   {0x10,0x0d,0x05},
   {0x0f,0x0c,0x05},
   {0x10,0x0f,0x13},
   {0x03,0x13,0x12}}
}};


/* ************************************************************************** *
 * Transparent colors                                                         */
Uint8 trcolor[COLORS] = {TRANSP_R,TRANSP_G,TRANSP_B};

/* ************************************************************************** *
 * Get ground colors                                                          */
void get_grounds_colors(const int ambience_id, const int ground_id
                       , const int pallet_id,Uint8* r,Uint8* g,Uint8* b){
  *r=grounds_colors[ambience_id][ground_id][pallet_id][CR];
  *g=grounds_colors[ambience_id][ground_id][pallet_id][CG];
  *b=grounds_colors[ambience_id][ground_id][pallet_id][CB];
}

/* ************************************************************************** *
 * Compute transparent colors (taking care of screen bpp deep)                */
void compute_trcolor(const SDL_Surface *surface){
  SDL_GetRGB( SDL_MapRGB(surface->format,TRANSP_R,TRANSP_G,TRANSP_B)
            , surface->format
            , &(trcolor[CR]),&(trcolor[CG]),&(trcolor[CB]));
}

/* ************************************************************************** *
 * Get transparent color for the given surface                                */
void get_surface_trcolor(const SDL_Surface *surface,Uint8* r,Uint8* g,Uint8* b){
  SDL_GetRGB( SDL_MapRGB(surface->format,TRANSP_R,TRANSP_G,TRANSP_B)
            , surface->format, r,g,b);
}

/* ************************************************************************** *
 * Fiil r,g,b with precomputed transparent color                              */
void get_trcolor(Uint8* r,Uint8* g,Uint8* b){
  *r=trcolor[CR];
  *g=trcolor[CG];
  *b=trcolor[CB];
}

