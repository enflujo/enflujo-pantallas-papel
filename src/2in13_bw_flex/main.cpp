#include <Arduino.h>
#include "common/display.h"
#include "cucaracha1.h"
#include "cucaracha2.h"
#include "cucaracha3.h"
#include "cucaracha4.h"
#include "cucaracha5.h"

// Declaración explícita por si el indexador no ve la cabecera aún
void Display_ClearStrong();

// ===== MODO DEMOSTRACIÓN =====
// ADVERTENCIA: Partial refresh continuo puede dañar la pantalla.
// Este modo incluye protecciones:
// - Full refresh cada FULL_REFRESH_CYCLES ciclos
// - Límite de MAX_DEMO_CYCLES iteraciones totales
// - Sleep automático al finalizar
// Para uso en producción, incrementar FRAME_DELAY_MS a 180000+ (3+ min)

#define FRAME_DELAY_MS 100        // Delay entre frames
#define FULL_REFRESH_CYCLES 2     // Full refresh cada 2 ciclos
#define MAX_DEMO_CYCLES 4        // Límite: 30 ciclos de animación (150 frames)

static inline void wait_ms(uint32_t ms) {
  uint32_t t0 = millis();
  while (millis() - t0 < ms) { delay(1); }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n=== 2.13 BW FLEX – cucaracha 5 frames (DEMO MODE) ===");
  Serial.printf("Frame delay: %dms | Full refresh every: %d cycles | Max cycles: %d\n", 
                FRAME_DELAY_MS, FULL_REFRESH_CYCLES, MAX_DEMO_CYCLES);

  if (DEV_Module_Init() != 0) {
    Serial.println("DEV init failed");
    while (true) delay(1000);
  }
  Display_Init();
  Display_Clear(); // full clear first to minimize ghosting
  Serial.println("Display initialized. Starting animation...");
}

void loop() {
  static const UBYTE* negros[] = {
    gImage_cucaracha1_b,
    gImage_cucaracha2_b,
    gImage_cucaracha3_b,
    gImage_cucaracha4_b,
    gImage_cucaracha5_b
  };

  static int cycleCount = 0;
  
  // Protección: limitar el número total de ciclos
  if (cycleCount >= MAX_DEMO_CYCLES) {
    Serial.println("\n=== DEMO COMPLETE ===");
    Serial.println("Performing strong clear (black/white cycles) before sleep...");
    Display_ClearStrong(); // Limpieza fuerte para eliminar fantasmas
    wait_ms(2000);   // Esperar a que termine el último refresh
    Serial.println("Display strongly cleared. Entering sleep mode to protect display.");
    Display_Sleep();
    Serial.println("Display in sleep mode. Reset to restart demo.");
    while (true) delay(10000); // Detener loop
  }

  // Full refresh periódico para evitar ghosting acumulado
  if (cycleCount > 0 && cycleCount % FULL_REFRESH_CYCLES == 0) {
    Serial.printf("\n[Cycle %d] Performing full refresh to clear ghosting...\n", cycleCount);
    Display_Clear();
    wait_ms(1000); // Pausa extra después de full refresh
  }

  Serial.printf("[Cycle %d/%d] ", cycleCount + 1, MAX_DEMO_CYCLES);
  for (int i = 0; i < 5; ++i) {
    Serial.printf("%d ", i + 1);
    Display_Present(negros[i], nullptr);
    wait_ms(FRAME_DELAY_MS);
  }
  Serial.println();
  
  cycleCount++;
}
