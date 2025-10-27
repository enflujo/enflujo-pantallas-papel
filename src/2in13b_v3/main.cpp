// #include <Arduino.h>
// #include "DEV_Config.h"
// #include "EPD.h"
// #include "GUI_Paint.h"
// #include <stdlib.h>
// #include "cucaracha1.h"
// #include "cucaracha2.h"
// #include "cucaracha3.h"
// #include "cucaracha4.h"
// #include "cucaracha5.h"

// static UBYTE *bufNegro = nullptr;
// static UBYTE *bufRojo  = nullptr;
// static UWORD imageSize = 0;

// void dibujarMireCalidad() {
//   // --- NEGRO ---
//   Paint_SelectImage(bufNegro);
//   Paint_Clear(WHITE);

//   const int W = EPD_2IN13B_V3_WIDTH;
//   const int H = EPD_2IN13B_V3_HEIGHT;

//   // Marco exterior
//   Paint_DrawRectangle(0, 0, W-1, H-1, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

//   // Barras verticales 1 px (nitidez horizontal)
//   for (int x = 4; x < 4+40; x += 2) {
//     Paint_DrawLine(x, 8, x, 8+40, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//   }
//   // Barras horizontales 1 px (nitidez vertical)
//   for (int y = 8; y < 8+40; y += 2) {
//     Paint_DrawLine(50, y, 50+40, y, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//   }

//   // Diagonales finas
//   Paint_DrawLine(100, 8, 140, 48, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//   Paint_DrawLine(100, 48, 140, 8, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

//   // Cuadriculado fino (checker) 1 px
//   for (int y = 56; y < 56+32; ++y) {
//     for (int x = 4; x < 4+32; ++x) {
//       if (((x + y) & 1) == 0) {
//         Paint_DrawPoint(x, y, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
//       }
//     }
//   }

//   // Cajas de gris por tramado (dithering manual)
//   // Bloques de 16x16 con distintas densidades
//   auto bloque = [&](int ox, int oy, int dens) {
//     for (int y = oy; y < oy+16; ++y) {
//       for (int x = ox; x < ox+16; ++x) {
//         if (((x + y) % 100) < dens) {
//           Paint_DrawPoint(x, y, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
//         }
//       }
//     }
//     Paint_DrawRectangle(ox, oy, ox+16, oy+16, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
//   };
//   bloque(50, 56, 10);  // ~10%
//   bloque(70, 56, 25);  // ~25%
//   bloque(90, 56, 40);  // ~40%
//   bloque(110,56, 60);  // ~60%
//   bloque(130,56, 80);  // ~80%

//   // Texto de distintos tamaños (mira de legibilidad)
//   Paint_DrawString_EN(4, 94, "Font12: abcXYZ 0123", &Font12, WHITE, BLACK);
//   Paint_DrawString_EN(4, 108,"Font16: Nitidez", &Font16, WHITE, BLACK);
//   Paint_DrawString_EN(4, 126,"Font20: Contraste", &Font20, WHITE, BLACK);

//   // --- ROJO (acento) ---
//   Paint_SelectImage(bufRojo);
//   Paint_Clear(WHITE);

//   // Bloque rojo para ver saturacion/registro
//   Paint_DrawRectangle(W-60, H-28, W-6, H-6, RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
//   Paint_DrawString_EN(W-58, H-24, "ROJO", &Font12, WHITE, RED);

//   // Cruces de registro (centro y esquinas)
//   auto cruz = [&](int cx, int cy){
//     Paint_DrawLine(cx-5, cy, cx+5, cy, RED, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//     Paint_DrawLine(cx, cy-5, cx, cy+5, RED, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//   };
//   cruz(W/2, H/2);
//   cruz(6, 6);
//   cruz(W-7, 6);
//   cruz(6, H-7);
//   cruz(W-7, H-7);
// }

// void setup()
// {
//   Serial.begin(115200);
//   DEV_Module_Init();

//   EPD_2IN13B_V3_Init();
//   // Evitamos clear completo para ahorrar tiempo (aceptamos residuos iniciales)
//   // EPD_2IN13B_V3_Clear();
//   // DEV_Delay_ms(200);
//   EPD_2IN13B_V3_SetVCOMInterval(0xB7);

//   // Cargamos una vez el plano rojo (si es estático) con el primer fotograma
//   EPD_2IN13B_V3_Display(gImage_cucaracha1_b, gImage_cucaracha1_y);
//   // A partir de aquí, sólo actualizaremos el negro para máxima velocidad
// }
// // Animación de 5 fotogramas, priorizando velocidad (sin clear entre frames)
// void loop() {
//   static const UBYTE* negros[] = {
//     gImage_cucaracha1_b,
//     gImage_cucaracha2_b,
//     gImage_cucaracha3_b,
//     gImage_cucaracha4_b,
//     gImage_cucaracha5_b
//   };
//   static const UBYTE* rojos[] = {
//     gImage_cucaracha1_y,
//     gImage_cucaracha2_y,
//     gImage_cucaracha3_y,
//     gImage_cucaracha4_y,
//     gImage_cucaracha5_y
//   };

//   for (int i = 0; i < 5; ++i) {
//     // Camino rápido: si el rojo no cambia, sólo actualizamos negro
//     EPD_2IN13B_V3_DisplayBlack(negros[i]);
//     // No EPD_2IN13B_V3_Clear();  // evitamos para máxima velocidad (aceptamos ghosting)
//     // Pequeña pausa opcional para no saturar el bus; el busy del panel ya bloquea
//     // DEV_Delay_ms(10);
//   }
// }


#include <Arduino.h>
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "common/display.h"
#include <stdlib.h>

static UBYTE *ImagenNegra = nullptr;
static UBYTE *ImagenRoja  = nullptr;

static inline void esperar_ms(uint32_t ms) {
  uint32_t t0 = millis();
  while (millis() - t0 < ms) { delay(10); }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n=== 2.13B V3 – prueba A/B/C (sin dormir intermedio) ===");

  if (DEV_Module_Init() != 0) {
    Serial.println("Inicialización DEV falló"); return;
  }

  Pantalla_Iniciar();                    // inicialización del envoltorio
  Serial.println("Inicialización de pantalla OK");

  // Limpia el panel antes del primer cuadro (importantísimo)
  Serial.println("Limpiando...");
  Pantalla_Limpiar();
  esperar_ms(1000);

  UWORD ancho = Pantalla_Ancho();
  UWORD alto = Pantalla_Alto();
  UDOUBLE tamano = ((ancho % 8 == 0) ? (ancho / 8) : (ancho / 8 + 1)) * alto;

  ImagenNegra = (UBYTE*)malloc(tamano);
  ImagenRoja  = (UBYTE*)malloc(tamano);
  if (!ImagenNegra || !ImagenRoja) { Serial.println("Sin RAM"); return; }

  // ====== Cuadro A: TODO BLANCO ======
  Serial.println("[A] BLANCO");
  memset(ImagenNegra, 0xFF, tamano);  // 1 = blanco en esta librería
  memset(ImagenRoja,  0xFF, tamano);

  Paint_NewImage(ImagenNegra, ancho, alto, 0, WHITE);
  Paint_SelectImage(ImagenNegra);
  Paint_Clear(WHITE);

  Paint_NewImage(ImagenRoja, ancho, alto, 0, WHITE);
  Paint_SelectImage(ImagenRoja);
  Paint_Clear(WHITE);

  Pantalla_Presentar(ImagenNegra, ImagenRoja);
  esperar_ms(1500);

  // ====== Cuadro B: NEGRO a la izquierda, SIN rojo ======
  Serial.println("[B] NEGRO izquierda");
  memset(ImagenNegra, 0xFF, tamano);
  memset(ImagenRoja,  0xFF, tamano);

  Paint_SelectImage(ImagenNegra);
  Paint_Clear(WHITE);
  Paint_DrawRectangle(0, 0, ancho/2, alto, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

  Paint_SelectImage(ImagenRoja);
  Paint_Clear(WHITE); // sin rojo

  Pantalla_Presentar(ImagenNegra, ImagenRoja);
  esperar_ms(1500);

  // ====== Cuadro C: ROJO círculo a la derecha, SIN negro ======
  Serial.println("[C] ROJO derecha");
  memset(ImagenNegra, 0xFF, tamano);
  memset(ImagenRoja,  0xFF, tamano);

  Paint_SelectImage(ImagenRoja);
  Paint_Clear(WHITE);
  int cx = (ancho*3)/4, cy = alto/2;
  int r  = min(ancho, alto)/4;
  // En búfer rojo: BLACK = píxel rojo, WHITE = blanco
  Paint_DrawCircle(cx, cy, r, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

  Paint_SelectImage(ImagenNegra);
  Paint_Clear(WHITE); // sin negro

  Pantalla_Presentar(ImagenNegra, ImagenRoja);
  esperar_ms(1500);

  // Dormimos solo al final
  Serial.println("Durmiendo");
  Pantalla_Dormir();
}

void loop() {}
