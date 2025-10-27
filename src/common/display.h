// Pequeño wrapper para unificar el uso de diferentes pantallas e-paper
#pragma once

#include <stdint.h>
#include "DEV_Config.h" // para tipos UBYTE/UWORD si se usan buffers de la lib

// Inicializa el driver de la pantalla activa (seleccionada por el entorno)
void Display_Init();

// Limpia la pantalla física (full update del panel)
void Display_Clear();

// Limpieza fuerte: realiza varios ciclos de negro/blanco a pantalla completa
// para eliminar fantasmas antes de dormir o almacenar. Opcional según panel.
// Si no está implementado para un panel, se puede mapear a Display_Clear.
void Display_ClearStrong();

// Envia los buffers a la pantalla (para paneles tricolor usar ambos)
// Para paneles B/W, pasar Red=nullptr y el wrapper lo ignorará
void Display_Present(const UBYTE* Black, const UBYTE* Red);

// Pone la pantalla en modo bajo consumo
void Display_Sleep();

// Dimensiones en píxeles del panel activo
uint16_t Display_Width();
uint16_t Display_Height();
