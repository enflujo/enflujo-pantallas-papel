#ifdef EPAPER_2IN13_BW_FLEX
#include "common/display.h"
#include "utility/EPD_2in13d.h"

// Implementación usando el driver Waveshare 2.13" D (B/W) con soporte parcial
static bool s_firstFrame = true;

void Display_Init() {
  EPD_2IN13D_Init();
}

void Display_Clear() {
  EPD_2IN13D_Clear();
  s_firstFrame = false; // ya hicimos un full refresh
}

void Display_ClearStrong() {
  // Alterna negro y blanco a pantalla completa para limpiar fantasmas
  UWORD widthBytes = (EPD_2IN13D_WIDTH % 8 == 0) ? (EPD_2IN13D_WIDTH / 8) : (EPD_2IN13D_WIDTH / 8 + 1);
  UWORD height = EPD_2IN13D_HEIGHT;
  // Calcular tamaño con entero estándar para evitar problemas de tipos
  unsigned long bufSize = (unsigned long)widthBytes * (unsigned long)height;

  UBYTE* buf = (UBYTE*)malloc(bufSize);
  if (!buf) {
    // Fallback a un solo clear
    EPD_2IN13D_Clear();
    s_firstFrame = false;
    return;
  }

  // Dos ciclos completos (negro->blanco) suelen ser suficientes
  for (int pass = 0; pass < 2; ++pass) {
    memset(buf, 0x00, bufSize); // negro
    EPD_2IN13D_Display(buf);
    memset(buf, 0xFF, bufSize); // blanco
    EPD_2IN13D_Display(buf);
  }

  free(buf);
  s_firstFrame = false;
}

void Display_Present(const UBYTE* Black, const UBYTE* /*Red*/) {
  // Para B/W solo usamos el buffer negro. Primer frame a pantalla completa,
  // siguientes con parcial para animación rápida.
  if (s_firstFrame) {
    EPD_2IN13D_Display((UBYTE*)Black);
    s_firstFrame = false;
  } else {
    EPD_2IN13D_DisplayPart((UBYTE*)Black);
  }
}

void Display_Sleep() {
  EPD_2IN13D_Sleep();
}

uint16_t Display_Width() {
  return EPD_2IN13D_WIDTH;  // 104
}

uint16_t Display_Height() {
  return EPD_2IN13D_HEIGHT; // 212
}

#endif // EPAPER_2IN13_BW_FLEX
