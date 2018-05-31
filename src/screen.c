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

#include "tools.h"
#include "screen.h"
#include "image.h"
#include "ressources.h"
#include <unistd.h>

static int screen_resolution[MAX_RES][3]={{320,220,32}
                                          ,{480,272,32}
                                          ,{480,320,32}
                                          ,{600,400,32}
                                          ,{640,420,32}
                                          ,{640,480,32}
                                          ,{720,480,32}
                                          ,{1280,720,32}};

static int screen_is_init = 0;
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void screen_set_gamma(int gamma){
  SDL_SetGamma(gamma,gamma,gamma);
}



int screen_get_width(screen_res_t res){
  return screen_resolution[res][0];
}

int screen_get_height(screen_res_t res){
  return screen_resolution[res][1];
}

int screen_get_bpp(screen_res_t res){
  return screen_resolution[res][2];
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
screen_t* screen_init(screen_res_t res){
  return screen_custom_init( screen_get_width(res)
                           , screen_get_height(res)
                           , screen_get_bpp(res));
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void screen_release(screen_t* s){
  ASSERT(s);
  ASSERT(s->surface);
  SDL_FreeSurface( s->surface );
  secure_free(s);
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void screen_reset_mode(screen_t* s){
  ASSERT(s);
  if(s->surface)
    SDL_FreeSurface( s->surface );
  if(s->mode == WIN_SCREEN_MODE){
    s->surface = SDL_SetVideoMode(s->w, s->h, s->bpp, SDL_HWSURFACE
                                                      | SDL_DOUBLEBUF );
    SDL_ShowCursor( SDL_ENABLE );
  }else{
    s->surface = SDL_SetVideoMode(s->w, s->h, s->bpp, SDL_HWSURFACE
                                                       | SDL_DOUBLEBUF
                                                       | SDL_FULLSCREEN );
    SDL_ShowCursor( SDL_DISABLE );
  }
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void screen_flip_mode(screen_t* s){
  ASSERT(s);
  s->mode = (s->mode == WIN_SCREEN_MODE) ? FULL_SCREEN_MODE : WIN_SCREEN_MODE;
  screen_reset_mode(s);
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void screen_loading_splash(screen_t* s){
  ASSERT(s);
  SDL_Rect offset;
  Uint8 alpha=0; 
  SDL_Surface* splash = image_load( DEFAULT_SPLASH );
  /* Recuperation des coordonnées */
  offset.x = (s->surface->w - splash->w)/2;
  offset.y = (s->surface->h - splash->h)/2;
  offset.h = splash->h;
  offset.w = splash->w;
  
  
  for(alpha=0; alpha<255;alpha++){
#ifndef PSP_MODE
    /* Wii, PC */
    usleep(500);
#else
    SDL_Delay(5);
#endif
  SDL_SetAlpha(splash,SDL_SRCALPHA | SDL_RLEACCEL, alpha);
  SDL_BlitSurface( splash, 0 , s->surface, &offset );
  SDL_Flip(s->surface);
  }

  SDL_FreeSurface(splash);  
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void screen_clean(screen_t* s){
  ASSERT(s);
  ASSERT(s->surface);
  SDL_FillRect( s->surface,NULL
              , SDL_MapRGB(s->surface->format,DEFAULT_CAM_COLOR
                          ,DEFAULT_CAM_COLOR,DEFAULT_CAM_COLOR));
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
screen_t* screen_custom_init(int w,int h,int bpp){
  ASSERT(!screen_is_init);
  screen_t* s = secure_malloc(sizeof(screen_t));
  s->surface = 0L;
  screen_is_init = 1;
  s->h = h;
  s->w = w;
  s->bpp = bpp;
  s->screen_cam_list = 0L;
  screen_set_gamma(1);
#ifdef PC_MODE
  s->mode = WIN_SCREEN_MODE;
#else
  s->mode = FULL_SCREEN_MODE;
#endif
  screen_reset_mode(s);
  return s;
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

camera_t* screen_add_camera(screen_t* s,int cx,int cy){
  return screen_add_custom_camera(s,cx,cy,s->w,s->h, s->bpp);
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

camera_t* screen_add_custom_camera(screen_t* s,int cx,int cy,int cw,int ch,int cbpp){
  camera_list_t * new_cam_cell;
  DBG("- adding Cams!\n");
  ASSERT(s);
  ASSERT(screen_is_init);
  new_cam_cell = secure_malloc(sizeof(camera_list_t));
  new_cam_cell->next = 0L;
  new_cam_cell->cam = camera_custom_init( cx, cy, cw, ch, cbpp,s->surface);

  if(!s->screen_cam_list){
    s->screen_cam_list = new_cam_cell;
  }else{
    new_cam_cell->next = s->screen_cam_list;
    s->screen_cam_list = new_cam_cell;
  }

  return s->screen_cam_list->cam;
  
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void screen_display(screen_t* s){
  ASSERT(s);
  ASSERT(screen_is_init);
#ifdef OLD_MODE
  if(!s->screen_cam_list){
    SDL_Flip(s->surface);
  }else{
    camera_list_t * cur_cam_slot = s->screen_cam_list;

    while(cur_cam_slot){
      if( camera_is_on(cur_cam_slot->cam) ){
        SDL_Rect offset;
        /* Recuperation des coordonnées */
        offset.x = cur_cam_slot->cam->screen_x;
        offset.y = cur_cam_slot->cam->screen_y;
        offset.h = cur_cam_slot->cam->h;
        offset.w = cur_cam_slot->cam->w;    
        SDL_BlitSurface( cur_cam_slot->cam->cam_surface, 0 , s->surface, &offset );
      }
      cur_cam_slot = cur_cam_slot->next;
    }
    SDL_Flip(s->surface);
  }
#else
  SDL_Flip(s->surface);  
#endif
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
