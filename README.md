# Pantallas e-paper

Herramientas y firmware para pantallas Waveshare 2.13" con ESP32.

## Instalación rápida

```bash
# Crear ambiente virtual en la raíz del proyecto
python3 -m venv .venv
source .venv/bin/activate

# Instalar todas las dependencias Python
pip install --upgrade pip
pip install -r requirements.txt

# Opcional: Instalar PlatformIO para compilar firmware
pip install --upgrade platformio
```

## Proyectos

### 1. Firmware ESP32 (PlatformIO)

Firmware para controlar pantallas e-paper Waveshare 2.13" desde un ESP32.

**Requisitos:**
- PlatformIO Core (CLI) o extensión de VS Code

**Instalación de PlatformIO:**

```bash
# Opción 1: Instalar PlatformIO Core (CLI)
pip install --upgrade platformio

# Opción 2: Instalar extensión en VS Code
# Buscar "PlatformIO IDE" en el marketplace de VS Code
```

**Entornos disponibles:**

- `2in13_bw_flex` - Demo de animación (cucaracha) con pantalla B/W flexible
- `2in13_bn_fotos` - Modo serial para recibir imágenes desde Raspberry Pi
- `2in13b_v3` - Pantalla tricolor B/W/R (negro/blanco/rojo)

**Compilar y subir firmware:**

```bash
# Compilar y subir demo
platformio run -e 2in13_bw_flex -t upload

# Compilar y subir modo fotos (serial)
platformio run -e 2in13_bn_fotos -t upload
```

### 2. Captura de fotos (Raspberry Pi → e-paper)

Directorio: `fotos-rpi-a-pantalla/`

Captura fotos con cámara Raspberry Pi y las muestra en la pantalla e-paper vía ESP32.

**Instalación:**

Las dependencias ya están instaladas con el `requirements.txt` principal. Si aún no lo has hecho:

```bash
# Desde la raíz del proyecto
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

**Uso:**

```bash
# Activar ambiente virtual (si no está activo)
source .venv/bin/activate

# Capturar y mostrar foto
cd fotos-rpi-a-pantalla
python capturar_enviar.py --capturar --difuminado floyd --puerto /dev/ttyACM0 --enviar

# Usar imagen existente
python capturar_enviar.py --imagen foto.jpg --difuminado floyd --puerto /dev/ttyACM0 --enviar
```

Ver [fotos-rpi-a-pantalla/README.md](fotos-rpi-a-pantalla/README.md) para más detalles.

### 3. Convertidor de imágenes a headers C

Archivo: `convertidor.py`

Convierte imágenes PNG/JPG a headers C para usar en firmware.

**Uso típico:**

```bash
# Activar ambiente virtual (si no está activo)
source .venv/bin/activate

# Convertir imagen
python convertidor.py dibujo.png salida.h --width 212 --height 104 --rotate 270 --name fotograma
```

En tu programa C:

```cpp
#include "salida.h"
EPD_2IN13B_V3_Display(gImage_fotograma_b, gImage_fotograma_y);
```

**Parámetros disponibles:**

```bash
--black-th 140   # Umbral de luminancia para negro
--red-min 150    # Mínimo R para rojo
--red-gap 60     # R - max(G,B) mínimo para rojo
--dither fs      # Dithering Floyd-Steinberg
--invert-black   # Invierte buffer negro
--invert-red     # Invierte buffer rojo
```

## Protocolo serial (firmware)

- **Baudios**: 115200
- **Comandos**:
  - `C` - Limpiar pantalla (confirmación: `c`) - ⚠️ **NO recomendado, causa pantalla en blanco**
  - `S` + 2756 bytes - Mostrar imagen (confirmación: `s`)
  - `Q` - Modo dormir (confirmación: `q`)
- **Formato de imagen**: 104×212 píxeles, 1-bit B/N, MSB primero, 0x00=negro, 0xFF=blanco

⚠️ **Importante**: NO usar comando `C` (limpiar) antes de enviar imágenes. Enviar imágenes directamente con `S` funciona correctamente.

## Hardware soportado

- **ESP32 Dev Module** (cualquier placa compatible)
- **Waveshare 2.13" e-Paper**:
  - Modelo D (B/W flexible) - 104×212 píxeles
  - Modelo B V3 (B/W/R tricolor) - 104×212 píxeles
- **Raspberry Pi** (cualquier modelo con cámara, probado en RPi 5)

## Estructura del repositorio

```
enflujo-pantallas-papel/
├── src/                           # Código fuente firmware ESP32
│   ├── common/                    # Código compartido
│   │   ├── display.h              # Interfaz unificada de pantalla
│   │   ├── display_2in13_bw_flex.cpp
│   │   ├── display_2in13b_v3.cpp
│   │   └── serial_frames.cpp      # Receptor de frames por serial
│   ├── 2in13_bw_flex/             # Demo animación
│   ├── 2in13_bn_fotos/            # Modo fotos serial
│   └── 2in13b_v3/                 # Firmware tricolor
├── fotos-rpi-a-pantalla/          # Scripts Python para Raspberry Pi
│   ├── capturar_enviar.py         # Programa principal
│   ├── dither.py                  # Procesamiento de imagen
│   ├── enviar_serial.py           # Comunicación serial
│   ├── requirements.txt           # Dependencias Python
│   └── README.md                  # Documentación detallada
├── lib/                           # Bibliotecas (Waveshare EPD)
├── convertidor.py                 # Convertidor imagen → header C
├── platformio.ini                 # Configuración PlatformIO
└── README.md                      # Este archivo
```

## Notas importantes

### Refresco de pantalla

- **Refresco parcial**: Transiciones rápidas sin parpadeo, puede causar ghosting después de muchas actualizaciones
- **Refresco completo**: Limpia ghosting pero causa parpadeo negro/blanco
- **Recomendación**: Usar refresco parcial durante operación normal, hacer refresco completo ocasionalmente
- **Intervalo mínimo**: 180 segundos entre refrescos (recomendado por fabricante)
- **Almacenamiento**: Limpiar pantalla antes de almacenar por tiempo prolongado

### Precauciones de hardware

⚠️ **Cable FPC frágil**:
- NO doblar el cable en dirección vertical a la pantalla
- NO doblar repetidamente
- NO doblar hacia el frente de la pantalla
- Fijar el cable durante desarrollo

⚠️ **Voltaje**: 3.3V (placas V2.1+ soportan 3.3V y 5V)

⚠️ **Cuidado físico**: Evitar caídas, golpes y presión excesiva en la pantalla

⚠️ **Modo sleep**: Enviar pantalla a sleep cuando no esté refrescando para evitar daño por alto voltaje prolongado

## Referencias

- [Waveshare 2.13" e-Paper HAT (D) Manual](https://www.waveshare.com/wiki/2.13inch_e-Paper_HAT_(D)_Manual)
- Documentación adicional en directorio `fotos-rpi-a-pantalla/README.md`

## Licencia

Este proyecto usa bibliotecas de Waveshare. Ver archivos de biblioteca para detalles de licencia.
```

### Communication Method

![4.37inch-e-paper-manual-2.jpg](https://www.waveshare.com/w/upload/d/df/4.37inch-e-paper-manual-2.jpg)

CSB (CS): Slave chip select signal, active at low level. When it is at low level, the chip is enabled.  
SCL (SCK/SCLK): Serial clock signal.  
D/C (DC): Data/command control signal, write command (Command) when the level is low; write data (Data/parameter) when the level is high.  
SDA (DIN): Serial data signal.  
Timing: CPHL=0, CPOL=0, i.e. SPI mode 0.  
Note: For specific information about SPI communication, you can search for information online on your own.  

### Operating Principle

The e-paper used in this product uses "microcapsule electrophoresis display" technology for image display. The basic principle is that charged nanoparticles suspended in a liquid migrate under the action of an electric field. The e-paper display screen displays patterns by reflecting ambient light and does not require a backlight. Under ambient light, the e-paper display screen is clearly visible, with a viewing angle of almost 180°. Therefore, e-paper displays are ideal for reading.

### Program Principle

We define the pixels in a monochrome picture, 0 is black and 1 is white.

White: □, Bit 1

Black: ■, Bit 0

The dot in the figure is called a pixel. As we know, 1 and 0 are used to define the color, therefore we can use one bit to define the color of one pixel, and 1 byte = 8 pixels.

For example, If we set the first 8 pixels to black and the last 8 pixels to white, we show it by codes, they will be 16 bit as below:

![E-paper hardware work 1](https://www.waveshare.com/w/upload/6/67/E-paper_hardware_work_1.png)

For computer, the data is saved in MSB format:

![E-paper hardware work 2](https://www.waveshare.com/w/upload/f/fd/E-paper_hardware_work_2.png)

So we can use two bytes for 16 pixels.

### Precautions

For e-Paper displays that support partial refresh, please note that you cannot refresh them with the partial refresh mode all the time. After refreshing partially several times, you need to fully refresh EPD once. Otherwise, the display effect will be abnormal, which cannot be repaired!

Note that the screen cannot be powered on for a long time. When the screen is not refreshed, please set the screen to sleep mode or power off it. Otherwise, the screen will remain in a high voltage state for a long time, which will damage the e-Paper and cannot be repaired!

When using the e-Paper display, it is recommended that the refresh interval is at least 180s, and refresh at least once every 24 hours. If the e-Paper is not used for a long time, you should use the program to clear the screen before storing it. (Refer to the datasheet for specific storage environment requirements.)

After the screen enters sleep mode, the sent image data will be ignored, and it can be refreshed normally only after initializing again.

Control the 0x3C or 0x50 (refer to the datasheet for details) register to adjust the border color. In the demo, you can adjust the Border Waveform Control register or VCOM AND DATA INTERVAL SETTING to set the border.

If you find that the created image data is displayed incorrectly on the screen, it is recommended to check whether the image size setting is correct, change the width and height settings of the image and try again.

The working voltage of the e-Paper display is 3.3V. If you buy the raw panel, you need to add a level convert circuit for compatibility with 5V voltage. The new version of the driver board (V2.1 and subsequent versions) has been added a level processing circuit, which can support both 3.3V and 5V. The old version only supports a 3.3V working environment. You can confirm the version before using it. (The one with the 20-pin chip on the PCB is generally the new version. And the version number is under the board name.)

The FPC cable of the screen is fragile, Please note: Do not bend the cable along the vertical direction of the screen to avoid tearing the cable; Do not repeatedly excessive bending line to avoid line fracture; Do not bend the cable toward the front of the screen to prevent the cable from being disconnected from the panel. It is recommended to use after fixing the cable during debugging and development.

The screen of e-Paper is relatively fragile, please try to avoid dropping, bumping and pressing hard.

We recommend that customers use the sample program provided by us to test with the corresponding development board.
