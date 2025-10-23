# Pantallas e-paper

Script para convertir PNG/JPG a arreglos C (black/red) para Waveshare 2.13" (B) V3.

Requisitos: Python 3 + Pillow
  pip install pillow

Uso típico (V3: 212x104; si usas rotación 270 como el demo):
  python convertidor.py dibujo.png salida.h --width 212 --height 104 --rotate 270 --name fotograma

En tu sketch:
  #include "salida.h"
  EPD_2IN13B_V3_Display(gImage_logo_b, gImage_logo_y);

Ajustes útiles:
  --black-th 140   Umbral de luminancia para negro (sube/baja si ves gris)
  --red-min 150    Mínimo R para rojo
  --red-gap 60     R - max(G,B) mínimo para rojo
  --dither fs      Dithering Floyd‑Steinberg para simular grises
  --invert-black   Invierte buffer negro si los colores salen invertidos
  --invert-red     Invierte buffer rojo si los colores salen invertidos
