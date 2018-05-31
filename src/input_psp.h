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

 
#ifndef INPUT_H
#define INPUT_H

#include "common.h"
#include <SDL/SDL.h>
#include "player.h"
#include "game.h"

/*** PSP JOY MAPPING ***/
#define PSP_TRIANGLE 0x00
#define PSP_CIRCLE   0x01
#define PSP_CROSS    0x02
#define PSP_SQUARE   0x03
#define PSP_LTRIGGER 0x04
#define PSP_RTRIGGER 0x05
#define PSP_DOWN     0x06
#define PSP_LEFT     0x07
#define PSP_UP       0x08
#define PSP_RIGHT    0x09
#define PSP_SELECT   0x0A
#define PSP_START    0x0B
#define PSP_NONE     0x0C
#define PSP_MAX      0x0D

/*
typedef enum { MODE_CMD
     , EXTRA_INFO
     , FIRE_CMD
     , JUMP_CMD
     , CROP_CMD
     , WSHOW_CMD 
     , WLEFT_CMD
     , WRIGHT_CMD
     , NINJA_CMD
     , UP_CMD
     , DOWN_CMD
     , LEFT_CMD
     , RIGHT_CMD
     , MAX_CMD
} lab_id_t;

char * cmd_labels[MAX_CMD] = {
  "PSP controls"
 ,""
 ,"fire:"
 ,"jump:"
 ,"dig:"
 ,"show weapon:"
 ,"prev weapon:"
 ,"next weapon:"
 ,"ninja hook:"
 ,"up:"
 ,"down:"
 ,"left:"
 ,"right:"
};
*/

char * cmd_key[2][MAX_CMD] = {
 { ""
  ,"Player"
  ,""
  ,""
  ,""
  ,""
  ,""
  ,""
  ,""
  ,""
  ,""
  ,""
  ,""
 },{ ""
  ,""
  ,""
  ,""
  ,""
  ,""
  ,""
  ,""
  ,""
  ,""
  ,""
  ,""
  ,""
 }
};


static __inline__ int are_controls_ready(){
  return 1;
}

enum{/* - -- --- WIIERO EVENTS ---- --- -- - */
    P1_ACTION_KEY_UP     = PSP_UP,
    P1_ACTION_KEY_DOWN   = PSP_DOWN,
    P1_ACTION_KEY_LEFT   = PSP_LEFT,
    P1_ACTION_KEY_RIGHT  = PSP_RIGHT,
    P1_ACTION_KEY_JUMP   = PSP_CIRCLE,
    P1_ACTION_KEY_HOOK   = PSP_CROSS,
    P1_ACTION_KEY_CHANGE = PSP_TRIANGLE,
    P1_ACTION_KEY_FIRE   = PSP_SQUARE,
    P1_ACTION_KEY_LCROP  = PSP_LTRIGGER,
    P1_ACTION_KEY_RCLOP  = PSP_RTRIGGER,

    P2_ACTION_KEY_UP     = PSP_NONE,
    P2_ACTION_KEY_DOWN   = PSP_NONE,
    P2_ACTION_KEY_LEFT   = PSP_NONE,
    P2_ACTION_KEY_RIGHT  = PSP_NONE,
    P2_ACTION_KEY_JUMP   = PSP_NONE,
    P2_ACTION_KEY_HOOK   = PSP_NONE,
    P2_ACTION_KEY_CHANGE = PSP_NONE,
    P2_ACTION_KEY_FIRE   = PSP_NONE,
    P2_ACTION_KEY_LCROP  = PSP_NONE,
    P2_ACTION_KEY_RCLOP  = PSP_NONE,
    
    GAME_ACTION_KEY_EXIT = PSP_START,
    GAME_ACTION_KEY_FLIP = PSP_NONE,
    GAME_ACTION_KEY_PAUSE= PSP_SELECT,
};/* - -- --- WIIERO EVENTS ---- --- -- */




/* PSP Joystick support */
static __inline__ Uint8* get_psp_joy_state(){
  static Uint8 joystate[PSP_MAX];
  int nb_checks = 10;
  SDL_Event event;
  /* Check for events */
  while((SDL_PollEvent(&event) != 0)&&(nb_checks)){
    if((event.type == SDL_JOYBUTTONDOWN)
      &&(event.jbutton.button<PSP_NONE))
         joystate[event.jbutton.button] = 1;
    if((event.type == SDL_JOYBUTTONUP)
      &&(event.jbutton.button<PSP_NONE))
         joystate[event.jbutton.button] = 0;
    nb_checks--;
  }
  return joystate;
}



/* PSP event control */
static __inline__ void game_check_event(game_t* g){ 
  /* CHECK GAME EVENTS */
  Uint8 *keystate;
  ASSERT(g);
  /*  ---------  PSP MODE --------- */
  keystate = get_psp_joy_state();
  // [PLAYER_1]
  if ( keystate[P1_ACTION_KEY_UP] )
    g->worms[PLAYER_1]->worms_action |= ACTION_UP;
  if ( keystate[P1_ACTION_KEY_DOWN] )
    g->worms[PLAYER_1]->worms_action |= ACTION_DOWN; 
  if ( keystate[P1_ACTION_KEY_LEFT] )
    g->worms[PLAYER_1]->worms_action |= ACTION_LEFT;
  if ( keystate[P1_ACTION_KEY_RIGHT] )
    g->worms[PLAYER_1]->worms_action |= ACTION_RIGHT; 
  if ( keystate[P1_ACTION_KEY_JUMP] )
    g->worms[PLAYER_1]->worms_action |= ACTION_JUMP; 
  if ( keystate[P1_ACTION_KEY_CHANGE] )
    g->worms[PLAYER_1]->worms_action |= ACTION_CHANGE;
  if ( keystate[P1_ACTION_KEY_HOOK] )
    g->worms[PLAYER_1]->worms_action |= ACTION_HOOK;
  if ( keystate[P1_ACTION_KEY_LCROP] )
    g->worms[PLAYER_1]->worms_action |= ACTION_L_ACT;
  if ( keystate[P1_ACTION_KEY_RCLOP] )
    g->worms[PLAYER_1]->worms_action |= ACTION_R_ACT;
  if ( keystate[P1_ACTION_KEY_FIRE] )
    g->worms[PLAYER_1]->worms_action |= ACTION_FIRE; 
  if ( keystate[GAME_ACTION_KEY_PAUSE] ){
    g->worms[PLAYER_1]->worms_action |= ACTION_PAUSE; 
  }
}


/* Player event update PSP */
static __inline__  void player_event_update(player_t* p,map_t* m,player_t* other_p){
  ASSERT(p);
  ASSERT(m);

  if(!(p->worms_status & STATUS_ALIVE)){
    if(p->ninja_hook->last_bullet)
      player_remove_hook(p,other_p);
    return;
  }
  
  p->worms_status &= ~STATUS_SHOW_W;
  
  /* Up & Down events */
  if((p->worms_action & ACTION_UP))
    player_look_up(p);
  if((p->worms_action & ACTION_DOWN))
    player_look_down(p);
  
  /* Ninja hook event */
  if(p->worms_action & ACTION_HOOK){
    if(!(p->worms_status & STATUS_NINJA_ACTION)){
      if(p->ninja_hook->last_bullet==0l){
        /* Launch */
        //printf("lauch\n");
        player_launch_hook(p);
      }else{
        /* Launch new */
        //printf("lauch new\n");
        player_remove_hook(p,other_p);
        player_launch_hook(p);
      }
      p->worms_status |= STATUS_NINJA_ACTION;
    } 
  }
  
  /* Jump event */
  if((p->worms_action & ACTION_JUMP)){
    player_jump(p);
    /* Remove hook */
    if(!(p->worms_status & STATUS_NINJA_ACTION))
      if(p->ninja_hook->last_bullet!=0l){
         player_remove_hook(p,other_p);
      }
    p->worms_status &= ~STATUS_NINJA_ACTION;
  }
    
  if(p->worms_action & ACTION_L_ACT){
    player_move_left(p);
    player_crop(p,m);
    p->worms_status &= ~STATUS_CHANGING_W;
  }else{
    if(p->worms_action & ACTION_R_ACT){
      player_move_right(p);
      player_crop(p,m);
      p->worms_status &= ~STATUS_CHANGING_W;
    }
    p->worms_status &= ~STATUS_CROPING;
  }
  
  p->worms_status &= ~STATUS_SHOW_W;
  if(p->worms_action & ACTION_CHANGE){
    if((p->worms_action & ACTION_RIGHT)){
        if(!(p->worms_status & STATUS_CHANGING_W))
        player_change(p,1);
      }else{
        if((p->worms_action & ACTION_LEFT)){
          if(!(p->worms_status & STATUS_CHANGING_W))
            player_change(p,-1);
        }else{
          p->worms_status &= ~STATUS_CHANGING_W;
        }
      }
      p->worms_status |= STATUS_SHOW_W;
  }else{
      /* if moving, no weapon change */
      p->worms_status &= ~STATUS_CHANGING_W;
      if((p->worms_action & ACTION_RIGHT))
        player_move_right(p);
      if((p->worms_action & ACTION_LEFT))
        player_move_left(p);
  }

  /* FIRE */
  if((p->worms_action & ACTION_FIRE))
     player_fire(p);  
}

#endif
