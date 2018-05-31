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


#include <stdio.h>
#include <string.h>
#include "lang.h"
#include "common.h"
#include "tools.h"

char *wiiero_label[WIIERO_LANG_NB_LABELS] = {
  /*Game labels */
    "Loading"
  , "Please connect 2 controlers first..."
  , "Match already started. use [return ...]"
  , "No match started. use [play wiiero]"
  , "Restart the game for apply the new resolution."
  , "press any key..."
  , "Pause"
  , "Suicide..."
  , "You have been killed !"
  /* Main Menu */
  , "play wiiero"
  , "option menu"
  , "restart game"
  , "reload conf"
  , "save config"
  , "how to play"
  , "about wiiero"
  , "return ... "
  , "exit wiiero"
  /* OPT labels */
  , "Killem all"
  , "Game of tag"
  , "CTF"
  , "On"
  , "Off"
  , "Dirt"
  , "Sand"
  , "Night"
  /* Option Menu Labels */
  , "mode:"
  , "lives:"
  , "flags:"
  , "countdown:"
  , "reloadtime:"
  , "max bonus:"
  , "bonus names:"
  , "resolution:"
  , "regen level:"
  , "nb rocks:"
  , "ambience:"
  , "shadow fx:"
  , "player1 name:"
  , "player2 name:"
  , "xtra weapons:"
  , "return ..." 
  /* Weapon selection part */
  , "weapons selection:"
  , "[cancel] : randomize"
  , "[ok]     : validate"
  /* Players name menu */
  , "player name: "
  /* Round score */
  , "score"
  , "Frags"
  , "Deaths"
  , "Suicides"
  , "Ratio"
  , "lifes"
  , "time"
  , "flags"
  , "win the round"
  , "round draw"
  /* Config Info */
  , "config reloaded..."
  , "config reload failed !"
  , "save completed..."
  , "save failed !"
  /* Exit*/
  , "Thanks for playing Wiiero !"
  , "good bye..."
  /* Weapon Names */
  , "handgun"
  , "uzi"
  , "chaingun"
  , "shotgun"
  , "super shotgun"
  , "minigun"
  , "cannon"
  , "bazooka"
  , "doomsday"
  , "winchester"
  , "larpa"
  , "bouncy larpa"
  , "zimm"
  , ".50 rifle"
  , "blaster"
  , "gauss gun"
  , "flamer"
  , "mini rocket"
  , "missile"
  , "rb rampage"
  , "hellraider"
  , "laser"
  , "grasshopper"
  , "booby trap"
  , "mine"
  , "bouncy mine"
  , "float mine"
  , "spikeballs"
  , "crackler"
  , "grenade"
  , "cluster bomb"
  , "mini nuke"
  , "big nuke"
  , "chiquita bomb"
  , "explosives"
  , "napalm"
  , "dirtball"
  /* Xtra */
  , "holy grenade"
  , "super cannon"
  , "ceiling mine"
  , "ghopper grenade"
  , "blue flammer"
  , "an-m14 grenade"
  , "gas grenade"
  , "plasma cannon"
  , "nuke larpa"
  , "ctrl nuke larpa"
  , "chiquita gun"
  , "vermifuge"
  , "jumping mine"
  , "trap"
  , "kamikaze"
  , "spike time bomb"
  , "bug case"
  , "vermi nuke"
  , "medikit"
  , "iron worm"
  , "pinball"
  /* Controls labesl */
  , "Controls"
  , "Fire"
  , "Jump"
  , "Dig"
  , "Show weapon"
  , "Prev weapon"
  , "Next weapon"
  , "Ninja hook"
  , "Up"
  , "Down"
  , "Left"
  , "Right"
  , "Pause"
  , "Menu"
  , "Ok"
  , "Cancel"
  /* New Wiiero labels (v1.2) */
  , "Mud particles:"
  , "Language:"
  , "Next menu page"
  , "Prev menu page"
};


#ifdef MULTI_LANG
# include <dirent.h>
#endif

char *wiiero_lang_files[MAX_LANGS];
Uint8 wiiero_loaded_lang_files;

Uint8 get_nb_loaded_lang_files(){
    return wiiero_loaded_lang_files;
}

void check_lang_files(){
#ifdef MULTI_LANG
 int lang_id=0;
 struct dirent *dir_entry;
 DIR *directory;
 memset(wiiero_lang_files,0L,sizeof(char*)*MAX_LANGS); // clean lang list
 wiiero_loaded_lang_files = 0;
 HARD_DBG("Open dir...");
 directory = opendir("lang");
 if(directory){
  while ((dir_entry = readdir(directory))){
    char* ext_pos = 0;     
    HARD_DBG(dir_entry->d_name);
    if((ext_pos = strstr(dir_entry->d_name,".wtf"))){
      *ext_pos = '\0';
      wiiero_lang_files[lang_id++] = strdup(dir_entry->d_name);
        wiiero_loaded_lang_files++;
    }
  }
  HARD_DBG("close dir...");
  closedir(directory);
 }
#endif
}

#define BUFF_LEN 512
void apply_lang(const char* filename){
  FILE* conf_file = 0L;
  static char buff[BUFF_LEN];
  int wiiero_label_id = WIIERO_LANG_GAME_LOAD;
  if (!filename)
   return;
  snprintf(buff,511,"/switch/wiiero/lang/%s.wtf",filename);
  conf_file = fopen(buff,"r");
  if(!conf_file)
    return;
  while(fgets(buff,BUFF_LEN, conf_file )){
      char* str = buff;
      while(str && *str != '\0' && (*str == ' ' || *str == '\t')) // head trim
        str++;
      if(str && *str != '#'){ //skip comments
       char *end =  str;
       while(end && *end!='\n' && *end!='\0' && *end!='#')
         end++;
       if(end) *end = '\0';
       if(*str){
           while(*end == '\0' || *end == ' '){ // tail trim
               *end = '\0'; end--;
           }
           wiiero_label[wiiero_label_id++] = strdup(str);
           //printf("\"%s\"\n",str);
       }
      }
  }
}



