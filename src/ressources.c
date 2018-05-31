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

#include "ressources.h"
#include "image.h"
#include "tools.h"
#include "surface_manip.h"

ressources_t* load_ressource(){

  ressources_t* r = secure_malloc(sizeof(ressources_t));
  DBG("- loading ressources\n");
  
  /* Player */
  r->worms1_skin = image_load(DEFAULT_PWSKIN);
  r->worms2_skin = image_load(DEFAULT_PWSKIN);
  r->worms_iron  = image_load(DEFAULT_PWSKIN);
  
  
  r->worms1_flesh = image_load(DEFAULT_PWFLESH);
  r->worms2_flesh = image_load(DEFAULT_PWFLESH);
  
  /* Map */ 
  r->breakable = image_load(DEFAULT_BREAKS);
  r->lrocks    = image_load(DEFAULT_LITTLE_ROCKS);
  r->brocks    = image_load(DEFAULT_BIG_ROCKS);
  
  
  /* Fx */
  r->gunfire_skin   = image_load(DEFAULT_GUNFIRE);
  r->shell          = image_load(DEFAULT_SHELL); 
  r->blue_explosion = image_load(DEFAULT_EXPLOSION_BLUE); 
  r->explosion      = image_load(DEFAULT_EXPLOSION);
  r->nuke_smoke     = image_load(DEFAULT_NUKE_SMOKE);
  r->zimm_flash     = image_load(DEFAULT_ZIMM_FLASH);
  r->teleport       = image_load(DEFAULT_TELEPORT);
  r->smoke          = image_load(DEFAULT_SMOKE);
  
  /* Weapons */  
  r->cannon         = image_load(DEFAULT_CANNON);
  r->grass_hopper   = image_load(DEFAULT_GRASS_HOPPER);
  r->mine           = image_load(DEFAULT_MINE);
  r->jmine          = image_load(DEFAULT_JUMP_MINE);
  r->grenade        = image_load(DEFAULT_GRENADE);
  r->gauss          = image_load(DEFAULT_GAUSS);
  r->cluster        = image_load(DEFAULT_CLUSTER);
  r->red_grenade    = image_load(DEFAULT_RED_GRENADE);
  r->gaz_grenade    = image_load(DEFAULT_GAZ_GRENADE);
  r->big_nuke       = image_load(DEFAULT_BIG_NUKE);
  r->mini_nuke      = image_load(DEFAULT_MINI_NUKE);
  r->nuke_piece     = image_load(DEFAULT_NUKE_PIECE);
  r->flamer         = image_load(DEFAULT_FLAMER);
  r->blue_flamer    = image_load(DEFAULT_BLUE_FLAMER);
  r->bouncy_larpa   = image_load(DEFAULT_BOUNCY_LARPA);
  r->bouncy_mine    = image_load(DEFAULT_BOUNCY_MINE);
  r->chiquita       = image_load(DEFAULT_CHIQUITA);
  r->chiquita_piece = image_load(DEFAULT_CHIQUITA_PIECE);
  r->crackler       = image_load(DEFAULT_CRACKLER);
  r->dart           = image_load(DEFAULT_DART);
  r->dirtball       = image_load(DEFAULT_DIRTBALL);
  r->doomsday       = image_load(DEFAULT_DOOMSDAY);
  r->explosives     = image_load(DEFAULT_EXPLOSIVES);
  r->float_mine     = image_load(DEFAULT_FLOAT_MINE);
  r->holy_grenade   = image_load(DEFAULT_HOLY_GRENADE);
  r->larpa          = image_load(DEFAULT_LARPA);
  r->missile        = image_load(DEFAULT_MISSILE);
  r->napalm         = image_load(DEFAULT_NAPALM);
  r->spikeball      = image_load(DEFAULT_SPIKEBALL);
  r->zimm           = image_load(DEFAULT_ZIMM);
  r->pinball        = image_load(DEFAULT_PINBALL);

  /* other */  
  r->house_p1       = image_load(DEFAULT_HOUSE);
  r->house_p2       = image_load(DEFAULT_HOUSE);
  r->hook           = image_load(DEFAULT_HOOK);
  r->health         = image_load(DEFAULT_HEALTH);
  r->flag_p1        = image_load(DEFAULT_FLAG);
  r->flag_p2        = image_load(DEFAULT_FLAG);
  r->bonus          = image_load(DEFAULT_BONUS);

  shift_ressource_colors(r->worms1_skin,-50,-50,50);
  shift_ressource_colors(r->worms2_skin,-50,0,-50);
  
  shift_ressource_colors(r->worms1_flesh,-50,-50,50);
  shift_ressource_colors(r->worms2_flesh,-50,0,-50);
  
  shift_ressource_colors(r->house_p1,-50,-50,50);
  shift_ressource_colors(r->house_p2,-50,0,-50);
  
  shift_ressource_colors(r->flag_p1,-50,0,-50);
  shift_ressource_colors(r->flag_p2,-50,-50,50);

  return r;
}



void shift_ressource_colors(SDL_Surface * s,Uint8 sr,Uint8 sg,Uint8 sb){
  int x,y;
  Uint8 r,g,b;
  for(x=0;x<s->w;x++){
    for(y=0;y<s->h;y++){
      get_pix_color(s,x,y,&r,&g,&b);
      if((r==g) && (g == b))
        put_pix_color(s,x,y,r+sr,g+sg,b+sb);
    }
  }
}


void release_ressource(ressources_t* r){
  ASSERT(r)
  SDL_FreeSurface( r->worms1_skin );
  SDL_FreeSurface( r->worms2_skin );
  SDL_FreeSurface( r->worms_iron );
  SDL_FreeSurface( r->worms1_flesh );
  SDL_FreeSurface( r->worms2_flesh );
  SDL_FreeSurface( r->breakable  );
  SDL_FreeSurface( r->lrocks );
  SDL_FreeSurface( r->brocks );
  SDL_FreeSurface( r->gunfire_skin );
  SDL_FreeSurface( r->shell );
  SDL_FreeSurface( r->blue_explosion );
  SDL_FreeSurface( r->explosion );
  SDL_FreeSurface( r->nuke_smoke );
  SDL_FreeSurface( r->zimm_flash );
  SDL_FreeSurface( r->teleport );
  SDL_FreeSurface( r->smoke );
  SDL_FreeSurface( r->cannon );
  SDL_FreeSurface( r->grass_hopper );
  SDL_FreeSurface( r->mine );
  SDL_FreeSurface( r->jmine );
  SDL_FreeSurface( r->grenade );
  SDL_FreeSurface( r->gauss );
  SDL_FreeSurface( r->cluster );
  SDL_FreeSurface( r->red_grenade );
  SDL_FreeSurface( r->gaz_grenade );
  SDL_FreeSurface( r->big_nuke );
  SDL_FreeSurface( r->mini_nuke );
  SDL_FreeSurface( r->nuke_piece );
  SDL_FreeSurface( r->flamer );
  SDL_FreeSurface( r->blue_flamer );
  SDL_FreeSurface( r->bouncy_larpa );
  SDL_FreeSurface( r->bouncy_mine );
  SDL_FreeSurface( r->chiquita );
  SDL_FreeSurface( r->chiquita_piece );
  SDL_FreeSurface( r->crackler );
  SDL_FreeSurface( r->dart );
  SDL_FreeSurface( r->dirtball );
  SDL_FreeSurface( r->doomsday );
  SDL_FreeSurface( r->explosives );
  SDL_FreeSurface( r->float_mine );
  SDL_FreeSurface( r->holy_grenade );
  SDL_FreeSurface( r->larpa );
  SDL_FreeSurface( r->missile );
  SDL_FreeSurface( r->napalm );
  SDL_FreeSurface( r->spikeball );
  SDL_FreeSurface(  r->zimm );
  SDL_FreeSurface( r->pinball );
  SDL_FreeSurface( r->house_p1 );
  SDL_FreeSurface( r->house_p2 );
  SDL_FreeSurface( r->hook );
  SDL_FreeSurface( r->health );
  SDL_FreeSurface( r->flag_p1 );
  SDL_FreeSurface( r->flag_p2 );
  SDL_FreeSurface( r->bonus );
}

