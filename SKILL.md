---
name: mowatch-dev
description: Development guide for MoWatch e-ink smartwatch apps. Use when developing watch faces, apps, or processing images for MoWatch. Includes API reference, bitmap format, Docker build process, and image processing for 1-bit e-ink display.
license: MIT
---

# MoWatch App Development Skill

Use this skill when developing watch faces or apps for MoWatch e-ink smartwatch.

## Hardware Specifications

- **Display**: 200x200 pixel monochrome e-ink
- **MCU**: ARM Cortex-M3
- **Colors**: Black and white only (1-bit)
- **Update**: Partial refresh supported, full refresh every 5 minutes recommended

## Bitmap Format

MoWatch uses **horizontal 8-point left-high-bit encoding**:

```c
// For a 120px wide image:
int16_t byteWidth = (120 + 7) / 8;  // = 15 bytes per row

// To check if pixel (x, y) is black:
if (bitmap[y * byteWidth + x / 8] & (128 >> (x & 7))) {
    // pixel is black
}
```

## Project Structure

```
app_name/
├── code/
│   ├── app_main.c          # Main application code
│   ├── header.h            # Included from ../../include/
│   └── *.h                 # Graphics data headers
└── gcc/
    ├── Makefile
    ├── app.ld              # Linker script
    ├── createmwa.py        # Package creator
    └── build.bat           # Windows build script
```

## Required Files from SDK

The Makefile expects these files at `../../`:
- `libs/init_datas.c`
- `libs/tiny-json.c`
- `libs/syscall_gcc.txt`
- `include/header.h`

## Building with Docker

1. **Create Dockerfile** (in SDK root):
```dockerfile
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y \
    gcc-arm-none-eabi \
    make \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /build
```

2. **Build Docker image**:
```bash
docker build -t mowatch-sdk .
```

3. **Compile**:
```bash
docker run --rm -v "/path/to/app_projects:/build" mowatch-sdk make -C /build/app_name/gcc
```

4. **Create .mwa package**:
```bash
cd gcc && python3 createmwa.py
```

## Key API Functions

```c
// Drawing
eink_clear(WHITE);                                    // Clear screen
eink_drawpixel(x, y, BLACK);                         // Draw pixel
eink_drawrect(x1, y1, x2, y2, color, MODE_FILL);    // Filled rectangle
eink_drawrect(x1, y1, x2, y2, color, MODE_EMPTY);   // Rectangle outline
eink_drawstr(x, y, str, font_size, color);          // Draw string

// Time
RTC_getHour(), RTC_getMin(), RTC_getSec()
RTC_getMon(), RTC_getDay(), RTC_getWeek()
RTC_getTimeStamp()

// System
watch_app_battpercent()  // Battery percentage 0-100
watch_app_exit()         // Exit to main menu
set_update_interval(ms)  // Set onUpdate interval

// Update types
NONE_UPDATE, PART_UPDATE, FULL_UPDATE
```

## App Entry Points

```c
void app_init(intptr_t *draw_ptr_t, intptr_t *onkey_ptr_t,
              intptr_t *update_ptr_t, intptr_t* func_arr) {
    __initialize_datas(func_arr);
    *draw_ptr_t = (intptr_t)onDraw;
    *onkey_ptr_t = (intptr_t)onKey;
    *update_ptr_t = (intptr_t)onUpdate;
    // Initialize your variables here
}

void onDraw(void) {
    // Called when screen needs redraw
}

UpdateType onKey(ButtonType key) {
    // Handle KEY_BACK, KEY_CENTER, etc.
    return PART_UPDATE;  // or NONE_UPDATE, FULL_UPDATE
}

UpdateType onUpdate(int delta) {
    // Called periodically based on set_update_interval()
    return NONE_UPDATE;  // Return update type if screen changed
}
```

## Image Processing for E-ink (Python)

### Converting images to 1-bit bitmap:

```python
from PIL import Image, ImageFilter
import numpy as np

def process_for_eink(img_path, output_size=(120, 120)):
    img = Image.open(img_path).convert('RGB')
    img = img.resize(output_size, Image.Resampling.LANCZOS)

    # Convert to grayscale
    gray = img.convert('L')

    # Edge detection for line art effect
    edges = gray.filter(ImageFilter.FIND_EDGES)

    # Threshold to 1-bit
    threshold = 128
    result = edges.point(lambda p: 0 if p > threshold else 255)

    return result

def image_to_c_array(img, var_name):
    """Convert PIL image to C header array"""
    width, height = img.size
    pixels = list(img.getdata())
    byte_width = (width + 7) // 8

    data = []
    for y in range(height):
        for bx in range(byte_width):
            byte = 0
            for bit in range(8):
                x = bx * 8 + bit
                if x < width:
                    idx = y * width + x
                    if pixels[idx] < 128:  # Black pixel
                        byte |= (128 >> bit)
            data.append(byte)

    # Format as C array
    lines = [f"const unsigned char {var_name}[{len(data)}] = {{"]
    for i in range(0, len(data), 15):
        chunk = data[i:i+15]
        lines.append("    " + ", ".join(f"0x{b:02X}" for b in chunk) + ",")
    lines.append("};")

    return "\n".join(lines)
```

### Skin color detection for avatars:

```python
def is_skin_color(r, g, b):
    """Detect skin tones to keep face area clean"""
    return (r > 170 and g > 130 and b > 110 and
            r > b and r > g - 40)

def is_blue_background(r, g, b):
    """Detect blue background for removal"""
    return (b > 180 and r < 80 and g < 180 and b > r + 100)
```

## Installation to Watch

1. Enable USB mode on MoWatch
2. Mount appears as "NO NAME" volume
3. Copy `.mwa` file to `/apps/` folder
4. Safely eject: `diskutil eject "/Volumes/NO NAME"`
5. Select watch face from watch menu

## Tips

- Keep graphics simple - e-ink has slow refresh
- Use partial updates when possible (PART_UPDATE)
- Full refresh (FULL_UPDATE) every 5 minutes prevents ghosting
- Test edge detection threshold carefully for clean 1-bit output
- Avatar/photo processing needs special handling to avoid noise
- Digits 20x32px work well for time display
- Leave margins from screen edges for HUD-style designs
