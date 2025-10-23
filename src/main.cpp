#include <Arduino.h>
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <stdlib.h>
#include "hola2.h"

static UBYTE *bufNegro = nullptr;
static UBYTE *bufRojo  = nullptr;
static UWORD imageSize = 0;

void dibujarMireCalidad() {
  // --- NEGRO ---
  Paint_SelectImage(bufNegro);
  Paint_Clear(WHITE);

  const int W = EPD_2IN13B_V3_WIDTH;
  const int H = EPD_2IN13B_V3_HEIGHT;

  // Marco exterior
  Paint_DrawRectangle(0, 0, W-1, H-1, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

  // Barras verticales 1 px (nitidez horizontal)
  for (int x = 4; x < 4+40; x += 2) {
    Paint_DrawLine(x, 8, x, 8+40, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  }
  // Barras horizontales 1 px (nitidez vertical)
  for (int y = 8; y < 8+40; y += 2) {
    Paint_DrawLine(50, y, 50+40, y, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  }

  // Diagonales finas
  Paint_DrawLine(100, 8, 140, 48, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  Paint_DrawLine(100, 48, 140, 8, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

  // Cuadriculado fino (checker) 1 px
  for (int y = 56; y < 56+32; ++y) {
    for (int x = 4; x < 4+32; ++x) {
      if (((x + y) & 1) == 0) {
        Paint_DrawPoint(x, y, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      }
    }
  }

  // Cajas de gris por tramado (dithering manual)
  // Bloques de 16x16 con distintas densidades
  auto bloque = [&](int ox, int oy, int dens) {
    for (int y = oy; y < oy+16; ++y) {
      for (int x = ox; x < ox+16; ++x) {
        if (((x + y) % 100) < dens) {
          Paint_DrawPoint(x, y, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
        }
      }
    }
    Paint_DrawRectangle(ox, oy, ox+16, oy+16, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  };
  bloque(50, 56, 10);  // ~10%
  bloque(70, 56, 25);  // ~25%
  bloque(90, 56, 40);  // ~40%
  bloque(110,56, 60);  // ~60%
  bloque(130,56, 80);  // ~80%

  // Texto de distintos tamaños (mira de legibilidad)
  Paint_DrawString_EN(4, 94, "Font12: abcXYZ 0123", &Font12, WHITE, BLACK);
  Paint_DrawString_EN(4, 108,"Font16: Nitidez", &Font16, WHITE, BLACK);
  Paint_DrawString_EN(4, 126,"Font20: Contraste", &Font20, WHITE, BLACK);

  // --- ROJO (acento) ---
  Paint_SelectImage(bufRojo);
  Paint_Clear(WHITE);

  // Bloque rojo para ver saturacion/registro
  Paint_DrawRectangle(W-60, H-28, W-6, H-6, RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  Paint_DrawString_EN(W-58, H-24, "ROJO", &Font12, WHITE, RED);

  // Cruces de registro (centro y esquinas)
  auto cruz = [&](int cx, int cy){
    Paint_DrawLine(cx-5, cy, cx+5, cy, RED, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(cx, cy-5, cx, cy+5, RED, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  };
  cruz(W/2, H/2);
  cruz(6, 6);
  cruz(W-7, 6);
  cruz(6, H-7);
  cruz(W-7, H-7);
}

void setup()
{
  Serial.begin(115200);
  DEV_Module_Init();

  EPD_2IN13B_V3_Init();
  EPD_2IN13B_V3_Clear();
  DEV_Delay_ms(200);

  // Mostrar la imagen que convertiste
  EPD_2IN13B_V3_Display(gImage_fotograma_b, gImage_fotograma_y);

  // EPD_2IN13B_V3_Sleep();  // si quieres dejar fijo
}

void loop() {}
