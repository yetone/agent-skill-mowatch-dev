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
