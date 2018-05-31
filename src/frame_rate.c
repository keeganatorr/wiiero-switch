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


#include "frame_rate.h"
#include "camera.h"
#include "font.h"

static int frame;
static int start_time; 
int is_framerate_on = 0;

void framer_init(){
  frame = 0;
  start_time = SDL_GetTicks();
}


int framer_get_diff(){
  return SDL_GetTicks() - start_time; 
}

void framer_on(){
#ifndef NO_FRAME_RATE
  is_framerate_on = 1; 
#endif
}

void framer_off(){
  is_framerate_on = 0;  
}


void framer_check(camera_t* cam){
  static char caption[ 64 ];
  frame++;
  if(  framer_get_diff()   > 1000){
    sprintf( caption, "fps:%.2f", (float)frame / ( framer_get_diff() / 1000.f ) );
    framer_init();
  }
  if(!is_framerate_on)
    return;
  if(cam)
    font_print_strict_pos(cam,caption
          , cam->w - 60, 3
          , FONT_STANDARD);
}

