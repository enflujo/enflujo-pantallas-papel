#include <Arduino.h>
#include "common/display.h"
#include "utility/EPD_2in13d.h"
#include "cucaracha1.h"
#include "cucaracha2.h"
#include "cucaracha3.h"
#include "cucaracha4.h"
#include "cucaracha5.h"


// ===== MODO DEMOSTRACIÓN =====
// ADVERTENCIA: Refresco parcial continuo puede dañar la pantalla.
// Este modo incluye protecciones:
// - Refresco completo cada CICLOS_REFRESCO_COMPLETO ciclos
// - Límite de CICLOS_MAXIMOS_DEMO iteraciones totales
// - Dormir automático al finalizar
// Para uso en producción, incrementar PAUSA_CUADRO_MS a 180000+ (3+ min)

#define PAUSA_CUADRO_MS 100           // Pausa entre cuadros
#define CICLOS_REFRESCO_COMPLETO 2    // Refresco completo cada 2 ciclos
#define CICLOS_MAXIMOS_DEMO 4         // Límite: 4 ciclos de animación

static inline void esperar_ms(uint32_t ms) {
  uint32_t t0 = millis();
  while (millis() - t0 < ms) { delay(1); }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  if (DEV_Module_Init() != 0) {
    Serial.println("Inicialización DEV falló");
    while (true) delay(1000);
  }
  Pantalla_Iniciar();
  Pantalla_Limpiar();
}

void loop() {
  static const UBYTE* negros[] = {
    gImage_cucaracha1_b,
    gImage_cucaracha2_b,
    gImage_cucaracha3_b,
    gImage_cucaracha4_b,
    gImage_cucaracha5_b
  };

  static int contadorCiclos = 0;
  
  if (contadorCiclos >= CICLOS_MAXIMOS_DEMO) {
    Serial.println("\n=== DEMO COMPLETA ===");
    Serial.println("Limpiando pantalla antes de dormir (refresco completo)...");
    // Usar refresco completo directo para fijar el blanco antes de dormir
    EPD_2IN13D_Clear();
    esperar_ms(2000);
    Serial.println("Entrando en modo dormir para proteger pantalla.");
    Pantalla_Dormir();
    Serial.println("Pantalla en modo dormir. Reiniciar para recomenzar demo.");
    while (true) delay(10000);
  }

  // Sin refrescos completos periódicos - solo refresco parcial para evitar parpadeos

  Serial.printf("[Ciclo %d/%d] ", contadorCiclos + 1, CICLOS_MAXIMOS_DEMO);
  for (int i = 0; i < 5; ++i) {
    Serial.printf("%d ", i + 1);
    Pantalla_Presentar(negros[i], nullptr);
    esperar_ms(PAUSA_CUADRO_MS);
  }
  Serial.println();
  contadorCiclos++;
}
