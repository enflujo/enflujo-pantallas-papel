#ifdef EPAPER_2IN13B_V3
#include "common/display.h"
#include "EPD.h"

// Implementación específica para Waveshare 2.13" b V3 (tricolor)

void Display_Init() {
  EPD_2IN13B_V3_Init();
}

void Display_Clear() {
  EPD_2IN13B_V3_Clear();
}

void Display_Present(const UBYTE* Black, const UBYTE* Red) {
  // Para este panel tricolor se requieren ambos planos; si Red es nullptr,
  // simplemente enviamos blanco en rojo.
  static UBYTE empty = 0xFF; // blanco
  const UBYTE* ry = Red ? Red : &empty;
  EPD_2IN13B_V3_Display(Black, ry);
}

void Display_Sleep() {
  EPD_2IN13B_V3_Sleep();
}

uint16_t Display_Width() {
  return EPD_2IN13B_V3_WIDTH; // 104
}

uint16_t Display_Height() {
  return EPD_2IN13B_V3_HEIGHT; // 212
}

#endif // EPAPER_2IN13B_V3
