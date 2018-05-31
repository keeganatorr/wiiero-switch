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

#ifndef TOOLS_H
#define TOOLS_H

#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef WII_MODE
#include <fat.h>
#endif
#ifdef SWITCH
#include <switch.h>
#endif

#ifdef DEBUG_ON
#define SHOW_FPS
#endif

#define ERROR(...) { \
          DBG("%s : %u\n",__FILE__, __LINE__); \
          DBG(__VA_ARGS__);\
          exit(EXIT_FAILURE);\
        };
        
#ifdef DEBUG_ON
#  define ASSERT(expression) { if(!(expression)) ERROR("Assert failure!\n"); };
#  define DBG(...) { char buffer[500]; sprintf(buffer,__VA_ARGS__); svcOutputDebugString(buffer,strlen(buffer)); printf(__VA_ARGS__); }
#  define HARD_DBG_CLEAR() { FILE * fp=fopen("dbg.txt","w"); fclose(fp);}
#  define HARD_DBG(...) { char buffer[500]; sprintf(buffer,__VA_ARGS__); svcOutputDebugString(buffer,strlen(buffer)); FILE * fp=fopen("/switch/wiiero/dbg.txt","a"); fprintf(fp,__VA_ARGS__); fclose(fp);}
#else
#  define ASSERT(expression) ;
#  define DBG(...) ;
#  define HARD_DBG_CLEAR() ;
#  define HARD_DBG(...) ;
//#  define HARD_DBG(...) { char buffer[500]; sprintf(buffer,__VA_ARGS__); svcOutputDebugString(buffer,strlen(buffer)); FILE * fp=fopen("/switch/wiiero/dbg.txt","a"); fprintf(fp,__VA_ARGS__); fclose(fp);}
//#  define HARD_DBG(...) ;
//#  define HARD_DBG(...) { FILE * fp=fopen("dbg.txt","a"); fprintf(fp,__VA_ARGS__); fclose(fp);}
#endif

/*
#ifdef DEBUG_ON
#  define ASSERT(expression) { if(!(expression)) ERROR("Assert failure!\n"); };
#  define DBG(...) printf(__VA_ARGS__);
#  define HARD_DBG_CLEAR() { FILE * fp=fopen("dbg.txt","w"); fclose(fp);}
#  define HARD_DBG(...) { FILE * fp=fopen("dbg.txt","a"); fprintf(fp,__VA_ARGS__); fclose(fp);}
#else
*/



static void* secure_malloc(size_t size);
static void secure_free(void* p);
extern long get_process_used_memory( void ) ;


/* * * * * * * * * * * * * * INLINE CODE * * * * * * * * * * * * * * * * * * * */

static __inline__ void* secure_malloc(size_t s){
  void *ptr ;
  ptr = malloc(s);
  memset(ptr,0,s);
  if(!ptr)
    ERROR("Out of memory !\n");
  return ptr;

}

static __inline__ void secure_free(void* p){
    ASSERT(p);
    free(p);
}

#endif
