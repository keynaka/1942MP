/*
 * PowerUp.cpp
 *
 *  Created on: 24 de may. de 2016
 *      Author: keynaka
 */

#include "PowerUp.h"

using namespace std;

PowerUp::PowerUp(SDL_Renderer * renderer, Resolucion* &resolucion, Posicion* posicionInicial) {

	//this->configuracion = conf; //TODO: hay que agregar que guarde su configuracion de XML

	vistaPowerUp = new PowerUpView(renderer, "Shot");
	viviendo = true;

	this->id = 1; //TODO:hay que hacer que cada powerUp tenga una ID??

	this->posicion = posicionInicial;

	anchoFondo = resolucion->getWidthScreen();
	largoFondo = resolucion->getHeightScreen();

}

PowerUp::~PowerUp() {
	delete vistaPowerUp;
	delete posicion;
}

void PowerUp::setVelocidadStandard(int vel){
	velocidadStandard = vel;
}

int PowerUp::getAnchoFondo() {
	return anchoFondo;
}

int PowerUp::getLargoFondo() {
	return largoFondo;
}

int PowerUp::getAncho() {
	return vistaPowerUp->getAnchoVista();
}

int PowerUp::getLargo() {
	return vistaPowerUp->getLargoVista();
}

string PowerUp::getId() {
	return this->id;
}

void PowerUp::moverEjeX(int velX) {
	posicion->mover(velX,0);
//	cout << "El ancho PowerUp es: " << getAncho() << endl;
//	cout << "El ancho PowerUp es: " << getAnchoFondo() << endl;
//	cout << "El largo PowerUp es: " << getLargoFondo() << endl;
	if ((posicion->getX() < 0) || (posicion->getX() + getAncho() > getAnchoFondo())) {
		posicion->mover(-velX,0);
	}
}

void PowerUp::moverEjeY(int velY) {
	posicion->mover(0,velY);
	//cout << "El largo fondo es: " << getLargoFondo() << endl;
	if ((posicion->getY() - getLargo() < 0) || (posicion->getY() > getLargoFondo())) {
		posicion->mover(0,-velY);
	}
}

void PowerUp::mover(int velX, int velY) {
	//El powerUp se mueve siempre para abajo
	moverEjeX(velX);
	moverEjeY(velY);
}

void PowerUp::mostrar(){
	vistaPowerUp->mostrar(posicion->getX(),posicion->getYsdl());
}

void PowerUp::vivir(){
		mover(0, -velocidadStandard);
		mostrar();
}

int PowerUp::getX(){
	return posicion->getX();
}

int PowerUp::getY(){
	return posicion->getY();
}

bool PowerUp::aunVive(){
	return viviendo;
}