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


#include "image.h"
#include "tools.h"
#include "surface_manip.h"

extern Uint8 transparent_r_value;
extern Uint8 transparent_g_value;
extern Uint8 transparent_b_value;
static SDL_Surface* screen=0l;

/*
SDL_Surface* image_load(char * filename){
  static int trcolor[3] = {0xff,0x3f,0x7f};
  return image_custom_load(filename,trcolor); 
}*/

void image_loader_set_screen(SDL_Surface* s){
  screen = s;
}

SDL_Surface* image_load(char * filename){
  HARD_DBG(" image_load call for %s ",filename);
  ASSERT(filename);
  ASSERT(screen);
  SDL_Surface* tmp = 0L;
  SDL_Surface* image = 0L;
  Uint8 r,g,b;
  HARD_DBG(" >> loading %s\n",filename);
  tmp = SDL_LoadBMP(filename);
  if(!tmp){
    HARD_DBG("can't load wiiero image [%s] !\n",filename);
    exit(EXIT_FAILURE);
  }

//  HARD_DBG(" >> Getting transparent color\n");
  get_surface_trcolor(tmp,&r,&g,&b);
//  HARD_DBG(" >> Setting transparent color\n");
  SDL_SetColorKey(tmp, SDL_SRCCOLORKEY, SDL_MapRGB(tmp->format,r,g,b));
  image = tmp;//SDL_DisplayFormat(tmp);
  //SDL_FreeSurface(tmp);
  ASSERT(image);
//  HARD_DBG(" >> image loaded\n");
  return image;
}
