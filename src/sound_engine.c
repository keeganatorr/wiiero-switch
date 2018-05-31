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

#include "sound_engine.h"
#include "font.h"
#include "tools.h"




#ifndef NO_SOUND
static int sengine_initialized = 0;

#define AUDIO_DIR "audio"
//#define AUDIO_DIR "./audio"

static sendine_sound_t sounds[WIIERO_MAX_SOUNDS] ={
/* -WIIERO_SOUND_GUN         */ {AUDIO_DIR"/gun.wav"  ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_SHOOTGUN    */ {AUDIO_DIR"/shoo.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_CANNON      */ {AUDIO_DIR"/cann.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_BAZOOK      */ {AUDIO_DIR"/bazo.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_LITTLE_AUTO */ {AUDIO_DIR"/lauto.wav",0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_BIG_AUTO    */ {AUDIO_DIR"/bauto.wav",0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_MINIGUN     */ {AUDIO_DIR"/mauto.wav",0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_ELEC        */ {AUDIO_DIR"/elec.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_ZIMM        */ {AUDIO_DIR"/zimm.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_MAGNUM      */ {AUDIO_DIR"/magn.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_BLAST       */ {AUDIO_DIR"/blas.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_LARPA       */ {AUDIO_DIR"/larp.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_RIFLE       */ {AUDIO_DIR"/rifl.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_LAUNCHER    */ {AUDIO_DIR"/laun.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_LITTLE_EXPLO*/ {AUDIO_DIR"/lexp.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_BIG_EXPLO   */ {AUDIO_DIR"/bexp.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_BURN        */ {AUDIO_DIR"/burn.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_LONG_BURN   */ {AUDIO_DIR"/lbur.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_BIPBIP      */ {AUDIO_DIR"/bbip.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_GAZ         */ {AUDIO_DIR"/gaz.wav"  ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_GRENADE     */ {AUDIO_DIR"/gren.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_FLAMMER     */ {AUDIO_DIR"/flam.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_HELLRAIDER  */ {AUDIO_DIR"/hell.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_LASER       */ {AUDIO_DIR"/laser.wav",0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_DEATH       */ {AUDIO_DIR"/death.wav",0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_GIFT        */ {AUDIO_DIR"/gift.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_HEALTH      */ {AUDIO_DIR"/heal.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_CLIP        */ {AUDIO_DIR"/clip.wav" ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_SHELL       */ {AUDIO_DIR"/shell.wav",0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_RELOAD      */ {AUDIO_DIR"/reloa.wav",0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_POP         */ {AUDIO_DIR"/pop.wav"  ,0l,SENGINE_TRACK_UNLOADED},
/* -WIIERO_SOUND_BEE         */ {AUDIO_DIR"/bee.wav"  ,0l,SENGINE_TRACK_UNLOADED}
};

#define MUSIC_DIR "music"

static sendine_music_t playlist[] = {
   {MUSIC_DIR"/Sandjorda.mp3",0l}
  ,{MUSIC_DIR"/KDRassault.mp3",0l}
  ,{MUSIC_DIR"/Goof.mp3",0l}
  ,{MUSIC_DIR"/RobotWars.mp3",0l}
};

#define GET_NB_ZICS (sizeof(playlist)/sizeof(sendine_music_t))

static int current_music = 0;
void sengine_playnext(){
  current_music = (current_music + 1) % GET_NB_ZICS;
  if(playlist[current_music].music_handler){
    Mix_PlayMusic(playlist[current_music].music_handler, 0);
  }else
    sengine_playnext();
}

void sengine_init_music(){
  int nb_zics = GET_NB_ZICS;
  int i = 0;
  /* Load zics */
  for( ; i < nb_zics; i++){
    playlist[i].music_handler = Mix_LoadMUS(playlist[i].music_name);
    if( ! playlist[i].music_handler ){
      HARD_DBG("  >> load failed\n");
	}
  }
  Mix_VolumeMusic(120);
  /* Start with a random music */
  current_music = rand() % GET_NB_ZICS;
  if(playlist[current_music].music_handler){
    Mix_PlayMusic(playlist[current_music].music_handler, 0);
  }
  Mix_HookMusicFinished(sengine_playnext);
}

void sengine_release_music(){
  /* Release music slots */
  int nb_zics = GET_NB_ZICS , i = 0;
  for( ; i < nb_zics; i++){
    if( playlist[i].music_handler ){
      Mix_FreeMusic(playlist[i].music_handler );
	}
  }
}
#endif

/* ************************************************************************** */
int sengine_init(){

#ifndef NO_SOUND
  int i;
  
  if(sengine_initialized == 1){
    return 1;
  }
  HARD_DBG("- Init sound\n");
  if (Mix_OpenAudio( WIIERO_SND_FREQ, WIIERO_SND_FORM, WIIERO_SND_CHAN
                   , WIIERO_SND_BUFF) != 0){
				   
    return 1;
  }
  HARD_DBG("  >> loading\n");
  sengine_initialized = 1;
  
  for(i=0;i<WIIERO_MAX_SOUNDS;i++){
    Mix_Chunk  *load_track = 0l;
    HARD_DBG("    %s\n",sounds[i].sound_name);
    load_track = Mix_LoadWAV(sounds[i].sound_name);
    if(load_track){
	  Mix_VolumeChunk(load_track,70);
      sounds[i].sound_handler = load_track;
      sounds[i].sound_info |= SENGINE_TRACK_LOADED;
    }else{
      sounds[i].sound_info |= SENGINE_TRACK_UNLOADED;
    }
  }
  Mix_AllocateChannels(WIIERO_SND_MIXC);
  sengine_init_music();
#endif
  return 0;
}


/* ************************************************************************** */
void sengine_release(){
#ifndef NO_SOUND
  int i;
  Mix_HaltMusic();
  for(i=0;i<WIIERO_MAX_SOUNDS;i++)
    if(sounds[i].sound_info & SENGINE_TRACK_LOADED)
      Mix_FreeChunk(sounds[i].sound_handler);
  sengine_release_music();
  Mix_CloseAudio();
#endif
}


/* ************************************************************************** */
void sengine_play(sengine_sound_id_t id,sengine_sound_action_t action){
#ifndef NO_SOUND
  if((sengine_initialized == 1) && (id != WIIERO_SOUND_NONE)) {
    HARD_DBG(" ## play request [%d], loops : %d\n",id,action);
    if(sounds[id].sound_info & SENGINE_TRACK_LOADED){
      HARD_DBG(" ## Get channel\n");
      if(Mix_GroupAvailable(-1) != -1){ // channel available ?
        HARD_DBG("   for %s\n",sounds[id].sound_name);
        HARD_DBG("     @ %p\n",sounds[id].sound_handler);
        Mix_PlayChannel( -1, sounds[id].sound_handler, (int)action );
      }
    }
  }
#endif
}

