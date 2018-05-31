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


#ifndef DYN_OBJECT_H
#define DYN_OBJECT_H

#include "common.h"
#include <SDL/SDL.h>
#include "object.h"
#include "ressources.h"

extern obj_t* create_flesh(int x,int y,ressources_t* r,int pl_id);
extern obj_t* create_blood_trail(int x,int y);
extern obj_t* create_blood_drop(int x,int y);
extern obj_t* create_shell(int x,int y,ressources_t* r);
extern obj_t* create_mud_particle(int x,int y,int acc_x,int acc_y);
extern obj_t* create_explosion(int x,int y,ressources_t* r,int size);
extern obj_t* create_flash(int x,int y,ressources_t* r);
extern obj_t* create_smoke(int x,int y,ressources_t* r);
extern obj_t* create_nuke_smoke(int x,int y,ressources_t* r);
extern obj_t* create_reactor_particle(int x,int y);
#endif
