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


#include <unistd.h>
#include <errno.h>
extern int errno; 
static char str_errno[128];
#include "game.h"
#include "tools.h"
#include "frame_rate.h"
#include "fast_trig.h"
#include "weapons.h"
#include "dynamic_objs.h"
#ifdef PC_MODE
#include "input_pc.h"
#endif
#ifdef PSP_MODE
#include "input_psp.h"
#endif
#ifdef WII_MODE
#include "input_wii.h"
#endif
#ifdef SWITCH
#include "input_switch.h"
#endif

#include "font.h"
#include "cpu_player.h"
#include "surface_manip.h"
#include "colors.h"
#include "image.h"
#include "menu.h"
#include "lang.h"

#define DIRT_AMB     0
#define SAND_AMB     1
#define NIGTH_AMB    2
#define NB_AMBIENCES 3
#define WIIERO_STD_AMBIENCE DIRT_AMB

#define SCREEN_WIDTH  screen_get_width(g->wiiero_opt_screen_resolution)
#define SCREEN_HEIGHT screen_get_height(g->wiiero_opt_screen_resolution)
#define SCREEN_BPP    screen_get_bpp(g->wiiero_opt_screen_resolution)

#ifdef OLD_MODE
#define CAMERA_OFF(...) { \
  ASSERT(g); \
  camera_clean(__VA_ARGS__); \
  screen_clean(g->wiiero_screen); \
  camera_switch_off(__VA_ARGS__); \
};
#else
#define CAMERA_OFF(...) { \
  ASSERT(g); \
  skeep_current_frame = 1; \
  camera_clean(__VA_ARGS__); \
  camera_switch_off(__VA_ARGS__); \
};
#endif

int bullet_time_effect_delay;

extern int debug_flag;
extern int is_framerate_on;
static int skeep_current_frame = 0;
static __inline__ void wiiero_set_message(game_t* g,char* message,fontsize_t font,game_status_t next_status,Uint8 alpha);

player_score_t game_score[NB_PLAYERS];
static player_id winner_id;
static player_id name_to_change;
Uint8 selected_amb;
static Uint8 round_running;

Uint8 transparent_r_value=0x00;
Uint8 transparent_g_value=0x00;
Uint8 transparent_b_value=0x00;
Uint8 mud_particle = WIIERO_STD_MUB;

char game_nicknames[NB_PLAYERS][WIIERO_NAME_LEN]={
  {'P','1','\0','\0','\0','\0','\0','\0','\0','\0'},
  {'P','2','\0','\0','\0','\0','\0','\0','\0','\0'}};


void wiiero_init(game_t* g)
{
  ASSERT(g);  
   if(SDL_Init(SDL_INIT_VIDEO |
#ifndef NO_SOUND
               SDL_INIT_AUDIO |
#endif
#ifdef PSP_MODE
               SDL_INIT_JOYSTICK |
#endif
               0 ) == -1) {
    ERROR( "SDL init failed !!");
  }

#ifdef PSP_MODE
  if(SDL_NumJoysticks()>0)
   SDL_JoystickOpen(0);
#endif
  HARD_DBG("- init config\n");
  g->wiiero_game_status           = GAME_SET_MENU;
  g->wiiero_opt_game_mode         = GAME_DEATHMATCH_MODE;
  g->wiiero_opt_nb_lifes          = WIIERO_STD_LIFES;
  g->wiiero_opt_nb_flags          = WIIERO_STD_FLAGS;
  g->wiiero_opt_got_time          = WIIERO_STD_TIME;
  g->wiiero_opt_gift_nb           = WIIERO_STD_GIFT_NB;
  g->wiiero_opt_loading_time      = WIIERO_STD_LOADING_TIME;
  g->wiiero_opt_bonus_name        = WIIERO_STD_BONUS_NAMES;
  g->wiiero_opt_mini_map          = WIIERO_STD_MINI_MAP;
  g->wiiero_opt_map_regen         = WIIERO_STD_MAP_REGEN;
  g->wiiero_opt_shadow            = WIIERO_STD_SHADOW;
  g->wiiero_opt_xtra_weap         = WIIERO_STD_XTRA_W;
  selected_amb                    = WIIERO_STD_AMBIENCE;
  g->wiiero_opt_nb_rocks          = WIIERO_STD_NB_ROCKS;
  g->wiiero_opt_screen_resolution = WIIERO_STD_SCREEN_RESOLUTION;
  round_running = 0;
  
   
  /* Load user config */
  wiiero_load_config(g);
  HARD_DBG("- init screen\n");
  g->wiiero_screen = screen_init(g->wiiero_opt_screen_resolution);

  /* compute transparent values from current SDL pallet */
  image_loader_set_screen(g->wiiero_screen->surface);
  compute_trcolor(g->wiiero_screen->surface);
  get_trcolor(&transparent_r_value,&transparent_g_value,&transparent_b_value);
  

  font_load((g->wiiero_screen)->surface);
  font_console_print(WIIERO_VERSION"\n",FONT_SMALL);
  screen_loading_splash(g->wiiero_screen);
  font_progress_print(wiiero_label[WIIERO_LANG_GAME_LOAD],FONT_SMALL);
}/*--------------------------------------------------------------------------*/


void wiiero_load(game_t* g){
  ASSERT(g);  
  font_console_print_debug("Load config...\n",FONT_SMALL);
  if(!wiiero_load_config(g))
    font_console_print("Load config skipped\n",FONT_SMALL); // crashes here // try no sound;

  font_console_print("Load audio\n",FONT_SMALL);
  font_console_print_debug("init audio mixer\n",FONT_SMALL);
  if(sengine_init() != 0)
    font_console_print("audio mixer init failed !!!\n",FONT_SMALL);

  font_console_print("Audio Mixer Loaded!\n",FONT_SMALL);
#ifdef WII_MODE
  init_wiimotes();
#endif
  ftrigo_check(MIN_OBJ_ANGLE,MAX_OBJ_ANGLE);
  font_console_print_debug("init video layers...\n",FONT_SMALL);
  g->wiiero_exit = 0;
  g->wiiero_frame_delay = (int)((1000.0 / WANTED_FPS) + 0.5 );
  g->wiiero_dynamic_objects = object_init_list();
  g->wiiero_bullets = weapon_init_bullet_list();
  HARD_DBG("- init cameras\n");
  g->wiiero_cameras[FULL_SCREEN_CAM]
     = screen_add_custom_camera( g->wiiero_screen
                               ,0,0
                               , SCREEN_WIDTH,SCREEN_HEIGHT
                               , SCREEN_BPP);
  g->wiiero_cameras[PLAYER_1_GAME_ZONE_CAM]
     = screen_add_custom_camera( g->wiiero_screen
                               , 0, 0
                               , SCREEN_WIDTH/2-1, 3*SCREEN_HEIGHT/4
                               , SCREEN_BPP);
  g->wiiero_cameras[PLAYER_2_GAME_ZONE_CAM]
     = screen_add_custom_camera( g->wiiero_screen
                               , SCREEN_WIDTH/2+1, 0
                               , SCREEN_WIDTH/2-1, 3*SCREEN_HEIGHT/4
                               , SCREEN_BPP);
  g->wiiero_cameras[GLOBAL_MINI_MAP_CAM]
     = screen_add_custom_camera( g->wiiero_screen
                               , (2*SCREEN_WIDTH)/5, (25*SCREEN_HEIGHT)/32
                               , SCREEN_WIDTH/5, SCREEN_HEIGHT/5
                               , SCREEN_BPP);

  HARD_DBG("%dx%d\n",(2*SCREEN_WIDTH)/5,(25*SCREEN_HEIGHT)/32);

  g->wiiero_cameras[PLAYER_1_STATS_ZONE_CAM]
     = screen_add_custom_camera( g->wiiero_screen
                               , 0, 3*SCREEN_HEIGHT/4
                               , SCREEN_WIDTH/100.0*35, SCREEN_HEIGHT/4
                               , SCREEN_BPP);

  g->wiiero_cameras[PLAYER_2_STATS_ZONE_CAM]
     = screen_add_custom_camera( g->wiiero_screen
                               , SCREEN_WIDTH/100.0*60, 3*SCREEN_HEIGHT/4
                               , SCREEN_WIDTH/100.0*35, SCREEN_HEIGHT/4
                               , SCREEN_BPP);
  font_console_print_debug("init ressources...\n",FONT_SMALL);
  g->wiiero_ressources = load_ressource();
  font_console_print_debug("init map...\n",FONT_SMALL);
  g->wiiero_map = map_init(g->wiiero_ressources,g->wiiero_opt_nb_rocks
                           ,g->wiiero_opt_screen_resolution);
  font_console_print_debug("init game...\n",FONT_SMALL);
  g->worms[PLAYER_1] = player_init( PLAYER_1
                                  , g->wiiero_cameras[PLAYER_1_GAME_ZONE_CAM]
                                  , g->wiiero_cameras[PLAYER_1_STATS_ZONE_CAM]
                                  , g->wiiero_ressources, g->wiiero_bullets
                                  , g->wiiero_dynamic_objects
                                  , g->wiiero_map->layers[STATICS_MAP_LAYER]
                                  , g->wiiero_opt_xtra_weap );
  g->worms[PLAYER_2] = player_init(PLAYER_2
                                  , g->wiiero_cameras[PLAYER_2_GAME_ZONE_CAM]
                                  , g->wiiero_cameras[PLAYER_2_STATS_ZONE_CAM]
                                  , g->wiiero_ressources, g->wiiero_bullets
                                  , g->wiiero_dynamic_objects
                                  , g->wiiero_map->layers[STATICS_MAP_LAYER]
                                  , g->wiiero_opt_xtra_weap );
  map_drow_cave( g->wiiero_map
               , g->worms[PLAYER_1]->worms.pos_x
               , g->worms[PLAYER_1]->worms.pos_y
               , 20);
  map_drow_cave( g->wiiero_map
               , g->worms[PLAYER_2]->worms.pos_x
               , g->worms[PLAYER_2]->worms.pos_y
               , 20);
  framer_init();
  font_console_print_debug("Lets rock !\n",FONT_SMALL);
  SDL_Delay(500);
  screen_clean(g->wiiero_screen);
  CAMERA_OFF(g->wiiero_cameras[FULL_SCREEN_CAM]);
}/*--------------------------------------------------------------------------*/


int wiiero_alive(game_t* g){
  ASSERT(g);
  return (g->wiiero_exit == 0);
}/*--------------------------------------------------------------------------*/


static __inline__ void wiiero_lock_layers(game_t* g){
  lock_layer(g->wiiero_map->layers[GROUND_MAP_LAYER],"ground");
  lock_layer(g->wiiero_map->layers[STATICS_MAP_LAYER],"static");
}/*--------------------------------------------------------------------------*/

static __inline__ void wiiero_unlock_layers(game_t* g){
  unlock_layer(g->wiiero_map->layers[GROUND_MAP_LAYER]);
  unlock_layer(g->wiiero_map->layers[STATICS_MAP_LAYER]);
}/*--------------------------------------------------------------------------*/

void wiiero_update_world(game_t* g){
  /* UPDATE WIIERO GAME */
  ASSERT(g);

  wiiero_lock_layers(g);

  player_event_update(g->worms[PLAYER_1],g->wiiero_map,g->worms[PLAYER_2]);
  player_event_update(g->worms[PLAYER_2],g->wiiero_map,g->worms[PLAYER_1]);
  DBG(" - UPDATE DYNAMICS \n");
  proceed_dynamics_objs( g->wiiero_dynamic_objects
                       , g->wiiero_map->layers[GROUND_MAP_LAYER]
                       , g->wiiero_map->layers[STATICS_MAP_LAYER]
                       , g->wiiero_map->layers[GROUND_MAP_LAYER]);
  DBG(" - UPDATE PLAYERS DATA\n");
  player_update(g->worms[PLAYER_1], g->wiiero_map->layers[GROUND_MAP_LAYER]
               , g->wiiero_map->layers[STATICS_MAP_LAYER]);
  player_update(g->worms[PLAYER_2], g->wiiero_map->layers[GROUND_MAP_LAYER]
               , g->wiiero_map->layers[STATICS_MAP_LAYER]);
  DBG(" - UPDATE PLAYERS RETICLES\n");

  player_is_aiming(g->worms[PLAYER_1],g->worms[PLAYER_2]);
  player_is_aiming(g->worms[PLAYER_2],g->worms[PLAYER_1]);
  if(g->wiiero_opt_gift_nb)
    if((rand()%(2000/g->wiiero_opt_gift_nb)) == 0){
      int x=0, y=0;
      get_empty_layer_position(&x,&y,g->wiiero_map->layers[STATICS_MAP_LAYER]);
      create_gift(g->wiiero_bullets,g->wiiero_ressources,x,y,g->wiiero_opt_xtra_weap );
    }

  DBG(" - UPDATE BULLETS\n");
  proceed_bullets( g->wiiero_bullets,g->wiiero_map->layers[GROUND_MAP_LAYER]
                 , g->wiiero_map->layers[STATICS_MAP_LAYER]
                 , g->worms[PLAYER_1],g->worms[PLAYER_2], g->wiiero_map->layers[GROUND_MAP_LAYER]);

  wiiero_unlock_layers(g);
}/*--------------------------------------------------------------------------*/

static __inline__ int wiiero_player_warning(player_id player,game_t* g){
  ASSERT(g);
  switch(g->wiiero_opt_game_mode){
    case GAME_DEATHMATCH_MODE:
          return ( game_score[player].nb_lifes < g->wiiero_opt_nb_lifes / 5);
          break;
    case GAME_OF_TAG_MODE:
          return ( game_score[player].tag_time < g->wiiero_opt_got_time / 5 );
          break;
    case GAME_CAPTURE_FLAG_MODE:
          return (player == PLAYER_1) 
                  ? (g->worms[PLAYER_2]->worms_status & STATUS_HAVE_FLAG)
                  : (g->worms[PLAYER_1]->worms_status & STATUS_HAVE_FLAG);
          break;
  }
  return 0;
}/*--------------------------------------------------------------------------*/

void wiiero_blit_world(game_t* g){
  /* BLIT WIIERO GAME */
  ASSERT(g);
  DBG(" - BLIT BG\n");
  camera_blit(g->worms[PLAYER_1]->worms_camera,g->wiiero_map->layers[BACKGROUND_LAYER]);
  camera_blit(g->worms[PLAYER_2]->worms_camera,g->wiiero_map->layers[BACKGROUND_LAYER]);
    
  DBG(" - BLIT FIRSTGROUND\n");
  if(g->wiiero_opt_shadow){
    camera_blit_shadow(g->worms[PLAYER_1]->worms_camera,g->wiiero_map->layers[GROUND_MAP_LAYER]);
    camera_blit_shadow(g->worms[PLAYER_2]->worms_camera,g->wiiero_map->layers[GROUND_MAP_LAYER]);
  }
  camera_blit(g->worms[PLAYER_1]->worms_camera,g->wiiero_map->layers[GROUND_MAP_LAYER]);
  camera_blit(g->worms[PLAYER_2]->worms_camera,g->wiiero_map->layers[GROUND_MAP_LAYER]);
  
  DBG(" - BLIT STATICS\n");
  camera_blit(g->worms[PLAYER_1]->worms_camera,g->wiiero_map->layers[STATICS_MAP_LAYER]);
  camera_blit(g->worms[PLAYER_2]->worms_camera,g->wiiero_map->layers[STATICS_MAP_LAYER]);

    
  DBG(" - BLIT CAM\n");
  player_show_on_cam(g->worms[PLAYER_1],g->wiiero_cameras[PLAYER_2_GAME_ZONE_CAM],wiiero_player_warning(PLAYER_1,g));
  player_show_on_cam(g->worms[PLAYER_2],g->wiiero_cameras[PLAYER_1_GAME_ZONE_CAM],wiiero_player_warning(PLAYER_2,g));

  DBG(" - BLIT PLAY\n");
  player_show(g->worms[PLAYER_1],wiiero_player_warning(PLAYER_1,g));
  player_show(g->worms[PLAYER_2],wiiero_player_warning(PLAYER_2,g));

  
  if(g->wiiero_game_status == GAME_PLAYING){
    DBG(" - BLIT BULLETS\n");
    blit_bullets( g->wiiero_bullets
                , g->wiiero_cameras[PLAYER_1_GAME_ZONE_CAM]
                , g->wiiero_cameras[PLAYER_2_GAME_ZONE_CAM]);

    DBG(" - BLIT DYNAMICS\n");
    blit_dynamics_objs( g->wiiero_dynamic_objects
                      , g->wiiero_cameras[PLAYER_1_GAME_ZONE_CAM]
                      , g->wiiero_cameras[PLAYER_2_GAME_ZONE_CAM]);
  }

  if(g->wiiero_opt_mini_map)
    update_minimap(g->wiiero_map,g->wiiero_cameras[GLOBAL_MINI_MAP_CAM]
                  , g->worms[PLAYER_1]->worms.pos_x
                  , g->worms[PLAYER_1]->worms.pos_y
                  , g->worms[PLAYER_2]->worms.pos_x
                  , g->worms[PLAYER_2]->worms.pos_y
                  ,( (g->worms[PLAYER_1]->worms_status & STATUS_STATS_UPDATE)
                   ||(g->worms[PLAYER_2]->worms_status & STATUS_STATS_UPDATE)));
    
  player_show_stats(g->worms[PLAYER_1],g->wiiero_opt_game_mode);
  player_show_stats(g->worms[PLAYER_2],g->wiiero_opt_game_mode);

}/*--------------------------------------------------------------------------*/


/************************
 ** WIIERO GAMING MODS **
 ******************************************************************************/
static __inline__ void wiiero_restart_game(game_t* g){
  player_id i;
  static int first = 0 ;
  for(i=PLAYER_1;i<NB_PLAYERS;i++){
    game_score[i].nb_lifes    = g->wiiero_opt_nb_lifes;
    game_score[i].nb_frags    = 0;
    game_score[i].nb_suicides = 0;
    game_score[i].nb_death    = 0;
    game_score[i].nb_flags    = 0;
    game_score[i].tag_time    = g->wiiero_opt_got_time;
  }
  if(first !=0){
    /* RESET MAP */
    if( g->wiiero_opt_map_regen )
      map_reset(g->wiiero_map,g->wiiero_ressources,g->wiiero_opt_nb_rocks);
    /* RESET PLAYERS */
    g->worms[PLAYER_1]->worms_status = DEFAULT_PLAYER_STATUS;
    g->worms[PLAYER_2]->worms_status = DEFAULT_PLAYER_STATUS;
    g->worms[PLAYER_1]->worms_action = DEFAULT_PLAYER_ACTIONS;
    g->worms[PLAYER_2]->worms_action = DEFAULT_PLAYER_ACTIONS;
    player_new_position(g->worms[PLAYER_1],g->wiiero_map->layers[GROUND_MAP_LAYER]
                , g->wiiero_map->layers[STATICS_MAP_LAYER]);
    player_new_position(g->worms[PLAYER_2],g->wiiero_map->layers[GROUND_MAP_LAYER]
                , g->wiiero_map->layers[STATICS_MAP_LAYER]);
    player_reset( g->worms[PLAYER_1],g->wiiero_map->layers[GROUND_MAP_LAYER]
                , g->wiiero_map->layers[STATICS_MAP_LAYER]);
    player_reset( g->worms[PLAYER_2],g->wiiero_map->layers[GROUND_MAP_LAYER]
                , g->wiiero_map->layers[STATICS_MAP_LAYER]);
    g->worms[PLAYER_1]->worms_status &= ~STATUS_RESETED;
    g->worms[PLAYER_2]->worms_status &= ~STATUS_RESETED;
  }

  if(g->wiiero_opt_game_mode == GAME_CAPTURE_FLAG_MODE){
    set_player_house(g->wiiero_bullets,g->wiiero_ressources,PLAYER_1,g->wiiero_map->layers[STATICS_MAP_LAYER]);
    set_player_house(g->wiiero_bullets,g->wiiero_ressources,PLAYER_2,g->wiiero_map->layers[STATICS_MAP_LAYER]);
    set_player_flag(g->wiiero_bullets,g->wiiero_ressources,PLAYER_1,g->wiiero_map->layers[STATICS_MAP_LAYER]);
    set_player_flag(g->wiiero_bullets,g->wiiero_ressources,PLAYER_2,g->wiiero_map->layers[STATICS_MAP_LAYER]);
  }
  first=1;
  bullet_time_effect_delay=0;
}/*--------------------------------------------------------------------------*/


static __inline__ void wiiero_got_game_mode(game_t* g){
  static int wiiero_time_tag=0;

  if(  (g->worms[PLAYER_1]->worms_status & STATUS_RESETED)
     &&(g->worms[PLAYER_2]->worms_status & STATUS_RESETED)){
     /* random tag */
     player_id tagged_player_id = rand()%NB_PLAYERS;
     g->worms[tagged_player_id]->worms_status |= STATUS_TAGGED;
     tagged_player_id = (tagged_player_id == PLAYER_1) ? PLAYER_2: PLAYER_1;
     g->worms[tagged_player_id]->worms_status &= ~STATUS_TAGGED;
     wiiero_time_tag = SDL_GetTicks();
  }else{
    if((g->worms[PLAYER_1]->worms_status & STATUS_RESETED)
       && (!(g->worms[PLAYER_1]->worms_status & STATUS_TAGGED))){
     /* tag player 1 */
     g->worms[PLAYER_1]->worms_status |= STATUS_TAGGED;
     g->worms[PLAYER_2]->worms_status &= ~STATUS_TAGGED;
     wiiero_time_tag = SDL_GetTicks();
    }
    if((g->worms[PLAYER_2]->worms_status & STATUS_RESETED)
       && (!(g->worms[PLAYER_2]->worms_status & STATUS_TAGGED))){
     /* tag player 2 */
     g->worms[PLAYER_2]->worms_status |= STATUS_TAGGED;
     g->worms[PLAYER_1]->worms_status &= ~STATUS_TAGGED;
     wiiero_time_tag = SDL_GetTicks();
    }
  }

  if((g->worms[PLAYER_1]->worms_status & STATUS_TAGGED)
   &&(g->worms[PLAYER_1]->worms_status & STATUS_ALIVE)){
    if(SDL_GetTicks() - wiiero_time_tag > 1000){
      game_score[PLAYER_1].tag_time--;
      wiiero_time_tag = SDL_GetTicks();
    }
  }
  if((g->worms[PLAYER_2]->worms_status & STATUS_TAGGED)
   &&(g->worms[PLAYER_2]->worms_status & STATUS_ALIVE)){
    if(SDL_GetTicks() - wiiero_time_tag > 1000){
      game_score[PLAYER_2].tag_time--;
      wiiero_time_tag = SDL_GetTicks();
    }
  }

  if(  (game_score[PLAYER_1].tag_time == 0)
    && (game_score[PLAYER_2].tag_time == 0)){
    /* equal */
      winner_id = GAME_DRAW;
    g->wiiero_game_status = GAME_SET_ROUND_STATS;
  }else{
    if(game_score[PLAYER_1].tag_time == 0){
      /* p2 win */
      winner_id = PLAYER_2;
      g->wiiero_game_status = GAME_SET_ROUND_STATS;
    }
    if(game_score[PLAYER_2].tag_time == 0){
      /* p1 win */
      winner_id = PLAYER_1;
      g->wiiero_game_status = GAME_SET_ROUND_STATS;
    }
  }
}/*--------------------------------------------------------------------------*/

static __inline__ void wiiero_deathm_game_mode(game_t* g){

  if(  ((game_score[PLAYER_1].nb_lifes == 0) && (g->worms[PLAYER_1]->worms_status & STATUS_ALIVE )) 
    && ((game_score[PLAYER_2].nb_lifes == 0) && (g->worms[PLAYER_2]->worms_status & STATUS_ALIVE ))){
    /* draw */
    winner_id = GAME_DRAW;
    g->wiiero_game_status = GAME_SET_ROUND_STATS;
  }else{
    if((game_score[PLAYER_1].nb_lifes == 0) && (g->worms[PLAYER_1]->worms_status & STATUS_ALIVE )){ 
      /* p2 win */
      winner_id = PLAYER_2;
      g->wiiero_game_status = GAME_SET_ROUND_STATS;
    }
    if((game_score[PLAYER_2].nb_lifes == 0) && (g->worms[PLAYER_2]->worms_status & STATUS_ALIVE )){ 
      /* p1 win */
      winner_id = PLAYER_1;
      g->wiiero_game_status = GAME_SET_ROUND_STATS;
    }
  }
}/*--------------------------------------------------------------------------*/

static __inline__ void wiiero_cflag_game_mode(game_t* g){
  if(game_score[PLAYER_1].nb_flags == g->wiiero_opt_nb_flags){ 
    /* p1 win */
    winner_id = PLAYER_1;
    g->wiiero_game_status = GAME_SET_ROUND_STATS;
  }
  if(game_score[PLAYER_2].nb_flags == g->wiiero_opt_nb_flags){ 
    /* p2 win */
    winner_id = PLAYER_2;
    g->wiiero_game_status = GAME_SET_ROUND_STATS;
  }
}
/******************************************************************************/




/******************
 ** WIIERO MENUs **
 ******************************************************************************/

/* ----------------------------- WEAPON_SELECT ------------------------------ */
/* Prepare Weapon selection status */
static __inline__ void wiiero_set_wselect(game_t* g){ 


  if(!are_controls_ready()){
    wiiero_set_message(g,wiiero_label[WIIERO_LANG_WARNING_CONNECT],FONT_STANDARD,GAME_MENU,230);
    return;
  }


  g->wiiero_game_status = GAME_WEAPON_SELECT;
  player_reset_weapons(g->worms[PLAYER_1],g->wiiero_opt_xtra_weap);
  player_reset_weapons(g->worms[PLAYER_2],g->wiiero_opt_xtra_weap);
  round_running = 1;
  clean_bullets_list(g->wiiero_bullets); /* reset game case */
  g->worms[PLAYER_1]->worms_status |= STATUS_STATS_UPDATE;
  g->worms[PLAYER_2]->worms_status |= STATUS_STATS_UPDATE;
}/*---------------------------------------------------------------------------*/
static __inline__ void wiiero_show_weapons(game_t* g,player_id id){
   int i=0;
   int camera_id = (id == PLAYER_1) ? PLAYER_1_GAME_ZONE_CAM
                                    : PLAYER_2_GAME_ZONE_CAM;
   font_print_strict_pos(g->wiiero_cameras[camera_id] 
                        ,wiiero_label[WIIERO_LANG_WSELECT_LABEL]
                        , g->wiiero_cameras[camera_id]->w/10
                        , g->wiiero_cameras[camera_id]->h/4 
                        , FONT_STANDARD);
   for(i=0;i<WEAPON_SLOTS;i++){
     font_print_strict_pos(g->wiiero_cameras[camera_id] 
                          , weapon_get_name( g->worms[id]->weapon_slots[i]->id ) 
                          , g->wiiero_cameras[camera_id]->w/10 + 10
                          , g->wiiero_cameras[camera_id]->h/4 + (i+2)*10
                          , (g->worms[id]->selected_weapon == i) ? FONT_SELECTED : FONT_STANDARD);
   }

   if(g->worms[id]->worms_action == ACTION_UP )
     g->worms[id]->selected_weapon = (g->worms[id]->selected_weapon > 0) 
                                     ? g->worms[id]->selected_weapon-1 
                                     : WEAPON_SLOTS -1 ;
   if(g->worms[id]->worms_action == ACTION_DOWN )
     g->worms[id]->selected_weapon = (g->worms[id]->selected_weapon < WEAPON_SLOTS-1) 
                                     ? g->worms[id]->selected_weapon+1 : 0 ;

   if(g->worms[id]->worms_action == ACTION_LEFT ){
     int max_weap = (g->wiiero_opt_xtra_weap)? MAX_WEAPONS_AND_XTRA : MAX_WEAPONS;
     int wid = (g->worms[id]->weapon_slots[g->worms[id]->selected_weapon])->id;
     wid = (wid > 0) ? wid-1 : max_weap-1;
     secure_free(g->worms[id]->weapon_slots[g->worms[id]->selected_weapon]);
     g->worms[id]->weapon_slots[g->worms[id]->selected_weapon] = weapon_init(wid);
   }

   if(g->worms[id]->worms_action == ACTION_RIGHT ){
     int max_weap = (g->wiiero_opt_xtra_weap)? MAX_WEAPONS_AND_XTRA : MAX_WEAPONS;
     int wid = (g->worms[id]->weapon_slots[g->worms[id]->selected_weapon])->id;
     wid = (wid < max_weap-1) ? wid+1 : 0;
     secure_free(g->worms[id]->weapon_slots[g->worms[id]->selected_weapon]);
     g->worms[id]->weapon_slots[g->worms[id]->selected_weapon] = weapon_init(wid);
   }
   if(g->worms[id]->worms_action & ACTION_CANCEL ){
     for(i=0;i<WEAPON_SLOTS;i++){
       secure_free(g->worms[id]->weapon_slots[i]);
       if(g->wiiero_opt_xtra_weap) g->worms[id]->weapon_slots[i] = weapon_init(rand()%MAX_WEAPONS_AND_XTRA);
       else g->worms[id]->weapon_slots[i] = weapon_init(rand()%MAX_WEAPONS);
     }
   }
   
  font_print_strict_pos( g->wiiero_cameras[camera_id],wiiero_label[WIIERO_LANG_WSELECT_KO]
                  , g->wiiero_cameras[camera_id]->w/10
                  , 3*g->wiiero_cameras[camera_id]->h/4
                  , FONT_STANDARD );
  font_print_strict_pos( g->wiiero_cameras[camera_id],wiiero_label[WIIERO_LANG_WSELECT_OK]
              , g->wiiero_cameras[camera_id]->w/10
              , 3*g->wiiero_cameras[camera_id]->h/4 + (font_get_height(FONT_STANDARD) + 2)
              , FONT_STANDARD );
}/*--------------------------------------------------------------------------*/
/* Weapon selection */
static __inline__  void wiiero_wselect(game_t* g){
  static Uint8 selection_done[2] = {0,0};
  wiiero_blit_world(g);
  if(selection_done[PLAYER_1]==0)
    wiiero_show_weapons(g,PLAYER_1);
  if(selection_done[PLAYER_2]==0)
    wiiero_show_weapons(g,PLAYER_2);
  if(g->worms[PLAYER_1]->worms_action & ACTION_OK ){
    selection_done[PLAYER_1] = 1;
#ifdef PSP_MODE
    selection_done[PLAYER_2] = 1;
#endif
  }
  if(g->worms[PLAYER_2]->worms_action & ACTION_OK )
    selection_done[PLAYER_2] = 1;

  if( (selection_done[PLAYER_1])
    &&(selection_done[PLAYER_2])){
     /*FINISH*/
    selection_done[PLAYER_1]=0;
    selection_done[PLAYER_2]=0;
    g->worms[PLAYER_1]->worms_action = ACTION_NONE;
    g->worms[PLAYER_2]->worms_action = ACTION_NONE;
    g->wiiero_game_status = GAME_SET_PLAYING;
  }
  SDL_Delay(50);
}/*---------------------------------------------------------------------------*/



/* --------------------------------- Menu ---------------------------------- */
static __inline__ void wiiero_set_menu(game_t* g){ 
  //camera_switch_off(g->wiiero_cameras[FULL_SCREEN_CAM]);
  CAMERA_OFF(g->wiiero_cameras[FULL_SCREEN_CAM]);
  g->wiiero_game_status = GAME_MENU;
  g->worms[PLAYER_1]->worms_status |= STATUS_STATS_UPDATE;
  g->worms[PLAYER_2]->worms_status |= STATUS_STATS_UPDATE;
}

static __inline__ void wiiero_menu(game_t* g){
  int i=0;
  static int selected_menu_entry=MENU_PLAY;
  menu_enty_t wiiero_menu[MENU_MAX] = {
    {MENU_PLAY   ,wiiero_label[WIIERO_LANG_MENU_PLAY], GAME_SET_WEAPON_SELECT},
    {MENU_OPTION ,wiiero_label[WIIERO_LANG_MENU_OPTI], GAME_SET_OPTIONS },
    {MENU_RESTART,wiiero_label[WIIERO_LANG_MENU_REST], GAME_SET_WEAPON_SELECT},
    {MENU_LOAD   ,wiiero_label[WIIERO_LANG_MENU_LOAD], GAME_LOAD_CONFIG},
    {MENU_SAVE   ,wiiero_label[WIIERO_LANG_MENU_SAVE], GAME_SAVE_CONFIG},
    {MENU_HELP   ,wiiero_label[WIIERO_LANG_MENU_HELP], GAME_HOW_TO_PLAY},
    {MENU_INFO   ,wiiero_label[WIIERO_LANG_MENU_INFO], GAME_ABOUT},
    {MENU_RETURN ,wiiero_label[WIIERO_LANG_MENU_RETU], GAME_PLAYING},
    {MENU_EXIT   ,wiiero_label[WIIERO_LANG_MENU_EXIT], GAME_EXIT}};
  wiiero_blit_world(g); 

  for(i=0;i<MENU_MAX;i++){
    if(i != selected_menu_entry)
     font_print_center(g->wiiero_cameras[PLAYER_1_GAME_ZONE_CAM] 
                , wiiero_menu[i].label
                , g->wiiero_cameras[PLAYER_1_GAME_ZONE_CAM]->w/2
                , g->wiiero_cameras[PLAYER_1_GAME_ZONE_CAM]->h/6 + (i*15) 
                , FONT_STANDARD);
    else
      font_print_center(g->wiiero_cameras[PLAYER_1_GAME_ZONE_CAM] 
                , wiiero_menu[i].label
                , g->wiiero_cameras[PLAYER_1_GAME_ZONE_CAM]->w/2
                , g->wiiero_cameras[PLAYER_1_GAME_ZONE_CAM]->h/6 + (i*15) 
                , FONT_SELECTED);
  }
  if(g->wiiero_game_status == GAME_MENU){
    if(g->worms[PLAYER_1]->worms_action == ACTION_UP 
      || g->worms[PLAYER_2]->worms_action == ACTION_UP)
      selected_menu_entry = (selected_menu_entry == 0) ? MENU_MAX-1
                                                       : (selected_menu_entry-1);
      if(g->worms[PLAYER_1]->worms_action == ACTION_DOWN 
      || g->worms[PLAYER_2]->worms_action == ACTION_DOWN)
      selected_menu_entry = (selected_menu_entry+1)%MENU_MAX;

    if(g->worms[PLAYER_1]->worms_action & ACTION_CANCEL 
      || g->worms[PLAYER_2]->worms_action & ACTION_CANCEL)
      selected_menu_entry = MENU_RETURN;

    if(g->worms[PLAYER_1]->worms_action & ACTION_OK 
      || g->worms[PLAYER_2]->worms_action & ACTION_OK){
      if(round_running && wiiero_menu[selected_menu_entry].id == MENU_PLAY)
        wiiero_set_message(g,wiiero_label[WIIERO_LANG_WARNING_INGAME],FONT_STANDARD,GAME_MENU,230);
      else
        if(!round_running && wiiero_menu[selected_menu_entry].id == MENU_RETURN)
          wiiero_set_message(g,wiiero_label[WIIERO_LANG_WARNING_NOGAME],FONT_STANDARD,GAME_MENU,230);
        else
          g->wiiero_game_status = wiiero_menu[selected_menu_entry].target;
    }
    g->worms[PLAYER_1]->worms_action = ACTION_NONE;
    g->worms[PLAYER_2]->worms_action = ACTION_NONE;
    SDL_Delay(60);
  }
}/*---------------------------------------------------------------------------*/


/* --------------------------- Option Callbacks ---------------------------- */
void option_menu_cb(game_t* g){
  g->wiiero_game_status = GAME_OPTIONS;
}/*--------------------------------------------------------------------------*/

void option_menu_bis_cb(game_t* g){
  g->wiiero_game_status = GAME_OPTIONS_BIS;
}/*--------------------------------------------------------------------------*/

void option_return_cb(game_t* g){
  g->wiiero_game_status = GAME_SET_MENU;
  weapon_set_options(g->wiiero_opt_bonus_name,g->wiiero_opt_loading_time);
  framer_on();
}/*--------------------------------------------------------------------------*/

void option_change_p1_name_cb(game_t* g){
  name_to_change = PLAYER_1;
  g->wiiero_game_status = GAME_SET_EDIT_NAME;
}/*--------------------------------------------------------------------------*/

void option_change_res_cb(game_t* g){
  /* if screen resolution il higth, disable shadow
     in order to preserve framerate */
  if(g->wiiero_opt_screen_resolution == RES_640_420) 
    g->wiiero_opt_shadow = 0;
  else
    g->wiiero_opt_shadow = 1;
  wiiero_save_config(g);
  wiiero_set_message(g,wiiero_label[WIIERO_LANG_WARNING_RESTART],FONT_STANDARD,GAME_OPTIONS,230);
}/*--------------------------------------------------------------------------*/

void option_change_p2_name_cb(game_t* g){
  name_to_change = PLAYER_2;
  g->wiiero_game_status = GAME_SET_EDIT_NAME;
}/*--------------------------------------------------------------------------*/

void option_regen_new_map(game_t* g){
    map_reset(g->wiiero_map,g->wiiero_ressources,g->wiiero_opt_nb_rocks);
    clean_bullets_list(g->wiiero_bullets);
}/*--------------------------------------------------------------------------*/

char* option_format_string_cb(char* label,void* data){
  static char string[128];
  snprintf(string,127,"%s %s",label,(char*)data);
  return string;
}/*--------------------------------------------------------------------------*/

char* option_format_gamemode_cb(char* label,void* data){
  static char string[128];
  switch(*((game_mode_t*)data)){
    case GAME_DEATHMATCH_MODE: snprintf(string,127,"%s %s",label,wiiero_label[WIIERO_LANG_DMA]); break;
    case GAME_OF_TAG_MODE: snprintf(string,127,"%s %s",label,wiiero_label[WIIERO_LANG_GOT]); break;
    case GAME_CAPTURE_FLAG_MODE: snprintf(string,127,"%s %s",label,wiiero_label[WIIERO_LANG_CTF]); break;
  }
  return string;
}/*--------------------------------------------------------------------------*/

char* option_format_value_cb(char* label,void* data){
  static char string[128];
  snprintf(string,127,"%s %d",label,(*((Uint8*)data)));
  return string;
}/*--------------------------------------------------------------------------*/

char* option_format_bigvalue_cb(char* label,void* data){
  static char string[128];
  snprintf(string,127,"%s %d",label,(*((Uint16*)data)));
  return string;
}/*--------------------------------------------------------------------------*/

char* option_format_time_cb(char* label,void* data){
  static char string[128];
  snprintf( string,127,"%s %.2dm%.2ds",label
          ,(*((Uint16*)data))/60,(*((Uint16*)data))%60);
  return string;
}/*--------------------------------------------------------------------------*/

char* option_format_activ_cb(char* label,void* data){
  static char string[128];
  snprintf(string,127,"%s %s",label,(*((Uint8*)data) == 0) 
                                    ? wiiero_label[WIIERO_LANG_INFO_OFF]
                                    : wiiero_label[WIIERO_LANG_INFO_ON]);
  return string;
}/*--------------------------------------------------------------------------*/

/* * [lang callbacks] * */
char* option_format_lang_cb(char* label,void* data){
  static char string[128];
  snprintf(string,127,"%s %s",label,(get_nb_loaded_lang_files() == 0) 
                                    ? "standard"
                                    : wiiero_lang_files[*((Uint8*)data)]);
  return string; 
}/*--------------------------------------------------------------------------*/

void option_apply_lang_cb(game_t* g){
  if(get_nb_loaded_lang_files()==0)
        return;
  apply_lang(wiiero_lang_files[g->wiiero_lang]);
}/*--------------------------------------------------------------------------*/

void option_llang_cb(void* val){
  if(get_nb_loaded_lang_files()==0)
        return;
  if(*((Uint8*)val) == 0)
   (*((Uint8*)val)) = get_nb_loaded_lang_files()-1;
  else
   (*((Uint8*)val)) -= 1;
}/*--------------------------------------------------------------------------*/

void option_rlang_cb(void* val){
  if(get_nb_loaded_lang_files()==0)
        return;
  (*((Uint8*)val)) += 1;
  if(*((Uint8*)val) == get_nb_loaded_lang_files())
    (*((Uint8*)val)) = 0;
}/*--------------------------------------------------------------------------*/
/* * [/lang callbacks] * */

void option_laction_gmode_cb(void* val){
  game_mode_t* data = (game_mode_t*)val;
  *data = (*data == 0)? GAME_CAPTURE_FLAG_MODE : *data -1;
}/*--------------------------------------------------------------------------*/

void option_raction_gmode_cb(void* val){
  game_mode_t* data = (game_mode_t*)val;
  *data = (*data+1)%(GAME_CAPTURE_FLAG_MODE+1);
}/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
void option_laction_bigvalue_cb(void* val){
  (*((Uint16*)val)) -=  (*((Uint16*)val) == 1 ) ? 0 : 1;
}

void option_raction_bigvalue_cb(void* val){
  (*((Uint16*)val)) +=  (*((Uint16*)val) == 255)? 0 : 1;
}

void option_laction_value_cb(void* val){
  (*((Uint8*)val)) -=  (*((Uint8*)val) == 0 ) ? 0 : 1;
}

void option_raction_value_cb(void* val){
  (*((Uint8*)val)) +=  (*((Uint8*)val) == 255)? 0 : 1;
}

void option_laction_time_cb(void* val){
  (*((Uint16*)val))-= ((*((Uint16*)val)) > 30) ? 30 : 0;
}

void option_raction_time_cb(void* val){
  (*((Uint16*)val))+= ((*((Uint16*)val)) < 900) ? 30 : 0;
}

void option_laction_res_cb(void* val){
  (*((screen_res_t*)val))=RES_320_240;
}

void option_raction_res_cb(void* val){
  (*((screen_res_t*)val))=RES_640_420;
}

void option_laction_option_cb(void* val){
  (*((Uint8*)val))=0;
}

void option_raction_option_cb(void* val){
  (*((Uint8*)val))=1;
}

void option_laction_ambi_value_cb(void* val){
  (*((Uint8*)val))= ((*((Uint8*)val)) == NB_AMBIENCES-1 ) ? 0 : ((*((Uint8*)val))+1);
}

void option_raction_ambi_value_cb(void* val){
  (*((Uint8*)val))= ((*((Uint8*)val)) == 0 ) ? NB_AMBIENCES-1 : ((*((Uint8*)val))-1);
}/*--------------------------------------------------------------------------*/

char* option_format_ambi_value_cb(char* label,void* data){
  static char string[128];
  static char *ambi_name[3];
  ambi_name[0] =  wiiero_label[WIIERO_LANG_OPT_DIRT];
  ambi_name[1] =  wiiero_label[WIIERO_LANG_OPT_SAND];
  ambi_name[2] =  wiiero_label[WIIERO_LANG_OPT_NIGHT];
  snprintf(string,127,"%s %s",label,ambi_name[(*((Uint8*)data))]);
  return string;
}/*--------------------------------------------------------------------------*/


char* option_format_resol_cb(char* label,void* data){
  static char string[128];
  snprintf(string,127,"%s %dx%d",label,screen_get_width(*((screen_res_t*)data))
                                ,screen_get_height(*((screen_res_t*)data)));
  return string;
}/* ------------------------------------------------------------------------- */

/* ------------------------- Option Action Handler -------------------------- */
static __inline__ void wiiero_option_actions( game_t* g
                                            , int * selected_option_entry
                                            , const int OPTION_RETURN
                                            , const int OPTION_MAX
                                            , const option_enty_t *wiiero_option){
  if(g->worms[PLAYER_1]->worms_action == ACTION_UP 
    || g->worms[PLAYER_2]->worms_action == ACTION_UP)
    *selected_option_entry = (*selected_option_entry == 0) 
                            ? OPTION_MAX-1
                            : (*selected_option_entry-1);
  if(g->worms[PLAYER_1]->worms_action == ACTION_DOWN 
    || g->worms[PLAYER_2]->worms_action == ACTION_DOWN)
    *selected_option_entry = (*selected_option_entry+1)%OPTION_MAX;

  if((g->worms[PLAYER_1]->worms_action == ACTION_LEFT
      || g->worms[PLAYER_2]->worms_action == ACTION_LEFT)
    && wiiero_option[*selected_option_entry].option_on_left)
    wiiero_option[*selected_option_entry].option_on_left(wiiero_option[*selected_option_entry].option_data);

  if((g->worms[PLAYER_1]->worms_action == ACTION_RIGHT
      || g->worms[PLAYER_2]->worms_action == ACTION_RIGHT)
    && wiiero_option[*selected_option_entry].option_on_right)
    wiiero_option[*selected_option_entry].option_on_right(wiiero_option[*selected_option_entry].option_data);

  if((g->worms[PLAYER_1]->worms_action & ACTION_OK 
      || g->worms[PLAYER_2]->worms_action & ACTION_OK)
    && wiiero_option[*selected_option_entry].option_action)
    wiiero_option[*selected_option_entry].option_action(g);

  if(g->worms[PLAYER_1]->worms_action & ACTION_CANCEL 
      || g->worms[PLAYER_2]->worms_action & ACTION_CANCEL)
      *selected_option_entry = OPTION_RETURN;

  SDL_Delay(100);
  /* checks */
  if(g->wiiero_opt_loading_time == 0)
    g->wiiero_opt_loading_time = 1;
}/* -------------------------------------------------------------------------- */


/* ------------------------------ Option Menu ------------------------------- */
static __inline__ void wiiero_set_option(game_t* g){ 
  //camera_switch_off(g->wiiero_cameras[FULL_SCREEN_CAM]);
  CAMERA_OFF(g->wiiero_cameras[FULL_SCREEN_CAM]);
  g->wiiero_game_status = GAME_OPTIONS;
  framer_off();
  g->worms[PLAYER_1]->worms_status |= STATUS_STATS_UPDATE;
  g->worms[PLAYER_2]->worms_status |= STATUS_STATS_UPDATE;
}/*--------------------------------------------------------------------------*/
static __inline__ void wiiero_option(game_t* g){
  int i=0;
  static int selected_option_entry=OPTION_MODE;
  option_enty_t wiiero_option[OPTION_MAX] = {
    {OPTION_MODE     ,wiiero_label[WIIERO_LANG_OPT_MENU_GMODE],&(g->wiiero_opt_game_mode),option_format_gamemode_cb,option_laction_gmode_cb,option_raction_gmode_cb,0l},
    {OPTION_LIVES    ,wiiero_label[WIIERO_LANG_OPT_MENU_LIVES],&(g->wiiero_opt_nb_lifes),option_format_bigvalue_cb,option_laction_bigvalue_cb,option_raction_bigvalue_cb,0L},
    {OPTION_FLAGS    ,wiiero_label[WIIERO_LANG_OPT_MENU_FLAGS],&(g->wiiero_opt_nb_flags),option_format_bigvalue_cb,option_laction_bigvalue_cb,option_raction_bigvalue_cb,0L},
    {OPTION_TIME     ,wiiero_label[WIIERO_LANG_OPT_MENU_COUNT],&(g->wiiero_opt_got_time),option_format_time_cb,option_laction_time_cb,option_raction_time_cb,0L},
    {OPTION_RELOAD   ,wiiero_label[WIIERO_LANG_OPT_MENU_RLTIM],&(g->wiiero_opt_loading_time),option_format_value_cb,option_laction_value_cb,option_raction_value_cb,0L},
    {OPTION_NB_BONUS ,wiiero_label[WIIERO_LANG_OPT_MENU_BONUS],&(g->wiiero_opt_gift_nb),option_format_value_cb,option_laction_value_cb,option_raction_value_cb,0L},
    {OPTION_BONUS_NM ,wiiero_label[WIIERO_LANG_OPT_MENU_BNAME],&(g->wiiero_opt_bonus_name),option_format_activ_cb,option_laction_option_cb,option_raction_option_cb,0L},
    {OPTION_LEVEL    ,wiiero_label[WIIERO_LANG_OPT_MENU_RESOL],&(g->wiiero_opt_screen_resolution),option_format_resol_cb,option_laction_res_cb,option_raction_res_cb,option_change_res_cb},
    {OPTION_LEV_REG  ,wiiero_label[WIIERO_LANG_OPT_MENU_REGEN],&(g->wiiero_opt_map_regen),option_format_activ_cb,option_laction_option_cb,option_raction_option_cb,0L},
    {OPTION_LEV_ROCKS,wiiero_label[WIIERO_LANG_OPT_MENU_ROCKS],&(g->wiiero_opt_nb_rocks),option_format_value_cb,option_laction_value_cb,option_raction_value_cb,option_regen_new_map},
    {OPTION_LEV_AMBI ,wiiero_label[WIIERO_LANG_OPT_MENU_AMBIE],&selected_amb,option_format_ambi_value_cb,option_laction_ambi_value_cb,option_raction_ambi_value_cb,option_regen_new_map},
    {OPTION_SHADOWS  ,wiiero_label[WIIERO_LANG_OPT_MENU_SHADO],&(g->wiiero_opt_shadow),option_format_activ_cb,option_laction_option_cb,option_raction_option_cb,0L},
    {OPTION_P1_NAME  ,wiiero_label[WIIERO_LANG_OPT_MENU_P1NAM],game_nicknames[PLAYER_1],option_format_string_cb,0L,0L,option_change_p1_name_cb},
    {OPTION_P2_NAME  ,wiiero_label[WIIERO_LANG_OPT_MENU_P2NAM],game_nicknames[PLAYER_2],option_format_string_cb,0L,0L,option_change_p2_name_cb},
    {OPTION_BIS_MENU ,wiiero_label[WIIERO_LANG_OPT_MENU_NEXT] ,0L,0L,0L,0L,option_menu_bis_cb},
    {OPTION_RETURN   ,wiiero_label[WIIERO_LANG_OPT_MENU_RETUR],0L,0L,0L,0L,option_return_cb}
  };
  wiiero_menu(g);
    
  for(i=0;i<OPTION_MAX;i++){
    if(i != selected_option_entry)
     font_print_center(g->wiiero_cameras[PLAYER_2_GAME_ZONE_CAM] 
                , ((wiiero_option[i].option_formater) 
                  ? wiiero_option[i].option_formater( wiiero_option[i].label
                                                , wiiero_option[i].option_data)
                  : wiiero_option[i].label)
                , g->wiiero_cameras[PLAYER_2_GAME_ZONE_CAM]->w/2 + 5
                , (g->wiiero_cameras[PLAYER_2_GAME_ZONE_CAM]->h - (OPTION_MAX*10)) / 2  + (i*10) + 5 
                , FONT_STANDARD);
    else
      font_print_center(g->wiiero_cameras[PLAYER_2_GAME_ZONE_CAM] 
                , ((wiiero_option[i].option_formater) 
                  ? wiiero_option[i].option_formater( wiiero_option[i].label
                                                , wiiero_option[i].option_data)
                  : wiiero_option[i].label)
                , g->wiiero_cameras[PLAYER_2_GAME_ZONE_CAM]->w/2 + 5
                , (g->wiiero_cameras[PLAYER_2_GAME_ZONE_CAM]->h - (OPTION_MAX*10)) / 2  + (i*10) + 5 
                , FONT_SELECTED );
  }
  wiiero_option_actions( g,&selected_option_entry,OPTION_RETURN,OPTION_MAX
                       , wiiero_option);
}
/*---------------------------------------------------------------------------*/

/* ---------------------------- Option Menu Bis ----------------------------- */

static __inline__ void wiiero_set_option_bis(game_t* g){ 
  //camera_switch_off(g->wiiero_cameras[FULL_SCREEN_CAM]);
  CAMERA_OFF(g->wiiero_cameras[FULL_SCREEN_CAM]);
  g->wiiero_game_status = GAME_OPTIONS_BIS;
  framer_off();
}/*--------------------------------------------------------------------------*/
    
static __inline__ void wiiero_option_bis(game_t* g){
  int i=0;
  static int selected_option_entry=OPTION_BIS_XTRA_W;
  option_enty_t wiiero_option[OPTION_BIS_MAX] = {
    {OPTION_BIS_XTRA_W ,wiiero_label[WIIERO_LANG_OPT_MENU_XTRAW],&(g->wiiero_opt_xtra_weap),option_format_activ_cb,option_laction_option_cb,option_raction_option_cb,0L},
    {OPTION_BIS_MUD    ,wiiero_label[WIIERO_LANG_OPT_MENU_MUB]  ,&mud_particle,option_format_activ_cb,option_laction_option_cb,option_raction_option_cb,0L},
    {OPTION_BIS_LANG   ,wiiero_label[WIIERO_LANG_OPT_MENU_LANG] ,&g->wiiero_lang,option_format_lang_cb,option_llang_cb,option_rlang_cb,option_apply_lang_cb},
    {OPTION_BIS_MENU   ,wiiero_label[WIIERO_LANG_OPT_MENU_PREV] ,0L,0L,0L,0L,option_menu_cb},
    {OPTION_BIS_RETURN ,wiiero_label[WIIERO_LANG_OPT_MENU_RETUR],0L,0L,0L,0L,option_return_cb}
  };
  wiiero_menu(g);
    
  for(i=0;i<OPTION_BIS_MAX;i++){
    if(i != selected_option_entry)
     font_print_center(g->wiiero_cameras[PLAYER_2_GAME_ZONE_CAM] 
                , ((wiiero_option[i].option_formater) 
                  ? wiiero_option[i].option_formater( wiiero_option[i].label
                                                , wiiero_option[i].option_data)
                  : wiiero_option[i].label)
                , g->wiiero_cameras[PLAYER_2_GAME_ZONE_CAM]->w/2 + 5
                , (g->wiiero_cameras[PLAYER_2_GAME_ZONE_CAM]->h - (OPTION_BIS_MAX*10)) / 2  + (i*10) + 5 
                , FONT_STANDARD);
    else
      font_print_center(g->wiiero_cameras[PLAYER_2_GAME_ZONE_CAM] 
                , ((wiiero_option[i].option_formater) 
                  ? wiiero_option[i].option_formater( wiiero_option[i].label
                                                , wiiero_option[i].option_data)
                  : wiiero_option[i].label)
                , g->wiiero_cameras[PLAYER_2_GAME_ZONE_CAM]->w/2 + 5
                , (g->wiiero_cameras[PLAYER_2_GAME_ZONE_CAM]->h - (OPTION_BIS_MAX*10)) / 2  + (i*10) + 5 
                , FONT_SELECTED );
  }
  wiiero_option_actions( g,&selected_option_entry,OPTION_BIS_RETURN
                       , OPTION_BIS_MAX,wiiero_option);
}/*---------------------------------------------------------------------------*/


static game_status_t next_stat;
/* --------------------------------- PAUSE ---------------------------------- */
/* Prepare pause status */
static __inline__ void wiiero_set_message(game_t* g,char* message,fontsize_t font,game_status_t next_status,Uint8 alpha){ 
  camera_switch_on(g->wiiero_cameras[FULL_SCREEN_CAM]);
  camera_clean(g->wiiero_cameras[FULL_SCREEN_CAM]);
  camera_set_alpha(g->wiiero_cameras[FULL_SCREEN_CAM],alpha);
  /* SHOW MESSAGE */
  font_print_center( g->wiiero_cameras[FULL_SCREEN_CAM], message
                   , g->wiiero_cameras[FULL_SCREEN_CAM]->w/2
                   , g->wiiero_cameras[FULL_SCREEN_CAM]->h/3
                   , font);
  font_print_center( g->wiiero_cameras[FULL_SCREEN_CAM],wiiero_label[WIIERO_LANG_WARNING_PRESSKEY]
                    , 3*g->wiiero_cameras[FULL_SCREEN_CAM]->w/4
                    , 5*g->wiiero_cameras[FULL_SCREEN_CAM]->h/8
                    , FONT_STANDARD );
  next_stat = next_status;
  g->wiiero_game_status = GAME_PAUSE;
  g->worms[PLAYER_1]->worms_action = ACTION_NONE;
  g->worms[PLAYER_2]->worms_action = ACTION_NONE;
  g->worms[PLAYER_1]->worms_status |= STATUS_STATS_UPDATE;
  g->worms[PLAYER_2]->worms_status |= STATUS_STATS_UPDATE;
}/*---------------------------------------------------------------------------*/
/* Pause */
static __inline__ void wiiero_pause(game_t* g){     
  SDL_Delay(100);
  if((g->worms[PLAYER_1]->worms_action != ACTION_NONE)||
     (g->worms[PLAYER_2]->worms_action != ACTION_NONE)){
     g->wiiero_game_status = next_stat;
     g->worms[PLAYER_1]->worms_action = ACTION_NONE;
     g->worms[PLAYER_2]->worms_action = ACTION_NONE;
     //camera_switch_off(g->wiiero_cameras[FULL_SCREEN_CAM]);
     CAMERA_OFF(g->wiiero_cameras[FULL_SCREEN_CAM]);
  }
}/*---------------------------------------------------------------------------*/



/* --------------------------------- Edit Name ---------------------------------- */
/* Prepare pause status */
static __inline__ void wiiero_set_edit_name(game_t* g){ 
  camera_switch_on(g->wiiero_cameras[FULL_SCREEN_CAM]);
  camera_clean(g->wiiero_cameras[FULL_SCREEN_CAM]);
  camera_set_alpha(g->wiiero_cameras[FULL_SCREEN_CAM],220);
  g->wiiero_game_status = GAME_EDIT_NAME;
}/*---------------------------------------------------------------------------*/
/* Pause */
static __inline__ void wiiero_edit_name(game_t* g){
  int i=0;
  static int current_char=0;
  camera_clean(g->wiiero_cameras[FULL_SCREEN_CAM]);
  font_print_center( g->wiiero_cameras[FULL_SCREEN_CAM],wiiero_label[WIIERO_LANG_PNAME_LABEL]
                       , g->wiiero_cameras[FULL_SCREEN_CAM]->w/2
                       , g->wiiero_cameras[FULL_SCREEN_CAM]->h/4
                       , FONT_STANDARD );

  for(i=0;i<WIIERO_NAME_LEN;i++){
    char caract = (game_nicknames[name_to_change][i] == '\0') ? '.' : game_nicknames[name_to_change][i];
    if(i==current_char){
      font_putc_strict_pos( g->wiiero_cameras[FULL_SCREEN_CAM],caract
                          , g->wiiero_cameras[FULL_SCREEN_CAM]->w/2 + (i - WIIERO_NAME_LEN/2) * 8
                          , g->wiiero_cameras[FULL_SCREEN_CAM]->h/3
                          , FONT_SELECTED );
    }else{
        font_putc_strict_pos( g->wiiero_cameras[FULL_SCREEN_CAM],caract
                            , g->wiiero_cameras[FULL_SCREEN_CAM]->w/2 + (i - WIIERO_NAME_LEN/2) * 8
                            , g->wiiero_cameras[FULL_SCREEN_CAM]->h/3
                            , FONT_STANDARD );
    }
  }

  font_print_center( g->wiiero_cameras[FULL_SCREEN_CAM],wiiero_label[WIIERO_LANG_WSELECT_OK]
                       , 3*g->wiiero_cameras[FULL_SCREEN_CAM]->w/4
                       , 3*g->wiiero_cameras[FULL_SCREEN_CAM]->h/4
                       , FONT_STANDARD );

  if(g->worms[name_to_change]->worms_action & ACTION_OK ){
     g->wiiero_game_status = GAME_SET_OPTIONS;
     //camera_switch_off(g->wiiero_cameras[FULL_SCREEN_CAM]);
     CAMERA_OFF(g->wiiero_cameras[FULL_SCREEN_CAM]);
  }

  if(g->worms[name_to_change]->worms_action == ACTION_UP)
    game_nicknames[name_to_change][current_char] = 
      (font_get_char_pos_in_charset(game_nicknames[name_to_change][current_char]) == CHARSET_SIZE-1)
       ? font_charset[0]
       : font_charset[font_get_char_pos_in_charset(game_nicknames[name_to_change][current_char])+1];

  if(g->worms[name_to_change]->worms_action == ACTION_DOWN)
    game_nicknames[name_to_change][current_char] = 
      (font_get_char_pos_in_charset(game_nicknames[name_to_change][current_char]) == 0) 
      ? font_charset[CHARSET_SIZE-1]
      : font_charset[font_get_char_pos_in_charset(game_nicknames[name_to_change][current_char])-1];

  if(g->worms[name_to_change]->worms_action == ACTION_LEFT)
    current_char = (current_char==0) ? WIIERO_NAME_LEN - 1 : current_char - 1;

  if(g->worms[name_to_change]->worms_action == ACTION_RIGHT)
    current_char = (current_char==WIIERO_NAME_LEN-1) ? 0 : current_char + 1;
  SDL_Delay(50);
}/*---------------------------------------------------------------------------*/



/* --------------------------------- PLAY ----------------------------------- */
/* Prepare playing status */
static __inline__ void wiiero_set_play(game_t* g){ 
  CAMERA_OFF(g->wiiero_cameras[FULL_SCREEN_CAM]);
  g->wiiero_game_status = GAME_PLAYING;
  wiiero_restart_game(g);
  if(g->wiiero_opt_mini_map){
    camera_switch_on(g->wiiero_cameras[GLOBAL_MINI_MAP_CAM]);
  }else{
    CAMERA_OFF(g->wiiero_cameras[GLOBAL_MINI_MAP_CAM]);
  }
  weapon_set_options(g->wiiero_opt_bonus_name,g->wiiero_opt_loading_time);
  g->worms[PLAYER_1]->worms_status |= STATUS_STATS_UPDATE;
  g->worms[PLAYER_2]->worms_status |= STATUS_STATS_UPDATE;
}/*---------------------------------------------------------------------------*/
/* Play */
static __inline__ void wiiero_play(game_t* g){ 
  /* CAM FOCUS */
  player_focus(g->worms[PLAYER_1]); 
  player_focus(g->worms[PLAYER_2]); 
  /* BLIT */   
  wiiero_blit_world(g);
  /* game uptade */ 
  wiiero_update_world(g);

  switch(g->wiiero_opt_game_mode){
    case GAME_DEATHMATCH_MODE:
         wiiero_deathm_game_mode(g);
         break;
    case GAME_OF_TAG_MODE:
         wiiero_got_game_mode(g);
         break;
    case GAME_CAPTURE_FLAG_MODE:
         wiiero_cflag_game_mode(g);
         break;
  }
  if((g->worms[PLAYER_1]->worms_action 
      | g->worms[PLAYER_2]->worms_action) & ACTION_PAUSE ){
     g->wiiero_game_status = GAME_SET_PAUSE;
     SDL_Delay(100);
  }
  if((g->worms[PLAYER_1]->worms_action 
      | g->worms[PLAYER_2]->worms_action) & ACTION_MENU ){
     g->wiiero_game_status = GAME_SET_MENU;
     SDL_Delay(100);
  }
}/*---------------------------------------------------------------------------*/

/* ----------------------------- ROUND STATS -------------------------------- */
/* Prepare round stats status */
static __inline__ void wiiero_set_round_stats(game_t* g){ 
  static char msg[512];
  player_id id;
  round_running = 0;
  clean_bullets_list(g->wiiero_bullets);
  clean_dynamic_obj_list(g->wiiero_dynamic_objects);
  camera_clean(g->wiiero_cameras[FULL_SCREEN_CAM]);
  camera_switch_on(g->wiiero_cameras[FULL_SCREEN_CAM]);
  camera_set_alpha(g->wiiero_cameras[FULL_SCREEN_CAM],170);
  /* SHOW Player 1 stats */

  for(id = PLAYER_1;id<NB_PLAYERS;id++){
    snprintf(msg,511,".:%s %s:.",game_nicknames[id],wiiero_label[WIIERO_LANG_SCORE]);
    font_print_strict_pos( g->wiiero_cameras[FULL_SCREEN_CAM],msg
                         , (1+4*id)*g->wiiero_cameras[FULL_SCREEN_CAM]->w/8
                         , g->wiiero_cameras[FULL_SCREEN_CAM]->h/6
                         , FONT_STANDARD);

    snprintf(msg,511,"  %s: %d",wiiero_label[WIIERO_LANG_FRAGS],game_score[id].nb_frags);
    font_print_strict_pos( g->wiiero_cameras[FULL_SCREEN_CAM],msg
                         , (1+4*id)*g->wiiero_cameras[FULL_SCREEN_CAM]->w/8
                         , g->wiiero_cameras[FULL_SCREEN_CAM]->h/6 + 10
                         , FONT_STANDARD);
    snprintf(msg,511,"  %s: %d",wiiero_label[WIIERO_LANG_DEATH],game_score[id].nb_death);
    font_print_strict_pos( g->wiiero_cameras[FULL_SCREEN_CAM],msg
                         , (1+4*id)*g->wiiero_cameras[FULL_SCREEN_CAM]->w/8
                         , g->wiiero_cameras[FULL_SCREEN_CAM]->h/6 + 20
                         , FONT_STANDARD);
    snprintf(msg,511,"  %s: %d",wiiero_label[WIIERO_LANG_SUICID],game_score[id].nb_suicides);
    font_print_strict_pos( g->wiiero_cameras[FULL_SCREEN_CAM],msg
                         , (1+4*id)*g->wiiero_cameras[FULL_SCREEN_CAM]->w/8
                         , g->wiiero_cameras[FULL_SCREEN_CAM]->h/6 + 30
                         , FONT_STANDARD);
    snprintf(msg,511,"  %s: %.2f",wiiero_label[WIIERO_LANG_RATIO],(game_score[id].nb_death==0) 
                 ? (game_score[id].nb_frags*1.0)/1
                 : (game_score[id].nb_frags*1.0)/game_score[PLAYER_1].nb_death);
    font_print_strict_pos( g->wiiero_cameras[FULL_SCREEN_CAM],msg
                         , (1+4*id)*g->wiiero_cameras[FULL_SCREEN_CAM]->w/8
                         , g->wiiero_cameras[FULL_SCREEN_CAM]->h/6 + 40
                         , FONT_STANDARD);
    switch(g->wiiero_opt_game_mode){
      case GAME_DEATHMATCH_MODE:
           snprintf(msg,511,"  %s: %d",wiiero_label[WIIERO_LANG_LIFES],game_score[id].nb_lifes);
           font_print_strict_pos( g->wiiero_cameras[FULL_SCREEN_CAM],msg
                             , (1+4*id)*g->wiiero_cameras[FULL_SCREEN_CAM]->w/8
                             , g->wiiero_cameras[FULL_SCREEN_CAM]->h/6 + 50
                             , FONT_STANDARD);
           break;
      case GAME_OF_TAG_MODE:
           snprintf(msg,511,"  %s: %dm%ds",wiiero_label[WIIERO_LANG_TIME],game_score[id].tag_time/60,game_score[id].tag_time%60);
           font_print_strict_pos( g->wiiero_cameras[FULL_SCREEN_CAM],msg
                             , (1+4*id)*g->wiiero_cameras[FULL_SCREEN_CAM]->w/8
                             , g->wiiero_cameras[FULL_SCREEN_CAM]->h/6 + 50
                             , FONT_STANDARD);
           break;
      case GAME_CAPTURE_FLAG_MODE:
           snprintf(msg,511,"  %s: %d",wiiero_label[WIIERO_LANG_FLAGS],game_score[id].nb_flags);
           font_print_strict_pos( g->wiiero_cameras[FULL_SCREEN_CAM],msg
                             , (1+4*id)*g->wiiero_cameras[FULL_SCREEN_CAM]->w/8
                             , g->wiiero_cameras[FULL_SCREEN_CAM]->h/6 + 50
                             , FONT_STANDARD);
           break;
    }
  }
  id=winner_id;
  if(id < GAME_DRAW)
    snprintf(msg,511,"%s %s.",game_nicknames[id],wiiero_label[WIIERO_LANG_WIN]);
  else
    snprintf(msg,511,"%s.",wiiero_label[WIIERO_LANG_DRAW]);
  font_print_strict_pos( g->wiiero_cameras[FULL_SCREEN_CAM],msg
                   , g->wiiero_cameras[FULL_SCREEN_CAM]->w/2-strlen(msg)*4
                   , g->wiiero_cameras[FULL_SCREEN_CAM]->h/6 + 70
                   , FONT_SELECTED);
				   
  /* Blit and wait a moment ... */				   
  screen_display(g->wiiero_screen);
  
  //SDL_Delay(3000); < Wiiero 1.2 Deadlock bugfix on wii... realy strange !
  SDL_Delay(1000);
  
  font_print_center( g->wiiero_cameras[FULL_SCREEN_CAM]
                   ,wiiero_label[WIIERO_LANG_WARNING_PRESSKEY]
                   , 3*g->wiiero_cameras[FULL_SCREEN_CAM]->w/4
                   , 3*g->wiiero_cameras[FULL_SCREEN_CAM]->h/4
                   , FONT_STANDARD );
				   
  g->wiiero_game_status = GAME_ROUND_STATS;
  g->worms[PLAYER_1]->worms_action = ACTION_NONE;
  g->worms[PLAYER_2]->worms_action = ACTION_NONE;
}/*--------------------------------------------------------------------------*/
/* Round stats */
static __inline__ void wiiero_round_stats(game_t* g){
  if((g->worms[PLAYER_1]->worms_action != ACTION_NONE)||
     (g->worms[PLAYER_2]->worms_action != ACTION_NONE)){
     g->wiiero_game_status = GAME_SET_MENU;
  }
}/*--------------------------------------------------------------------------*/



int wiiero_load_config(game_t* g){
  FILE* conf_file = fopen(WIIERO_STD_OPT_FILE,"r");
  if(!conf_file){
    snprintf(str_errno,127, "Error: %s.\n", strerror(errno));
    return 0;
  }
  fread(&(g->wiiero_opt_game_mode)   ,sizeof(game_mode_t),1,conf_file);
  fread(&(g->wiiero_opt_nb_lifes)    ,sizeof(Uint16),1,conf_file);
  fread(&(g->wiiero_opt_nb_flags)    ,sizeof(Uint16),1,conf_file);
  fread(&(g->wiiero_opt_got_time)    ,sizeof(Uint16),1,conf_file);
  fread(&(g->wiiero_opt_gift_nb)     ,sizeof(Uint8),1,conf_file);
  fread(&(g->wiiero_opt_loading_time),sizeof(Uint8),1,conf_file);
  fread(&(g->wiiero_opt_bonus_name)  ,sizeof(Uint8),1,conf_file);
  fread(&(g->wiiero_opt_mini_map)    ,sizeof(Uint8),1,conf_file);
  fread(&(g->wiiero_opt_map_regen)   ,sizeof(Uint8),1,conf_file);
  fread(&(g->wiiero_opt_shadow)      ,sizeof(Uint8),1,conf_file);
  fread(&(g->wiiero_opt_xtra_weap)   ,sizeof(Uint8),1,conf_file);
  fread(&(g->wiiero_opt_nb_rocks)    ,sizeof(Uint8),1,conf_file);
  fread(&selected_amb                ,sizeof(Uint8),1,conf_file);
  fread(game_nicknames[PLAYER_1]     ,sizeof(char),10,conf_file);
  fread(game_nicknames[PLAYER_2]     ,sizeof(char),10,conf_file);
  fread(&(g->wiiero_opt_screen_resolution),sizeof(screen_res_t),1,conf_file);
  /* Wiiero 1.2 new config file part */
  fread(&mud_particle,sizeof(Uint8),1,conf_file);
  if(!feof(conf_file)){
    Uint16 langlen = 0;
    fread(&langlen,sizeof(Uint16),1,conf_file);
    if(langlen){
        char filename[256];
        memset(filename,0,sizeof(char)*256);
        fread(filename,sizeof(char),langlen,conf_file);
        fread(&(g->wiiero_lang),sizeof(Uint8),1,conf_file); 
        apply_lang (filename);
    } 
  }

  fclose(conf_file);
  return 1;
}/*--------------------------------------------------------------------------*/

int wiiero_save_config(game_t* g){
  FILE* conf_file = fopen(WIIERO_STD_OPT_FILE,"w");
  Uint16 langlen = 0;
  if(!conf_file) return 0;
  fwrite(&(g->wiiero_opt_game_mode)   ,sizeof(game_mode_t),1,conf_file);
  fwrite(&(g->wiiero_opt_nb_lifes)    ,sizeof(Uint16),1,conf_file);
  fwrite(&(g->wiiero_opt_nb_flags)    ,sizeof(Uint16),1,conf_file);
  fwrite(&(g->wiiero_opt_got_time)    ,sizeof(Uint16),1,conf_file);
  fwrite(&(g->wiiero_opt_gift_nb)     ,sizeof(Uint8),1,conf_file);
  fwrite(&(g->wiiero_opt_loading_time),sizeof(Uint8),1,conf_file);
  fwrite(&(g->wiiero_opt_bonus_name)  ,sizeof(Uint8),1,conf_file);
  fwrite(&(g->wiiero_opt_mini_map)    ,sizeof(Uint8),1,conf_file);
  fwrite(&(g->wiiero_opt_map_regen)   ,sizeof(Uint8),1,conf_file);
  fwrite(&(g->wiiero_opt_shadow)      ,sizeof(Uint8),1,conf_file);
  fwrite(&(g->wiiero_opt_xtra_weap)   ,sizeof(Uint8),1,conf_file);
  fwrite(&(g->wiiero_opt_nb_rocks)    ,sizeof(Uint8),1,conf_file);
  fwrite(&selected_amb                ,sizeof(Uint8),1,conf_file);
  fwrite(game_nicknames[PLAYER_1]     ,sizeof(char),10,conf_file);
  fwrite(game_nicknames[PLAYER_2]     ,sizeof(char),10,conf_file);
  fwrite(&(g->wiiero_opt_screen_resolution),sizeof(screen_res_t),1,conf_file); 
  fwrite(&mud_particle                ,sizeof(Uint8),1,conf_file);
  if(get_nb_loaded_lang_files()){
      langlen = strlen(wiiero_lang_files[g->wiiero_lang]);
      fwrite(&langlen, sizeof(Uint16),1,conf_file);
      fwrite(wiiero_lang_files[g->wiiero_lang],sizeof(char),langlen,conf_file); 
      fwrite(&(g->wiiero_lang),sizeof(Uint8),1,conf_file); 
  }else
      fwrite(&langlen,sizeof(Uint16),1,conf_file);  

  fclose(conf_file);
  return 1;
}/*--------------------------------------------------------------------------*/


static __inline__ void wiiero_how_to_play(game_t* g){
  int w = g->wiiero_cameras[FULL_SCREEN_CAM]->w / 20;
  int h = g->wiiero_cameras[FULL_SCREEN_CAM]->h / 20;
  lab_id_t lab_id = MODE_CMD;
  camera_switch_on(g->wiiero_cameras[FULL_SCREEN_CAM]);
  camera_clean(g->wiiero_cameras[FULL_SCREEN_CAM]);
  camera_set_alpha(g->wiiero_cameras[FULL_SCREEN_CAM],250);
  font_print_center( g->wiiero_cameras[FULL_SCREEN_CAM]
                   , wiiero_label[WIIERO_LANG_CTRL_LABEL]
                   , g->wiiero_cameras[FULL_SCREEN_CAM]->w/2 
				   , h, FONT_SELECTED);

  h += (font_get_height(FONT_SELECTED)+4) + 8;
  for(lab_id=FIRE_CMD;lab_id<MAX_CMD;lab_id++)
    font_print_strict_pos( g->wiiero_cameras[FULL_SCREEN_CAM]
                         , wiiero_label[(WIIERO_LANG_CTRL_FIRST_ID-1)+lab_id]
                         , w,  h+= (font_get_height(FONT_STANDARD)+4) , FONT_STANDARD);

  h = g->wiiero_cameras[FULL_SCREEN_CAM]->h / 20 + 8;
  for(lab_id=EXTRA_INFO;lab_id<MAX_CMD;lab_id++){
    font_print_strict_pos( g->wiiero_cameras[FULL_SCREEN_CAM], cmd_key[0][lab_id]
                         , 2*g->wiiero_cameras[FULL_SCREEN_CAM]->w / 6
                         ,  h+= (font_get_height(FONT_STANDARD)+4) , FONT_STANDARD);
    font_print_strict_pos( g->wiiero_cameras[FULL_SCREEN_CAM], cmd_key[1][lab_id]
                         , 3*g->wiiero_cameras[FULL_SCREEN_CAM]->w / 6,  h , FONT_STANDARD);
    font_print_strict_pos( g->wiiero_cameras[FULL_SCREEN_CAM], cmd_key[2][lab_id]
                         , 4*g->wiiero_cameras[FULL_SCREEN_CAM]->w / 6,  h , FONT_STANDARD);
    font_print_strict_pos( g->wiiero_cameras[FULL_SCREEN_CAM], cmd_key[3][lab_id]
                         , 5*g->wiiero_cameras[FULL_SCREEN_CAM]->w / 6,  h , FONT_STANDARD);
  }

  next_stat = GAME_MENU;
  g->wiiero_game_status = GAME_PAUSE;
  g->worms[PLAYER_1]->worms_action = ACTION_NONE;
  g->worms[PLAYER_2]->worms_action = ACTION_NONE;
  SDL_Delay(100);
}/*--------------------------------------------------------------------------*/


#define ABOUT_LINES 19
static __inline__ void wiiero_about(game_t* g){
  static char * about[ABOUT_LINES] = 
  {  ""
   , "Two worms... lots of heavy weapons... lots of fun!"
   , "" 
   , "Wiiero is a Liero clone for Wii PSP and PC.       "
   , "This program is free software.                    "
   , ""
   , "Developement : BScrk                              "
   , "Design : BScrk  based on original liero design    "
   , "Beta testers : Kyfran and Pedro                   "
   , "Xtra weapons ideas : BScrk and Kyfran             "
   , "Original game concept : Joosa Riekkinen in 1998   "
   , "Sound Track : Binaerpilot      www.binaerpilot.no "   
   , "Special thanks to Flark.                          "
   , ""
   , "Wiiero is licensed under the GPL.                 "
   , "Thanks for trying Wiiero...                       "
   , ""
   , "                                             BScrk"
   , "Copyright [C] 2008 2009             wiiero.free.fr"
  };
  
  int h = g->wiiero_cameras[FULL_SCREEN_CAM]->h / 20;
  lab_id_t lab_id = MODE_CMD;
  camera_switch_on(g->wiiero_cameras[FULL_SCREEN_CAM]);
  camera_clean(g->wiiero_cameras[FULL_SCREEN_CAM]);
  camera_set_alpha(g->wiiero_cameras[FULL_SCREEN_CAM],250);
  font_print_center( g->wiiero_cameras[FULL_SCREEN_CAM], WIIERO_VERSION
                   , g->wiiero_cameras[FULL_SCREEN_CAM]->w/2 , h, FONT_SELECTED);

  h += (font_get_height(FONT_SELECTED)+4);
  for(lab_id=0;lab_id<ABOUT_LINES;lab_id++)
    font_print_center( g->wiiero_cameras[FULL_SCREEN_CAM], about[lab_id]
                     , g->wiiero_cameras[FULL_SCREEN_CAM]->w/2
                     ,  h+= (font_get_height(FONT_STANDARD)+4) , FONT_STANDARD);

 
  next_stat = GAME_MENU;
  g->wiiero_game_status = GAME_PAUSE;
  SDL_Delay(100);
}/*--------------------------------------------------------------------------*/

void wiiero_cycle(game_t* g){
  ASSERT(g);
  int start_time = SDL_GetTicks();
  skeep_current_frame = 0;
  /* EVENTS */
  player_clean(g->worms[PLAYER_1]);
  player_clean(g->worms[PLAYER_2]);
  game_check_event(g);
  switch(g->wiiero_game_status){
    case GAME_SET_MENU: 
      wiiero_set_menu(g);
      break;
    case GAME_MENU: 
      wiiero_menu(g);
      break;
    case GAME_SET_OPTIONS: 
      wiiero_set_option(g);
      break;
    case GAME_OPTIONS: 
      wiiero_option(g);
      break;
    case GAME_OPTIONS_BIS: 
      wiiero_option_bis(g);
      break;
    case GAME_SET_WEAPON_SELECT:
      wiiero_set_wselect(g);
      break;
    case GAME_WEAPON_SELECT:
      wiiero_wselect(g);
      break;
    case GAME_SET_PAUSE:
      wiiero_set_message(g,wiiero_label[WIIERO_LANG_GAME_PAUSED],FONT_SELECTED,GAME_PLAYING,230);
      break;
    case GAME_PAUSE:
      wiiero_pause(g);
      break;
    case GAME_SET_PLAYING:
      wiiero_set_play(g);
      break;
    case GAME_PLAYING:
      wiiero_play(g);
      break;
    case GAME_SET_ROUND_STATS:
      wiiero_set_round_stats(g);
      break;
    case GAME_ROUND_STATS:
      wiiero_round_stats(g);
      break;
    case GAME_LOAD_CONFIG:
      if(wiiero_load_config(g))
            wiiero_set_message(g,wiiero_label[WIIERO_LANG_LOAD_OK],FONT_SELECTED,GAME_MENU,230);
      else
            wiiero_set_message(g,wiiero_label[WIIERO_LANG_LOAD_KO],FONT_STANDARD,GAME_MENU,230);
      break;
      break;
    case GAME_SAVE_CONFIG:
      if(wiiero_save_config(g))
            wiiero_set_message(g,wiiero_label[WIIERO_LANG_SAVE_OK],FONT_SELECTED,GAME_MENU,230);
      else
            wiiero_set_message(g,wiiero_label[WIIERO_LANG_SAVE_KO],FONT_STANDARD,GAME_MENU,230);
      break;
    case GAME_HOW_TO_PLAY:
      wiiero_how_to_play(g);
      break;
    case GAME_ABOUT:
      wiiero_about(g);
      break;
    case GAME_SET_EDIT_NAME:
      wiiero_set_edit_name(g);
      break;
    case GAME_EDIT_NAME:
      wiiero_edit_name(g);
      break;
    case GAME_EXIT:
      g->wiiero_exit = 1;
      camera_switch_on(g->wiiero_cameras[FULL_SCREEN_CAM]);
      camera_clean(g->wiiero_cameras[FULL_SCREEN_CAM]);
      camera_set_alpha(g->wiiero_cameras[FULL_SCREEN_CAM],200);
      font_print_center( g->wiiero_cameras[FULL_SCREEN_CAM]
                        ,wiiero_label[WIIERO_LANG_GAME_THX]
                       , g->wiiero_cameras[FULL_SCREEN_CAM]->w/2
                       , g->wiiero_cameras[FULL_SCREEN_CAM]->h/3
                       , FONT_STANDARD);
     font_print_center( g->wiiero_cameras[FULL_SCREEN_CAM]
                       ,wiiero_label[WIIERO_LANG_GAME_BYE]
                       , g->wiiero_cameras[FULL_SCREEN_CAM]->w/2
                       , g->wiiero_cameras[FULL_SCREEN_CAM]->h/2
                       , FONT_STANDARD);
      break;
  }


  framer_check(g->wiiero_cameras[PLAYER_2_GAME_ZONE_CAM]);
  if(!skeep_current_frame)
    screen_display(g->wiiero_screen);
  if((SDL_GetTicks() - start_time) < g->wiiero_frame_delay + (bullet_time_effect_delay/8)){
#ifndef PSP_MODE
    /* Wii, PC */
    usleep((g->wiiero_frame_delay + (bullet_time_effect_delay/8) - (SDL_GetTicks() - start_time))*1000);
#else
    SDL_Delay(g->wiiero_frame_delay + (bullet_time_effect_delay/8) - (SDL_GetTicks() - start_time));
#endif
  }
  if(bullet_time_effect_delay>0)
        bullet_time_effect_delay-=4;
}/*--------------------------------------------------------------------------*/


void wiiero_quit(game_t* g){
  ASSERT(g)
  int i;/*
  printf("Player 1 stats\n");
  printf("|`````````````\n");
  printf("| - lifes    : %d\n",game_score[PLAYER_1].nb_lifes);
  printf("| - frags    : %d\n",game_score[PLAYER_1].nb_frags);
  printf("| - suicides : %d\n",game_score[PLAYER_1].nb_suicides);
  printf("\\ - deaths   : %d\n\n",game_score[PLAYER_1].nb_death);
  printf("Player 2 stats:\n");
  printf("|`````````````\n");
  printf("| - lifes    : %d\n",game_score[PLAYER_2].nb_lifes);
  printf("| - frags    : %d\n",game_score[PLAYER_2].nb_frags);
  printf("| - suicides : %d\n",game_score[PLAYER_2].nb_suicides);
  printf("\\ - deaths   : %d\n",game_score[PLAYER_2].nb_death);*/
  
  sengine_release();

#ifdef PC_MODE
  g->wiiero_screen->mode = FULL_SCREEN_MODE;
  screen_flip_mode(g->wiiero_screen);
#endif
  
  //release bullets  
  clean_bullets_list(g->wiiero_bullets);
  //release dynamic objects  
  clean_dynamic_obj_list(g->wiiero_dynamic_objects);  
  //release ressources  
  release_ressource(g->wiiero_ressources); 
  
  for(i=0;i<NB_MAX_CAMS;i++)
    camera_release(g->wiiero_cameras[i]);
  map_release(g->wiiero_map);
  screen_release(g->wiiero_screen);
}/*--------------------------------------------------------------------------*/
