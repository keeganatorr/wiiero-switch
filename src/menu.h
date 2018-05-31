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

#ifndef MENU_H
#define MENU_H

#include "common.h"
  enum{ MENU_PLAY
      , MENU_OPTION
      , MENU_RESTART
      , MENU_LOAD
      , MENU_SAVE
      , MENU_HELP
      , MENU_INFO
      , MENU_RETURN
      , MENU_EXIT
      , MENU_MAX
  };
  typedef struct{ 
    Uint8 id;
    char *label;
    game_status_t target;
  }menu_enty_t;
  
  
  typedef void  (*option_action_cb)(game_t* g);
  typedef void  (*option_action_on_value_cb)(void* val);
  typedef char* (*option_formater_cb)(char* label,void* data);
  
  enum{ OPTION_MODE
      , OPTION_LIVES
      , OPTION_FLAGS
      , OPTION_TIME
      , OPTION_RELOAD
      , OPTION_NB_BONUS
      , OPTION_BONUS_NM
      , OPTION_LEVEL
      , OPTION_LEV_REG
      , OPTION_LEV_ROCKS
      , OPTION_LEV_AMBI
      , OPTION_SHADOWS
      , OPTION_P1_NAME
      , OPTION_P2_NAME
      , OPTION_BIS_MENU
      , OPTION_RETURN
      , OPTION_MAX
  };
  enum{ OPTION_BIS_XTRA_W
      , OPTION_BIS_MUD
      , OPTION_BIS_LANG
      , OPTION_MENU
      , OPTION_BIS_RETURN
      , OPTION_BIS_MAX
  };
  typedef struct{ 
    Uint8 id;
    char *label;
    void* option_data;
    option_formater_cb option_formater;
    option_action_on_value_cb   option_on_left;
    option_action_on_value_cb   option_on_right;
    option_action_cb   option_action;
  }option_enty_t;
  
#endif
