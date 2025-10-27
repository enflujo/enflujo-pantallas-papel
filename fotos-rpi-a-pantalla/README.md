# Camara a pantalla papel

Captura fotos con la cámara del Raspberry Pi y muéstralas en pantalla e-paper con dithering.

## Hardware

- Raspberry Pi 5 con cámara
- ESP32 con pantalla e-paper Waveshare 2.13" B/W flex (104×212 píxeles)
- Conexión USB serial entre Pi y ESP32 (`/dev/ttyACM0`)

## Instalación

**NOTA**: Este proyecto comparte el ambiente virtual con el repositorio principal.

```bash
# Desde la raíz del repositorio (enflujo-pantallas-papel/)
python3 -m venv .venv
source .venv/bin/activate

# Actualizar pip (recomendado)
pip install --upgrade pip

# Instalar dependencias
pip install -r requirements.txt
```

### Dependencias

- **Pillow** (>=10.4.0) - Procesamiento de imágenes (difuminado, redimensión)
- **pyserial** (>=3.5) - Comunicación serial con ESP32

## Uso

### Capturar y mostrar foto en vivo

```bash
# Activar ambiente (si no está activo)
source ../.venv/bin/activate

# Desde el directorio fotos-rpi-a-pantalla/
python capturar_enviar.py --capturar --difuminado floyd --puerto /dev/ttyACM0 --enviar
```

### Usar imagen existente

```bash
python capturar_enviar.py --imagen foto.jpg --difuminado floyd --puerto /dev/ttyACM0 --enviar
```

### Opciones disponibles

```
--capturar             Capturar nueva foto con la cámara
--imagen RUTA          Usar imagen existente
--puerto /dev/ttyACM0  Puerto serial del ESP32
--enviar               Enviar a la pantalla de tinta electrónica
--difuminado floyd     Método de difuminado:
                         floyd    - Floyd-Steinberg (mejor para fotos)
                         bayer    - Bayer ordenado (patrones geométricos)
                         ninguno  - Umbral simple (alto contraste)
--rotacion 0           Rotación: 0, 90, 180, 270 grados
--espejo               Espejo horizontal
--salida out           Directorio para archivos de salida
--ancho-pantalla 104   Ancho de pantalla (por defecto: 104)
--alto-pantalla 212    Alto de pantalla (por defecto: 212)
```

## Archivos del proyecto

```
cam2paper/
├── capture_and_send.py       # Programa principal
├── dither.py                 # Procesamiento de imagen y difuminado
├── send_serial.py            # Comunicación serial con ESP32
├── requirements.txt          # Dependencias Python
└── out/                      # Salidas (captura.jpg, vista_previa_1bit.png, cuadro_bn.bin)
```

## Ejemplos

```bash
# Solo procesar (sin enviar)
python capture_and_send.py --imagen foto.jpg --difuminado floyd

# Capturar con rotación
python capture_and_send.py --capturar --rotacion 90 --puerto /dev/ttyACM0 --enviar

# Usar difuminado Bayer
python capture_and_send.py --capturar --difuminado bayer --puerto /dev/ttyACM0 --enviar
```

## Firmware ESP32

El firmware del ESP32 debe estar en el entorno `2in13_bw_flex` con la bandera `USE_SERIAL_FRAMES` del proyecto `enflujo-pantallas-papel`:

```bash
cd ../enflujo-pantallas-papel
platformio run -t upload
```

## Protocolo serial

- Baudios: 115200
- Comandos:
  - `C` - Limpiar pantalla (CONFIRMACIÓN: `c`) - **NO recomendado**
  - `S` + 2756 bytes - Mostrar imagen (CONFIRMACIÓN: `s`)
  - `Q` - Modo dormir (CONFIRMACIÓN: `q`)
- Formato de imagen: 104×212 píxeles, 1-bit BN, bit más significativo primero, 0x00=negro, 0xFF=blanco

## Notas importantes

- **NO usar comando `C` (limpiar)** antes de enviar imágenes - causa pantalla en blanco
- Enviar imágenes directamente con comando `S` funciona correctamente
- La librería Waveshare maneja internamente todos los tiempos y refrescos
