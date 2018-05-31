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
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


long get_process_used_memory( void ) {
  char proc_filename[ 256 ];
  char line[ 256 ];
  long ram = 0;         /* returned value */
  pid_t pid = getpid(); /* get current process ID */
  FILE* f;

  /* build current process filename */
  sprintf( proc_filename, "/proc/%d/status", pid );
  
  /* try to open it */
  if ( (f = fopen( proc_filename, "r" )) == NULL )
    return ram;

  /* parse it */
  while ( fgets( line, sizeof( line ), f ) )
    if ( sscanf( line, "VmSize:%ld", &ram ) ) {
      ram <<= 10; /* convert size to bytes */
      break;
    }

  /* close it */
  fclose( f );

  /* return process used memory */
  return ram;

}
