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

#include "surface_manip.h"
#include "tools.h"
#include "colors.h"

extern Uint8 selected_amb;
extern Uint8 transparent_r_value;
extern Uint8 transparent_g_value;
extern Uint8 transparent_b_value;

void get_empty_layer_position(int* x, int* y,const SDL_Surface* layer){
    Uint8 r=0x0, g=0x0, b=0x0;
    while((r!=transparent_r_value) && (g!=transparent_g_value) && (b!=transparent_b_value)){
      *x = rand() % layer->w;
      *y = rand() % layer->h;
      get_pix_color(layer,*x,*y,&r,&g,&b);
    }
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static Uint32 get_pix_uintcolor(const SDL_Surface* surface, int x, int y){
  ASSERT(surface);
  int bpp = surface->format->BytesPerPixel;
  Uint8 *p = 0L;
  p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
  ASSERT(p);
  switch(bpp){
    case 1:
      return *p;
    case 2:
      return *((Uint16 *)p);
    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
        return p[0] << 16 | p[1] << 8 | p[2];
      else
        return p[0] | p[1] << 8 | p[2] << 16;
    case 4:
      return *((Uint32 *)p);
    default:
      return 0;
  }
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 
void get_pix_color(const SDL_Surface * surface, int x, int y
                  , Uint8 * r, Uint8 * g, Uint8 * b){
  ASSERT(surface);
  ASSERT(r);ASSERT(g);ASSERT(b);
  if((x>=surface->w) || (x<0) || (y>=surface->h) || (y<0)){
    *r = 0x00;
    *g = 0x00;
    *b = 0x00;
  }else{
    SDL_GetRGB(get_pix_uintcolor(surface, x, y), surface->format,
             (Uint8*)   r, (Uint8*) g, (Uint8*) b);
  }
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void put_pix_color(SDL_Surface* surface, int x, int y, int r, int g, int b){
  ASSERT(surface);
  if((x>=surface->w) || (x<0) || (y>=surface->h) || (y<0))
    return;
  else{
  SDL_Rect particle;
  particle.x = x;
  particle.y = y;
  particle.w = 1; 
  particle.h = 1;
  SDL_FillRect(surface,&particle,SDL_MapRGB( surface->format, r, g, b));
  }
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 

void drow_hline(SDL_Surface* surface,int x,int y,int w,int h,int cr, int cg, int cb){
  SDL_Rect r;
  ASSERT(surface);
  r.x = x;r.y = y;
  r.w = w;r.h = h;
  SDL_FillRect(surface, &r
              , SDL_MapRGB( surface->format,cr,cg,cb) );  
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */



void drow_colored_hline(SDL_Surface* surface,int x,int y,int w,int h){
  ASSERT(surface); 
  int lx,ly;
  Uint8 r,g,b;
  for(lx=x;lx<x+w;lx++)
    for(ly=y;ly<y+h;ly++){
      int color = rand() % CPALETT; 
      get_grounds_colors(selected_amb,FRONT_GR,color,&r,&g,&b);
      put_pix_color(surface,lx,ly,r,g,b);
    }
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void drow_circle(SDL_Surface* surface,int cx,int cy,int cr,int r,int g,int b){
  int d, y, x;
  ASSERT(surface);
  d = 3 - (2 * cr);
  x = 0;
  y = cr;
  while (y >= x) {
    drow_hline(surface,cx - x, cy - y, 2 * x + 1,1,r,g,b);
    drow_hline(surface,cx - x, cy + y, 2 * x + 1,1,r,g,b);
    drow_hline(surface,cx - y, cy - x, 2 * y + 1,1,r,g,b);
    drow_hline(surface,cx - y, cy + x, 2 * y + 1,1,r,g,b);
    if (d < 0)
      d = d + (4 * x) + 6;
    else {
      d = d + 4 * (x - y) + 10;
      y--;
    }
    x++;
  }
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void drow_colored_circle(SDL_Surface* surface,int cx,int cy,int cr){
  int d, y, x;
  ASSERT(surface);
  d = 3 - (2 * cr);
  x = 0;
  y = cr;
  while (y >= x) {
    drow_colored_hline(surface,cx - x, cy - y, 2 * x + 1,1);
    drow_colored_hline(surface,cx - x, cy + y, 2 * x + 1,1);
    drow_colored_hline(surface,cx - y, cy - x, 2 * y + 1,1);
    drow_colored_hline(surface,cx - y, cy + x, 2 * y + 1,1);
    /*drow_hline(surface,cx - x, cy - y, 2 * x + 1,1,0xff,0xff,0xff);
    drow_hline(surface,cx - x, cy + y, 2 * x + 1,1,0xff,0xff,0xff);
    drow_hline(surface,cx - y, cy - x, 2 * y + 1,1,0xff,0xff,0xff);
    drow_hline(surface,cx - y, cy + x, 2 * y + 1,1,0xff,0xff,0xff);*/
    if (d < 0)
      d = d + (4 * x) + 6;
    else {
      d = d + 4 * (x - y) + 10;
      y--;
    }
    x++;
  }
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void drow_line(SDL_Surface* surface,int x1, int y1, int x2, int y2,int cr, int cg, int cb){
  //Uint32 coul = SDL_MapRGB( surface->format,cr,cg,cb);
  int d, dx, dy, aincr, bincr, xincr, yincr, x, y;

  if (abs(x2 - x1) < abs(y2 - y1)) {
    /* parcours par l'axe vertical */
    if (y1 > y2) {
      switch_values(&x1, &x2);
      switch_values(&y1, &y2);
    }
    xincr = x2 > x1 ? 1 : -1;
    dy = y2 - y1;
    dx = abs(x2 - x1);
    d = 2 * dx - dy;
    aincr = 2 * (dx - dy);
    bincr = 2 * dx;
    x = x1;
    y = y1;
    put_pix_color(surface,x, y, cr, cg,  cb);
    for (y = y1+1; y <= y2; ++y) {
      if (d >= 0) {
        x += xincr;
        d += aincr;
      } else
        d += bincr;

      put_pix_color(surface,x, y, cr, cg,  cb);
    }
  } else {
    /* parcours par l'axe horizontal */
    if (x1 > x2) {
      switch_values(&x1, &x2);
      switch_values(&y1, &y2);
    }
    yincr = y2 > y1 ? 1 : -1;
    dx = x2 - x1;
    dy = abs(y2 - y1);
    d = 2 * dy - dx;
    aincr = 2 * (dy - dx);
    bincr = 2 * dy;
    x = x1;
    y = y1;
    put_pix_color(surface,x, y, cr, cg,  cb);
    for (x = x1+1; x <= x2; ++x) {
      if (d >= 0) {
        y += yincr;
        d += aincr;
      } else
        d += bincr;
      put_pix_color(surface,x, y, cr, cg,  cb);
    }
  }    
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
SDL_Surface* surface_try_to_optimize(SDL_Surface* in, char* layer_name){
#ifdef LAYERS_OPTIM_MODE
  SDL_Surface* tmp = 0l;
  tmp = SDL_DisplayFormat(in);
  if(!tmp){
    printf("can't optimise %s!\n",layer_name);
    return in;
  }else{
    //printf("%s optim done!\n",layer_name);
    SDL_FreeSurface(in);
    return tmp;
  }
#else
  return in;
#endif
}/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
