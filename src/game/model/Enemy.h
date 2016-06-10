#ifndef SRC_MODEL_Enemy_H_
#define SRC_MODEL_Enemy_H_

#include "../view/AvionView.h"
#include "Vivible.h"
#include "../controller/Timer.h"
#include "Resolucion.h"
#include "../model/Posicion.h"
#include "../../libs/data/player_data.h"
#include "../../xml/conf/AvionConf.h"
#include <string>
#include "Misil.h"
#include "../view/ExplosionView.h"
#include "../view/Screen.h"

class Enemy: public Vivible {
private:
	string id;
	int anchoFondo, largoFondo;
	int velocidadStandard;
	int vida;
	bool viviendo;

	bool realizandoVueltereta;

	bool llegoPuntoDeEstacionamiento;
	bool estacionando;
	Timer* t;

	Screen* screen;
	AvionView* vistaAvion;
	ExplosionView* explosion;
	Posicion* posicion;
	AvionConf* configuracion;

	Posicion* posicionAEstacionar;

	void realizoVueltereta();
	void mostrarVueltereta(int frame);

	void mostrar(int velX, int velY);
	void mover(int velX, int velY);
	void moverEjeX(int velX);
	void moverEjeY(int velY);

	int angleX;
	int angleY;

public:
	Enemy(Screen* screen, SDL_Renderer * renderer, Resolucion* &resolucion, Posicion* posicionInicial, AvionConf* conf);
	~Enemy();

	string getId();
	int getAnchoFondo();
	int getLargoFondo();
	int getAncho();
	int getLargo();
	int getX();
	int getY();
	int getVida();

	AvionConf* getConfiguracion();

	void setVelocidadStandard(int vel);
	bool haciendoVueltereta();
	void inicializoVueltereta();

	void vivir(int velX, int velY);
	void vivirRandom();
	bool aunVive();
	void desconectar();

	void setAmetralladora();

	void recibirMisil(Misil* misil);
	bool tieneHP();
	void setHP(int hp);

	void inicializoEstacionar();
	bool estaEstacionando();
	void mostrarEstacionar(int frame);
	void despegar();

	Posicion* getPosicion();

	bool hit(int, int);
	void moverRandom();
	void morir();
};

#endif /* SRC_MODEL_Enemy_H_ */
