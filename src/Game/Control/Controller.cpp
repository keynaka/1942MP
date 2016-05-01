#include "Controller.h"

using namespace std;

Controller::Controller(Vivible * unObj){
	obj = unObj;
	velX = 0;
	velY = 0;
}

Controller::~Controller(){
		delete obj;
}

void Controller::press(SDL_Event *event){

    if( event->type == SDL_KEYDOWN && event->key.repeat == 0){
        switch( event->key.keysym.sym ){
            case SDLK_RIGHT: velX += obj->getAncho() / distanciaDeDesplazamiento; break;
            case SDLK_LEFT: velX -= obj->getAncho() / distanciaDeDesplazamiento; break;

            case SDLK_UP: velY -= obj->getLargo() / distanciaDeDesplazamiento; break;
            case SDLK_DOWN: velY += obj->getLargo() / distanciaDeDesplazamiento; break;
        }
    }
    else if( event->type == SDL_KEYUP && event->key.repeat == 0){
        switch( event->key.keysym.sym ){
            case SDLK_RIGHT: velX -= obj->getAncho() / distanciaDeDesplazamiento; break;
            case SDLK_LEFT: velX += obj->getAncho() / distanciaDeDesplazamiento; break;

            case SDLK_UP: velY += obj->getLargo() / distanciaDeDesplazamiento; break;
            case SDLK_DOWN: velY -= obj->getLargo() / distanciaDeDesplazamiento; break;
        }
    }
}

void Controller::hacerVivir(){
	obj->vivir(velX, velY);
}