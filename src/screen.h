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

#ifndef SCREEN_H
#define SCREEN_H

#include "common.h"
#include <SDL/SDL.h>
#include "camera.h"


#ifdef PC_MODE
  #define SCREEN_RES   RES_640_420
#endif

#ifdef PSP_MODE
  #define SCREEN_RES   RES_480_272
#endif

#ifdef WII_MODE
#define SCREEN_RES   RES_320_240
//#define SCREEN_RES   RES_640_420
#endif

#ifdef SWITCH
//#define SCREEN_RES   RES_1280_720
#define SCREEN_RES   RES_320_240
//#define SCREEN_RES   RES_1280_720
//#define SCREEN_RES   RES_640_420
#endif



#define WANTED_FPS 25

enum{
  WIN_SCREEN_MODE,
  FULL_SCREEN_MODE
};

typedef struct camera_list_s{
  void * next;
  camera_t * cam;  
}camera_list_t;

typedef struct{
  int h;
  int w;
  int bpp;
  int mode;
  SDL_Surface* surface;
  camera_list_t* screen_cam_list;
}screen_t;



screen_t* screen_init(screen_res_t res);
void      screen_release(screen_t* s);
void      screen_loading_splash(screen_t* s);
void      screen_clean(screen_t* s);
screen_t* screen_custom_init(int w,int h,int bpp);
camera_t* screen_add_camera(screen_t* s,int cx,int cy);
camera_t* screen_add_custom_camera(screen_t* s,int cx,int cy,int cw,int ch,int cbpp);
void      screen_display(screen_t* s);
void      screen_flip_mode(screen_t* s);
int       screen_get_width(screen_res_t res);
int       screen_get_height(screen_res_t res);
int       screen_get_bpp(screen_res_t res);
#endif
