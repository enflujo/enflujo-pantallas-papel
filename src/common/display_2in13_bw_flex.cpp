#ifdef EPAPER_2IN13_BW_FLEX
#include "common/display.h"
#include "utility/EPD_2in13d.h"

void Pantalla_Iniciar() {
  EPD_2IN13D_Init();
}

void Pantalla_Limpiar() {
  EPD_2IN13D_Clear();
}

void Pantalla_Presentar(const UBYTE* Negro, const UBYTE* /*Rojo*/) {
  // SIEMPRE usar refresco completo para fotos individuales
  // Esto elimina ghosting pero causa parpadeo negro (aceptable para fotos)
  // Para animaciones rápidas sin parpadeo, usar EPD_2IN13D_DisplayPart() directamente
  EPD_2IN13D_Display((UBYTE*)Negro);
}

void Pantalla_Dormir() {
  EPD_2IN13D_Sleep();
}

uint16_t Pantalla_Ancho() {
  return EPD_2IN13D_WIDTH;  // 104
}

uint16_t Pantalla_Alto() {
  return EPD_2IN13D_HEIGHT; // 212
}

#endif // EPAPER_2IN13_BW_FLEX
