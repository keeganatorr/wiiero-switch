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


 #include "object.h"
 #include "surface_manip.h"
 
extern int debug_flag;

int check_all_collisions( obj_t* obj,SDL_Surface* ground,SDL_Surface* statics
                        , void* userdata){
  ASSERT(obj);
  ASSERT(ground);
  ASSERT(statics);
  int coll=0;
  int i = 0;
  int last_x = obj->pos_x;
  int last_y = obj->pos_y;
  if(obj->acc_x || obj->acc_y){
    while(i <= MOVE_DECOMPO){
      int x,y;
      x = obj->pos_x + ((i * obj->acc_x) / MOVE_DECOMPO);
      y = obj->pos_y + ((i * obj->acc_y) / MOVE_DECOMPO);
      if((x!=last_x) || (y!=last_y)){
        /* Check collisions */
        DBG("test %d (%d) %d (%d)\n",x,((i * obj->acc_x) / MOVE_DECOMPO)
                                      ,y,((i * obj->acc_y) / MOVE_DECOMPO));
        coll =  obj->test_collision_cb(ground, x , y,obj->acc_x,obj->acc_y); 
        coll += obj->test_collision_cb(statics, x , y,obj->acc_x,obj->acc_y);
        if(coll!=0){ 
           DBG("pos_x %d , pos_y %d\n",obj->pos_x, obj->pos_y);
           DBG("acc_x %d , acc_y %d\n",obj->acc_x, obj->acc_y);
           DBG("collision : %dx%d\n",x,y);
           if(obj->on_collision_cb)
              obj->on_collision_cb(obj,last_x,last_y,x,y,0L,0L,userdata);
           return coll;
        }
        last_x = x;
        last_y = y;
      }
      i++;
    }
  }
  DBG("%d-%d\n",last_x,last_y);
  obj->pos_x = last_x;
  obj->pos_y = last_y;
  return coll;
}

void apply_physics(obj_t* obj,SDL_Surface* ground,SDL_Surface* statics,void* userdata){
  ASSERT(obj);
  ASSERT(ground);
  ASSERT(statics);
  DBG(" - APPLY GRAV\n");
  apply_gravity(obj);
  DBG(" - CHECK ACC\n");
  check_acceleration(obj);
  DBG(" - CHECK COLL\n");
  check_all_collisions(obj,ground,statics,userdata);
}


obj_list_t* object_init_list(){
  return secure_malloc(sizeof(obj_list_t));
}

void object_add_to_list(obj_list_t* l,obj_t* obj){
  ASSERT(l);
  obj_cell_t* cell = 0l;
 
  if(l->len >= MAX_SUPPORTED_DOBJS){
    obj->remove_flag = 1;
  }
 
  cell = secure_malloc(sizeof(obj_cell_t));
  cell->object = obj;
  
  if(l->head)
    cell->next = l->head;
  else
    cell->next = 0L;
  
  l->head = cell;
  l->len++;
}

void proceed_dynamics_objs(obj_list_t* l,SDL_Surface* ground,SDL_Surface* statics, void* coll_cb_userdata){
  ASSERT(l);
  ASSERT(ground);
  ASSERT(statics);
  obj_cell_t* last_cell = 0l;
  obj_cell_t* cell_to_remove = 0l;
  obj_cell_t* cell = l->head;
  if(!cell)
    return;
  while(cell){
    if(cell->object->remove_flag){
      secure_free(cell->object);
      cell->object = 0L;
      /* update head */
      if(l->head == cell)
        l->head = cell->next;
      if(last_cell)
        last_cell->next = cell->next;
      cell_to_remove = cell;
      cell = cell->next;
      secure_free(cell_to_remove);
      l->len--;
    }else{
      apply_physics(cell->object,ground,statics,coll_cb_userdata);
      last_cell=cell;
      cell = cell->next;
    }
  }
}


void obj_pix(obj_t* obj,SDL_Surface* ground,int offset_x,int offset_y){
  put_pix_color(ground,obj->pos_x-offset_x,obj->pos_y-offset_y,0xFF,0,0);
}

extern void clean_dynamic_obj_list(obj_list_t* l){
  ASSERT(l);
  obj_cell_t* cell = l->head; 
  obj_cell_t* cell_to_remove = 0l;
  while(cell){
    if(cell->object){
      secure_free(cell->object);
      cell->object = 0L;
      cell_to_remove = cell;
      cell = cell->next;
      secure_free(cell_to_remove);
      l->len--;
    }
  }
  l->head = 0l;
}

void blit_dynamics_objs(obj_list_t* l,camera_t* cam1,camera_t* cam2){
  ASSERT(l);
  ASSERT(cam1);
  ASSERT(cam2);
  obj_cell_t* cell = l->head;
  if(!cell)
    return;
  while(cell){
    if(cell->object->blit_cb){
      if((cell->object->pos_x >= cam1->map_x)
       &&(cell->object->pos_x < (cam1->map_x + cam1->w))
       &&(cell->object->pos_y >= cam1->map_y)
       &&(cell->object->pos_y < (cam1->map_y + cam1->h)))
          cell->object->blit_cb(cam1,cell->object,l);

      if((cell->object->pos_x >= cam2->map_x)
       &&(cell->object->pos_x < (cam2->map_x + cam2->w))
       &&(cell->object->pos_y >= cam2->map_y)
       &&(cell->object->pos_y < (cam2->map_y + cam2->h)))
        cell->object->blit_cb(cam2,cell->object,l);
    }
    if(cell->object->update_cb)
      cell->object->update_cb(cell->object,0L,0L,0L);
    cell = cell->next;
  }
}
