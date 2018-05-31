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

#ifndef COMMON_H
#define COMMON_H


/* Game resolutions */
typedef enum { RES_320_240
              ,RES_480_272
              ,RES_480_320
              ,RES_600_400
              ,RES_640_420
              ,RES_640_480
              ,RES_720_480
              ,RES_1280_720
              ,MAX_RES
} screen_res_t;


/* Enable multilang spport */
#define MULTI_LANG

/* Old rendering mode */
#define OLD_MODE

/* InGame debug (LoadScreen) */
#define INGAME_DEBUG_ON
//#define DEBUG_ON


/* Disable framerate */
//#define NO_FRAME_RATE

/* Disable sound */
#define NO_SOUND

#endif
