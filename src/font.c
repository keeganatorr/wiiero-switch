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


#include "font.h"
#include "tools.h"
#include "image.h"
#include "ressources.h"
#include "common.h"

static SDL_Surface * used_littlefont = 0L;
static SDL_Surface * used_medfont = 0L;
static SDL_Surface * used_bigfont = 0L;
static SDL_Surface * used_font = 0L;
static SDL_Surface * output_surface = 0L;

#define FONT_IMG_H_ELEMS 6
#define FONT_IMG_W_ELEMS 7

const char font_charset[CHARSET_SIZE] = "abcdefghijklmnopqrstuvwxyz.:+!0123456789[] ";
static Uint8 font_charpos[256]= {
   42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42, /*                  \t\n  \f\r     */ 
   42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42, /*                                 */
   42,29,42,42,42,42,42,42,42,42,42,28,42,42,26,42, /*   ! " # $ % & ' ( ) * + , - . / */
   30,31,32,33,34,35,36,37,38,39,27,42,42,42,42,42, /* 0 1 2 3 4 5 6 7 8 9 : ; < = > ? */
   42, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14, /* @ A B C D E F G H I J K L M N O */
   15,16,17,18,19,20,21,22,23,24,25,40,42,41,42,42, /* P Q R S T U V W X Y Z [ \ ] ^ _ */
   42, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14, /* ` a b c d e f g h i j k l m n o */
   15,16,17,18,19,20,21,22,23,24,25,42,42,42,42,42, /* p q r s t u v w x y z { | } ~   */
   42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42, /*                                 */
   42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42, /*                                 */
   42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42, /*                                 */
   42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42, /*                                 */
   42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42, /*                                 */
   42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42, /*                                 */
   42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42, /*                                 */
   42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42  /*                                 */
  };

Uint8 font_get_char_pos_in_charset(char c){
return font_charpos[(int)c];
}

/* ************************************************************************** */
void font_load(SDL_Surface* s){
  HARD_DBG("- loading "DEFAULT_MFONT"\n");
  used_medfont     = image_load(DEFAULT_MFONT);
  HARD_DBG("- loading "DEFAULT_LFONT"\n");
  used_littlefont  = image_load(DEFAULT_LFONT);
  HARD_DBG("- loading "DEFAULT_BFONT"\n");
  used_bigfont     = image_load(DEFAULT_BFONT); 
  used_font = 0L;
  output_surface = s;
}


/* ************************************************************************** */
static __inline__ void 
sdl_print_char_on_cam(camera_t* c,char charact,int x,int y,int off_x,int off_y){
  ASSERT(c);
  ASSERT(charact);
  SDL_Rect camera_offset;
  SDL_Rect font_offset;
  Uint8 text = charact;
  if(charact == ' ')
    return;

  text = font_get_char_pos_in_charset(text);
  font_offset.w = used_font->w / FONT_IMG_W_ELEMS;
  font_offset.h = used_font->h / FONT_IMG_H_ELEMS;
  font_offset.x = (text / FONT_IMG_H_ELEMS ) * font_offset.w;
  font_offset.y = (text % FONT_IMG_H_ELEMS ) * font_offset.h;
  camera_offset.x = x - off_x - font_offset.w / 2;
  camera_offset.y = y - off_y - font_offset.h / 2 + 1;
 // SDL_BlitSurface(used_font, &font_offset ,s, &camera_offset);
  camera_blit_surface_on(c,used_font,&font_offset,&camera_offset);
}
/* ************************************************************************** */
static __inline__ void 
sdl_print_char_on_surface(SDL_Surface* s,char charact,int x,int y,int off_x,int off_y){
  ASSERT(s);
  ASSERT(charact);
  SDL_Rect camera_offset;
  SDL_Rect font_offset;
  Uint8 text = charact;
  if(charact == ' ')
    return;

  text = font_get_char_pos_in_charset(text);
  font_offset.w = used_font->w / FONT_IMG_W_ELEMS;
  font_offset.h = used_font->h / FONT_IMG_H_ELEMS;
  font_offset.x = (text / FONT_IMG_H_ELEMS ) * font_offset.w;
  font_offset.y = (text % FONT_IMG_H_ELEMS ) * font_offset.h;
  camera_offset.x = x - off_x - font_offset.w / 2;
  camera_offset.y = y - off_y - font_offset.h / 2 + 1;
  SDL_BlitSurface(used_font, &font_offset ,s, &camera_offset);
}

/* ************************************************************************** */
void font_print_char(camera_t* c,char charact,int x, int y,int off_x,int off_y){
  ASSERT(c);
  ASSERT(charact);
  sdl_print_char_on_cam(c,charact,x,y,off_x,off_y);  
}

/* ************************************************************************** */
void font_print(camera_t* c,char* text,int x,int y,fontsize_t fsize){
  ASSERT(c);
  ASSERT(text);
  used_font = (fsize == FONT_BIG) 
              ? used_bigfont 
              : ((output_surface->w > 500)
                 ? used_medfont
                 : used_littlefont);
  ASSERT(used_font)
  x -= (strlen(text) * used_font->w / FONT_IMG_W_ELEMS)/2;
  while(*text){
    font_print_char(c,*text,x,y,c->map_x,c->map_y);
    text++;
    x += (used_font->w / FONT_IMG_W_ELEMS);
  }
}

/* ************************************************************************** */
void font_print_center(camera_t* c,char* text,int x,int y,fontsize_t fsize){
  ASSERT(c);
  if(!text) text = "";
  used_font = (fsize == FONT_BIG) ? used_bigfont : ((output_surface->w > 500)?used_medfont:used_littlefont);
  ASSERT(used_font)
  x -= (strlen(text) * used_font->w / FONT_IMG_W_ELEMS)/2;
  while(*text){
    font_print_char(c,*text,x,y,0,0);
    text++;
    x += (used_font->w / FONT_IMG_W_ELEMS);
  }
}

/* ************************************************************************** */
void font_print_strict_pos(camera_t* c,char* text,int x,int y,fontsize_t fsize){
  ASSERT(c);
  //ASSERT(text);
  if(!text) text = "";
  used_font = (fsize == FONT_BIG) ? used_bigfont : ((output_surface->w > 500)?used_medfont:used_littlefont);
  ASSERT(used_font);
  while(*text){
    font_print_char(c,*text,x,y,0,0);
    text++;
    x += (used_font->w / FONT_IMG_W_ELEMS);
  } 
}
/* ************************************************************************** */
int font_get_width(fontsize_t fsize){
  used_font = (fsize == FONT_BIG) ? used_bigfont : ((output_surface->w > 500)?used_medfont:used_littlefont);
  return used_littlefont->w / FONT_IMG_W_ELEMS;
}
/* ************************************************************************** */
int font_get_height(fontsize_t fsize){
  used_font = (fsize == FONT_BIG) ? used_bigfont : ((output_surface->w > 500)?used_medfont:used_littlefont);
  return used_littlefont->h / FONT_IMG_H_ELEMS;}
/* ************************************************************************** */
void font_putc_strict_pos(camera_t* c,char charact,int x, int y,fontsize_t fsize){
  ASSERT(c);
  used_font = (fsize == FONT_BIG) ? used_bigfont : ((output_surface->w > 500)?used_medfont:used_littlefont);
  sdl_print_char_on_cam(c,charact,x,y,0,0);
}

/* ************************************************************************** */
void font_console_print_debug(char* text,fontsize_t fsize){
#ifdef INGAME_DEBUG_ON
  font_console_print(text,fsize);
#else
  font_progress_print(".",fsize);
#endif
}

/* ************************************************************************** */
void font_console_print(char* text,fontsize_t fsize){

  static int console_x_pos=4;
  static int console_y_pos=4;
  SDL_Surface* s = output_surface;
  ASSERT(s);
  ASSERT(text);
  used_font = (fsize == FONT_BIG) ? used_bigfont : ((output_surface->w > 500)?used_medfont:used_littlefont);
  ASSERT(used_font);
  while(*text){
    if(*text=='\n'){
      console_y_pos+=used_font->h/FONT_IMG_H_ELEMS+1;
      if(console_y_pos >= s->h)
        console_y_pos = 4;
      console_x_pos=4;
    }else{
      sdl_print_char_on_surface(s,*text,console_x_pos,console_y_pos,0,0);
      console_x_pos += (used_font->w / FONT_IMG_W_ELEMS);
    }
    text++;
  } 
  SDL_Flip(s);

}

/* ************************************************************************** */
void font_progress_print(char* text,fontsize_t fsize){
  static int x=0;
  static int y=0;
  static Uint8 initialised = 0;
  SDL_Surface* s = output_surface;
  ASSERT(s);
  ASSERT(text);
  used_font = (fsize == FONT_BIG) ? used_bigfont : ((output_surface->w > 500)?used_medfont:used_littlefont);
  if(initialised == 0){
   x = s->w/2 - (strlen(text) * used_font->w / FONT_IMG_W_ELEMS)/2;
   y = s->h/2 + 30;
   initialised = 1;
  }
  ASSERT(used_font);
  while(*text){
    sdl_print_char_on_surface(s,*text,x,y,0,0);
    text++;
    x += (used_font->w / FONT_IMG_W_ELEMS);
  }
  SDL_Flip(output_surface);

}
