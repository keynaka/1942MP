/*
 * Posicion.cpp
 *
 *  Created on: 29/04/2016
 *      Author: gonzalo
 */

#include "Posicion.h"

Posicion::Posicion(int x, int y) {
	initResolucion();
	setPosicion(x, (limiteY - y));
}

Posicion::~Posicion() {
	resolucion->~Resolucion();
}

void Posicion::initResolucion() {
	resolucion = new Resolucion();
	limiteX = 0;
	limiteY = resolucion->getHeightScreen();
}

void Posicion::setPosicion(int x, int y) {
	this->x = x;
	this->y = y;
}

int Posicion::getX() {
	return x;
}

int Posicion::getY() {
	return y;
}

void Posicion::moverHorizontal(int x) {
	this->x += x;
}

void Posicion::moverVertical(int y) {
	this->y += y;
}


