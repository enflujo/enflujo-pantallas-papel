#ifndef PANTALLA_H
#define PANTALLA_H

#include "DEV_Config.h"

// Interfaz común de pantalla para todos los paneles de tinta electrónica
void Pantalla_Iniciar();
void Pantalla_Limpiar();
void Pantalla_Presentar(const UBYTE* Negro, const UBYTE* Rojo);
void Pantalla_Dormir();

uint16_t Pantalla_Ancho();
uint16_t Pantalla_Alto();

#endif // PANTALLA_H
