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
#include <stdlib.h>

#ifdef PC_MODE
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>
#endif

#include <time.h>
#include "game.h"
#include "tools.h"
#include "fast_trig.h"
#include "lang.h"
int main(int argc, char * argv[]) {
  game_t game;
  srand(time(NULL));


#ifdef PC_MODE
  if(1){//set coredump
    struct rlimit rlim;
    rlim.rlim_cur = RLIM_INFINITY;
    rlim.rlim_max = RLIM_INFINITY;
    if( setrlimit(RLIMIT_CORE, &rlim) == 0 ) {
      //fprintf(stdout, "Coredump available\n");
    }else {
      HARD_DBG("error setrlimit coredump\n");
      exit(EXIT_FAILURE);
    }
  }
#endif

#ifdef WII_MODE
  /* Init wii sd fat layer */
  if(!fatInitDefault()){
    HARD_DBG("can't init SD card fat io interface... bye bye :-(");
    exit(EXIT_FAILURE);
  } 
#endif
  HARD_DBG_CLEAR();
  check_lang_files();
  //apply_lang(wiiero_lang_files[1]);

  HARD_DBG("init...\n");
  wiiero_init(&game);
  HARD_DBG("Loading...\n");
  wiiero_load(&game);
 
  //printf("Memory usage: %f\n", (1.0*get_process_used_memory() / 1024) / 1024);
  HARD_DBG("Running game...\n");
  while(wiiero_alive(&game)){
    wiiero_cycle(&game);
  }
  
  HARD_DBG("exiting...\n");  
  wiiero_quit(&game);
  //printf("Memory usage: %f\n", (1.0*get_process_used_memory() / 1024) / 1024);
  exit(EXIT_SUCCESS);
  return(0); 
}
