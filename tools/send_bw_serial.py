#!/usr/bin/env python3
import argparse
import os
import sys
import time
from typing import Tuple

try:
    import serial  # pyserial
except ImportError:
    serial = None

try:
    from PIL import Image
except ImportError:
    Image = None

# Panel defaults for Waveshare 2.13" D (B/W flexible)
DEFAULT_WIDTH = 104
DEFAULT_HEIGHT = 212

ACKS = {
    'C': b'c',
    'K': b'k',
    'S': b's',
    'Q': b'q',
}


def center_fit(im: Image.Image, target: Tuple[int, int]) -> Image.Image:
    tw, th = target
    iw, ih = im.size
    # scale to fill, then center-crop
    scale = max(tw / iw, th / ih)
    nw, nh = int(round(iw * scale)), int(round(ih * scale))
    im2 = im.resize((nw, nh), Image.LANCZOS)
    ox = max(0, (nw - tw) // 2)
    oy = max(0, (nh - th) // 2)
    return im2.crop((ox, oy, ox + tw, oy + th))


def pack_bw_msb_first(img1: Image.Image) -> bytes:
    # img1 must be mode '1' (1-bit). 0=black, 255=white
    w, h = img1.size
    row_bytes = (w + 7) // 8
    out = bytearray(row_bytes * h)
    px = img1.load()
    idx = 0
    for y in range(h):
        b = 0
        cnt = 0
        for x in range(w):
            val = px[x, y]
            bit = 1 if val != 0 else 0  # 1=white, 0=black (ink)
            b = (b << 1) | bit
            cnt += 1
            if cnt == 8:
                out[idx] = b & 0xFF
                idx += 1
                b = 0
                cnt = 0
        if cnt:
            out[idx] = (b << (8 - cnt)) & 0xFF
            idx += 1
    return bytes(out)


def send_cmd(ser: 'serial.Serial', cmd: bytes, expect: bytes, payload: bytes = b'', timeout: float = 5.0):
    ser.reset_input_buffer()
    ser.write(cmd + payload)
    ser.flush()
    t0 = time.time()
    while time.time() - t0 < timeout:
        if ser.in_waiting:
            rx = ser.read(1)
            if rx == expect:
                return True
        time.sleep(0.001)
    return False


def main():
    p = argparse.ArgumentParser(description="Send a 1bpp image to ESP32 e-paper serial firmware")
    p.add_argument('--port', required=True, help='Serial port, e.g., /dev/ttyACM0')
    p.add_argument('--baud', type=int, default=115200)
    p.add_argument('--image', help='Input image file to display')
    p.add_argument('--rotate', type=int, default=0, choices=[0, 90, 180, 270])
    p.add_argument('--width', type=int, default=DEFAULT_WIDTH)
    p.add_argument('--height', type=int, default=DEFAULT_HEIGHT)
    p.add_argument('--dither', choices=['none', 'floyd'], default='floyd')
    p.add_argument('--strong-clear', action='store_true', help='Perform strong clear before sending frame')
    p.add_argument('--repeat', type=int, default=None, help='Repeat frame send N times')
    p.add_argument('--sleep', action='store_true', help='Put display to sleep at the end')
    p.add_argument('--preview', action='store_true', help='Save preview images (processed)')
    args = p.parse_args()

    if serial is None:
        print('pyserial not installed. Please: pip install pyserial', file=sys.stderr)
        sys.exit(2)
    if Image is None:
        print('Pillow not installed. Please: pip install pillow', file=sys.stderr)
        sys.exit(2)

    if not args.image:
        print('--image is required (capture not implemented in this script)', file=sys.stderr)
        sys.exit(2)

    # Load and preprocess
    im = Image.open(args.image).convert('RGB')
    im = center_fit(im, (args.width, args.height))
    if args.rotate:
        im = im.rotate(args.rotate, expand=False, fillcolor=(255, 255, 255))

    if args.dither == 'floyd':
        im1 = im.convert('1', dither=Image.FLOYDSTEINBERG)
    else:
        im1 = im.convert('1', dither=Image.NONE)

    if args.preview:
        prev_path = os.path.splitext(args.image)[0] + f"_preview_{args.width}x{args.height}_{args.dither}.png"
        im1.save(prev_path)
        print(f"Saved preview: {prev_path}")

    data = pack_bw_msb_first(im1)
    expected = ((args.width + 7)//8) * args.height
    if len(data) != expected:
        print(f"Packed size mismatch: got {len(data)} vs expected {expected}", file=sys.stderr)
        sys.exit(1)

    repeats = args.repeat
    if repeats is None:
        repeats = 2 if args.strong_clear else 1

    # Open serial
    with serial.Serial(args.port, args.baud, timeout=0.1) as ser:
        time.sleep(0.3)
        ser.reset_input_buffer()
        ser.reset_output_buffer()

        if args.strong_clear:
            print('Sending strong clear (K)...', end='', flush=True)
            ok = send_cmd(ser, b'K', ACKS['K'])
            print(' ok' if ok else ' FAILED')
            time.sleep(0.2)

        for i in range(repeats):
            print(f'Sending frame {i+1}/{repeats} ({len(data)} bytes)...', end='', flush=True)
            ok = send_cmd(ser, b'S', ACKS['S'], payload=data)
            print(' ok' if ok else ' FAILED')
            time.sleep(0.2)

        if args.sleep:
            print('Sending sleep (Q)...', end='', flush=True)
            ok = send_cmd(ser, b'Q', ACKS['Q'], timeout=2.0)
            if ok:
                print(' ok')
            else:
                # Some firmwares stop responding immediately after sleep; lack of ack is acceptable
                print(' no-ack (likely already asleep)')

    print('Done.')


if __name__ == '__main__':
    main()
