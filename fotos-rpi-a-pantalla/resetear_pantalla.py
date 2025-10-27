#!/usr/bin/env python3
"""
Script de emergencia para resetear pantalla e-paper cuando queda en blanco.

Envía un cuadro completamente blanco para forzar un refresco completo
y resetear el estado interno de la pantalla.
"""
import argparse
import time
import serial

def resetear_pantalla(puerto: str):
    """Envía cuadro blanco para resetear estado de pantalla."""
    print(f"Abriendo serial {puerto}...")
    ser = serial.Serial(puerto, 115200, timeout=2)
    time.sleep(1.0)
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    time.sleep(0.5)
    
    # Crear cuadro completamente blanco: 0xFF = todos los píxeles blancos
    # 104 ancho ÷ 8 = 13 bytes por fila
    # 13 bytes × 212 filas = 2756 bytes
    cuadro_blanco = bytes([0xFF] * 2756)
    
    print(f"Enviando cuadro blanco ({len(cuadro_blanco)} bytes) para resetear...")
    ser.write(b'S')
    ser.flush()
    time.sleep(0.2)
    
    ser.write(cuadro_blanco)
    ser.flush()
    
    # Esperar confirmación
    limite_tiempo = time.time() + 20.0
    while time.time() < limite_tiempo:
        if ser.in_waiting:
            b = ser.read(1)
            if b == b's':
                print("✓ Pantalla reseteada correctamente (cuadro blanco mostrado)")
                ser.close()
                return True
        time.sleep(0.01)
    
    print("⚠ Tiempo de espera agotado, pero el cuadro probablemente se envió")
    ser.close()
    return False

def main():
    parser = argparse.ArgumentParser(description="Resetea pantalla e-paper enviando cuadro blanco")
    parser.add_argument("--puerto", default="/dev/ttyACM0", help="Puerto serial (por defecto: /dev/ttyACM0)")
    args = parser.parse_args()
    
    print("=" * 60)
    print("RESETEO DE PANTALLA E-PAPER")
    print("=" * 60)
    print()
    print("Este script envía un cuadro completamente blanco para")
    print("resetear el estado interno de la pantalla cuando queda en blanco.")
    print()
    
    resetear_pantalla(args.puerto)
    
    print()
    print("Ahora puedes enviar tu imagen normalmente:")
    print("  python capturar_enviar.py --capturar --difuminado floyd --puerto /dev/ttyACM0 --enviar")

if __name__ == "__main__":
    main()
