#include "PlayersController.h"

using namespace std;

PlayersController::PlayersController(Vivible * unObj,SDL_Renderer* renderer, Resolucion* resolucion){
	AvionConf* config = ((Avion*) unObj)->getConfiguracion();
	velocidadStandard = config->velocidadDesplazamiento;;
	obj = unObj;
	obj->setVelocidadStandard(velocidadStandard);
	velX = 0;
	velY = 0;
	resolucionPantalla = resolucion;

	misilConf = new MisilConf();
	strcpy(misilConf->disparosSpriteID,config->disparosSpriteID);
	misilConf->velocidadDisparos = config->velocidadDisparos + config->velocidadDesplazamiento;
	controlDeMisiles = new ControllerMissiles(misilConf, renderer);
}

PlayersController::~PlayersController(){
		delete obj;
		delete controlDeMisiles;
		delete misilConf;
}

void PlayersController::press(SDL_Event *event){
        switch(event->key.keysym.sym){

            case SDLK_RIGHT: velX += velocidadStandard; break;
            case SDLK_LEFT: velX -= velocidadStandard; break;
            case SDLK_UP: velY += velocidadStandard; break;
            case SDLK_DOWN: velY -= velocidadStandard; break;
            case SDLK_AT: desconectarAvion = true ;break; // la o es una letra cualquiera. Como esta hecho con eventos, se tuvo que dejar asi
            case SDLK_KP_ENTER: obj->inicializoVueltereta();break;
            case SDLK_SPACE:
				if (!obj->haciendoVueltereta()){
					cout << "tirando misiles" << endl;
					crearMisil = true;
				}
				break;
            case SDLK_CLEAR: controlDeMisiles->setAmetralladora(); break;
            case SDLK_HASH: obj->inicializoEstacionar(); break;
            case SDLK_ASTERISK: obj->setHP(0); break;

        }

}

void PlayersController::hacerVivir(){

	if (desconectarAvion) {
		obj->desconectar();
		desconectarAvion = false;
	}

	obj->vivir(velX, velY);

	if (crearMisil) {
		controlDeMisiles->crearNuevoMisilEnPosicion(obj->getX()+25, obj->getY(), resolucionPantalla, misilConf);
		crearMisil = false;
	}
	controlDeMisiles->hacerVivir();
}
