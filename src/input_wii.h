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
#include "player.h"
#include "game.h"
#include "font.h"

#include <stdlib.h>
#include <string.h>
#include <wiiuse/wpad.h>

#define MAX_WIIMOTES 2
#define MAX_WII_CONTROLS 4

enum {
   WII_CONTROL_MODE_WIIMOTE
  ,WII_CONTROL_MODE_NUNCHUK
  ,WII_CONTROL_MODE_CLASSIC
  ,WII_CONTROL_MODE_GAMECUB
};

#define GC_PAD_SENSIBILITY 18

#ifndef PAD_CHAN_0
# define PAD_CHAN_0 PAD_CHAN0
#endif
#ifndef PAD_CHAN_1
# define PAD_CHAN_1 PAD_CHAN1
#endif

// Wrapp CG control detection on wiiuse expentions
#define WPAD_EXP_GAMECUBE (WPAD_EXP_UNKNOWN-1)


typedef enum { 
       MODE_CMD
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
     , MENU_CMD
     , PAUSE_CMD
     , OK_CMD
     , CANCEL_CMD
     , MAX_CMD
} lab_id_t;


static char * cmd_key[MAX_WII_CONTROLS][MAX_CMD] = {
 { ""
  ,"wiimote"
  ,"2"
  ,"1"
  ,"b"
  ,"a"
  ,"a+1"
  ,"a+2"
  ,"1+2"
  ,"+ up"
  ,"+ down"
  ,"+ left"
  ,"+ right"
  ,"plus"
  ,"home"
  ,"2"
  ,"1"
 },{ ""
  ,"nunchuck"
  ,"b"
  ,"c"
  ,"a"
  ,"+ down"
  ,"+ left"
  ,"+ right"
  ,"z"
  ,"stick u"
  ,"stick d"
  ,"stick l"
  ,"stick r"
  ,"minus"
  ,"home"
  ,"a"
  ,"b"
 },{ ""
  ,"classic"
  ,"y"
  ,"b"
  ,"a"
  ,"x"
  ,"x+left"
  ,"x+right"
  ,"r"
  ,"+ up"
  ,"+ down"
  ,"+ left"
  ,"+ right"
  ,"minus"
  ,"home"
  ,"b or y"
  ,"a or x"
 },{ ""
  ,"gamecube"
  ,"b"
  ,"a"
  ,"x"
  ,"+ down"
  ,"+ left"
  ,"+ right"
  ,"r"
  ,"stick u"
  ,"stick d"
  ,"stick l"
  ,"stick r"
  ,"z"
  ,"start"
  ,"a"
  ,"b or x"
 }
};

int connected=0;

int control_mode = WII_CONTROL_MODE_WIIMOTE;
static int gc_chan[WPAD_MAX_WIIMOTES]; 

static __inline__ int wiimote_status_detection(int chan,u32 *ext_type){  
  u32 ret=0;
  ret=WPAD_Probe(chan,ext_type);
  switch(ret) {
    case WPAD_ERR_NONE:
      if(!(connected & ((chan == WPAD_CHAN_0) ? 0x01 : 0x02))){
        connected |= (chan == WPAD_CHAN_0) ? 0x01 : 0x02;
        WPAD_Rumble(chan, 1);
        SDL_Delay(300);
        WPAD_Rumble(chan, 0);
      }
      return 1;
      break;
    case WPAD_ERR_NO_CONTROLLER: // future use ...
    case WPAD_ERR_NOT_READY:     // future use ...
    default:
      //connected &= (chan == WPAD_CHAN_0) ? ~0x01 : ~0x02;
      return 0;
  }
}

static __inline__ int gc_status_detection(s32 chan,u32 *ext_type){ 
  static PADStatus pad[4];  
  memset(&pad[chan],0,sizeof(PADStatus));
  PAD_Read(pad);
  chan = gc_chan[chan];
  switch(pad[chan].err) {
    case PAD_ERR_NONE:
    case PAD_ERR_TRANSFER: // hack, strange result for channel 1 ...
      if(!(connected & ((chan == PAD_CHAN_0) ? 0x01 : 0x02))){
        connected |= (chan == PAD_CHAN_0) ? 0x01 : 0x02;
        PAD_ControlMotor(chan,PAD_MOTOR_RUMBLE);
        SDL_Delay(300);
        PAD_ControlMotor(chan,PAD_MOTOR_STOP);
      }
	  if(ext_type)
        *ext_type = WPAD_EXP_GAMECUBE;
      return 1;
      break;
    default:
      return 0;
  }
}


static __inline__ int controls_detection(int chan,u32 *ext_type){  
 if (gc_status_detection(chan,ext_type))
   return 1;
 if(wiimote_status_detection(chan,ext_type))
   return 1;
 connected &= (chan == PAD_CHAN_0) ? ~0x01 : ~0x02;
 return 0;
}


static __inline__ void init_wiimotes(){
  /* init controls */
  /* build gc chan mapping */
  gc_chan[WPAD_CHAN_0] = PAD_CHAN_0;
  gc_chan[WPAD_CHAN_1] = PAD_CHAN_1;
  /* init wiimotes */
  font_console_print_debug("setting wiimotes...\n",FONT_SMALL);   SDL_Delay(1000);
  WPAD_Init();
  WPAD_ScanPads();
  wiimote_status_detection(WPAD_CHAN_0,0l);
  wiimote_status_detection(WPAD_CHAN_1,0l);
  /* init GC pads */
  font_console_print_debug("setting gamecube...\n",FONT_SMALL);   SDL_Delay(1000);
  PAD_Init();
  PAD_ScanPads();
  gc_status_detection(WPAD_CHAN_0,0l);
  gc_status_detection(WPAD_CHAN_1,0l);
  font_console_print_debug("Done...\n",FONT_SMALL);   SDL_Delay(1000);

}

static __inline__ int are_controls_ready(){
  return ((connected & 0x03) == 0x03);
}


static __inline__ void handle_event(int chan,player_t* p){
  struct expansion_t ext;
  u32 ext_type;//Extension type
  
  p->worms_action = ACTION_NONE;
  
  // check if the controler is on
  if( !controls_detection(chan,&ext_type)  )
   return;
  
  control_mode = WII_CONTROL_MODE_WIIMOTE;
  if(ext_type == WPAD_EXP_NUNCHUK)       control_mode = WII_CONTROL_MODE_NUNCHUK;
  else if(ext_type == WPAD_EXP_CLASSIC)  control_mode = WII_CONTROL_MODE_CLASSIC;
  else if(ext_type == WPAD_EXP_GAMECUBE) control_mode = WII_CONTROL_MODE_GAMECUB;

  // reset control mode
  p->worms_status &= ~(STATUS_NUNCHUCK_ON|STATUS_CLASSIC_ON|STATUS_GAMECUBE_ON);  
  switch(control_mode){
  case WII_CONTROL_MODE_NUNCHUK:
    { 
      /* NUNCHUCK CONTROLS */
      WPAD_Expansion(chan,&ext);
      p->worms_status |=  STATUS_NUNCHUCK_ON;
      if(ext.nunchuk.js.mag > 0.5){
        if((ext.nunchuk.js.ang > 338) ||  (ext.nunchuk.js.ang < 22))  p->worms_action |= ACTION_UP;
        else if((ext.nunchuk.js.ang >  22) &&  (ext.nunchuk.js.ang < 47))  p->worms_action |= (ACTION_UP|ACTION_RIGHT);
        else if((ext.nunchuk.js.ang >  47) &&  (ext.nunchuk.js.ang < 132)) p->worms_action |= ACTION_RIGHT;
        else if((ext.nunchuk.js.ang > 132) &&  (ext.nunchuk.js.ang < 157)) p->worms_action |= (ACTION_RIGHT|ACTION_DOWN);
        else if((ext.nunchuk.js.ang > 157) &&  (ext.nunchuk.js.ang < 202)) p->worms_action |= ACTION_DOWN;
        else if((ext.nunchuk.js.ang > 202) &&  (ext.nunchuk.js.ang < 228)) p->worms_action |= (ACTION_DOWN|ACTION_LEFT);
        else if((ext.nunchuk.js.ang > 228) &&  (ext.nunchuk.js.ang < 313)) p->worms_action |= ACTION_LEFT;
        else if((ext.nunchuk.js.ang > 313) &&  (ext.nunchuk.js.ang < 338)) p->worms_action |= (ACTION_LEFT|ACTION_UP);
      }
      u32 data = WPAD_ButtonsHeld(chan);
      if(data & WPAD_NUNCHUK_BUTTON_C) p->worms_action |= ACTION_JUMP;
      if(data & WPAD_BUTTON_B        ) p->worms_action |= ACTION_FIRE | ACTION_CANCEL;
      if(data & WPAD_BUTTON_DOWN     ) p->worms_action |= ACTION_CHANGE;
      if(data & WPAD_BUTTON_LEFT     ) p->worms_action |= ACTION_CHANGE;
      if(data & WPAD_BUTTON_RIGHT    ) p->worms_action |= ACTION_CHANGE;
      data = WPAD_ButtonsDown(chan);
      if(data & WPAD_NUNCHUK_BUTTON_Z) p->worms_action |= ACTION_HOOK;
      if(data & WPAD_BUTTON_A        ) p->worms_action |= ACTION_CROP | ACTION_OK;
      if(data &  WPAD_BUTTON_HOME    ) p->worms_action |= ACTION_MENU;
      if(data & WPAD_BUTTON_PLUS     ) p->worms_action |= ACTION_PAUSE;
      if(data & WPAD_BUTTON_LEFT     ) p->worms_action |= ACTION_L_ACT;
      if(data & WPAD_BUTTON_RIGHT    ) p->worms_action |= ACTION_R_ACT;
    }
    break;
  case WII_CONTROL_MODE_WIIMOTE:
    { 
      /* WIIMOTE ONLY */
      u32 data = WPAD_ButtonsHeld(chan);
      if(data & WPAD_BUTTON_A    )  p->worms_action |= ACTION_CHANGE; 
      if(data & WPAD_BUTTON_UP   )  p->worms_action |= ACTION_LEFT; 
      if(data & WPAD_BUTTON_DOWN )  p->worms_action |= ACTION_RIGHT; 
      if(data & WPAD_BUTTON_LEFT )  p->worms_action |= ACTION_DOWN; 
      if(data & WPAD_BUTTON_RIGHT)  p->worms_action |= ACTION_UP; 
      if(!(data & WPAD_BUTTON_A)){
        if( data & WPAD_BUTTON_2 )  p->worms_action |= ACTION_FIRE ;
        if( data & WPAD_BUTTON_1 )  p->worms_action |= ACTION_JUMP | ACTION_CANCEL;  
      }
      data = WPAD_ButtonsDown(chan);
      if(data &  WPAD_BUTTON_PLUS ) p->worms_action |= ACTION_HOOK;
      if(data &  WPAD_BUTTON_HOME ) p->worms_action |= ACTION_MENU;
      if(data &  WPAD_BUTTON_MINUS) p->worms_action |= ACTION_PAUSE;
      if(data &  WPAD_BUTTON_B    ) p->worms_action |= ACTION_CROP; 
      if(data & WPAD_BUTTON_2     ) p->worms_action |= ACTION_FIRE | ACTION_OK;
      if(data & WPAD_BUTTON_1     ) p->worms_action |= ACTION_JUMP ;  
    }
    break;
  case WII_CONTROL_MODE_CLASSIC:
    { 
      /* CLASSIC CONTOL */
      p->worms_status |= STATUS_CLASSIC_ON;
      u32 data = WPAD_ButtonsHeld(chan);
      if( data & WPAD_CLASSIC_BUTTON_X      ) p->worms_action |= ACTION_CHANGE; 
      if( data & WPAD_CLASSIC_BUTTON_LEFT   && !(p->worms_action & ACTION_CHANGE)) p->worms_action |= ACTION_LEFT; 
      if( data & WPAD_CLASSIC_BUTTON_RIGHT  && !(p->worms_action & ACTION_CHANGE)) p->worms_action |= ACTION_RIGHT;   
      if( data & WPAD_CLASSIC_BUTTON_DOWN   ) p->worms_action |= ACTION_DOWN; 
      if( data & WPAD_CLASSIC_BUTTON_UP     ) p->worms_action |= ACTION_UP; 
      if( data & WPAD_CLASSIC_BUTTON_Y      ) p->worms_action |= ACTION_FIRE;
      if( data & WPAD_CLASSIC_BUTTON_B      ) p->worms_action |= ACTION_JUMP;  
      if( data & WPAD_CLASSIC_BUTTON_A      ) p->worms_action |= ACTION_CANCEL;
      if( data & WPAD_CLASSIC_BUTTON_Y      ) p->worms_action |= ACTION_CANCEL ;
      
      data = WPAD_ButtonsDown(chan);
      if( data & WPAD_CLASSIC_BUTTON_A      ) p->worms_action |= ACTION_CROP ;
      if( data & WPAD_CLASSIC_BUTTON_B      ) p->worms_action |= ACTION_OK ;
      if( data & WPAD_CLASSIC_BUTTON_X      ) p->worms_action |= ACTION_OK ;
      if( data & WPAD_CLASSIC_BUTTON_FULL_R ) p->worms_action |= ACTION_HOOK;
      if( data & WPAD_CLASSIC_BUTTON_HOME   ) p->worms_action |= ACTION_MENU;
      if( data & WPAD_CLASSIC_BUTTON_MINUS  ) p->worms_action |= ACTION_PAUSE;
      if( data & WPAD_CLASSIC_BUTTON_LEFT   &&  (p->worms_action & ACTION_CHANGE)) p->worms_action |= ACTION_L_ACT; 
      if( data & WPAD_CLASSIC_BUTTON_RIGHT  &&  (p->worms_action & ACTION_CHANGE)) p->worms_action |= ACTION_R_ACT; 
    }
    break;
   case WII_CONTROL_MODE_GAMECUB:
    { 
      /* GAMECUBE CONTOL */
      p->worms_status |= STATUS_GAMECUBE_ON;
      u16 gc_buttons_down = PAD_ButtonsDown(gc_chan[chan]);
      u16 gc_buttons_hold = PAD_ButtonsHeld(gc_chan[chan]);
      int gc_pad_x = PAD_StickX(gc_chan[chan]);
      int gc_pad_y = PAD_StickY(gc_chan[chan]);
      if(gc_pad_y > GC_PAD_SENSIBILITY)       p->worms_action |= ACTION_UP;
      else if(gc_pad_y < -GC_PAD_SENSIBILITY) p->worms_action |= ACTION_DOWN;
      if(gc_pad_x > GC_PAD_SENSIBILITY)       p->worms_action |= ACTION_RIGHT;
      else if(gc_pad_x < -GC_PAD_SENSIBILITY) p->worms_action |= ACTION_LEFT;
	  
      if( gc_buttons_hold & PAD_BUTTON_UP     ) p->worms_action |= ACTION_CHANGE; 
      if( gc_buttons_hold & PAD_BUTTON_DOWN   ) p->worms_action |= ACTION_CHANGE; 
      if( gc_buttons_hold & PAD_BUTTON_B      ) p->worms_action |= ACTION_FIRE;
      if( gc_buttons_hold & PAD_BUTTON_A      ) p->worms_action |= ACTION_JUMP;  
      if( gc_buttons_hold & PAD_BUTTON_LEFT   ) p->worms_action |= ACTION_CHANGE; 
      if( gc_buttons_hold & PAD_BUTTON_RIGHT  ) p->worms_action |= ACTION_CHANGE; 
      if( gc_buttons_hold & PAD_TRIGGER_L     ) p->worms_action |= ACTION_CHANGE; 
      if( gc_buttons_hold & PAD_BUTTON_X      ) p->worms_action |= ACTION_CANCEL;
      if( gc_buttons_hold & PAD_BUTTON_B      ) p->worms_action |= ACTION_CANCEL;

      if( gc_buttons_down & PAD_BUTTON_X      ) p->worms_action |= ACTION_CROP;
      if( gc_buttons_down & PAD_BUTTON_A      ) p->worms_action |= ACTION_OK ;
      if( gc_buttons_down & PAD_BUTTON_Y      ) p->worms_action |= ACTION_CANCEL ;
      if( gc_buttons_down & PAD_BUTTON_B      ) p->worms_action |= ACTION_CANCEL ;
      if( gc_buttons_down & PAD_TRIGGER_R     ) p->worms_action |= ACTION_HOOK;
      if( gc_buttons_down & PAD_BUTTON_START  ) p->worms_action |= ACTION_MENU;
      if( gc_buttons_down & PAD_TRIGGER_Z     ) p->worms_action |= ACTION_PAUSE;
      if( gc_buttons_down & PAD_BUTTON_LEFT   ) p->worms_action |= ACTION_L_ACT; 
      if( gc_buttons_down & PAD_BUTTON_RIGHT  ) p->worms_action |= ACTION_R_ACT; 
	  if( gc_buttons_down & PAD_TRIGGER_L     ) p->worms_action |= ACTION_L_ACT; 

    }
    break;
  }
}

/* Wii event control */
static __inline__ void game_check_event(game_t* g){ 
  WPAD_ScanPads();
  PAD_ScanPads();
  handle_event(WPAD_CHAN_0,g->worms[PLAYER_1]);
  handle_event(WPAD_CHAN_1,g->worms[PLAYER_2]);
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
  
  /* * * * * * * * * * * * * WIIMOTE ONLY * * * * * * * * * * * * * * * * * */
  if(!(p->worms_status & (STATUS_NUNCHUCK_ON | STATUS_CLASSIC_ON | STATUS_GAMECUBE_ON))){
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
  }else{
    /* * * * * * * * * * * * * NUNCHUCK * * * * * * * * * * * * * * * * * */
      /* Up & Down events */
      if((p->worms_action & ACTION_UP)){
        player_look_up(p);
      }else{
        if((p->worms_action & ACTION_DOWN))
          player_look_down(p);
        else p->reticle_pitch = ANGLE_PITCH;
      }
  
      /* change */
      if(p->worms_action & ACTION_L_ACT){
        p->worms_status |= STATUS_SHOW_W;
        player_change(p,1);
      }
      if(p->worms_action & ACTION_R_ACT){
        p->worms_status |= STATUS_SHOW_W;
        player_change(p,-1);
      }
      if(p->worms_action & ACTION_CHANGE){
        p->worms_status |= STATUS_SHOW_W;
      }

      if( p->worms_action & ACTION_HOOK ){
        if(!(p->worms_status & STATUS_NINJA_ACTION)){
          if(p->ninja_hook->last_bullet==0l){/* Launch */
            player_launch_hook(p);
          }else{/* Launch new */
            player_remove_hook(p,other_p);
            player_launch_hook(p);
          }
          p->worms_status |= STATUS_NINJA_ACTION;
        } 
      }
      /* NINJA HOOK */
      if(p->worms_action & ACTION_JUMP){
        player_jump(p);
        /* Remove hook */
        if(!(p->worms_status & STATUS_NINJA_ACTION)){
          if(p->ninja_hook->last_bullet!=0l){
             player_remove_hook(p,other_p);
          }
        }
      }
      p->worms_status &= ~STATUS_NINJA_ACTION;
  
      /* FIRE */
      if((p->worms_action & ACTION_FIRE))
        player_fire(p);  

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
}



#endif

