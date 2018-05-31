/*
 *  __      _ .__  .__
 * /  \    / \(__) (__)  ____ ______  ____
 * \   \/\/  /|  | |  |_/ __ \\_  _ \/  _ \ 
 *  \       //   |/   |\  ___/ |  |\(  <_> )
 *   \__/\_/ \__ \\__ \ \___  >|__|  \____/ 
 *              \/   \/     \/ By BScrk
 * Liero clone on Wii, PSP and Linux
 * Author: Luca Benevolo
 * Original concept : Joosa Riekkinen in 1998
 * Version: 0.1 Beta, September 2008
 */
 
 /*           /!\ DEPRECATED CODE /!\              */
 
#ifndef INPUT_H
#define INPUT_H

#include "common.h"
#include "player.h"
#include "game.h"
#include "font.h"

#include <wiiuse/wiiuse.h>
#include <stdlib.h>

#define MAX_WIIMOTES 2

static int connected = 0;
wiimote** wiimotes;

static void init_wiimotes(){
  int found=0, connected=0;
  char detect_message[128];
  /* init */
  font_console_print("wiimotes detection...",FONT_SMALL);
  wiimotes =  wiiuse_init(MAX_WIIMOTES,0l);
  /* detection */
  found = wiiuse_find(wiimotes, MAX_WIIMOTES, 5);
  connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);
  snprintf(detect_message,127," ok, %d wiimotes connected (on %d found)\n",connected,found);
  font_console_print(detect_message,FONT_SMALL);
  switch(connected){
    case 0 : font_console_print("no wiimote !",FONT_SMALL); break;
    default: wiiuse_set_leds(wiimotes[1], WIIMOTE_LED_2,0l); 
             wiiuse_rumble(wiimotes[1], 1);
             connected |= 0x01;
    case 1 : wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_1,0l);
             wiiuse_rumble(wiimotes[0], 1);
             connected |= 0x02;
             break;
  }
  SDL_Delay(1000);
  if(connected & 0x01) wiiuse_rumble(wiimotes[0], 0);
  if(connected & 0x02) wiiuse_rumble(wiimotes[1], 0);
}


static void handle_event(struct wiimote_t* wm,player_t* p){
  if (IS_PRESSED(wm, WIIMOTE_BUTTON_A))    p->worms_action |= ACTION_CHANGE; 
  if (IS_PRESSED(wm, WIIMOTE_BUTTON_UP))   p->worms_action |= ACTION_LEFT; 
  if (IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN)) p->worms_action |= ACTION_RIGHT; 
  if (IS_PRESSED(wm, WIIMOTE_BUTTON_LEFT)) p->worms_action |= ACTION_DOWN; 
  if (IS_PRESSED(wm, WIIMOTE_BUTTON_RIGHT))p->worms_action |= ACTION_UP; 
  if (IS_PRESSED(wm, WIIMOTE_BUTTON_TWO))  p->worms_action |= ACTION_FIRE;
  if (IS_PRESSED(wm, WIIMOTE_BUTTON_ONE))  p->worms_action |= ACTION_JUMP;  
  
  if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_PLUS)) p->worms_action |= ACTION_HOOK;
  if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_HOME)) p->worms_action |= ACTION_MENU;
  if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_MINUS))p->worms_action |= ACTION_PAUSE;
  if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_B))    p->worms_action |= ACTION_CROP; 
}

/* Wii event control */
static __inline__ void game_check_event(game_t* g){ 
  wiiuse_poll(wiimotes, MAX_WIIMOTES);
  if(connected & 0x01) handle_event(wiimotes[0],g->worms[PLAYER_1]);
  if(connected & 0x02) handle_event(wiimotes[1],g->worms[PLAYER_2]);
}


/* Player event update Wii */
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
  if((p->worms_action & ACTION_UP)){
    player_look_up(p);
  }else{
    if((p->worms_action & ACTION_DOWN))
      player_look_down(p);
    else p->reticle_pitch = ANGLE_PITCH;
  }
  
  /* change */
  if(p->worms_action & ACTION_CHANGE){
    p->worms_status |= STATUS_SHOW_W;
    if(p->worms_action & ACTION_JUMP)
      player_change(p,1);
    if(p->worms_action & ACTION_FIRE)
      player_change(p,-1);
  }else{
    p->worms_status &= ~STATUS_CHANGING_W;
    /* Ninja hook */
    if((p->worms_action & ACTION_JUMP) && (p->worms_action & ACTION_FIRE)){
      if(!(p->worms_status & STATUS_NINJA_ACTION)){
        if(p->ninja_hook->last_bullet==0l){/* Launch */
          player_launch_hook(p);
        }else{/* Launch new */
          player_remove_hook(p,other_p);
          player_launch_hook(p);
        }
        p->worms_status |= STATUS_NINJA_ACTION;
      } 
    }else{ /* Jump or Fire */
      if(p->worms_action & ACTION_JUMP){
        player_jump(p);
        /* Remove hook */
        if(!(p->worms_status & STATUS_NINJA_ACTION))
          if(p->ninja_hook->last_bullet!=0l){
             player_remove_hook(p,other_p);
          }
      }
      p->worms_status &= ~STATUS_NINJA_ACTION;
      /* FIRE */
      if((p->worms_action & ACTION_FIRE))
       player_fire(p);  
    }
  }

  /* CROP */
  if(p->worms_action & ACTION_CROP){
      player_crop(p,m);
  }else{
      p->worms_status &= ~STATUS_CROPING;
  }
  
  /* Move */
  if((p->worms_action & ACTION_RIGHT))
    player_move_right(p);
  if((p->worms_action & ACTION_LEFT))
    player_move_left(p);
  
}



#endif

