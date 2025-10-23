#!/usr/bin/env python3
"""
Convierte una imagen (PNG/JPG) en dos arreglos C (black y red) para Waveshare 2.13" e-Paper (B) V3.
- Genera un .h con dos arrays: gImage_<name>_b[] y gImage_<name>_y[]
- Empaqueta bits MSB->LSB por fila (8 px por byte)
- Map de color:
    * ROJO: píxeles "rojizos" según umbral
    * NEGRO: píxeles oscuros (luma < umbral_negro)
    * BLANCO: lo demás

Uso:
    python epaper_convert.py entrada.png salida.h --width 212 --height 104 --rotate 270
Opciones:
    --black-th 140           (0..255) umbral de negro por luminancia
    --red-min  150           (0..255) mínimo canal R para considerar rojo
    --red-gap  60            R - max(G,B) mínimo para considerar rojo
    --dither   off|fs        'fs' aplica Floyd-Steinberg a blanco/negro antes de clasificar
    --invert-black           invierte lógica del buffer negro (1=negro)
    --invert-red             invierte lógica del buffer rojo   (1=rojo)
"""

import sys, argparse
from PIL import Image

def pack_bits_row(bits):
    """bits: list[int 0/1] length = width; MSB first"""
    out = bytearray()
    b = 0
    cnt = 0
    for bit in bits:
        b = (b << 1) | (bit & 1)
        cnt += 1
        if cnt == 8:
            out.append(b & 0xFF)
            b = 0
            cnt = 0
    if cnt != 0:
        b = b << (8 - cnt)
        out.append(b & 0xFF)
    return out

def classify_pixel(rgb, black_th, red_min, red_gap):
    r,g,b = rgb
    # Luma Rec.601
    y = int(0.299*r + 0.587*g + 0.114*b)
    is_red = r >= red_min and (r - max(g,b)) >= red_gap
    is_black = y < black_th
    # prioridad: rojo > negro > blanco
    if is_red:
        return 'R'
    elif is_black:
        return 'B'
    else:
        return 'W'

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("input", help="Imagen PNG/JPG de entrada")
    ap.add_argument("output", help="Archivo header .h de salida")
    ap.add_argument("--width", type=int, required=True, help="Ancho del panel (ej: 212 para V3)")
    ap.add_argument("--height", type=int, required=True, help="Alto del panel (ej: 104 para V3)")
    ap.add_argument("--rotate", type=int, default=270, choices=[0,90,180,270], help="Rotación a aplicar")
    ap.add_argument("--black-th", type=int, default=140)
    ap.add_argument("--red-min", type=int, default=150)
    ap.add_argument("--red-gap", type=int, default=60)
    ap.add_argument("--dither", choices=["off","fs"], default="off")
    ap.add_argument("--invert-black", action="store_true")
    ap.add_argument("--invert-red", action="store_true")
    ap.add_argument("--name", default="custom", help="Nombre base de los arrays")
    args = ap.parse_args()

    im = Image.open(args.input).convert("RGB")

    # Ajuste de tamaño: encajar manteniendo aspecto (letterbox)
    im_aspect = im.width / im.height
    target_aspect = args.width / args.height
    if im_aspect > target_aspect:
        new_w = args.width
        new_h = int(round(args.width / im_aspect))
    else:
        new_h = args.height
        new_w = int(round(args.height * im_aspect))
    im2 = im.resize((new_w, new_h), Image.LANCZOS)
    canvas = Image.new("RGB", (args.width, args.height), (255,255,255))
    ox = (args.width - new_w) // 2
    oy = (args.height - new_h) // 2
    canvas.paste(im2, (ox, oy))

    if args.rotate != 0:
        canvas = canvas.rotate(args.rotate, expand=False, fillcolor=(255,255,255))

    if args.dither == "fs":
        bw = canvas.convert("1", dither=Image.FLOYDSTEINBERG)
    else:
        bw = canvas.convert("L")

    W, H = canvas.size
    black_bits_rows = []
    red_bits_rows = []

    for y in range(H):
        row_black = []
        row_red = []
        for x in range(W):
            r,g,b = canvas.getpixel((x,y))
            if args.dither == "fs":
                y_luma = 0 if bw.getpixel((x,y)) == 0 else 255
                if y_luma == 0:
                    cls = 'B'
                else:
                    cls = classify_pixel((r,g,b), args.black_th, args.red_min, args.red_gap)
            else:
                cls = classify_pixel((r,g,b), args.black_th, args.red_min, args.red_gap)

            # Por defecto: 0 = tinta (negro/rojo), 1 = blanco
            if cls == 'B':
                bit_b = 0; bit_r = 1
            elif cls == 'R':
                bit_b = 1; bit_r = 0
            else:
                bit_b = 1; bit_r = 1

            if args.invert_black: bit_b ^= 1
            if args.invert_red:   bit_r ^= 1

            row_black.append(bit_b)
            row_red.append(bit_r)

        # Empaque MSB primero
        def pack(bits):
            out = bytearray(); b = 0; cnt = 0
            for bit in bits:
                b = (b << 1) | (bit & 1); cnt += 1
                if cnt == 8: out.append(b & 0xFF); b = 0; cnt = 0
            if cnt: out.append((b << (8-cnt)) & 0xFF)
            return out
        black_bits_rows.append(pack(row_black))
        red_bits_rows.append(pack(row_red))

    black_data = b"".join(black_bits_rows)
    red_data   = b"".join(red_bits_rows)

    bytes_per_row = (W + 7)//8
    expected = bytes_per_row * H

    name = args.name
    with open(args.output, "w", encoding="utf-8") as f:
        f.write("// Generado por epaper_convert.py\n")
        f.write("// Tamano: {}x{}, bytes por buffer: {}\n".format(W, H, expected))
        f.write("#pragma once\n\n")
        f.write("#define IMG_{}__WIDTH  {}\n".format(name.upper(), W))
        f.write("#define IMG_{}__HEIGHT {}\n\n".format(name.upper(), H))
        f.write("const unsigned char gImage_{}_b[{}] = {{\n".format(name, expected))
        for i, b in enumerate(black_data):
            if i % 16 == 0: f.write("  ")
            f.write("0x{:02X},".format(b))
            if i % 16 == 15: f.write("\n")
            else: f.write(" ")
        if expected % 16 != 0: f.write("\n")
        f.write("};\n\n")
        f.write("const unsigned char gImage_{}_y[{}] = {{\n".format(name, expected))
        for i, b in enumerate(red_data):
            if i % 16 == 0: f.write("  ")
            f.write("0x{:02X},".format(b))
            if i % 16 == 15: f.write("\n")
            else: f.write(" ")
        if expected % 16 != 0: f.write("\n")
        f.write("};\n")
    print("OK ->", args.output)
    print('Incluir y usar: #include "{}"'.format(args.output))
    print("EPD_2IN13B_V3_Display(gImage_{}_b, gImage_{}_y);".format(name, name))

if __name__ == "__main__":
    main()
