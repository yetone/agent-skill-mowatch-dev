---
name: mowatch-dev
description: Development guide for MoWatch e-ink smartwatch apps. Use when developing watch faces, apps, or processing images for MoWatch. Includes complete API reference, bitmap format, Docker build process, and image processing for 1-bit e-ink display.
license: MIT
---

# MoWatch App Development Skill

Use this skill when developing watch faces or apps for MoWatch e-ink smartwatch.

## Hardware Specifications

- **Display**: 200x200 pixel monochrome e-ink
- **MCU**: ARM Cortex-M3
- **Colors**: Black and white only (1-bit)
- **Update**: Partial refresh supported, full refresh every 5 minutes recommended

## Memory Constraints (IMPORTANT!)

```c
FLASH (rx)  : ORIGIN = 0x10050000, LENGTH = 40K   // Code + constants
RAM (xrw)   : ORIGIN = 0x11001000, LENGTH = 4K    // Variables only!
```

**Only 4K RAM available!** Keep global variables minimal. Use `const` for bitmap data.

## Constants

```c
#define SCREEN_WIDTH   200
#define SCREEN_HEIGHT  200

#define BLACK   0
#define WHITE   0xFF
#define GREY    0x80

#define MODE_EMPTY  0    // Rectangle outline
#define MODE_FILL   1    // Filled rectangle

// File access flags
#define FA_READ           0x01
#define FA_WRITE          0x02
#define FA_OPEN_EXISTING  0x00
#define FA_CREATE_NEW     0x04
#define FA_CREATE_ALWAYS  0x08
#define FA_OPEN_ALWAYS    0x10
#define FA_OPEN_APPEND    0x30
```

## Enums and Data Structures

### Update Types
```c
typedef enum UPDAT_TYPE {
    FULL_UPDATE,    // Full screen refresh (prevents ghosting)
    PART_UPDATE,    // Partial refresh (faster but may ghost)
    NONE_UPDATE,    // No screen update
} UpdateType;
```

### Button Types
```c
typedef enum MBUTTON_TYPE {
    KEY_NULL,
    KEY_CENTER,       // Center button press
    KEY_UP,           // Up button press
    KEY_DOWN,         // Down button press
    KEY_CENTER_UP,    // Center then Up
    KEY_UP_CENTER,    // Up then Center
    KEY_CENTER_DOWN,  // Center then Down
    KEY_DOWN_CENTER,  // Down then Center
    KEY_UP_DOWN,      // Up then Down
    KEY_DOWN_UP,      // Down then Up
    KEY_BACK,         // Back/exit
} ButtonType;
```

### Rotation
```c
typedef enum ROTATE {
    ROTATE_0,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270,
} Rotate;
```

### Bluetooth State
```c
typedef enum BLUESTTE {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
} BlueState;
```

### Weather Data
```c
typedef enum WEATHERDAY {
    TOTDAY,         // Today
    TOMORROW,
    AFTERTOMOROW,   // Day after tomorrow
    DAY_COUNT,
} WeatherDay;

typedef struct TODAYDATA {
    uint16_t moonicon;      // Moon phase icon index
    uint16_t pressure;      // Air pressure
    uint16_t humidity;      // Humidity percentage
    uint8_t sunraise[6];    // Sunrise time string "HH:MM"
    uint8_t sunset[6];      // Sunset time string "HH:MM"
    uint8_t lunar[16];      // Lunar date string
} TodayData;

typedef struct WEATHER {
    uint8_t date;           // Day of month
    char day_temp;          // Daytime temperature
    char night_temp;        // Nighttime temperature
    uint16_t day_icon;      // Daytime weather icon
    uint16_t night_icon;    // Nighttime weather icon
} Weather;
```

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
app_projects/
├── include/
│   └── header.h              # SDK header (DO NOT MODIFY)
├── libs/
│   ├── init_datas.c          # SDK init (DO NOT MODIFY)
│   ├── tiny-json.c           # JSON library
│   ├── syscall_gcc.txt       # Syscall definitions
│   └── app.ld                # Default linker script
└── app_yourapp/
    ├── code/
    │   ├── app_main.c        # Your main code
    │   └── graphics.h        # Your bitmap data (optional)
    └── gcc/
        ├── Makefile
        ├── app.ld            # Copy from libs/
        └── createmwa.py      # Package creator
```

## Complete API Reference

### Drawing Functions

```c
// Clear entire screen
void eink_clear(uint16_t color);

// Draw single pixel
void eink_drawpixel(uint16_t x, uint16_t y, uint16_t color);

// Draw line
void eink_drawline(uint16_t start_x, uint16_t start_y,
                   uint16_t end_x, uint16_t end_y, uint16_t color);

// Draw dashed line
void eink_drawdashedline(uint16_t start_x, uint16_t start_y,
                         uint16_t end_x, uint16_t end_y, uint16_t color);

// Draw rectangle (MODE_EMPTY=outline, MODE_FILL=filled)
void eink_drawrect(uint16_t start_x, uint16_t start_y,
                   uint16_t end_x, uint16_t end_y,
                   uint16_t color, uint8_t fill_mode);

// Draw circle
void eink_drawcircle(int x_center, int y_center, int radius,
                     int color, int fill_mode);

// Draw bitmap - THE MOST IMPORTANT FUNCTION!
// x, y: top-left position
// w, h: width and height in pixels
// data[]: bitmap array (horizontal 8-point left-high-bit)
// color: drawing color (BLACK or WHITE)
// transparent: 0=draw both colors, 1=only draw foreground
void eink_draw_bmp(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                   const uint8_t data[], uint16_t color, uint8_t transparent);

// Draw ASCII string
// Returns: width of drawn string
uint16_t eink_drawstr(uint16_t x, uint16_t y, const unsigned char *chr,
                      uint16_t size, uint16_t color);

// Draw Chinese string (UTF-8)
uint16_t eink_drawchstr(uint16_t x, uint16_t y, const unsigned char *chr,
                        uint16_t color);

// Draw string within rectangle bounds
uint16_t eink_draw_rectstr(const unsigned char *chr,
                           uint16_t start_x, uint16_t start_y,
                           uint16_t end_x, uint16_t end_y, uint16_t color);

// Set screen rotation
void eink_set_rotate(Rotate rotate);

// Get weather icon bitmap (32x32) into buffer
void get_weather_icon(uint16_t iconidx, uint8_t *buffer);
```

### Time Functions

```c
int RTC_getYear(void);      // Full year (e.g., 2024)
int RTC_getMon(void);       // Month 1-12
int RTC_getDay(void);       // Day 1-31
int RTC_getHour(void);      // Hour 0-23
int RTC_getMin(void);       // Minute 0-59
int RTC_getSec(void);       // Second 0-59
int RTC_getWeek(void);      // Day of week (0=Sunday)
int RTC_getTimeStamp(void); // Unix timestamp
```

### System Functions

```c
// Get battery percentage (0-100)
uint8_t watch_app_battpercent(void);

// Exit app, return to main menu
void watch_app_exit(void);

// Set update interval for onUpdate() callback (in milliseconds)
void set_update_interval(uint32_t interval);

// Get current update interval
uint32_t get_update_interval(void);

// Log message (for debugging)
void watch_app_log(char* log);

// Get Bluetooth connection state
BlueState watch_app_bluestate(void);

// Check if weather data is available
uint8_t watch_app_isweather_ok(void);

// Get today's data (sunrise, sunset, lunar, etc.)
TodayData watch_app_getToday(void);

// Get weather for specified day
Weather watch_app_getweather(WeatherDay day);

// HTTP request (async, result via callback)
void watch_app_http_req(const char* url, void(req_callback)(char*));
```

### File Operations

```c
// Read file
// Returns: bytes read
uint32_t watch_app_read_file(const uint8_t* file_name, uint8_t* buffer,
                             uint32_t len, uint32_t seekofs);

// Write file
// fa_mode: FA_CREATE_ALWAYS, FA_OPEN_APPEND, etc.
uint32_t watch_app_write_file(const uint8_t* file_name, uint8_t* buffer,
                              uint32_t len, uint32_t seekofs, uint8_t fa_mode);

// Delete file
uint8_t watch_app_delete_file(const uint8_t* file_name);

// Create directory
uint8_t watch_app_mkdir(const uint8_t* dir);
```

### Memory Functions

```c
// Allocate memory (use sparingly - only 4K RAM!)
void *m_malloc(uint32_t size);

// Free memory
void m_free(void *ptr);
```

### Utility Functions

```c
// Format string (like sprintf)
void co_sprintf(char *out, const char *format, ...);

// Get UTF-8 string length
uint16_t utf_len(const unsigned char *chr);

// Random number
int rand(void);
void srand(unsigned int seed);
```

### Dialog Functions

```c
// Show message dialog with OK/Cancel
// submit callback receives: 1=OK, 0=Cancel
void create_msg_dialog(const uint8_t* title, const uint8_t* msg,
                       void (*submit)(uint8_t ok));

// Show menu dialog
void create_menu_dialog(const char* title, const char** menu_item_names,
                        const uint8_t count, void (*submit)(uint8_t confirm));

// Show number picker dialog
void create_picker_dialog(uint8_t num, const uint8_t* nums,
                          const uint8_t count, void (*submit)(uint8_t data));
```

## App Entry Points

```c
#include "header.h"

// Called when screen needs to be drawn
void onDraw(void) {
    eink_clear(WHITE);
    // Draw your UI here
}

// Called when button is pressed
// Return update type to trigger screen refresh
UpdateType onKey(ButtonType key) {
    switch(key) {
        case KEY_BACK:
            watch_app_exit();
            break;
        case KEY_UP:
            // Handle up button
            break;
        case KEY_DOWN:
            // Handle down button
            break;
    }
    return PART_UPDATE;  // or NONE_UPDATE, FULL_UPDATE
}

// Called periodically based on set_update_interval()
UpdateType onUpdate(int delta) {
    static uint8_t lastmin = 61;
    if (lastmin != RTC_getMin()) {
        lastmin = RTC_getMin();
        return PART_UPDATE;  // Refresh screen when minute changes
    }
    return NONE_UPDATE;
}

// App initialization - REQUIRED
void app_init(intptr_t *draw_ptr_t, intptr_t *onkey_ptr_t,
              intptr_t *update_ptr_t, intptr_t* func_arr) {
    __initialize_datas(func_arr);
    *draw_ptr_t = (intptr_t)onDraw;
    *onkey_ptr_t = (intptr_t)onKey;
    *update_ptr_t = (intptr_t)onUpdate;

    // Initialize your variables here
    set_update_interval(1000);  // Check every 1 second
}
```

## Complete Minimal Example

```c
// app_main.c - Simple digital clock using eink_drawstr
#include "header.h"

void onDraw(void) {
    eink_clear(WHITE);

    char buf[32];

    // Draw time (large, centered)
    co_sprintf(buf, "%02d:%02d", RTC_getHour(), RTC_getMin());
    eink_drawstr(50, 80, (unsigned char*)buf, 32, BLACK);

    // Draw date
    co_sprintf(buf, "%04d-%02d-%02d", RTC_getYear(), RTC_getMon(), RTC_getDay());
    eink_drawstr(40, 130, (unsigned char*)buf, 16, BLACK);

    // Draw battery
    co_sprintf(buf, "BAT:%d%%", watch_app_battpercent());
    eink_drawstr(140, 5, (unsigned char*)buf, 12, BLACK);
}

UpdateType onKey(ButtonType key) {
    if (key == KEY_BACK || key == KEY_CENTER) {
        watch_app_exit();
    }
    return NONE_UPDATE;
}

UpdateType onUpdate(int delta) {
    static uint8_t lastmin = 61;
    if (lastmin != RTC_getMin()) {
        lastmin = RTC_getMin();
        if (RTC_getMin() % 5 == 0) {
            return FULL_UPDATE;
        }
        return PART_UPDATE;
    }
    return NONE_UPDATE;
}

void app_init(intptr_t *draw_ptr_t, intptr_t *onkey_ptr_t,
              intptr_t *update_ptr_t, intptr_t* func_arr) {
    __initialize_datas(func_arr);
    *draw_ptr_t = (intptr_t)onDraw;
    *onkey_ptr_t = (intptr_t)onKey;
    *update_ptr_t = (intptr_t)onUpdate;
    set_update_interval(1000);
}
```

---

## SDK Source Files (COMPLETE - Copy These Exactly)

### gcc/app.ld (Linker Script)

```ld
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x10050000, LENGTH = 40K
    RAM (xrw)       : ORIGIN = 0x11001000, LENGTH = 4K
}

ENTRY(app_init)

SECTIONS
{
    .text : ALIGN(4)
    {
        *(.text .text.*)
        *(.rodata .rodata.* .constdata .constdata.*)
        KEEP(*(.eh_frame*))
        *(.glue_7)
        *(.glue_7t)
    } >FLASH

    _sidata = LOADADDR(.data);

    .data : ALIGN(4)
    {
        _sdata = . ;
        *(.data .data.*)
        . = ALIGN(4);
        _edata = . ;
    } >RAM AT>FLASH

    .bss (NOLOAD) : ALIGN(4)
    {
        _sbss = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM

    .keep_section : ALIGN(4)
    {
        *(.keep_section)
        KEEP(*(.keep_section*))
    } >FLASH
}
```

### gcc/Makefile (Complete)

```makefile
# ---------------------------------- #
# Project name - CHANGE THIS         #
# ---------------------------------- #
PROJECT_NAME := MoWatchAPP

# ---------------------------------- #
# Path config                        #
# ---------------------------------- #
TOP_DIR     := ../..
PROJECT_DIR := ../code
OBJECT_DIR  := ./objects

# ---------------------------------- #
# Toolchain config                   #
# ---------------------------------- #
TOOLCHAIN := arm-none-eabi

CC      := $(TOOLCHAIN)-gcc
AS      := $(TOOLCHAIN)-as
LD      := $(TOOLCHAIN)-ld
OBJDUMP := $(TOOLCHAIN)-objdump
OBJCOPY := $(TOOLCHAIN)-objcopy
SIZE    := $(TOOLCHAIN)-size

# ---------------------------------- #
# ld/lib config                      #
# ---------------------------------- #
LD_SCRIPT := ./app.ld
LD_C := $(TOP_DIR)/libs/syscall_gcc.txt

# ---------------------------------- #
# Source files config                #
# ---------------------------------- #
SRC_FILES += $(TOP_DIR)/libs/init_datas.c
SRC_FILES += $(TOP_DIR)/libs/tiny-json.c
SRC_FILES += $(PROJECT_DIR)/app_main.c

INC_PATH += -I"$(TOP_DIR)/include"
INC_PATH += -I"$(PROJECT_DIR)"

# ----------------------------------- #
# Objects files                       #
# ----------------------------------- #
BASE_SRC  = $(notdir $(SRC_FILES))
BASE_OBJS = $(BASE_SRC:%.c=%.o)
OBJS      = $(BASE_OBJS:%.o=$(OBJECT_DIR)/%.o)
BASE_ElF  = $(OBJECT_DIR)/$(PROJECT_NAME).elf
VPATH = $(dir $(SRC_FILES))

# ---------------------------------- #
# C flags                            #
# ---------------------------------- #
CFLAGS += -mcpu=cortex-m3
CFLAGS += -mthumb
CFLAGS += -O2
CFLAGS += -fmessage-length=0 -fsigned-char
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -g3
CFLAGS += -std=gnu11

# ---------------------------------- #
# LD flags                           #
# ---------------------------------- #
LDFLAGS += -mcpu=cortex-m3
LDFLAGS += -mthumb
LDFLAGS += -O2
LDFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += -g3
LDFLAGS += -Xlinker --gc-sections
LDFLAGS += --specs=nosys.specs -u _printf_float

# ---------------------------------- #
# Build targets                      #
# ---------------------------------- #
all: Target_Path Target_OBJS Target_ELF Target_DONE

Target_Path:
	@mkdir -p $(OBJECT_DIR)

Target_OBJS: $(OBJS)

$(OBJECT_DIR)/%.o: %.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) $(INC_PATH) -c -o $@ $<

Target_ELF: $(BASE_ElF)

$(BASE_ElF): $(OBJS)
	@$(CC) $(LDFLAGS) -T $(LD_SCRIPT) -T $(LD_C) -Wl,-Map=$(PROJECT_NAME).map -o $@ $^
	@$(OBJCOPY) -O binary -S $@ $(PROJECT_NAME).bin
	@echo "Created $(PROJECT_NAME).bin"

Target_DONE:
	@$(SIZE) $(BASE_ElF)
	@echo "Build complete: $(PROJECT_NAME)"

clean:
	rm -rf $(OBJECT_DIR) *.bin *.map *.hex *.dis *.elf

.PHONY: all clean Target_Path Target_OBJS Target_ELF Target_DONE
```

### gcc/createmwa.py (Complete)

```python
import re
import os
import hashlib
import time

# ============================================
# CONFIGURATION - CHANGE THESE FOR YOUR APP
# ============================================

# Binary input filename (from make)
inputbin = "MoWatchAPP.bin"

# Output .mwa filename
outputmwa = "SimpleWatch.mwa"

# App icon: 48x48 bitmap, 288 bytes
# Format: horizontal 8-point left-high-bit
# Default icon (clock shape):
appicon_bmp = [
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFE,0x00,0x00,0x00,0x03,0xFF,0xFF,
0xC0,0x00,0x00,0x0F,0xFF,0xFF,0xF0,0x00,0x00,0x1F,0xFF,0xFF,0xF8,0x00,0x00,0x7F,
0xFF,0xFF,0xFE,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0x80,
0x03,0xFF,0xFF,0xFF,0xFF,0xC0,0x07,0xFF,0xFF,0xFF,0xFF,0xE0,0x07,0xFF,0xFF,0xFF,
0xFF,0xE0,0x0F,0xFF,0xFF,0xFF,0xFF,0xF0,0x0F,0xFF,0xFC,0x3F,0xFF,0xF0,0x1F,0xFF,
0xFC,0x3F,0xFF,0xF8,0x1F,0xFF,0xFC,0x3F,0xFF,0xF8,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,
0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x7F,0xFF,0xFC,0x3F,
0xFF,0xFE,0x7F,0xFF,0xFC,0x3F,0xFF,0xFE,0x7F,0xFF,0xFC,0x3F,0xFF,0xFE,0x7F,0xFF,
0xFC,0x3F,0xFF,0xFE,0x7F,0xFF,0xFC,0x3F,0xFF,0xFE,0x7F,0xFF,0xFC,0x00,0xFF,0xFE,
0x7F,0xFF,0xFC,0x00,0x7F,0xFE,0x7F,0xFF,0xFC,0x00,0x7F,0xFE,0x7F,0xFF,0xFC,0x00,
0x7F,0xFE,0x7F,0xFF,0xFF,0x00,0x7F,0xFE,0x7F,0xFF,0xFF,0x80,0xFF,0xFE,0x3F,0xFF,
0xFF,0xFF,0xFF,0xFC,0x3F,0xFF,0xFF,0xFF,0xFF,0xFC,0x3F,0xFF,0xFF,0xFF,0xFF,0xFC,
0x1F,0xFF,0xFF,0xFF,0xFF,0xF8,0x1F,0xFF,0xFF,0xFF,0xFF,0xF8,0x0F,0xFF,0xFF,0xFF,
0xFF,0xF0,0x0F,0xFF,0xFF,0xFF,0xFF,0xF0,0x07,0xFF,0xFF,0xFF,0xFF,0xE0,0x07,0xFF,
0xFF,0xFF,0xFF,0xE0,0x03,0xFF,0xFF,0xFF,0xFF,0xC0,0x01,0xFF,0xFF,0xFF,0xFF,0x80,
0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x7F,0xFF,0xFF,0xFE,0x00,0x00,0x1F,0xFF,0xFF,
0xF8,0x00,0x00,0x0F,0xFF,0xFF,0xF0,0x00,0x00,0x03,0xFF,0xFF,0xC0,0x00,0x00,0x00,
0x7F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
]

# App slot (usually 1)
app_slot = 1
app_head_end = 0xFF

# ============================================
# DO NOT MODIFY BELOW THIS LINE
# ============================================

with open("MoWatchAPP.map", "r") as f:
    lines = f.readlines()

init_addr = 0
pattern = re.compile(r"\s+(0x[0-9a-fA-F]+)\s+([a-zA-Z_][a-zA-Z0-9_]*)")
for line in lines:
    match = pattern.match(line)
    if match:
        address, name = match.groups()
        if name == "app_init":
            init_addr = int(address, 16)
            break

if init_addr & 1 == 0:
    init_addr = init_addr | 1

def generate_uuid():
    timestamp = str(time.time()).encode()
    hash_object = hashlib.md5()
    hash_object.update(timestamp)
    md5_hash = hash_object.hexdigest()
    uuid = "{}{}{}{}{}".format(md5_hash[0:8], md5_hash[8:12], md5_hash[12:16], md5_hash[16:20], md5_hash[20:])
    return uuid

uuid = generate_uuid()

with open(inputbin, "rb") as f:
    appbin = f.read()

if os.path.isfile(outputmwa):
    os.remove(outputmwa)
    print(f"Deleted existing {outputmwa}")

with open(outputmwa, "wb+") as fi:
    fi.write(uuid.encode("utf-8"))
    fi.write(init_addr.to_bytes(4, "little"))
    fi.write(app_slot.to_bytes(1, "little"))
    fi.write(app_head_end.to_bytes(1, "little"))
    for bit in appicon_bmp:
        fi.write(bit.to_bytes(1, "little"))
    for byte in appbin:
        fi.write(byte.to_bytes(1, "little"))

print(f"Created {outputmwa} - copy to watch /apps/ folder")
```

---

## Building with Docker

1. **Create Dockerfile** (in SDK root, same level as app_projects):
```dockerfile
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y \
    gcc-arm-none-eabi \
    make \
    python3 \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /build
```

2. **Build Docker image**:
```bash
docker build -t mowatch-sdk .
```

3. **Compile**:
```bash
docker run --rm -v "/path/to/app_projects:/build" mowatch-sdk make -C /build/app_yourapp/gcc
```

4. **Create .mwa package**:
```bash
cd gcc && python3 createmwa.py
```

---

## Quick Start: Create New Project

```bash
# In app_projects directory
mkdir -p app_mywatch/code app_mywatch/gcc

# Create app_main.c in code/
# Copy Makefile, app.ld, createmwa.py to gcc/ (from above)
# Edit createmwa.py to set outputmwa name

# Build
docker run --rm -v "$(pwd):/build" mowatch-sdk make -C /build/app_mywatch/gcc

# Package
cd app_mywatch/gcc && python3 createmwa.py
```

---

## Image Processing for E-ink (Python)

### Converting images to 1-bit bitmap:

```python
from PIL import Image, ImageFilter

def process_for_eink(img_path, output_size=(120, 120)):
    img = Image.open(img_path).convert('RGB')
    img = img.resize(output_size, Image.Resampling.LANCZOS)
    gray = img.convert('L')
    edges = gray.filter(ImageFilter.FIND_EDGES)
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
                    if pixels[idx] < 128:
                        byte |= (128 >> bit)
            data.append(byte)

    lines = [f"const unsigned char {var_name}[{len(data)}] = {{"]
    for i in range(0, len(data), 15):
        chunk = data[i:i+15]
        lines.append("    " + ", ".join(f"0x{b:02X}" for b in chunk) + ",")
    lines.append("};")
    return "\n".join(lines)
```

---

## Installation to Watch

1. Enable USB mode on MoWatch
2. Mount appears as "NO NAME" volume
3. Copy `.mwa` file to `/apps/` folder
4. Safely eject: `diskutil eject "/Volumes/NO NAME"`
5. Select watch face from watch menu

---

## Tips

- **Memory**: Only 4K RAM! Use `const` for all bitmap data
- **Refresh**: Use PART_UPDATE when possible, FULL_UPDATE every 5 minutes
- **Graphics**: Keep simple - e-ink has slow refresh
- **Digits**: 20x32px works well for time display
- **Layout**: Leave margins from screen edges for HUD-style designs
- **Testing**: Use `watch_app_log()` for debugging
- **Battery**: Check `watch_app_battpercent()` to show battery status

---

# Appendix: Complete SDK Source Files

These files must be placed in the SDK directory structure. Create them exactly as shown.

## include/header.h

```c
#ifndef HEADER_H
#define HEADER_H
#include <stdint.h>

#define SCREEN_WIDTH   200
#define SCREEN_HEIGHT  200

#define BLACK   0
#define WHITE   0xFF
#define GREY    0x80

#define MODE_EMPTY  0
#define MODE_FILL   1

#define FA_READ           0x01
#define FA_WRITE          0x02
#define FA_OPEN_EXISTING  0x00
#define FA_CREATE_NEW     0x04
#define FA_CREATE_ALWAYS  0x08
#define FA_OPEN_ALWAYS    0x10
#define FA_OPEN_APPEND    0x30

#define SCREEN_TYPE_EINK 0
#define SCREEN_TYPE_MONO 1
#define SCREEN_TYPE SCREEN_TYPE_EINK

typedef enum UPDAT_TYPE {
    FULL_UPDATE,
    PART_UPDATE,
    NONE_UPDATE,
} UpdateType;

typedef enum MBUTTON_TYPE {
    KEY_NULL,
    KEY_CENTER,
    KEY_UP,
    KEY_DOWN,
    KEY_CENTER_UP,
    KEY_UP_CENTER,
    KEY_CENTER_DOWN,
    KEY_DOWN_CENTER,
    KEY_UP_DOWN,
    KEY_DOWN_UP,
    KEY_BACK,
} ButtonType;

typedef struct TODAYDATA {
    uint16_t moonicon;
    uint16_t pressure;
    uint16_t humidity;
    uint8_t sunraise[6];
    uint8_t sunset[6];
    uint8_t lunar[16];
} TodayData;

typedef struct WEATHER {
    uint8_t date;
    char day_temp;
    char night_temp;
    uint16_t day_icon;
    uint16_t night_icon;
} Weather;

typedef enum WEATHERDAY {
    TOTDAY,
    TOMORROW,
    AFTERTOMOROW,
    DAY_COUNT,
} WeatherDay;

typedef enum ROTATE {
    ROTATE_0,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270,
} Rotate;

typedef enum BLUESTTE {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
} BlueState;

__attribute__((section(".keep_section"))) void onDraw(void);
__attribute__((section(".keep_section"))) UpdateType onUpdate(int delta);
__attribute__((section(".keep_section"))) UpdateType onKey(ButtonType key);
__attribute__((section(".keep_section"))) void app_init(intptr_t *draw_ptr_t, intptr_t *onkey_ptr_t, intptr_t* onupdate_ptr_t, intptr_t* func_arr);

void __initialize_datas(const intptr_t* func_arr);

void *m_malloc(uint32_t size);
void m_free(void *ptr);
int rand(void);
void srand(unsigned int seed);
void co_sprintf(char *out, const char *format, ...);
uint16_t utf_len(const unsigned char *chr);

void create_msg_dialog(const uint8_t* title, const uint8_t* msg, void (*submit)(uint8_t ok));
void create_menu_dialog(const char* title, const char** menu_item_names, const uint8_t count, void (*submit)(uint8_t confirm));
void create_picker_dialog(uint8_t num, const uint8_t* nums, const uint8_t count, void (*submit)(uint8_t data));
uint8_t watch_app_isweather_ok(void);
uint8_t watch_app_battpercent(void);
TodayData watch_app_getToday(void);
Weather watch_app_getweather(WeatherDay day);
BlueState watch_app_bluestate(void);
void watch_app_http_req(const char* url, void(req_callback)(char*));
void watch_app_log(char* log);
void watch_app_exit(void);
uint32_t watch_app_read_file(const uint8_t* file_name, uint8_t* buffer, uint32_t len, uint32_t seekofs);
uint32_t watch_app_write_file(const uint8_t* file_name, uint8_t* buffer, uint32_t len, uint32_t seekofs, uint8_t fa_mode);
uint8_t watch_app_delete_file(const uint8_t* file_name);
uint8_t watch_app_mkdir(const uint8_t* dir);

void set_update_interval(uint32_t interval);
uint32_t get_update_interval(void);

void eink_clear(uint16_t color);
void eink_drawpixel(uint16_t x, uint16_t y, uint16_t color);
void eink_drawline(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color);
void eink_drawdashedline(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t Color);
void eink_drawrect(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color, uint8_t fill_mode);
void eink_drawcircle(int x_center, int y_center, int radius, int color, int fill_mode);
void eink_draw_bmp(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t data[], uint16_t color, uint8_t transparent);
void eink_set_rotate(Rotate rotate);
void get_weather_icon(uint16_t iconidx, uint8_t *buffer);
uint16_t eink_drawstr(uint16_t x, uint16_t y, const unsigned char *chr, uint16_t size, uint16_t color);
uint16_t eink_drawchstr(uint16_t x, uint16_t y, const unsigned char *chr, uint16_t color);
uint16_t eink_draw_rectstr(const unsigned char *chr, uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color);

int RTC_getYear(void);
int RTC_getMon(void);
int RTC_getDay(void);
int RTC_getHour(void);
int RTC_getMin(void);
int RTC_getSec(void);
int RTC_getWeek(void);
int RTC_getTimeStamp(void);

#endif
```

## libs/syscall_gcc.txt

```
rand = 0x759;
srand = 0x76d;
co_sprintf = 0x3841;
```

## libs/init_datas.c

```c
#include "header.h"
#include <stdint.h>

enum FUNC_TAGS {
    tag_watch_app_battpercent,
    tag_watch_app_http_req,
    tag_watch_app_log,
    tag_watch_app_exit,
    tag_watch_app_mkdir,
    tag_watch_app_write_file,
    tag_watch_app_read_file,
    tag_watch_app_delete_file,
    tag_watch_app_bluestate,
    tag_watch_app_getweather,
    tag_watch_app_getToday,
    tag_watch_app_isweather_ok,
    tag_eink_clear,
    tag_eink_drawpixel,
    tag_eink_drawline,
    tag_eink_drawdashedline,
    tag_eink_drawrect,
    tag_eink_drawcircle,
    tag_eink_draw_bmp,
    tag_eink_drawstr,
    tag_eink_drawchstr,
    tag_eink_draw_rectstr,
    tag_RTC_getYear,
    tag_RTC_getMon,
    tag_RTC_getDay,
    tag_RTC_getHour,
    tag_RTC_getMin,
    tag_RTC_getSec,
    tag_RTC_getWeek,
    tag_RTC_getTimeStamp,
    tag_create_msg_dialog,
    tag_create_menu_dialog,
    tag_os_malloc,
    tag_os_free,
    tag_utf_len,
    tag_create_picker_dialog,
    tag_eink_set_rotate,
    tag_set_interval,
    tag_get_interval,
    tag_get_weather_icon,
    tag_beep_on,
    tag_beep_off,
    tag_buzzer,
    tag_count,
};

const intptr_t *func_array;

extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

void __initialize_datas(const intptr_t *func_arr) {
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    uint32_t *end = &_edata;
    while(dst < end) { *dst++ = *src++; }
    src = &_sbss;
    end = &_ebss;
    while(src < end) { *src++ = 0; }
    func_array = func_arr;
}

typedef void* (*func_malloc_ptr_t)(uint32_t);
typedef void (*func_eink_clear_ptr_t)(uint16_t);
typedef void (*func_free_ptr_t)(void*);
typedef uint16_t (*func_utf_len_ptr_t)(const unsigned char*);
typedef void (*func_get_weather_icon_ptr_t)(uint16_t iconidx, uint8_t *buffer);
typedef void (*func_msgdialog_ptr_t)(const uint8_t* title, const uint8_t* msg, void (*submit)(uint8_t ok));
typedef void (*func_pickerdialog_ptr_t)(uint8_t num, const uint8_t* nums, uint8_t count, void (*submit)(uint8_t data));
typedef void (*func_menudialog_ptr_t)(const char* title, const char** menu_item_names, const uint8_t count, void (*submit)(uint8_t confirm));
typedef uint8_t (*func_get_ptr_t)(void);
typedef TodayData (*func_gettoday_ptr_t)(void);
typedef Weather (*func_getweather_ptr_t)(WeatherDay);
typedef BlueState (*func_getblue_ptr_t)(void);
typedef void (*func_httpreq_ptr_t)(const char* url, void(req_callback)(char*));
typedef void (*func_void_ptr_t)(void);
typedef void (*func_enable_ptr_t)(uint8_t);
typedef void (*func_set_interval_ptr_t)(uint32_t);
typedef uint32_t (*func_get_interval_ptr_t)(void);

void set_update_interval(uint32_t interval) {
    return ((func_set_interval_ptr_t)(intptr_t)func_array[tag_set_interval])(interval);
}
uint32_t get_update_interval(void) {
    return ((func_get_interval_ptr_t)(intptr_t)func_array[tag_get_interval])();
}
void get_weather_icon(uint16_t iconidx, uint8_t *buffer) {
    return ((func_get_weather_icon_ptr_t)(intptr_t)func_array[tag_get_weather_icon])(iconidx, buffer);
}
void eink_set_rotate(uint8_t rotate) {
    return ((func_enable_ptr_t)(intptr_t)func_array[tag_eink_set_rotate])(rotate);
}
void create_picker_dialog(uint8_t num, const uint8_t* nums, const uint8_t count, void (*submit)(uint8_t data)) {
    return ((func_pickerdialog_ptr_t)(intptr_t)func_array[tag_create_picker_dialog])(num, nums, count, submit);
}
void *m_malloc(uint32_t size) {
    return ((func_malloc_ptr_t)(intptr_t)func_array[tag_os_malloc])(size);
}
void m_free(void *ptr) {
    return ((func_free_ptr_t)(intptr_t)func_array[tag_os_free])(ptr);
}
uint16_t utf_len(const unsigned char *chr) {
    return ((func_utf_len_ptr_t)(intptr_t)func_array[tag_utf_len])(chr);
}
void create_msg_dialog(const uint8_t* title, const uint8_t* msg, void (*submit)(uint8_t ok)) {
    return ((func_msgdialog_ptr_t)(intptr_t)func_array[tag_create_msg_dialog])(title, msg, submit);
}
void create_menu_dialog(const char* title, const char** menu_item_names, const uint8_t count, void (*submit)(uint8_t confirm)) {
    return ((func_menudialog_ptr_t)(intptr_t)func_array[tag_create_menu_dialog])(title, menu_item_names, count, submit);
}
uint8_t watch_app_isweather_ok(void) {
    return ((func_get_ptr_t)(intptr_t)func_array[tag_watch_app_isweather_ok])();
}
uint8_t watch_app_battpercent(void) {
    return ((func_get_ptr_t)(intptr_t)func_array[tag_watch_app_battpercent])();
}
TodayData watch_app_getToday(void) {
    return ((func_gettoday_ptr_t)(intptr_t)func_array[tag_watch_app_getToday])();
}
Weather watch_app_getweather(WeatherDay day) {
    return ((func_getweather_ptr_t)(intptr_t)func_array[tag_watch_app_getweather])(day);
}
BlueState watch_app_bluestate(void) {
    return ((func_getblue_ptr_t)(intptr_t)func_array[tag_watch_app_bluestate])();
}
void watch_app_http_req(const char* url, void(req_callback)(char*)) {
    return ((func_httpreq_ptr_t)(intptr_t)func_array[tag_watch_app_http_req])(url, req_callback);
}
typedef void (*func_applog_ptr_t)(char* log);
void watch_app_log(char* log) {
    return ((func_applog_ptr_t)(intptr_t)func_array[tag_watch_app_log])(log);
}
void watch_app_exit(void) {
    return ((func_void_ptr_t)(intptr_t)func_array[tag_watch_app_exit])();
}
typedef uint32_t (*func_readfile_ptr_t)(const uint8_t* file_name, uint8_t* buffer, uint32_t len, uint32_t seekofs);
typedef uint32_t (*func_writefile_ptr_t)(const uint8_t* file_name, uint8_t* buffer, uint32_t len, uint32_t seekofs, uint8_t fa_mode);
typedef uint8_t (*func_mkdel_ptr_t)(const uint8_t* file_name);
uint32_t watch_app_read_file(const uint8_t* file_name, uint8_t* buffer, uint32_t len, uint32_t seekofs) {
    return ((func_readfile_ptr_t)(intptr_t)func_array[tag_watch_app_read_file])(file_name, buffer, len, seekofs);
}
uint32_t watch_app_write_file(const uint8_t* file_name, uint8_t* buffer, uint32_t len, uint32_t seekofs, uint8_t fa_mode) {
    return ((func_writefile_ptr_t)(intptr_t)func_array[tag_watch_app_write_file])(file_name, buffer, len, seekofs, fa_mode);
}
uint8_t watch_app_mkdir(const uint8_t* dir) {
    return ((func_mkdel_ptr_t)(intptr_t)func_array[tag_watch_app_mkdir])(dir);
}
uint8_t watch_app_delete_file(const uint8_t* file_name) {
    return ((func_mkdel_ptr_t)(intptr_t)func_array[tag_watch_app_delete_file])(file_name);
}
void eink_clear(uint16_t color) {
    return ((func_eink_clear_ptr_t)(intptr_t)func_array[tag_eink_clear])(color);
}
typedef void (*func_drawpixel_ptr_t)(uint16_t x, uint16_t y, uint16_t color);
void eink_drawpixel(uint16_t x, uint16_t y, uint16_t color) {
    return ((func_drawpixel_ptr_t)(intptr_t)func_array[tag_eink_drawpixel])(x, y, color);
}
typedef void (*func_drawline_ptr_t)(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color);
void eink_drawline(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color) {
    return ((func_drawline_ptr_t)(intptr_t)func_array[tag_eink_drawline])(start_x, start_y, end_x, end_y, color);
}
void eink_drawdashedline(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color) {
    return ((func_drawline_ptr_t)(intptr_t)func_array[tag_eink_drawdashedline])(start_x, start_y, end_x, end_y, color);
}
typedef void (*func_drawrect_ptr_t)(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color, uint8_t fill_mode);
void eink_drawrect(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color, uint8_t fill_mode) {
    return ((func_drawrect_ptr_t)(intptr_t)func_array[tag_eink_drawrect])(start_x, start_y, end_x, end_y, color, fill_mode);
}
typedef void (*func_drawcircle_ptr_t)(int x_center, int y_center, int radius, int color, int fill_mode);
void eink_drawcircle(int x_center, int y_center, int radius, int color, int fill_mode) {
    return ((func_drawcircle_ptr_t)(intptr_t)func_array[tag_eink_drawcircle])(x_center, y_center, radius, color, fill_mode);
}
typedef void (*func_drawbmp_ptr_t)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t data[], uint16_t color, uint8_t transparent);
void eink_draw_bmp(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t data[], uint16_t color, uint8_t transparent) {
    return ((func_drawbmp_ptr_t)(intptr_t)func_array[tag_eink_draw_bmp])(x, y, w, h, data, color, transparent);
}
typedef uint16_t (*func_drawstr_ptr_t)(uint16_t x, uint16_t y, const unsigned char *chr, uint16_t size, uint16_t color);
uint16_t eink_drawstr(uint16_t x, uint16_t y, const unsigned char *chr, uint16_t size, uint16_t color) {
    return ((func_drawstr_ptr_t)(intptr_t)func_array[tag_eink_drawstr])(x, y, chr, size, color);
}
typedef uint16_t (*func_drawchstr_ptr_t)(uint16_t x, uint16_t y, const unsigned char *chr, uint16_t color);
uint16_t eink_drawchstr(uint16_t x, uint16_t y, const unsigned char *chr, uint16_t color) {
    return ((func_drawchstr_ptr_t)(intptr_t)func_array[tag_eink_drawchstr])(x, y, chr, color);
}
typedef uint16_t (*func_drawrectstr_ptr_t)(const unsigned char *chr, uint16_t start_x, uint16_t start_y, uint16_t stop_x, uint16_t stop_y, uint16_t color);
uint16_t eink_draw_rectstr(const unsigned char *chr, uint16_t start_x, uint16_t start_y, uint16_t stop_x, uint16_t stop_y, uint16_t color) {
    return ((func_drawrectstr_ptr_t)(intptr_t)func_array[tag_eink_draw_rectstr])(chr, start_x, start_y, stop_x, stop_y, color);
}
typedef int(*func_rtc_ptr_t)(void);
int RTC_getDay(void) { return ((func_rtc_ptr_t)(intptr_t)func_array[tag_RTC_getDay])(); }
int RTC_getHour(void) { return ((func_rtc_ptr_t)(intptr_t)func_array[tag_RTC_getHour])(); }
int RTC_getMin(void) { return ((func_rtc_ptr_t)(intptr_t)func_array[tag_RTC_getMin])(); }
int RTC_getMon(void) { return ((func_rtc_ptr_t)(intptr_t)func_array[tag_RTC_getMon])(); }
int RTC_getSec(void) { return ((func_rtc_ptr_t)(intptr_t)func_array[tag_RTC_getSec])(); }
int RTC_getTimeStamp(void) { return ((func_rtc_ptr_t)(intptr_t)func_array[tag_RTC_getTimeStamp])(); }
int RTC_getWeek(void) { return ((func_rtc_ptr_t)(intptr_t)func_array[tag_RTC_getWeek])(); }
int RTC_getYear(void) { return ((func_rtc_ptr_t)(intptr_t)func_array[tag_RTC_getYear])(); }
```

## libs/tiny-json.c

Note: This is a third-party JSON library. For basic watch faces, you may not need it.
The Makefile includes it by default. See https://github.com/rafagafe/tiny-json for full source.

For simple projects, you can create an empty stub file or download from the GitHub repo above
