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


#include "tools.h"
#include "fast_trig.h"
#include <string.h>

#ifdef TRIGO_BUILD
#include "math.h"
  
void ftrigo_cdump(int f_angl,int l_angl){
  int angle;
  printf("static double sin_precalc[]={");
  for(angle=f_angl;angle<=l_angl;angle++){
    printf("%f,",sin( angle * M_PI / 180.0f ));
  }printf("0};\n\n");
  
  printf("static double sin_precalc[]={");
  for(angle=f_angl;angle<=l_angl;angle++){
     printf("%f,",cos( angle * M_PI / 180.0f ));
  }printf("0};\n\n");
}

void ftrigo_check_values(int f_angl,int l_angl){
  int angle;
  DBG("check sin\n");
  for(angle=f_angl;angle<=l_angl;angle++){
    char val1[16];
    char val2[16];
    snprintf(val1,15,"%f",sin( angle * M_PI / 180.0f ));
    snprintf(val2,15,"%f",trigo_sin(angle));
    if (strcmp(val1,val2)!=0){
      printf("DIFF %s %s\n",val1,val2);
    }
  }
  DBG("check cos\n");
  for(angle=f_angl;angle<=l_angl;angle++){
    char val1[16];
    char val2[16];
    snprintf(val1,15,"%f",cos( angle * M_PI / 180.0f ));
    snprintf(val2,15,"%f",trigo_cos(angle));
    if (strcmp(val1,val2)!=0){
      printf("DIFF %s %s\n",val1,val2);
    }
  }
}
#endif

void ftrigo_check(int first_angl,int end_angl){
  ASSERT(first_angl == FIRST_ANGLE);
  ASSERT(end_angl   == LAST_ANGLE);
  #ifdef TRIGO_BUILD
    ftrigo_check_values(FIRST_ANGLE,LAST_ANGLE);
  #endif
}
