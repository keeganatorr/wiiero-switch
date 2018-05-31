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

#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"
#include <SDL/SDL.h>

#define CAMERA_NOSHAKE 0  /* Don't shake camera on explosion */
#define CAMERA_SHAKE   1  /* Shake camera on explosion */
#define DEFAULT_CAM_COLOR 0x00 /* default camera color (black) */


typedef enum{
  CAMERA_OFF = 0,
  CAMERA_ON  = 1
}camera_status; /* Camera status */


typedef struct{
 int x;
 int y;
}point_t; /* Point struct */


typedef struct{
  int shake_mode;              /* Camera shake mode */
  int screen_x;                /* Camera x position on screen */
  int screen_y;                /* Camera y position on screen */
  int w;                       /* Camera width  */
  int h;                       /* Camera height */
  int bpp;                     /* Camera bpp    */
  int alpha;                   /* Camera alpha (only on OLD_MODE) */
  int map_x;                   /* Camera x position on game map   */
  int map_y;                   /* Camera x position on game map   */
  point_t focused_pos;         /* Camera focused point  */
  SDL_Surface * cam_surface;   /* Camera dest surface */
  camera_status status;        /* Camera status */
}camera_t; /* camera struct */


/* ************************************************************************** */
extern camera_t* camera_init(screen_res_t res,SDL_Surface * surface);
extern camera_t* camera_custom_init( int x,int y,int w,int h,int bpp
                                   , SDL_Surface * surface);
extern void camera_release( camera_t* cam);
extern void camera_switch_on( camera_t* c);
extern void camera_switch_off( camera_t* c);
extern int  camera_is_on( camera_t* c);
extern void camera_clean( camera_t* c);
extern void camera_set_alpha( camera_t* c,int alpha_value);
extern void camera_focus_on( camera_t* c,int x,int y,int max_w,int max_h);
extern void camera_blit( camera_t* c,SDL_Surface * src_surface);
extern void camera_blit_shadow( camera_t* c,SDL_Surface * src_surface);
extern void camera_set_shake_frames( camera_t* c,int shake_frames);
extern void camera_blit_surface_on( camera_t *c,SDL_Surface *src_surface
                                  , SDL_Rect *skinpos,SDL_Rect *campos);
extern void camera_drow_hline( camera_t *c,int x,int y,int w,int h
                             , int cr,int cg, int cb);
extern void camera_put_pix_color( camera_t *c, int x, int y
                                , int cr, int cg, int cb);
extern void camera_fillrect( camera_t *c,SDL_Rect *rect,Uint32 color);
extern void camera_drow_line( camera_t *c, int x1, int y1, int x2, int y2
                            , int cr, int cg, int cb);
/* ************************************************************************** */

#endif
