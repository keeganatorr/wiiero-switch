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

#ifndef OBJECT_H
#define OBJECT_H

#include "common.h"
#include <SDL/SDL.h>
#include "camera.h"
#include "tools.h"

#define MOVE_PITCH 2
#define ANGLE_PITCH 1
#define MAX_ANGLE_PITCH 12

#define MOVE_DECOMPO 30
#define MAX_ACC_RATIO 6

#define MAX_OBJ_ANGLE 90
#define MIN_OBJ_ANGLE -90


#define GRAVITY_ACC_REDUCTION 0.1

#define MAX_SUPPORTED_DOBJS 2000


typedef enum{
    NONE_SIDE          = -1 ,
    RIGHT_SIDE         = 0,
    LEFT_SIDE          = 1,
    MAX_SIDE_POSITIONS = 2
}side_t;

typedef void (*on_coll_callback)( void* obj
                                , int lastx, int lasty
                                , int colx,  int coly
                                , void* p1,void* p2
                                , void* userdata);

typedef int (*test_coll_callback)(SDL_Surface* ground,int pos_x,int pos_y,int acc_x,int acc_y);

typedef void (*blit_callback)(camera_t* c,void* obj,void* userdata);

typedef void (*update_callback)(void* obj,void *p1,void *p2, void* userdata);

enum{
    ANGLE_DOWN_FULL,
    ANGLE_DOWN_3_4,
    ANGLE_DOWN_2_4,
    ANGLE_DOWN_1_4,
    ANGLE_MIDDLE,
    ANGLE_UP_1_4, 
    ANGLE_UP_2_4,
    ANGLE_UP_3_4,
    ANGLE_UP_FULL,    
    MAX_ANGLES
};


typedef struct{
  int                 pos_x;             // X position
  int                 pos_y;             // Y position
  int                 acc_x;             // X acceleration
  int                 acc_y;             // Y acceleration
  int                 resist;            // object resistency
  float               gravit;            //
  int                 angle;             // angle
  side_t              side;              // side 
  test_coll_callback  test_collision_cb; // collision test callback
  on_coll_callback    on_collision_cb;   // on collision callback
  blit_callback       blit_cb;           // blit callback
  update_callback     update_cb;         // update callback
  Uint8               animation_pos;     // animation posision
  Uint8               animation_sleep;   // animation posision
  union{
    struct{  
      SDL_Surface*      skin;            // Object skin
      };       
    struct{                              // Or particle info 
      Uint8           w;                 //  - Width           
      Uint8           h;                 //  - Height          
      Uint8           r;                 //  - Blue 
      Uint8           g;                 //  - Green          
      Uint8           b;                 //  - Blue           
    };
  }; 
  Uint8               remove_flag;
}obj_t;


typedef struct obj_cell_s{
  obj_t* object;
  struct obj_cell_s *next;
}obj_cell_t;

typedef struct{
  obj_cell_t *head;
  int         len;
}obj_list_t;


extern void obj_pix(obj_t* obj,SDL_Surface* ground,int offset_x,int offset_y);
extern void apply_physics( obj_t* obj,SDL_Surface* ground,SDL_Surface* statics
                         , void* coll_cb_userdata);
static void apply_gravity(obj_t* obj);
static void check_acceleration(obj_t* obj);
extern obj_list_t* object_init_list();
extern void object_add_to_list(obj_list_t* l,obj_t* obj);
extern void proceed_dynamics_objs(obj_list_t* l,SDL_Surface* ground,SDL_Surface* statics
                  , void* coll_cb_userdata);

extern void blit_dynamics_objs(obj_list_t* l,camera_t* cam1,camera_t* cam2);

extern void clean_dynamic_obj_list(obj_list_t* l);


/* * * * * * * * * * * * * * INLINE CODE * * * * * * * * * * * * * * * * * * * */
static __inline__ void apply_gravity(obj_t* obj){
  ASSERT(obj);
  obj->gravit = (obj->gravit < 1 ) 
                ? obj->gravit + (GRAVITY_ACC_REDUCTION*obj->resist)
                : 1;
  if(obj->gravit == 1){
     obj->acc_y++;   
     obj->gravit = 0; 
  }   
}

static __inline__ void check_acceleration(obj_t* obj){
   /* check acceleration */ 
  ASSERT(obj);
  obj->acc_x = (obj->acc_x > MAX_ACC_RATIO*MOVE_PITCH) 
                ? MAX_ACC_RATIO*MOVE_PITCH 
                : obj->acc_x; 
  obj->acc_x = (obj->acc_x < -MAX_ACC_RATIO*MOVE_PITCH) 
                ? -MAX_ACC_RATIO*MOVE_PITCH 
                : obj->acc_x;  
  obj->acc_y = (obj->acc_y > MAX_ACC_RATIO*MOVE_PITCH) 
                ? MAX_ACC_RATIO*MOVE_PITCH 
                : obj->acc_y; 
  obj->acc_y = (obj->acc_y < -MAX_ACC_RATIO*MOVE_PITCH) 
                ? -MAX_ACC_RATIO*MOVE_PITCH 
                : obj->acc_y;  
}

#endif
