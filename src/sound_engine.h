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

#ifndef SOUND_ENGINE_H
#define SOUND_ENGINE_H

#include "common.h"

#include <SDL/SDL.h>
#ifndef NO_SOUND
#include <SDL/SDL_mixer.h>
# define WIIERO_SND_FREQ  44000 //MIX_DEFAULT_FREQUENCY	// 22050
# define WIIERO_SND_FORM  MIX_DEFAULT_FORMAT
# define WIIERO_SND_CHAN  1
# define WIIERO_SND_BUFF  1024
# define WIIERO_SND_MIXC  24
#endif

enum{
  SENGINE_TRACK_UNLOADED = 0x00,
  SENGINE_TRACK_LOADED   = 0x01
};

typedef  enum{
  SENGINE_LOOP_ACTION = -1,
  SENGINE_PLAY_ACTION = 0,
  SENGINE_STOP_ACTION = 1,
}sengine_sound_action_t;

typedef enum{
  WIIERO_SOUND_GUN,
  WIIERO_SOUND_SHOOTGUN,
  WIIERO_SOUND_CANNON,
  WIIERO_SOUND_BAZOOK,
  WIIERO_SOUND_LITTLE_AUTO,
  WIIERO_SOUND_BIG_AUTO,
  WIIERO_SOUND_MINIGUN,
  WIIERO_SOUND_ELEC,
  WIIERO_SOUND_ZIMM,
  WIIERO_SOUND_MAGNUM,
  WIIERO_SOUND_BLAST,
  WIIERO_SOUND_LARPA,
  WIIERO_SOUND_RIFLE,
  WIIERO_SOUND_LAUNCHER,
  WIIERO_SOUND_LITTLE_EXPLO,
  WIIERO_SOUND_BIG_EXPLO,
  WIIERO_SOUND_BURN,
  WIIERO_SOUND_LONG_BURN,
  WIIERO_SOUND_BIPBIP,
  WIIERO_SOUND_GAZ,
  WIIERO_SOUND_GRENADE,
  WIIERO_SOUND_FLAMMER,
  WIIERO_SOUND_HELLRAIDER,
  WIIERO_SOUND_LASER,
  WIIERO_SOUND_DEATH,
  WIIERO_SOUND_GIFT,
  WIIERO_SOUND_HEALTH,
  WIIERO_SOUND_CLIP,
  WIIERO_SOUND_SHELL,
  WIIERO_SOUND_RELOAD,
  WIIERO_SOUND_POP,
  WIIERO_SOUND_BEE,
  WIIERO_SOUND_NONE,
  WIIERO_MAX_SOUNDS = WIIERO_SOUND_NONE
}sengine_sound_id_t;



typedef struct{
  char        sound_name[128];
  Uint8       sound_info;
#ifndef NO_SOUND
  Mix_Chunk  *sound_handler;
#else
  void       *sound_handler;
#endif
}sendine_sound_t;



typedef struct{
  char        music_name[128];
#ifndef NO_SOUND
  Mix_Music  *music_handler;
#else
  void       *music_handler;
#endif
}sendine_music_t;


extern int  sengine_init();
extern void sengine_release();
extern void sengine_play(sengine_sound_id_t id,sengine_sound_action_t action);

  
#endif
