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

#include "player.h"
#include "tools.h"
#include "map.h"
#include "fast_trig.h"
#include "surface_manip.h"
#include "dynamic_objs.h"
#include "weapons.h"
#include "font.h"
#include "sound_engine.h"
#include "lang.h"

extern int debug_flag;

enum{
    COLL_UP,
    COLL_MIDDY,
    COLL_DOWN,
    COLL_HEIGHT
};
enum{
    COLL_LEFT,
    COLL_MIDDX,
    COLL_RIGHT,
    COLL_WIDTH
};

extern int bullet_time_effect_delay;
extern player_score_t game_score[NB_PLAYERS];
extern Uint8 transparent_r_value;
extern Uint8 transparent_g_value;
extern Uint8 transparent_b_value;

static int coll_matrix[COLL_WIDTH][COLL_HEIGHT];
void player_show_gunfire_on_cam(player_t* p,camera_t* camera,int); 

int worms_check_collision_cb(SDL_Surface* ground,int pos_x,int pos_y,int acc_x,int acc_y){
  ASSERT(ground);
  Uint8 r,g,b;
  int x,y;
  int coll=0;
  
  x=COLL_MIDDX;y=COLL_MIDDY;
  get_pix_color(ground,pos_x+(x-1),pos_y+(y-1),&r,&g,&b);
  if(r!=transparent_r_value || g!=transparent_g_value || b!=transparent_b_value ){
    coll=1;
    coll_matrix[x][y] = 1;
  }
  
  if(acc_y<0){
    /* check Up border*/
    if(pos_y < 0){
      coll=1;
      coll_matrix[COLL_LEFT][COLL_UP] = 1;
      coll_matrix[COLL_MIDDX][COLL_UP]= 1;
      coll_matrix[COLL_RIGHT][COLL_UP]= 1;
    }else{
      /* check Up collision*/
      x=COLL_MIDDX;y=COLL_UP;
      get_pix_color(ground,pos_x+(x-1),pos_y+(y-1),&r,&g,&b);
      if(r!=transparent_r_value || g!=transparent_g_value || b!=transparent_b_value ){
        coll=1;
        coll_matrix[x][y] = 1;
      }
    }
  }else{
    /* check Down borders*/
    if(pos_y > ground->h){
      coll=1;
      coll_matrix[COLL_RIGHT][COLL_DOWN]= 1;
      coll_matrix[COLL_MIDDX][COLL_DOWN]= 1;
      coll_matrix[COLL_RIGHT][COLL_DOWN]= 1;
    }else{
      /* check down collision*/
      x=COLL_MIDDX;y=COLL_DOWN;
      get_pix_color(ground,pos_x+(x-1),pos_y+(y-1),&r,&g,&b);
      if(r!=transparent_r_value || g!=transparent_g_value || b!=transparent_b_value ){
        coll=1;
        coll_matrix[x][y] = 1;
      }
    }
  }

  /* if no X acceleration ... exit */
  if(acc_x == 0)
    return coll;

  if(acc_x > 0){
    /* check right borders*/
    if(pos_x > ground->w){
      coll=1;
      coll_matrix[COLL_RIGHT][COLL_UP]   = 1;
      coll_matrix[COLL_RIGHT][COLL_MIDDY]= 1;
      coll_matrix[COLL_RIGHT][COLL_DOWN] = 1;
    }else{
      /* check right/up collision */
      x=COLL_RIGHT;y=COLL_UP;
      get_pix_color(ground,pos_x+(x-1),pos_y+(y-1),&r,&g,&b);
      if(r!=transparent_r_value || g!=transparent_g_value || b!=transparent_b_value ){
        coll=1;
        coll_matrix[x][y] = 1;
      }
      /* check right/middle collision */
      x=COLL_RIGHT;y=COLL_MIDDY;
      get_pix_color(ground,pos_x+(x-1),pos_y+(y-1),&r,&g,&b);
      if(r!=transparent_r_value || g!=transparent_g_value || b!=transparent_b_value ){
        coll=1;
        coll_matrix[x][y] = 1;
      }
    }
  }else{    
    /* check left borders*/
    if(pos_x < 0){
      coll=1;
      coll_matrix[COLL_LEFT][COLL_UP]   = 1;
      coll_matrix[COLL_LEFT][COLL_MIDDY]= 1;
      coll_matrix[COLL_LEFT][COLL_DOWN] = 1;
    }else{
      /* check left/up collision */
      x=COLL_LEFT;y=COLL_UP;
      get_pix_color(ground,pos_x+(x-1),pos_y+(y-1),&r,&g,&b);
      if(r!=transparent_r_value || g!=transparent_g_value || b!=transparent_b_value ){
        coll=1;
        coll_matrix[x][y] = 1;
      }
      /* check left/middle collision */
      x=COLL_LEFT;y=COLL_MIDDY;
      get_pix_color(ground,pos_x+(x-1),pos_y+(y-1),&r,&g,&b);
      if(r!=transparent_r_value || g!=transparent_g_value || b!=transparent_b_value ){
        coll=1;
        coll_matrix[x][y] = 1;
      }
    }
  }

  return coll;
}



void worms_on_collision_cb( void* o , int lastx, int lasty
                          , int colx,  int coly
                          , void* null, void* null2, void* userdata){
  obj_t* obj = (obj_t*)o;
  player_t* player = (player_t*)userdata;

  if(obj->acc_y==0){
    obj->pos_y = lasty;
  }else{
    if(obj->acc_y<0){ /* UP */
      if(coll_matrix[COLL_MIDDX][COLL_UP]||coll_matrix[COLL_MIDDX][COLL_MIDDY])
        obj->pos_y = lasty;
      else
        obj->pos_y = coly;
    }else{ /* DOWN */
      if(coll_matrix[COLL_MIDDX][COLL_DOWN]||coll_matrix[COLL_MIDDX][COLL_MIDDY]){
        obj->pos_y = lasty;
        player->worms_status |= STATUS_JUMPABLE;
      }else{
        obj->pos_y = coly;
      }
    }
    obj->acc_y = 0;
  }   

  if(obj->acc_x==0){
    obj->pos_x = lastx;
  }else{
    //printf("%d %d %d\n",obj->pos_x ,lastx, colx);
    if(obj->acc_x<0){ /* Left */
      if(!coll_matrix[COLL_LEFT][COLL_UP]){
        if(!coll_matrix[COLL_LEFT][COLL_MIDDY]){
          obj->pos_x = colx;
        }else{
          obj->pos_x = colx;
          obj->pos_y -=1;
        }
      } 
    }else{ /* Right */
      if(!coll_matrix[COLL_RIGHT][COLL_UP]){
        if(!coll_matrix[COLL_RIGHT][COLL_MIDDY]){
          obj->pos_x = colx;
        }else{
          obj->pos_x = colx;
          obj->pos_y -=1;
        }
      } 
    }
    obj->acc_x = obj->acc_x / 2 ;
  }
  DBG("final %dx%d\n",obj->pos_x,obj->pos_y);
}


void player_reset( player_t* p, SDL_Surface* ground
                 , SDL_Surface* statics){
  ASSERT(p);
  ASSERT(ground);
  ASSERT(statics);
 /* get_empty_layer_position(&(p->worms.pos_x),&(p->worms.pos_y),statics);
  drow_circle( ground,p->worms.pos_x,p->worms.pos_y,8
             , transparent_r_value,transparent_g_value,transparent_b_value);*/
  p->worms.acc_x = DEFAULT_PLAYER_XACC;
  p->worms.acc_y = DEFAULT_PLAYER_YACC;
  p->worms.resist = DEFAULT_PLAYER_RESIST;
  p->worms_action = DEFAULT_PLAYER_ACTIONS;
  p->worms.animation_pos = DEFAULT_PLAYER_ANIM_POS;
  p->worms.animation_sleep = DEFAULT_PLAYER_ANIM_SLEEP;
  p->worms.angle = DEFAULT_PLAYER_ANGLE;
  p->worms_health = DEFAULT_PLAYER_HEALTH;
  p->worms.remove_flag = 0;
  p->worms_status |= STATUS_ALIVE ;
  p->worms_status &= ~STATUS_HAVE_FLAG ;
  p->worms_status &= ~STATUS_FREEZED ;
  p->worms_status &= ~STATUS_JUMPABLE ;
  p->worms_status &= ~STATUS_EXPLODE ;
  p->worms_status &= ~STATUS_FIREING ;
  p->worms_status &= ~STATUS_SHOT ;
  p->worms_status |= STATUS_STATS_UPDATE;
  p->worms_status &= ~STATUS_AIMING ;
  p->worms_status &= ~STATUS_CROPING ;
  p->worms_status &= ~STATUS_CHANGING_W ;
  p->worms_status &= ~STATUS_TRICKED ;
  p->worms_status &= ~STATUS_NINJA_ACTION ;
  p->worms_status &= ~STATUS_SHOW_W ;
  p->worms_status &= ~STATUS_SUICIDE;
  p->worms_status |= STATUS_RESETED;
  p->worms_status &= ~(STATUS_FREEZED | STATUS_UNFOCUSED);
                     
  p->reticle_pitch = ANGLE_PITCH;
  p->last_stats_update =0;
  /*p->worms_shockwave = 0;*/
  p->worms.skin = (p->id == PLAYER_1) ? p->r->worms1_skin : p->r->worms2_skin ;
}

void player_init_weapons(player_t* p,int xtra_on){
  ASSERT(p)
  int i=0;
  for(i=0;i<WEAPON_SLOTS;i++){
    if(xtra_on) p->weapon_slots[i] =  weapon_init(rand()%MAX_WEAPONS_AND_XTRA);
    else p->weapon_slots[i] =  weapon_init(rand()%MAX_WEAPONS);
  }
}

void player_reset_weapons(player_t* p,int xtra_on){
  ASSERT(p)
  int i=0;
  for(i=0;i<WEAPON_SLOTS;i++){
    secure_free(p->weapon_slots[i]); 
    if(xtra_on) p->weapon_slots[i] =  weapon_init(rand()%MAX_WEAPONS_AND_XTRA);
    else p->weapon_slots[i] =  weapon_init(rand()%MAX_WEAPONS);
  }
}

player_t* player_init( player_id id , camera_t* c,camera_t* sc,ressources_t* r
                     , bullet_list_t* bl, obj_list_t* dl,SDL_Surface* statics
                     , int xtra_weap){
  player_t* p = 0L;
  Uint8 cr=0x0;
  Uint8 cg=0x0;
  Uint8 cb=0x0;
  DBG("- init player\n");
  ASSERT(c);
  ASSERT(sc);
  ASSERT(c);
  ASSERT(bl); 
  ASSERT(dl);
  p = secure_malloc(sizeof(player_t));
  p->r = r;
  p->id = id;
  p->ninja_hook = weapon_init(WEAPON_NINJA);
  while(cr!=transparent_r_value || cg!=transparent_g_value || cb!=transparent_b_value ){
    p->worms.pos_x = (id == PLAYER_1) 
                   ? rand() % (MAP_WIDTH / 2) /* first map part */
                   :(MAP_WIDTH / 2) + rand() % (MAP_WIDTH / 2); /* second map part */
    p->worms.pos_y = rand() % MAP_HEIGHT; 
    get_pix_color(statics,p->worms.pos_x,p->worms.pos_y,&cr,&cg,&cb);
  }
  
  p->worms.acc_x = DEFAULT_PLAYER_XACC;
  p->worms.acc_y = DEFAULT_PLAYER_YACC;
  p->worms.resist = DEFAULT_PLAYER_RESIST;
  p->worms_status = DEFAULT_PLAYER_STATUS;
  p->worms_action = DEFAULT_PLAYER_ACTIONS;
  p->worms.animation_pos = DEFAULT_PLAYER_ANIM_POS;
  p->worms.angle = DEFAULT_PLAYER_ANGLE;
  p->worms_health = DEFAULT_PLAYER_HEALTH;
  p->worms.animation_sleep = DEFAULT_PLAYER_ANIM_SLEEP;
  p->worms.remove_flag = 0;
  p->worms.blit_cb = 0L;
  p->last_stats_update =0;
  p->worms.test_collision_cb = worms_check_collision_cb;
  p->worms.on_collision_cb = worms_on_collision_cb;
  p->worms.side = (id == PLAYER_1) ? RIGHT_SIDE : LEFT_SIDE ;
  p->worms_status |= STATUS_ALIVE ;
  p->worms_camera = c;
  p->worms_stats_camera = sc;
  player_init_weapons(p,xtra_weap);
  p->selected_weapon=0;
  p->bullet_list_link = bl;
  p->dynamic_list_link = dl;
  p->reticle_pitch = ANGLE_PITCH;
  /*p->worms_shockwave = 0;*/
  p->worms.skin = (id == PLAYER_1) ? r->worms1_skin : r->worms2_skin ;
  player_focus(p);
  p->worms_status |= STATUS_STATS_UPDATE;
  return p;
}

void player_pix(player_t* p){
 obj_pix( &(p->worms),p->worms_camera->cam_surface
        , p->worms_camera->map_x,p->worms_camera->map_y);
}

void player_is_aiming(player_t* p , player_t* target){
  p->worms_status &= ~STATUS_AIMING;

  float dist = fast_sqrt( fast_sqr( (p->worms.pos_x-WEAPON_POS_X)
                                         - target->worms.pos_x)
                               + fast_sqr( (p->worms.pos_y-WEAPON_POS_Y)
                                         - (target->worms.pos_y-2)));
  
  int y =0;
  int x = (p->worms.side == LEFT_SIDE) 
          ? (p->worms.pos_x-WEAPON_POS_X) - dist * ftrigo_cos(p->worms.angle)
          : (p->worms.pos_x-WEAPON_POS_X) + dist * ftrigo_cos(p->worms.angle);
  
  if( fast_abs(x - target->worms.pos_x) <= (10+dist/100) ){
    y = (p->worms.pos_y-WEAPON_POS_Y) - dist * ftrigo_sin(p->worms.angle);
    if( fast_abs(y - (target->worms.pos_y-2)) <= (10+dist/50) ){
      p->worms_status |= STATUS_AIMING;
    }
  }
}


void player_show(player_t* p,int warding_flag){
 ASSERT(p);
 player_show_on_cam(p,p->worms_camera,warding_flag);
}

void player_show_on_cam(player_t* p,camera_t* camera,int warding_flag){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  int looking_height=0;
  ASSERT(p);
  ASSERT(camera);

  if(!(p->worms_status & STATUS_ALIVE)){
    return;
  }

  /* WORMS PART */
  /* compute sprite to show */
  skin_offset.x = 0;
  skin_offset.y = 0;
  skin_offset.w = p->worms.skin->w / (MAX_SIDE_POSITIONS *  DEFAULT_PWSKIN_ANIM_STEPS); 
  skin_offset.h = p->worms.skin->h / MAX_LOOKING_HEIGHTS;  
  skin_offset.x = ( (p->worms.animation_pos) * skin_offset.w) 
                   + (( p->worms.side == RIGHT_SIDE) 
                        ? 0
                        : skin_offset.w * DEFAULT_PWSKIN_ANIM_STEPS);

  if(p->worms.angle <= MAX_PLAYER_ANGLE && p->worms.angle > 70)  
      looking_height = LOOKING_UP_FULL;
  if(p->worms.angle <= 70 && p->worms.angle > 50)  
      looking_height = LOOKING_UP_3_4;
  if(p->worms.angle <= 50 && p->worms.angle > 30)  
      looking_height = LOOKING_UP_2_4;
  if(p->worms.angle <= 30 && p->worms.angle > 10)  
      looking_height = LOOKING_UP_1_4; 
  if(p->worms.angle <= 10 && p->worms.angle > -10)
      looking_height = LOOKING_MIDDLE;
  if(p->worms.angle <= -10 && p->worms.angle > -30)  
      looking_height = LOOKING_DOWN_1_4; 
  if(p->worms.angle <= -30 && p->worms.angle >= MIN_PLAYER_ANGLE)
      looking_height = LOOKING_DOWN_2_4; 

  skin_offset.y = (p->worms.skin->h / MAX_LOOKING_HEIGHTS) * looking_height;
  /* compute worms position into the camera */
  camera_offset.x = p->worms.pos_x 
                    - camera->map_x
                    - skin_offset.w / 2;
  camera_offset.y = p->worms.pos_y 
                    - camera->map_y
                    - skin_offset.h / 2 + 1;
  /* Blit worms */
  if( (camera_offset.x > -skin_offset.w) && (camera_offset.x <= camera->w)
    &&(camera_offset.y > -skin_offset.h) && (camera_offset.y <= camera->h)){
      camera_blit_surface_on(camera,p->worms.skin, &skin_offset, &camera_offset );
  }
    
    
  /* RETICLE PART */
  if(!(p->worms_status & STATUS_FIREING))
  {
    int x,y;
    x = p->reticle_x - camera->map_x;
    y = p->reticle_y - camera->map_y;
    if(p->worms_status & STATUS_AIMING){
      camera_put_pix_color(camera,x,y-1,0,255,0);
      camera_put_pix_color(camera,x-1,y,0,255,0);
      camera_put_pix_color(camera,x,y,50,255,50);
      camera_put_pix_color(camera,x+1,y,0,255,0);
      camera_put_pix_color(camera,x,y+1,0,255,0);
    }else{
      camera_put_pix_color(camera,x,y-1,255,0,0);
      camera_put_pix_color(camera,x-1,y,255,0,0);
      camera_put_pix_color(camera,x,y,255,50,50);
      camera_put_pix_color(camera,x+1,y,255,0,0);
      camera_put_pix_color(camera,x,y+1,255,0,0);

    }
  }


  if(p->worms_status & STATUS_FIREING)
    player_show_gunfire_on_cam(p,camera,looking_height);

  if(p->worms_status & STATUS_SHOW_W){
    font_print(camera, weapon_get_name( p->weapon_slots[p->selected_weapon]->id ) 
                                      , p->worms.pos_x , p->worms.pos_y - 15
                                      , FONT_SMALL);
  }

  if(p->worms_status & STATUS_HAVE_FLAG){
    int x = p->worms.pos_x - camera->map_x;
    int y = p->worms.pos_y - camera->map_y;
    camera_put_pix_color(camera,x-(( p->worms.side == RIGHT_SIDE) ? 8: -7),y-9,255,0,0);
    camera_put_pix_color(camera,x-(( p->worms.side == RIGHT_SIDE) ? 7: -6),y-9,255,0,0);
    camera_put_pix_color(camera,x-(( p->worms.side == RIGHT_SIDE) ? 6: -5),y-9,255,0,0);
    camera_put_pix_color(camera,x-(( p->worms.side == RIGHT_SIDE) ? 7: -6),y-8,255,0,0);
    camera_put_pix_color(camera,x-(( p->worms.side == RIGHT_SIDE) ? 6: -5),y-8,255,0,0);
    camera_put_pix_color(camera,x-(( p->worms.side == RIGHT_SIDE) ? 6: -5),y-8,255,0,0);
    camera_put_pix_color(camera,x-(( p->worms.side == RIGHT_SIDE) ? 6: -5),y-7,255,0,0);
    camera_put_pix_color(camera,x-(( p->worms.side == RIGHT_SIDE) ? 6: -5),y-6,255,0,0);
    camera_put_pix_color(camera,x-(( p->worms.side == RIGHT_SIDE) ? 6: -5),y-5,255,0,0);

  }

  if(warding_flag){
    int x = p->worms.pos_x - camera->map_x;
    int y = p->worms.pos_y - camera->map_y;
    camera_put_pix_color(camera,x-(( p->worms.side == RIGHT_SIDE) ? 6: -5),y-9,255,0,0);
    camera_put_pix_color(camera,x-(( p->worms.side == RIGHT_SIDE) ? 6: -5),y-8,255,0,0);
    camera_put_pix_color(camera,x-(( p->worms.side == RIGHT_SIDE) ? 6: -5),y-7,255,0,0);
    camera_put_pix_color(camera,x-(( p->worms.side == RIGHT_SIDE) ? 6: -5),y-5,255,0,0);
  }
}


void player_show_gunfire_on_cam(player_t* p,camera_t* camera,int looking_height){
  SDL_Rect camera_offset;
  SDL_Rect skin_offset;
  ASSERT(p);
  ASSERT(camera); 
  
  /* GUNFIRE FLAME PART */  
  /* compute sprite to show */
  skin_offset.x = 0; 
  skin_offset.y = 0;  
  skin_offset.w = p->r->gunfire_skin->w / (MAX_SIDE_POSITIONS *  DEFAULT_GUNFIRE_ANIM_STEPS); 
  skin_offset.h = p->r->gunfire_skin->h / MAX_LOOKING_HEIGHTS;  
  skin_offset.x = ( (p->gunfire_anim_pos/DEFAULT_GUNFIRE_ANIM_SLEEP) * skin_offset.w) 
                   + (( p->worms.side == RIGHT_SIDE) 
                        ? 0  
                        : skin_offset.w * DEFAULT_GUNFIRE_ANIM_STEPS);
    
  
  skin_offset.y = (p->r->gunfire_skin->h / MAX_LOOKING_HEIGHTS) * looking_height;
  /* compute fire position into the camera */
  camera_offset.x = p->reticle_x + ((p->worms.pos_x - p->reticle_x) /2)
                    - camera->map_x
                    - skin_offset.h / 2;
  camera_offset.y = p->reticle_y + ((p->worms.pos_y - p->reticle_y + 2)/2)
                    - camera->map_y
                    - skin_offset.h / 2;
  /* Blit gunfire */
  if((camera_offset.x > -skin_offset.w) && (camera_offset.x <= camera->w)
     && (camera_offset.y > -skin_offset.h) && (camera_offset.y <= camera->h)){
        camera_blit_surface_on( camera,p->r->gunfire_skin, &skin_offset
                              , &camera_offset );
  }
  p->gunfire_anim_pos--;
  if( p->gunfire_anim_pos == 0xff )
    p->worms_status &= ~STATUS_FIREING;
}



void player_show_stats(player_t* p,game_mode_t gm){
  ASSERT(p)
  camera_t* camera = p->worms_stats_camera;
  int step = camera->w/21;
  char tmp_string[128];
  
  if((!(p->worms_status & STATUS_STATS_UPDATE))
    && ((SDL_GetTicks() - p->last_stats_update) < PLAYER_STATS_UPDATE_IDLE))
    return;
  
  camera_clean(camera);  
  p->last_stats_update = SDL_GetTicks();
  p->worms_status &= ~STATUS_STATS_UPDATE;
  camera_drow_hline(camera
            , 1*step, camera->h/10 - 2 
            , 20*step, 3, 0x20, 0xff, 0x20);
    
 if(p->worms_health<100){
    camera_drow_hline(camera
              , 1*step + ((p->worms_health*20*step)/100), camera->h/10 - 2  
              , (20*step*(100-p->worms_health))/100 , 3, 0xff, 0x20, 0x20); 

   camera_drow_hline( camera, 20*step, camera->h/10 - 2  
                    , 1*step, 3, 0xff, 0x20, 0x20);
  }
  if(p->weapon_slots[p->selected_weapon]->ammo){
  camera_drow_hline( camera, 1*step, 2*camera->h/10 - 2  
                   , 20*step, 3, 0xCC, 0xCC, 0xCC);
  camera_drow_hline( camera, 1*step, 2*camera->h/10 - 2  
                   , (20*step*(p->weapon_slots[p->selected_weapon]->ammo))
                     / weapon_get_max_ammo(p->weapon_slots[p->selected_weapon])
                   , 3, 0x20, 0x20, 0xFF);
  }else{
    camera_drow_hline( camera, 1*step, 2*camera->h/10 - 2 
                     , 20*step, 3, 0xCC, 0xCC, 0xCC);
    camera_drow_hline(camera, 1*step, 2*camera->h/10 - 2  
           , (20*step*(p->weapon_slots[p->selected_weapon]->recharge_step))
             / weapon_get_nb_recharge_steps(p->weapon_slots[p->selected_weapon])
           , 3, 0x98, 0x00, 0x98);
  }
  snprintf(tmp_string,127," %s: %d",wiiero_label[WIIERO_LANG_FRAGS]
          , game_score[p->id].nb_frags);
  font_print_strict_pos(camera,tmp_string,1*step,3*camera->h/10+3,FONT_SMALL);
  snprintf( tmp_string,127," %s: %d",wiiero_label[WIIERO_LANG_SUICID]
          , game_score[p->id].nb_suicides);
  font_print_strict_pos(camera,tmp_string,1*step,3*camera->h/10+12,FONT_SMALL);
  snprintf( tmp_string,127," %s: %d",wiiero_label[WIIERO_LANG_DEATH]
          , game_score[p->id].nb_death);
  font_print_strict_pos(camera,tmp_string,1*step,3*camera->h/10+21,FONT_SMALL);
  switch(gm){
    case GAME_DEATHMATCH_MODE:
         snprintf( tmp_string,127," %s: %d",wiiero_label[WIIERO_LANG_LIFES]
                 , game_score[p->id].nb_lifes);
         font_print_strict_pos( camera, tmp_string, 1*step
                              , 3*camera->h/10+30, FONT_SMALL);
         break;
    case GAME_OF_TAG_MODE:
         snprintf(tmp_string,127," %s: %.2dm%.2ds",wiiero_label[WIIERO_LANG_TIME]
                 , game_score[p->id].tag_time/60,game_score[p->id].tag_time%60);
         font_print_strict_pos( camera, tmp_string, 1*step
                              , 3*camera->h/10+30, FONT_SMALL);
         break;
    case GAME_CAPTURE_FLAG_MODE:
         snprintf( tmp_string,127," %s: %d",wiiero_label[WIIERO_LANG_FLAGS]
                 , game_score[p->id].nb_flags);
         font_print_strict_pos(camera, tmp_string, 1*step
                              , 3*camera->h/10+30, FONT_SMALL);
  }
}

void player_look_up(player_t* p){
  ASSERT(p);
  if(p->worms_status & STATUS_FREEZED)
    return;
  int angle_pitch = (p->reticle_pitch/2 + 1);
  p->worms.angle = (p->worms.angle + angle_pitch <= MAX_PLAYER_ANGLE) 
                      ? p->worms.angle + angle_pitch
                      : MAX_PLAYER_ANGLE;
  if(angle_pitch < MAX_ANGLE_PITCH)
    p->reticle_pitch++;
}

void player_look_down(player_t* p){
  ASSERT(p);
  if(p->worms_status & STATUS_FREEZED)
    return;
  int angle_pitch = (p->reticle_pitch/2 + 1);
  p->worms.angle = (p->worms.angle - angle_pitch >= MIN_PLAYER_ANGLE) 
                      ? p->worms.angle - angle_pitch
                      : MIN_PLAYER_ANGLE;
  if(angle_pitch < MAX_ANGLE_PITCH)
    p->reticle_pitch++;
}

void player_move_left(player_t* p){
  ASSERT(p);
  if(p->worms_status & STATUS_FREEZED)
    return;
  if(!p->worms.acc_x)
    p->worms.acc_x -= MOVE_PITCH;
  p->worms.side = LEFT_SIDE;
  if(p->worms.animation_sleep > 0){
     p->worms.animation_sleep --;
  }else{
    p->worms.animation_pos = (p->worms.animation_pos+1)%3;
    p->worms.animation_sleep = DEFAULT_PLAYER_ANIM_SLEEP;
  }
  
}

void player_move_right(player_t* p){
  ASSERT(p);
  if(p->worms_status & STATUS_FREEZED)
    return;
  if(!p->worms.acc_x)
    p->worms.acc_x += MOVE_PITCH;
  p->worms.side = RIGHT_SIDE;
  if(p->worms.animation_sleep > 0){
     p->worms.animation_sleep --;
  }else{
    p->worms.animation_pos = (p->worms.animation_pos+1)%3;
    p->worms.animation_sleep = DEFAULT_PLAYER_ANIM_SLEEP;
  }
  
}

void player_jump(player_t* p){
  ASSERT(p);
  if(p->worms_status & STATUS_FREEZED)
      return;
  if(p->worms_status & STATUS_JUMPABLE){
    p->worms_status &= ~STATUS_JUMPABLE;
    p->worms.acc_y -= JUMP_POWER;
  }
}

void player_crop(player_t* p,map_t* m){
  ASSERT(p);
  ASSERT(m);
  Uint8 r,g,b;
  int padding_x = (p->reticle_x-p->worms.pos_x)/3;
  int padding_y = (p->reticle_y-p->worms.pos_y)/3;
  obj_list_t* olist = p->dynamic_list_link;
  ASSERT(olist);
  if(p->worms_status & STATUS_FREEZED)
        return;
  p->worms_status |= STATUS_CROPING;
  padding_y = (padding_y<-3) ? -3 : padding_y;
  get_pix_color(m->layers[GROUND_MAP_LAYER]
               , p->worms.pos_x + padding_x, p->worms.pos_y + padding_y - 3 
               , &r, &g, &b);
  if( r!=transparent_r_value){
    object_add_to_list(olist,create_mud_particle(p->worms.pos_x+(rand()%5)-2
        ,p->worms.pos_y+(rand()%5)-2,(p->worms.side == RIGHT_SIDE) ?-1 : 1,-2));
    object_add_to_list(olist,create_mud_particle(p->worms.pos_x+(rand()%5)-2
        ,p->worms.pos_y+(rand()%5)-1,(p->worms.side == RIGHT_SIDE) ?-2 : 2,-2));
    object_add_to_list(olist,create_mud_particle(p->worms.pos_x+(rand()%5)-2
        ,p->worms.pos_y+(rand()%5)-1,(p->worms.side == RIGHT_SIDE) ?-1 : 1,-1));
  }
  map_drow_cave(m, p->worms.pos_x + padding_x,p->worms.pos_y + padding_y - 3 ,CROP_SIZE);
}

void player_change(player_t* p,int indice){
  if(p->worms_status & STATUS_FREEZED)
    return;
  p->worms_status |= STATUS_CHANGING_W;
  p->selected_weapon = (p->selected_weapon + indice) % WEAPON_SLOTS;
  if(p->selected_weapon == 0xff)
    p->selected_weapon = (WEAPON_SLOTS-1); 
  p->worms_status |= STATUS_STATS_UPDATE;
}

void player_fire(player_t* p){
  ASSERT(p);
  obj_list_t* olist = p->dynamic_list_link;
  ASSERT(olist);
  if(!weapon_can_shoot(p->weapon_slots[p->selected_weapon])){
    return;
  }else{
    float dist = RETICLE_SPACING*1.5;
    Uint8 weapon_info;
    int weapon_acc_y = - (dist * ftrigo_sin(p->worms.angle));
    int weapon_acc_x = (p->worms.side == LEFT_SIDE) 
          ? - (dist * ftrigo_cos(p->worms.angle))
          :   (dist * ftrigo_cos(p->worms.angle));
    
    weapon_info = weapon_shoot(p->weapon_slots[p->selected_weapon],p,weapon_acc_x,weapon_acc_y);
    if(!(weapon_info & NO_SHELL))
      object_add_to_list(olist,create_shell(p->worms.pos_x,p->worms.pos_y,p->r));
      
    if(!(weapon_info & NO_GUNFIRE)){
      p->worms_status |= STATUS_FIREING;
      p->gunfire_anim_pos = DEFAULT_GUNFIRE_ANIM_START*DEFAULT_GUNFIRE_ANIM_SLEEP+(DEFAULT_GUNFIRE_ANIM_SLEEP-1);
      /*p->worms_shockwave = 2;*/
    }
    p->worms_status |= STATUS_STATS_UPDATE;
  }
}


void player_clean(player_t* p){
  p->worms_action = ACTION_NONE;
}

void player_focus(player_t* p){
  ASSERT(p);
    
  if(p->worms_status & STATUS_UNFOCUSED){
    if(p->worms_status & STATUS_RESET_FOCUS)
        p->worms_status &= ~(STATUS_UNFOCUSED | STATUS_RESET_FOCUS);
    return;
  }
  if(p->worms_status & STATUS_RESET_FOCUS)
    p->worms_status &= ~(STATUS_UNFOCUSED | STATUS_RESET_FOCUS);
  camera_focus_on(p->worms_camera,p->worms.pos_x,p->worms.pos_y,MAP_WIDTH,MAP_HEIGHT);  
}


void player_bleeds(player_t* p){ 
  ASSERT(p);
  ASSERT(p->dynamic_list_link);
  object_add_to_list(p->dynamic_list_link,create_blood_trail(p->worms.pos_x,p->worms.pos_y));
}


void player_new_position(player_t * p,SDL_Surface* ground, SDL_Surface* statics){
  get_empty_layer_position(&(p->worms.pos_x),&(p->worms.pos_y),statics);
  drow_circle( ground,p->worms.pos_x,p->worms.pos_y,8
             , transparent_r_value,transparent_g_value,transparent_b_value);
}


void player_update(player_t* p,SDL_Surface* ground, SDL_Surface* statics){
  ASSERT(p);
  player_id pl_id = p->id;
  obj_list_t* olist = p->dynamic_list_link; 
  ASSERT(olist);      

  if(p->worms_status & STATUS_RESETED){
    p->worms_status &= ~STATUS_RESETED;
  }
  if(p->worms_status & STATUS_EXPLODE){
    int i=0;
    /* BOOM ! ;-) */
    p->worms_status &= ~STATUS_ALIVE;
    p->worms_killed++;
    p->worms_health=0;
    player_bleeds(p);
    bullet_time_effect_delay = 80;
    for(i=0;i<10;i++){
      object_add_to_list(olist,create_blood_trail(p->worms.pos_x,p->worms.pos_y));
      object_add_to_list(olist,create_flesh(p->worms.pos_x,p->worms.pos_y,p->r,pl_id));  
    }
    sengine_play(WIIERO_SOUND_DEATH,SENGINE_PLAY_ACTION);
    p->worms_status &= ~STATUS_TRICKED;
    player_new_position(p,ground,statics);
    p->worms_status |= (STATUS_FREEZED | STATUS_UNFOCUSED);
  }
  

  if(!(p->worms_status & STATUS_ALIVE)){
    p->worms_status &= ~STATUS_EXPLODE;
    if(p->worms_health++ == 100)
      player_reset(p, ground, statics);
    
    if(p->worms_status & STATUS_SUICIDE){
      font_print_strict_pos(p->worms_camera, wiiero_label[WIIERO_LANG_GAME_SUICIDE]
                                , 5
                                , 5,FONT_SMALL );
    }else{
      font_print_strict_pos(p->worms_camera, wiiero_label[WIIERO_LANG_GAME_KILLED]
                                , 5
                                , 5,FONT_SMALL);
    }
    
    p->worms_status |= STATUS_STATS_UPDATE;
    if(p->worms_health > 20){ /* Scroll to new worms position */
        int x,y;
        float pitch_x = (1.0*p->worms.pos_x - p->worms_camera->focused_pos.x ) / (101-p->worms_health);
        float pitch_y = (1.0*p->worms.pos_y - p->worms_camera->focused_pos.y ) / (101-p->worms_health);
        pitch_x += (pitch_x>0) ? 0.99 : -0.99;
        pitch_y += (pitch_y>0) ? 0.99 : -0.99;
        x = p->worms_camera->focused_pos.x + (int)pitch_x;
        y = p->worms_camera->focused_pos.y + (int)pitch_y;
        camera_focus_on(p->worms_camera,x,y,MAP_WIDTH,MAP_HEIGHT);
    }
    return;
  }

  /* update weapon state */
  if(weapon_update(p->weapon_slots[p->selected_weapon]) == 1)
    p->worms_status |= STATUS_STATS_UPDATE;
  
  /* health critical */
  if((p->worms_health < 20)&&(p->worms_health!=0))
    if(rand()%(p->worms_health) <= 1 )
     object_add_to_list(olist,create_blood_drop(p->worms.pos_x,p->worms.pos_y-2));
     
  /* clean collision matrix*/
  DBG(" - PHYSICS \n");
  memset(&coll_matrix,0,sizeof(int)*COLL_WIDTH*COLL_HEIGHT);
  p->worms_status &= ~STATUS_JUMPABLE;
  apply_physics(&(p->worms), ground, statics, p);
  
  
  /* reticle update if alive */ 
  if(!(p->worms_status & STATUS_ALIVE))
    return;
  p->reticle_x = (p->worms.side == LEFT_SIDE)
              ?  (p->worms.pos_x-WEAPON_POS_X) - (RETICLE_SPACING * ftrigo_cos(p->worms.angle))
              :  (p->worms.pos_x-WEAPON_POS_X) + (RETICLE_SPACING * ftrigo_cos(p->worms.angle));
  p->reticle_y = (p->worms.pos_y-WEAPON_POS_Y) - (RETICLE_SPACING * ftrigo_sin(p->worms.angle));
  
}


void player_shot(player_t* p,player_id origin,Uint8 damage,int acc_x,int acc_y,void* userdata){ 

  if(p->worms_status & STATUS_EXPLODE)
    return;

  /*p->worms_shockwave = 6;*/
  
  if((p->worms_health - damage)<=0){
    p->worms_health = 0;
    if( !(p->worms_status & STATUS_PROTECTED)){
      p->worms_status |= STATUS_EXPLODE;
      p->worms_status &= ~STATUS_ALIVE;
      p->worms_status |= STATUS_STATS_UPDATE;
      game_score[p->id].nb_lifes   -= 1;
      game_score[p->id].nb_death   += 1;
      if(p->worms_status & STATUS_HAVE_FLAG)
        drop_player_flag(p->bullet_list_link,p->r,p->id, p->worms.pos_x , p->worms.pos_y);

      p->worms_status &= ~STATUS_HAVE_FLAG;
      if(p->id == origin){
        p->worms_status |= STATUS_SUICIDE;
        game_score[p->id].nb_suicides++;
      }else{
        game_score[origin].nb_frags++;
        p->worms_status &= ~STATUS_SUICIDE;
      }
    }
    return;
  }
  
  p->worms_health -= damage;
  p->worms.acc_x += acc_x;
  p->worms.acc_y += acc_y;
  if( !(p->worms_status & STATUS_PROTECTED)) 
    player_bleeds(p);
  p->worms_status |= STATUS_STATS_UPDATE;
}

void player_regen(player_t* p,Uint8 health){ 
  ASSERT(p)
  p->worms_health += health;
  if (p->worms_health > DEFAULT_PLAYER_HEALTH){
    p->worms_health = DEFAULT_PLAYER_HEALTH;
  }
  p->worms_status |= STATUS_STATS_UPDATE;
}

void player_launch_hook(player_t* p){
  /* Launch hook */
  ASSERT(p->ninja_hook->last_bullet == 0l)
  float dist = RETICLE_SPACING*1.5;
  int weapon_acc_y = - (dist * ftrigo_sin(p->worms.angle));
  int weapon_acc_x = (p->worms.side == LEFT_SIDE) 
        ? - (dist * ftrigo_cos(p->worms.angle))
        :   (dist * ftrigo_cos(p->worms.angle));
  p->worms_rope_len = DEFAULT_ROPE_LEN;
  weapon_shoot(p->ninja_hook,p,weapon_acc_x,weapon_acc_y);
}


void player_change_rope_len(player_t* p,int len_modif){
    p->worms_rope_len += len_modif;
    if(p->worms_rope_len == 0) 
        p->worms_rope_len = ROPE_LEN_CHANGE_PITCH;   
}

void player_remove_hook(player_t* p,player_t* other_p){
  /* Remove Hook */
  ASSERT(p->ninja_hook->last_bullet)
  ninja_hook_disconnect(p->ninja_hook->last_bullet,p,other_p,0l);
  p->ninja_hook->last_bullet->obj.remove_flag = 1;
  p->ninja_hook->last_bullet=0l;

}
