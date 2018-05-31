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


#include "map.h"
#include "font.h"
#include "tools.h"
#include "surface_manip.h"
#include "colors.h"
#include "screen.h"

#define MAX_BACK_OBJECTS  100
#define MAX_BREAK_OBJECTS 100


#define MAX_CAVES 30

extern Uint8 selected_amb;
extern Uint8 transparent_r_value;
extern Uint8 transparent_g_value;
extern Uint8 transparent_b_value;

void fill_background(map_t* m,ressources_t* r);
void fill_ground(map_t* m,ressources_t* r);
void map_drow_big_cave(map_t* m);
void fill_statics(map_t* m,ressources_t* r,int);


map_t* map_init(ressources_t* loaded_res, int nb_rocks,screen_res_t res){
  map_t* m;
  int i = BACKGROUND_LAYER;
  DBG("- init map\n");
  DBG("- size: %dx%d",MAP_WIDTH,MAP_HEIGHT);
  m = secure_malloc(sizeof(map_t));

  while(i < MAX_TOTAL_LAYERS)
    m->layers[i++] 
     = SDL_CreateRGBSurface( SDL_SWSURFACE
                           , MAP_WIDTH
                           , MAP_HEIGHT
                           , screen_get_bpp(res)
                           ,0,0,0,0);

  font_console_print_debug("  background layer optim...\n",FONT_SMALL);
  m->layers[BACKGROUND_LAYER] = surface_try_to_optimize(m->layers[BACKGROUND_LAYER],"background layer");
  font_console_print_debug("  ground layer optim...\n",FONT_SMALL);
  m->layers[GROUND_MAP_LAYER] = surface_try_to_optimize(m->layers[GROUND_MAP_LAYER],"ground layer");
  font_console_print_debug("  static layer optim...\n",FONT_SMALL);
  m->layers[STATICS_MAP_LAYER] = surface_try_to_optimize(m->layers[STATICS_MAP_LAYER],"static layer");
  map_reset(m,loaded_res,nb_rocks);
  return m;
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void   map_release(map_t* m){
  ASSERT(m)
  int i = BACKGROUND_LAYER;
  while(i < MAX_TOTAL_LAYERS)
    SDL_FreeSurface(m->layers[i++]);
  secure_free(m);
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void map_reset(map_t* m ,ressources_t* loaded_res, int nb_rocks){
  ASSERT(m);
  ASSERT(loaded_res); 
  fill_background(m,loaded_res);
  fill_ground(m,loaded_res);
  fill_statics(m,loaded_res, nb_rocks);
  m->last_minimap_update=-10000;
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void update_minimap( map_t* m,camera_t *cam , int p1_x, int p1_y
                   , int p2_x, int p2_y, int force){
  ASSERT(m);
  ASSERT(cam); 
  int x,y;
  float x_step = MAP_WIDTH / (cam->w+1.0f);
  float y_step = MAP_HEIGHT/ (cam->h+1.0f);
                       
  if(((SDL_GetTicks() - m->last_minimap_update)<MINI_MAP_UPDATE_IDLE)
     &&(force == 0))
      return;

  m->last_minimap_update = SDL_GetTicks();
  /* Update mini_map */
  for(x=0; x < cam->w; x++){
    for(y=0; y < cam->h; y++){
      Uint8 r,g,b;
      get_pix_color(m->layers[STATICS_MAP_LAYER],x*x_step,y*y_step,&r,&g,&b);
      if((r==transparent_r_value)&&(g ==transparent_g_value)&&(b ==transparent_b_value))
        get_pix_color(m->layers[GROUND_MAP_LAYER],x*x_step,y*y_step,&r,&g,&b);
      if((r==transparent_r_value)&&(g ==transparent_g_value)&&(b ==transparent_b_value)){
        get_grounds_colors(selected_amb,BACK_GR,0,&r,&g,&b);
      }
      camera_put_pix_color(cam,x,y,r,g,b);
    }
  }
  /* Update players position */
  p1_x = (p1_x/x_step == 0) ? 1 : p1_x/x_step;
  p1_y = (p1_y/y_step == 0) ? 1 : p1_y/y_step;
  p1_x = (p1_x >= cam->w) ? cam->w-1 : p1_x;
  p1_y = (p1_y >= cam->h) ? cam->h-1 : p1_y;
  
  camera_put_pix_color(cam,p1_x-1, p1_y-1, 0, 0, 255);
  camera_put_pix_color(cam,p1_x-1, p1_y+1, 0, 0, 255);
  camera_put_pix_color(cam,p1_x  , p1_y  , 0, 0, 255);
  camera_put_pix_color(cam,p1_x+1, p1_y-1, 0, 0, 255);
  camera_put_pix_color(cam,p1_x+1, p1_y+1, 0, 0, 255);
  
  p2_x = (p2_x/x_step == 0) ? 1 : p2_x/x_step;
  p2_y = (p2_y/y_step == 0) ? 1 : p2_y/y_step;
  p2_x = (p2_x >= cam->w) ? cam->w-1 : p2_x;
  p2_y = (p2_y >= cam->h) ? cam->h-1 : p2_y;
  
  camera_put_pix_color(cam,p2_x-1, p2_y-1, 0, 255, 0);
  camera_put_pix_color(cam,p2_x-1, p2_y+1, 0, 255, 0);  
  camera_put_pix_color(cam,p2_x  , p2_y  , 0, 255, 0);
  camera_put_pix_color(cam,p2_x+1, p2_y-1, 0, 255, 0);
  camera_put_pix_color(cam,p2_x+1, p2_y+1, 0, 255, 0);
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void fill_background(map_t* m,ressources_t* r){
  ASSERT(m);
  ASSERT(r); 
  int i,x,y;
  static Uint8 bg_loaded = 0;
  
  if(bg_loaded)
    return;
  bg_loaded=1;
  for(x=0; x < MAP_WIDTH; x ++){
    for(y=0; y < MAP_HEIGHT; y ++){
      int color = rand() % CPALETT;
      Uint8 r,g,b;
      get_grounds_colors(selected_amb,BACK_GR,color,&r,&g,&b);
      put_pix_color( m->layers[BACKGROUND_LAYER],x,y,r,g,b);
    }
  }
  
  for(i=0; i< MAX_BACK_OBJECTS; i++){
      SDL_Rect pos;
      SDL_Rect skin_offset;
      pos.x = rand() % (MAP_WIDTH  - (r->breakable->w/2)); 
      pos.y = rand() % (MAP_HEIGHT - (r->breakable->h/4));  
      skin_offset.x = rand() % 2 * (r->breakable->w/2); 
      skin_offset.y = rand() % 4 * (r->breakable->h/4);  
      skin_offset.w = r->breakable->w/2; 
      skin_offset.h = r->breakable->h/4;
      SDL_SetAlpha(r->breakable,SDL_SRCALPHA | SDL_RLEACCEL, 150);
      SDL_BlitSurface(r->breakable, &skin_offset ,m->layers[BACKGROUND_LAYER] , &pos );
      SDL_SetAlpha(r->breakable,SDL_SRCALPHA | SDL_RLEACCEL, 255);
  }
}/* * * * * * * * * *i * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void fill_ground(map_t* m,ressources_t* r){
  int i,x,y;
  ASSERT(r);
  ASSERT(m);
  /* put ground */
    
  /* TODO, REMOVE THIS (empty map for weapons tests) !!! *//*
  SDL_FillRect( m->layers[GROUND_MAP_LAYER],NULL
              , SDL_MapRGB(m->layers[GROUND_MAP_LAYER]->format
                          , transparent_r_value
                          , transparent_g_value
                          , transparent_b_value));
  SDL_SetColorKey(m->layers[GROUND_MAP_LAYER]
               , SDL_SRCCOLORKEY
               , SDL_MapRGB( m->layers[GROUND_MAP_LAYER]->format
                           , transparent_r_value
                           , transparent_g_value
                           , transparent_b_value));
  return;*/
    
  for(x=0; x < MAP_WIDTH; x ++){
    for(y=0; y < MAP_HEIGHT; y ++){
      int color = rand() % CPALETT; 
      Uint8 r,g,b;
      get_grounds_colors(selected_amb,FRONT_GR,color,&r,&g,&b);
      put_pix_color( m->layers[GROUND_MAP_LAYER],x,y,r,g,b);
   }
  }

  /* put objects */
  for(i=0; i< MAX_BREAK_OBJECTS; i++){
      SDL_Rect pos;
      SDL_Rect skin_offset;
      pos.x = rand() % (MAP_WIDTH  - (r->breakable->w/2)); 
      pos.y = rand() % (MAP_HEIGHT - (r->breakable->h/4));  
      skin_offset.x = rand() % 2 * (r->breakable->w/2); 
      skin_offset.y = rand() % 4 * (r->breakable->h/4);  
      skin_offset.w = r->breakable->w/2; 
      skin_offset.h = r->breakable->h/4;
      SDL_BlitSurface(r->breakable, &skin_offset ,m->layers[GROUND_MAP_LAYER] , &pos );
  }
  for(i=0; i< MAX_CAVES; i++)
    map_drow_big_cave(m);

  SDL_SetColorKey(m->layers[GROUND_MAP_LAYER]
                 , SDL_SRCCOLORKEY
                 , SDL_MapRGB( m->layers[GROUND_MAP_LAYER]->format
                             , transparent_r_value
                             , transparent_g_value
                             , transparent_b_value));
  
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void fill_statics(map_t* m,ressources_t* r, int nb_rocks){
  int i;
  ASSERT(r);
  ASSERT(m);
  
  SDL_FillRect( m->layers[STATICS_MAP_LAYER],NULL
              , SDL_MapRGB( m->layers[STATICS_MAP_LAYER]->format
                          , transparent_r_value
                          , transparent_g_value
                          , transparent_b_value));
  /* put little objects */
  for(i=0; i< nb_rocks; i++){
      SDL_Rect pos;
      SDL_Rect skin_offset;
      pos.x = rand() % (MAP_WIDTH  - (r->lrocks->w)); 
      pos.y = rand() % (MAP_HEIGHT - (r->lrocks->h/6));  
      skin_offset.x = 0; 
      skin_offset.y = rand() % 6 * (r->lrocks->h/6);  
      skin_offset.w = r->lrocks->w; 
      skin_offset.h = r->lrocks->h/6;
      SDL_BlitSurface(r->lrocks, &skin_offset ,m->layers[STATICS_MAP_LAYER] , &pos );
  }
  
  /* put BIG objects */
  for(i=0; i< nb_rocks; i++){
      SDL_Rect pos;
      SDL_Rect skin_offset;
      pos.x = rand() % (MAP_WIDTH  - (r->brocks->w)); 
      pos.y = rand() % (MAP_HEIGHT - (r->brocks->h/3));  
      skin_offset.x = 0; 
      skin_offset.y = rand() % 3 * (r->brocks->h/3);  
      skin_offset.w = r->brocks->w; 
      skin_offset.h = r->brocks->h/3;
      SDL_BlitSurface(r->brocks, &skin_offset ,m->layers[STATICS_MAP_LAYER] , &pos );
  }

  SDL_SetColorKey(m->layers[STATICS_MAP_LAYER]
                 , SDL_SRCCOLORKEY
                 , SDL_MapRGB( m->layers[STATICS_MAP_LAYER]->format
                             , transparent_r_value
                             , transparent_g_value
                             , transparent_b_value));
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void map_drow_cave(map_t* m,int cx, int cy, int cr){
  drow_circle( m->layers[GROUND_MAP_LAYER],cx,cy,cr
             , transparent_r_value,transparent_g_value,transparent_b_value);
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void map_clean_zone(map_t* m,int zx, int zy, int zw,int zh){
  drow_hline( m->layers[GROUND_MAP_LAYER],zx,zy,zw,zh
            , transparent_r_value,transparent_g_value,transparent_b_value);
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void map_drow_big_cave(map_t* m){
  ASSERT(m);
  int i=0;
  int cx = (rand() % m->layers[GROUND_MAP_LAYER]->w);
  int cy = (rand() % m->layers[GROUND_MAP_LAYER]->h);
  int cr = (rand() % 20) + 18;
  int nb_circles = (rand() % 8) + 3;
  int delta = cr / 2;
  for(i =0 ; i < nb_circles;i++)
    drow_circle(m->layers[GROUND_MAP_LAYER],cx + rand() % 2*delta-delta
                                           ,cy + rand() % delta-delta/2
                                           ,cr
                                           , transparent_r_value
                                           ,transparent_g_value
                                           ,transparent_b_value);
  
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
