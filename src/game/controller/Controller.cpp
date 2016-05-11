#include "Controller.h"

using namespace std;

Controller::Controller(Vivible * unObj,SDL_Renderer* &renderer, Resolucion* resolucion, Client* client){
	this->cliente = client;
	velocidadStandard = 7;
	obj = unObj;
	obj->setVelocidadStandard(velocidadStandard);
	velX = 0;
	velY = 0;
	resolucionPantalla = resolucion;
 	controlDeMisiles = new ControllerMissiles(renderer);
}

Controller::~Controller(){
		delete obj;
		delete controlDeMisiles;
}

void Controller::press(SDL_Event *event){
  Evento* e;
  CompanionEvent* ce = new CompanionEvent();

    if( event->type == SDL_KEYDOWN && event->key.repeat == 0){
        switch( event->key.keysym.sym ){
            case SDLK_RIGHT: velX += velocidadStandard; cliente->sendData(ce->right(obj->getId())); break;
            case SDLK_LEFT: velX -= velocidadStandard; cliente->sendData(ce->left(obj->getId())); break;

            case SDLK_UP: velY += velocidadStandard; cliente->sendData(ce->up(obj->getId())); break;
            case SDLK_DOWN: velY -= velocidadStandard; cliente->sendData(ce->down(obj->getId()));break;

            case SDLK_KP_ENTER: obj->inicializoVueltereta(); cliente->sendData(ce->roll(obj->getId())); break;

			case SDLK_SPACE:
				if (!obj->haciendoVueltereta())
					controlDeMisiles->crearNuevoMisilEnPosicion(obj->getX()+25, obj->getY(), resolucionPantalla);
					cliente->sendData(ce->shot(obj->getId()));
				break;
        }
    }
    else if( event->type == SDL_KEYUP && event->key.repeat == 0){
        switch( event->key.keysym.sym ){
            case SDLK_RIGHT: velX -= velocidadStandard; cliente->sendData(ce->left(obj->getId())); break;
            case SDLK_LEFT: velX += velocidadStandard; cliente->sendData(ce->right(obj->getId())); break;

            case SDLK_UP: velY -= velocidadStandard; cliente->sendData(ce->down(obj->getId())); break;
            case SDLK_DOWN: velY += velocidadStandard; cliente->sendData(ce->up(obj->getId())); break;

            /*case SDLK_SPACE: controlDeMisiles->crearNuevoMisilEnPosicion(obj->getX(),obj->getY()); break;*/
        }
    }
}

void Controller::hacerVivir(){
	obj->vivir(velX, velY);
	controlDeMisiles->hacerVivir();
}
