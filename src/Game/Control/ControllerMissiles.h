/*
 * ControllerAutomatic.h
 *
 *  Created on: 30 de abr. de 2016
 *      Author: ramon
 */

#ifndef SRC_GAME_CONTROL_CONTROLLERMISSILES_H_
#define SRC_GAME_CONTROL_CONTROLLERMISSILES_H_

#include "../Model/CompositeVivibles.h"
#include "SDL2/SDL.h"
#include "../Model/Avion.h"
#include "../Model/Misil.h"

#include <iostream>

using namespace std;

class ControllerMissiles{
	public:
		ControllerMissiles(SDL_Renderer* &renderer);
		virtual ~ControllerMissiles();
		void press(SDL_Event *event);
		void hacerVivir();
		void crearNuevoMisilEnPosicion(int x, int y);
	private:
		CompositeVivibles* vivibles;
		SDL_Renderer* rendererMisil;
		int distanciaDeDesplazamiento = 5;//Todo sacarlo del hardcodeo

};

#endif /* SRC_GAME_CONTROL_CONTROLLERMISSILES_H_ */
