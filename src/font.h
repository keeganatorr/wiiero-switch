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


#ifndef FONT_H
#define FONT_H
#include <SDL/SDL.h>
#include "camera.h"

typedef enum{
  FONT_STANDARD = 0,
  FONT_SMALL = FONT_STANDARD,
  FONT_SELECTED = 1,
  FONT_BIG = FONT_SELECTED
}fontsize_t;

#define CHARSET_SIZE 43
extern const char font_charset[CHARSET_SIZE];

void  font_load(SDL_Surface* console_surface);
void  font_print(camera_t* c,char* text,int x,int y,fontsize_t fsize);
void  font_print_center(camera_t* c,char* text,int x,int y,fontsize_t fsize);
void  font_print_strict_pos(camera_t* c,char* text,int x,int y,fontsize_t fsize);
void  font_putc_strict_pos(camera_t* c,char charact,int x, int y,fontsize_t fsize);
void  font_console_print(char* text,fontsize_t fsize);
void  font_console_print_debug(char* text,fontsize_t fsize);
void  font_progress_print(char* text,fontsize_t fsize);
Uint8 font_get_char_pos_in_charset(char);
int   font_get_width(fontsize_t f);
int   font_get_height(fontsize_t f);
 #endif /* FONT_H */
