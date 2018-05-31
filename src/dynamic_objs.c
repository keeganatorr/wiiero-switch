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


#include <stdio.h>

#include "dynamic_objs.h"
#include "tools.h"
#include "ressources.h"
#include "surface_manip.h"
#include "camera.h"
#include "sound_engine.h"
#include "colors.h"

extern Uint8 selected_amb;
extern Uint8 transparent_r_value;
extern Uint8 transparent_g_value;
extern Uint8 transparent_b_value;


/* ************************************************************************** *
 * Standard collision test callback                                           */
int std_dynamic_collision_cb( SDL_Surface* ground,int pos_x,int pos_y,int acc_x
                            , int acc_y){
  ASSERT(ground);
  Uint8 r,g,b;
  /* if out of ground ... collision */
  if((pos_x>=ground->w) || (pos_x<0) || (pos_y>=ground->h) || (pos_y<0))
    return 1;
  get_pix_color(ground,pos_x,pos_y,&r,&g,&b);
  /* if current position is not transparent ... collision */                      
  if(!(  (r==transparent_r_value)
       &&(g ==transparent_g_value)
       &&(b ==transparent_b_value)))
    return 1;
  return 0;
}


/* ************************************************************************** *
 * Particles collision callback                                               */
void particle_on_collision_cb(void* obj, int lastx, int lasty, int colx,int coly
                             ,void* none,void* none2, void* userdata){
  SDL_Rect particle;
  ASSERT(obj)
  ASSERT(userdata)
  SDL_Surface* ground = (SDL_Surface*) userdata;
  obj_t* o = (obj_t*)obj;
  colx = (colx < 0)? 0 : colx ;
  coly = (coly < 0)? 0 : coly ;
  colx = (colx >= ground->w) ? ground->w-1 : colx ;
  coly = (coly >= ground->h) ? ground->h-1 : coly ;
  particle.x = colx;
  particle.y = coly;
  particle.w = o->w;
  particle.h = o->h;
  SDL_FillRect(ground,&particle,SDL_MapRGB( ground->format
                                          , o->r
                                          , o->g
                                          , o->b));
  o->remove_flag = 1;
}

/* ************************************************************************** *
 * Particles blit callback                                                    */
void particle_blit_cb(camera_t* c,void* obj,void* userdata){
  SDL_Rect particle;
  ASSERT(obj)
  obj_t* o = (obj_t*)obj;
  particle.x = o->pos_x - c->map_x;
  particle.y = o->pos_y - c->map_y;
  particle.w = o->w; 
  particle.h = o->h;
  camera_fillrect(c,&particle,SDL_MapRGB(c->cam_surface->format,o->r,o->g,o->b));
}

/* ************************************************************************** *
 * Create a new particle                                                      */
obj_t* create_particle( int x,int y,int acc_x,int acc_y,Uint8 w
                      , Uint8 h,Uint8 r,Uint8 g,Uint8 b){
  obj_t* o = secure_malloc(sizeof(obj_t));
  o->pos_x = x;
  o->pos_y = y;  
  o->acc_x = acc_x;
  o->acc_y = acc_y;
  o->resist = 2;
  o->gravit=0;
  o->side = NONE_SIDE;
  o->w = w;
  o->h = h;
  o->r = r;
  o->g = g;
  o->b = b;
  o->test_collision_cb = std_dynamic_collision_cb;
  o->on_collision_cb   = particle_on_collision_cb;
  o->blit_cb           = particle_blit_cb;
  return o;
}



/* ************************************************************************** *
 * Create a new blood particle                                                */
obj_t* create_blood_particle(int x,int y,int acc_x,int acc_y){
  Uint8 r,h,w;
  obj_t* blood_particle;
  w = ((rand() % 11)/10) + 1;
  h = ((rand() % 11)/10) + 1;
  r = 126 + (rand() % 130); 
  blood_particle = create_particle(x,y,acc_x,acc_y,w,h,r,0,0);
  blood_particle->blit_cb = particle_blit_cb;
  return blood_particle;
}

obj_t* create_blood_drop(int x,int y){
  return create_blood_particle(x,y,((rand() % 3) - 1),(- (rand() % 2) - 2));
}

/* ************************************************************************** *
 * Mud particle collision callback                                            */
void mud_on_collision_cb( void* obj, int lastx, int lasty, int colx,  int coly
                             ,void* none,void* none2, void* userdata){
  SDL_Rect particle;
  ASSERT(obj)
  ASSERT(userdata)
  SDL_Surface* ground = (SDL_Surface*) userdata;
  obj_t* o = (obj_t*)obj;
  colx = (colx < 0)? 0 : colx ;
  coly = (coly < 0)? 0 : coly ;
  colx = (colx >= ground->w) ? ground->w-1 : colx ;
  coly = (coly >= ground->h) ? ground->h-1 : coly ;
  particle.x = lastx;
  particle.y = lasty;
  particle.w = o->w;
  particle.h = o->h;
  SDL_FillRect(ground,&particle,SDL_MapRGB( ground->format
                                          , o->r
                                          , o->g
                                          , o->b));
  o->remove_flag = 1;
}

/* ************************************************************************** *
 * Create a new Mud particle                                                  */
obj_t* create_mud_particle(int x,int y,int acc_x,int acc_y){
  obj_t* mud_particle;
  int i = rand() % CPALETT;
  Uint8 r,g,b;
  get_grounds_colors(selected_amb,FRONT_GR,i,&r,&g,&b);
  mud_particle = create_particle(x,y,acc_x,acc_y,1,1,r,g,b);
  mud_particle->blit_cb = particle_blit_cb;
  mud_particle->on_collision_cb = mud_on_collision_cb;
  return mud_particle;
}


/* ************************************************************************** *
 * Blood trail blit callback                                                  */
void blood_trail_blit_cb(camera_t* c,void* obj,void* userdata){
  ASSERT(obj)
  obj_t* o = (obj_t*)obj;
  int acc_x,acc_y;
  particle_blit_cb(c,obj,userdata);
  
  acc_x = ( o->acc_x > 0 ) ? o->acc_x-1 : o->acc_x+1;
  acc_y = o->acc_y+1;
  acc_x = ( o->acc_x == 0 ) ? 0 : acc_x;
  /* add trail */
  if(userdata)
    object_add_to_list( (obj_list_t*)userdata
                      , create_blood_particle( o->pos_x
                                             , o->pos_y
                                             , acc_x
                                             , acc_y) ); 
}

/* ************************************************************************** *
 * Create Blood trail                                                         */
obj_t* create_blood_trail(int x,int y){
  Uint8 r,h,w;
  obj_t* bloodp;
  int acc_x,acc_y;
  w = (rand() % 2) + 1;
  h = (rand() % 2) + 1;
  r = 126 + (rand() % 130); 
  acc_x = (rand() % (MAX_ACC_RATIO+1)) - MAX_ACC_RATIO/2;
  acc_y = -(rand()%(MAX_ACC_RATIO/2)); 
  x+=(rand()%9-4);
  y+=(rand()%9-4);
  bloodp = create_particle(x,y,acc_x,acc_y,w,h,r,0,0);
  bloodp->blit_cb = blood_trail_blit_cb;
  return bloodp;
}



/* Flesh source */
enum{
  PLAYER_1,
  PLAYER_2,
  NB_PLAYERS 
}; 

/* ************************************************************************** *
 * Flesh peace collision callback                                             */
void flesh_on_collision_cb( void* obj, int lastx, int lasty, int colx,  int coly
                         ,void* none,void* none2, void* userdata){
  SDL_Rect map_offset;
  SDL_Rect skin_offset;
  ASSERT(obj)
  obj_t* o = (obj_t*)obj;
  o->pos_x = colx;
  o->pos_y = coly;
  skin_offset.x = ( o->skin->w / DEFAULT_PWFLESH_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_PWFLESH_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  map_offset.x = colx;
  map_offset.y = coly;
  SDL_BlitSurface(o->skin, &skin_offset ,(SDL_Surface*) userdata, &map_offset);
  o->remove_flag = 1;
}

/* ************************************************************************** *
 * Blit Flesh peace                                                           */
void flesh_blit_cb(camera_t* c,void* obj,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(obj)
  ASSERT(c)
  ASSERT(userdata)
  obj_t* o = (obj_t*)obj;
  int acc_x,acc_y;                                  
  skin_offset.x = ( o->skin->w / DEFAULT_PWFLESH_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_PWFLESH_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  o->animation_pos = (o->animation_pos+1)% DEFAULT_PWFLESH_ANIM_STEPS;
  acc_x = ( o->acc_x > 0 ) ? o->acc_x-1 : o->acc_x+1;
  acc_y = o->acc_y+1;
  acc_x = ( o->acc_x == 0 ) ? 0 : acc_x;
  /* add blood trail */
  if(userdata)
    object_add_to_list( (obj_list_t*)userdata
                      , create_blood_particle( o->pos_x , o->pos_y
                                             , acc_x, acc_y) ); 
}

/* ************************************************************************** *
 * Create Flesh peace                                                         */
obj_t* create_flesh(int x,int y,ressources_t* r,int pl_id){
  obj_t* o = secure_malloc(sizeof(obj_t));
  o->pos_x = x + rand()%13 - 6;
  o->pos_y = y - rand()%5;  
  o->acc_x = (rand() % (MAX_ACC_RATIO+1)) - MAX_ACC_RATIO/2;
  o->acc_y = -2 -(rand()%(MAX_ACC_RATIO/2)); 
  o->resist = 3;
  o->gravit=0;
  o->side = NONE_SIDE;
  o->test_collision_cb = std_dynamic_collision_cb;
  o->on_collision_cb   = flesh_on_collision_cb;
  o->blit_cb           = flesh_blit_cb;
  o->skin = r->worms1_flesh;
  if(pl_id == PLAYER_2)
   o->skin = r->worms2_flesh; 
  o->animation_pos = 0;
  return o;
}


/* ************************************************************************** *
 * Ammo shell collision callback                                              */
void shell_on_collision_cb( void* obj, int lastx, int lasty, int colx,  int coly
                          ,void* none, void* none2, void* userdata){
  obj_t* o = (obj_t*)obj;  
  ASSERT(obj);
  if(o->acc_y<=1 && o->acc_y>=-1){
    SDL_Rect map_offset;
    SDL_Rect skin_offset;
    o->pos_x = colx;
    o->pos_y = coly;
    skin_offset.x = ( o->skin->w / DEFAULT_SHELL_ANIM_STEPS) * o->animation_pos; 
    skin_offset.y = 0;  
    skin_offset.w = o->skin->w / DEFAULT_SHELL_ANIM_STEPS; 
    skin_offset.h = o->skin->h;
    map_offset.x = colx;
    map_offset.y = coly;
    SDL_BlitSurface(o->skin, &skin_offset ,(SDL_Surface*) userdata, &map_offset);
    o->remove_flag = 1;
  }else{
    if(( lastx == o->pos_x) && (lasty == o->pos_y) )
      o->acc_x = -(9*o->acc_x/10);
    else 
      o->acc_x = (9*o->acc_x/10);
    o->acc_y = -(o->acc_y/2);
  } 
  if(rand()%2==0)
    sengine_play(WIIERO_SOUND_SHELL,SENGINE_PLAY_ACTION);  
}

/* ************************************************************************** *
 * Blit ammo shell                                                            */
void shell_blit_cb(camera_t* c,void* obj,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(obj)
  ASSERT(c)
  ASSERT(userdata)
  obj_t* o = (obj_t*)obj;
  skin_offset.x = ( o->skin->w / DEFAULT_SHELL_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_SHELL_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  o->animation_pos = (o->animation_pos+1)% DEFAULT_SHELL_ANIM_STEPS;
}

/* ************************************************************************** *
 * Create ammo shell                                                          */
obj_t* create_shell(int x,int y,ressources_t* r){
  obj_t* o = secure_malloc(sizeof(obj_t));
  o->pos_x = x + (rand() % 3) - 1;
  o->pos_y = y-3 + (rand() % 3) - 1;  
  o->acc_x = (rand() % 5) - 2;
  o->acc_y = - (rand() % 2) - 2;
  o->resist = 3;
  o->gravit=0;
  o->side = NONE_SIDE;
  o->test_collision_cb = std_dynamic_collision_cb;
  o->on_collision_cb   = shell_on_collision_cb;
  o->blit_cb           = shell_blit_cb;
  o->skin = r->shell;
  o->animation_pos = 0;
  return o;
}

/* ************************************************************************** *
 * Explosion blit callback                                                    */
void explosion_blit_cb(camera_t* c,void* obj,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(obj)
  ASSERT(c)
  ASSERT(userdata)
  obj_t* o = (obj_t*)obj;
  skin_offset.x = ( o->skin->w / DEFAULT_EXPLOSION_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_EXPLOSION_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
  if( o->animation_pos >= 7 ) camera_set_shake_frames(c,o->animation_pos);
}

/* ************************************************************************** *
 * Explosion update callback                                                  */
void explosion_update(void* obj,void *p1,void *p2, void* userdata){
  ASSERT(obj)
  obj_t* o = (obj_t*)obj;
  o->animation_pos = (o->animation_pos-1);
  if(o->animation_pos == 0){ 
    o->remove_flag = 1;
    return;
  }
}

/* ************************************************************************** *
 * Create explosion                                                           */
obj_t* create_explosion(int x,int y,ressources_t* r,int size){
  obj_t* o = secure_malloc(sizeof(obj_t));
  o->pos_x = x;
  o->pos_y = y;
  o->acc_x = 0;
  o->acc_y = 0;
  o->resist = 0;
  o->gravit=0;
  o->side = NONE_SIDE;
  o->test_collision_cb = 0L;
  o->on_collision_cb   = 0L;
  o->blit_cb           = explosion_blit_cb;
  o->update_cb         = explosion_update;
  o->skin = r->explosion;
  o->animation_pos = size;
  return o;
}

/* ************************************************************************** *
 * Smoke blit callback                                                        */
void smoke_blit_cb(camera_t* c,void* obj,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(obj)
  ASSERT(c)
  ASSERT(userdata)
  obj_t* o = (obj_t*)obj;
  skin_offset.x = ( o->skin->w / DEFAULT_SMOKE_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;
  skin_offset.w = o->skin->w / DEFAULT_SMOKE_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
}

/* ************************************************************************** *
 * Smoke update callback                                                      */
void smoke_update(void* obj,void *p1,void *p2, void* userdata){
  ASSERT(obj)
  obj_t* o = (obj_t*)obj;  
  o->animation_pos += 1;
  if(o->animation_pos >= DEFAULT_SMOKE_ANIM_STEPS ){ 
    o->remove_flag = 1;
  }
}
/* ************************************************************************** *
 * Create smoke                                                               */
obj_t* create_smoke(int x,int y,ressources_t* r){
  obj_t* o = secure_malloc(sizeof(obj_t));
  o->pos_x = x;
  o->pos_y = y;  
  o->acc_x = 0;
  o->acc_y = 0;
  o->resist = 0;
  o->gravit=0;
  o->side = NONE_SIDE;
  o->test_collision_cb = 0L;
  o->on_collision_cb   = 0L;
  o->blit_cb           = smoke_blit_cb;
  o->update_cb         = smoke_update;
  o->skin = r->smoke;
  o->animation_pos = 0;
  return o;
}


/* ************************************************************************** *
 * Rocket reactor particle update callback                                    */
void reactor_particle_update(void* obj,void *p1,void *p2, void* userdata){
  ASSERT(obj)
  obj_t* o = (obj_t*)obj;  
  o->animation_pos += 1;
  if(o->animation_pos >= DEFAULT_REACTOR_PARTICLE_STEPS ){ 
    o->remove_flag = 1;
    return;
  }
  o->r -=  3;
  o->g -= 10;
  o->b -=  4; 
}

/* ************************************************************************** *
 * Create rocket reactor particle                                             */
obj_t* create_reactor_particle(int x,int y){
  obj_t* o = secure_malloc(sizeof(obj_t));
  o->pos_x = x;
  o->pos_y = y;  
  o->acc_x = 0;
  o->acc_y = 0;
  o->resist = 0;
  o->gravit=0;
  o->remove_flag = 0;
  o->test_collision_cb = 0L;
  o->on_collision_cb   = 0L;
  o->blit_cb           = particle_blit_cb;
  o->update_cb         = reactor_particle_update;
  o->animation_pos = 0;
  o->side = NONE_SIDE;
  o->h = 1;
  o->w = 1;
  o->r = 0xF4;
  o->g = 0xE8;
  o->b = 0x3C;
  return o;
}

/* ************************************************************************** *
 * Nuke smoke blit callback                                                   */
void nuke_smoke_blit_cb(camera_t* c,void* obj,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(obj)
  ASSERT(c)
  ASSERT(userdata)
  obj_t* o = (obj_t*)obj;
  skin_offset.x = ( o->skin->w / DEFAULT_NUKE_SMOKE_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_NUKE_SMOKE_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
}

/* ************************************************************************** *
 * Nuke smoke update callback                                                 */
void nuke_smoke_update(void* obj,void *p1,void *p2, void* userdata){
  ASSERT(obj)
  obj_t* o = (obj_t*)obj;  
  o->animation_pos += 1;
  if(o->animation_pos >= DEFAULT_NUKE_SMOKE_ANIM_STEPS ){ 
    o->remove_flag = 1;
  }
}

/* ************************************************************************** *
 * Create nuke smoke                                                          */
obj_t* create_nuke_smoke(int x,int y,ressources_t* r){
  obj_t* o = secure_malloc(sizeof(obj_t));
  o->pos_x = x;
  o->pos_y = y;  
  o->acc_x = 0;
  o->acc_y = 0;
  o->resist = 0;
  o->gravit=0;
  o->side = NONE_SIDE;
  o->test_collision_cb = 0L;
  o->on_collision_cb   = 0L;
  o->blit_cb           = nuke_smoke_blit_cb;
  o->update_cb         = nuke_smoke_update;
  o->skin = r->nuke_smoke;
  o->animation_pos = 0;
  return o;
}

/* ************************************************************************** *
 * Flash blit callback                                                        */
void flash_blit_cb(camera_t* c,void* obj,void* userdata){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(obj)
  ASSERT(c)
  ASSERT(userdata)
  obj_t* o = (obj_t*)obj;
  skin_offset.x = ( o->skin->w / DEFAULT_FLASH_ANIM_STEPS) * o->animation_pos; 
  skin_offset.y = 0;  
  skin_offset.w = o->skin->w / DEFAULT_FLASH_ANIM_STEPS; 
  skin_offset.h = o->skin->h;
  /* compute object position into the camera */
  camera_offset.x = o->pos_x - c->map_x - skin_offset.w / 2;
  camera_offset.y = o->pos_y - c->map_y - skin_offset.h / 2 + 1;
  //SDL_BlitSurface(o->skin, &skin_offset ,c->cam_surface, &camera_offset); 
  camera_blit_surface_on(c,o->skin,&skin_offset,&camera_offset);
}

/* ************************************************************************** *
 * Flash update callback                                                      */
void flash_update(void* obj,void *p1,void *p2, void* userdata){
  ASSERT(obj)
  obj_t* o = (obj_t*)obj;  
  o->animation_pos += 1;
  if(o->animation_pos >= DEFAULT_FLASH_ANIM_STEPS ){ 
    o->remove_flag = 1;
  }
}

/* ************************************************************************** *
 * Create flash callback                                                      */
obj_t* create_flash(int x,int y,ressources_t* r){
  obj_t* o = secure_malloc(sizeof(obj_t));
  o->pos_x = x;
  o->pos_y = y;  
  o->acc_x = 0;
  o->acc_y = 0;
  o->resist = 0;
  o->gravit=0;
  o->side = NONE_SIDE;
  o->test_collision_cb = 0L;
  o->on_collision_cb   = 0L;
  o->blit_cb           = flash_blit_cb;
  o->update_cb         = flash_update;
  o->skin = r->zimm_flash;
  o->animation_pos = 0;
  return o;
}


