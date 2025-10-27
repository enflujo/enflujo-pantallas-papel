#ifdef EPAPER_2IN13B_V3
#include "common/display.h"
#include "EPD.h"

// Implementación específica para Waveshare 2.13" b V3 (tricolor)

void Pantalla_Iniciar() {
  EPD_2IN13B_V3_Init();
}

void Pantalla_Limpiar() {
  EPD_2IN13B_V3_Clear();
}

void Pantalla_Presentar(const UBYTE* Negro, const UBYTE* Rojo) {
  // Para este panel tricolor se requieren ambos planos; si Rojo es nullptr,
  // simplemente enviamos blanco en rojo.
  static UBYTE vacio = 0xFF; // blanco
  const UBYTE* rj = Rojo ? Rojo : &vacio;
  EPD_2IN13B_V3_Display(Negro, rj);
}

void Pantalla_Dormir() {
  EPD_2IN13B_V3_Sleep();
}

uint16_t Pantalla_Ancho() {
  return EPD_2IN13B_V3_WIDTH; // 104
}

uint16_t Pantalla_Alto() {
  return EPD_2IN13B_V3_HEIGHT; // 212
}

#endif // EPAPER_2IN13B_V3
