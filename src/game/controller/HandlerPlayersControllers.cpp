/*
 * HandlerPlayersControllers.cpp
 *
 *  Created on: 07/05/2016
 *      Author: gonzalo
 */

#include "HandlerPlayersControllers.h"

HandlerPlayersControllers::HandlerPlayersControllers(SDL_Renderer* &renderer, Resolucion* resolucion) {
	this->renderer = renderer;
	this->resolucion = resolucion;
}

HandlerPlayersControllers::~HandlerPlayersControllers() {
	for (map<string, IController*>::iterator it = this->mapaControllers.begin(); it != this->mapaControllers.end(); ++it) {
		delete it->second;
	}
}

void HandlerPlayersControllers::setPlayer(Avion* player) {
	IController* controller = new PlayersController(player, renderer, resolucion);
	this->mapaControllers[player->getId()] = controller;
}

void HandlerPlayersControllers::mover(string id, char evento) {

	IController* controller = mapaControllers[id];
	SDL_Event* eventoSdl;
	SDL_Keycode code;

	switch(evento) {
		case 'R': code = SDLK_RIGHT		; break;
		case 'L': code = SDLK_LEFT		; break;
		case 'U': code = SDLK_UP		; break;
		case 'D': code = SDLK_DOWN		; break;
		case 'E': code = SDLK_KP_ENTER	; break;
		case 'S': code = SDLK_SPACE		; break;
	}

	eventoSdl->key.keysym.sym = code;
	controller->press(eventoSdl);
}

void HandlerPlayersControllers::hacerVivir() {
	for (map<string, IController*>::iterator it = this->mapaControllers.begin(); it != this->mapaControllers.end(); ++it) {
		it->second->hacerVivir();
	}
}

