#include <Arduino.h>
#include "common/display.h"
#include "common/serial_frames.h"
#include "utility/EPD_2in13d.h"

// Modo "fotos": receptor serial exclusivo para cuadros 1-bit BN
// Notas:
// - Limpieza inicial en setup para estado conocido
// - 'C' está deshabilitado durante operación para evitar blancos
// - Enviar con 'S'+2756 bytes; el ACK es 's'

void setup() {
  Serial.begin(115200);
  delay(200);
  if (DEV_Module_Init() != 0) {
    Serial.println("Inicialización DEV falló");
    while (true) delay(1000);
  }
  Pantalla_Iniciar();
  Serial.println("\n=== 2.13 BN FLEX – MODO FOTOS (SERIAL EXCLUSIVO) ===");
  Serial.println("Limpiando pantalla (refresco inicial)...");
  EPD_2IN13D_Clear();  // Refresco completo inicial para estado conocido
  Serial.println("Pantalla lista.");
  Serial.println("Protocolo: 'S'+<cuadro> actualizar, 'Q' dormir, 'C' ignorado");
}

void loop() {
  CuadrosSerial_IntentarRecibirYPresentar();
  delay(1);
}
