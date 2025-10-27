// Receptor simple de cuadros seriales para pantallas de tinta electrónica BN de 1-bit
#pragma once

#include <stdint.h>

// Configura constantes de protocolo
// Comando 'S' seguido de búfer BN empaquetado en bruto de tamaño anchoBytes*alto
// Comandos opcionales:
// - 'C' limpiar pantalla
// - 'Q' dormir pantalla

// Llama frecuentemente en loop(); cuando se recibe un cuadro completo, actualizará la pantalla.
void CuadrosSerial_IntentarRecibirYPresentar();
