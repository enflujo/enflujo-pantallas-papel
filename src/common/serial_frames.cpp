#include "serial_frames.h"
#include <Arduino.h>
#include "common/display.h"
#ifdef EPAPER_2IN13_BW_FLEX
#include "utility/EPD_2in13d.h"
#endif

static inline uint32_t bytesAncho() {
  uint16_t ancho = Pantalla_Ancho();
  return (ancho % 8 == 0) ? (ancho / 8) : (ancho / 8 + 1);
}

static inline uint32_t tamanoCuadro() {
  return bytesAncho() * (uint32_t)Pantalla_Alto();
}

static uint8_t* s_bufer = nullptr;
static uint32_t s_tamano = 0;

static void asegurarBufer() {
  uint32_t necesario = tamanoCuadro();
  if (s_bufer && s_tamano == necesario) return;
  if (s_bufer) { free(s_bufer); s_bufer = nullptr; }
  s_bufer = (uint8_t*)malloc(necesario);
  s_tamano = necesario;
}

// Lectura bloqueante con tiempo límite
static bool leerBytes(uint8_t* destino, uint32_t n, uint32_t tiempoLimiteMs) {
  uint32_t inicio = millis();
  uint32_t obtenidos = 0;
  while (obtenidos < n) {
    if (Serial.available()) {
      int c = Serial.read();
      if (c >= 0) destino[obtenidos++] = (uint8_t)c;
    } else {
      if (millis() - inicio > tiempoLimiteMs) return false;
      delay(1);
    }
  }
  return true;
}

void CuadrosSerial_IntentarRecibirYPresentar() {
  if (!Serial.available()) return;
  char comando = (char)Serial.peek();
  
  // Solo acepta comandos válidos
  if (!(comando == 'S' || comando == 'C' || comando == 'Q')) {
    Serial.read(); // descarta
    return;
  }
  
  Serial.read(); // consume byte de comando
  Serial.printf("[RX] comando=%c\n", comando);

  switch (comando) {
    case 'C':
      // IMPORTANTE: Deshabilitado para evitar estado de pantalla en blanco.
      // Algunos paneles quedan en mal estado tras limpiar y el siguiente 'S' no muestra imagen.
      // Respondemos OK para no romper flujos existentes, pero no ejecutamos limpiar.
      Serial.println("[C] Ignorado (limpieza deshabilitada para proteger la pantalla)");
      Serial.write('c'); // confirmación sin limpiar
      break;
      
    case 'Q':
      Serial.println("[Q] Durmiendo...");
      Pantalla_Dormir();
      Serial.println("[Q] Hecho");
      Serial.write('q'); // confirmación
      break;
      
    case 'S': {
      asegurarBufer();
      if (!s_bufer) { 
        Serial.println("[S] ERROR: malloc falló");
        Serial.write('E'); 
        break; 
      }
      uint32_t n = s_tamano;
      Serial.printf("[S] Esperando %u bytes...\n", n);
      if (!leerBytes(s_bufer, n, 10000)) {
        Serial.println("[S] TIEMPO AGOTADO");
        Serial.write('T');
        break;
      }
      Serial.printf("[S] Recibidos %u bytes\n", n);
      // Presentación mediante la interfaz común: respeta el modo de refresco del panel
      Pantalla_Presentar(s_bufer, nullptr);
      Serial.println("[S] Hecho");
      Serial.write('s'); // confirmación
      break;
    }
  }
  Serial.flush();
}
