ROOT_DIR := $(abspath ..)
Q ?= @

IMGTOOL_PY := $(ROOT_DIR)/external/mcuboot/scripts/imgtool.py

COMPILER ?= arm-none-eabi-gcc

COMMON_DIR = $(ROOT_DIR)/common
COMMON_SRC_DIR = $(COMMON_DIR)/src
EXTERNAL_DIR := $(ROOT_DIR)/external

APPLICATION_DIR = $(ROOT_DIR)/application
BUILD_DIR  := $(APPLICATION_DIR)/build
PROJECT_SRC_DIR = $(APPLICATION_DIR)/src

SRC_FILES += \
	$(PROJECT_SRC_DIR)/main.c \
	$(PROJECT_SRC_DIR)/startup.c \
	$(PROJECT_SRC_DIR)/minimal_rp2350_flash.c \
	$(PROJECT_SRC_DIR)/minimal_rp2350_uart.c \
	$(COMMON_SRC_DIR)/shell/shell.c \
	$(COMMON_SRC_DIR)/shell/shell_commands.c \
	$(COMMON_SRC_DIR)/shell/shell_port.c \
	$(COMMON_SRC_DIR)/minimum_arm_image_def_block.S \
	$(COMMON_SRC_DIR)/unhandled_user_irq.S \
	$(COMMON_SRC_DIR)/logging/log_port.c \
	

############## PICO-SDK ##############
PICO_SDK_BASE_DIR := $(EXTERNAL_DIR)/pico-sdk
PICO_SDK_SRC_DIR = $(PICO_SDK_BASE_DIR)/src

SRC_FILES += \
	$(PICO_SDK_SRC_DIR)/rp2_common/hardware_uart/uart.c \
	$(PICO_SDK_SRC_DIR)/rp2_common/hardware_clocks/clocks.c \
	$(PICO_SDK_SRC_DIR)/rp2_common/hardware_gpio/gpio.c \
	$(PICO_SDK_SRC_DIR)/rp2_common/hardware_xosc/xosc.c \
	$(PICO_SDK_SRC_DIR)/rp2_common/hardware_pll/pll.c \
	$(PICO_SDK_SRC_DIR)/rp2_common/hardware_irq/irq.c \
	$(PICO_SDK_SRC_DIR)/rp2_common/hardware_timer/timer.c \
	$(PICO_SDK_SRC_DIR)/common/hardware_claim/claim.c \
	$(PICO_SDK_SRC_DIR)/rp2_common/hardware_irq/irq_handler_chain.S \
	$(PICO_SDK_SRC_DIR)/rp2_common/pico_platform_panic/panic.c \
	$(PICO_SDK_SRC_DIR)/rp2_common/pico_stdio/stdio.c \
	$(PICO_SDK_SRC_DIR)/common/pico_sync/mutex.c \
	$(PICO_SDK_SRC_DIR)/common/pico_time/time.c \
	$(PICO_SDK_SRC_DIR)/rp2_common/hardware_sync/sync.c \
	$(PICO_SDK_SRC_DIR)/rp2_common/hardware_sync_spin_lock/sync_spin_lock.c \
	$(PICO_SDK_SRC_DIR)/common/pico_sync/lock_core.c \

INCLUDE_PATHS += \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/hardware_uart/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/hardware_resets/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/hardware_xosc/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/hardware_gpio/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/hardware_irq/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/hardware_clocks/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/hardware_pll/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/hardware_sync/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/hardware_timer/include/ \
	-I$(PICO_SDK_SRC_DIR)/common/pico_base_headers/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2350/pico_platform/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/pico_platform_compiler/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2350/hardware_regs/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2350/hardware_structs/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/pico_platform_sections/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/pico_platform_panic/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/pico_platform_common/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/hardware_base/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/hardware_sync_spin_lock/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/pico_runtime_init/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/pico_runtime/include/ \
	-I$(PICO_SDK_SRC_DIR)/common/hardware_claim/include/ \
	-I$(PICO_SDK_SRC_DIR)/common/pico_sync/include/ \
	-I$(PICO_SDK_SRC_DIR)/common/pico_time/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/pico_time_adapter/include/ \
	-I$(PICO_SDK_SRC_DIR)/common/pico_base_headers/include/ \
	-I$(PICO_SDK_SRC_DIR)/common/pico_base/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/pico_platform/include/ \
	-I$(PICO_SDK_SRC_DIR)/rp2350/pico_platform/include/ \
	-I$(PICO_SDK_SRC_DIR)/common/pico_binary_info/include \
	-I$(PICO_SDK_SRC_DIR)/rp2_common/pico_stdio/include/ \
############## PICO-SDK ##############

MCUBOOT_DIR := $(EXTERNAL_DIR)/mcuboot/boot/bootutil
MCUBOOT_SRC_DIR := $(MCUBOOT_DIR)/src
MCUBOOT_INC_DIR := $(MCUBOOT_DIR)/include

MCUBOOT_SRC_FILES += \
	$(COMMON_SRC_DIR)/mcuboot_port.c \
	$(MCUBOOT_SRC_DIR)/bootutil_misc.c \

# Also need mcuboot_config/ on path
MCUBOOT_INC_PATHS += \
-I$(MCUBOOT_INC_DIR) \
-I$(COMMON_DIR)/include/mcuboot_config/ \

SRC_FILES += $(MCUBOOT_SRC_FILES)
INCLUDE_PATHS += $(MCUBOOT_INC_PATHS)

INCLUDE_PATHS += -I$(COMMON_DIR)/include

# 1. Get all C files and convert to .o
OBJ_FILES := $(patsubst $(ROOT_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter %.c,$(SRC_FILES)))

# 2. Get all S files and convert to .o
OBJ_FILES += $(patsubst $(ROOT_DIR)/%.S,$(BUILD_DIR)/%.o,$(filter %.S,$(SRC_FILES)))

INCLUDE_PATHS += \
	-I$(APPLICATION_DIR)/include

TARGET ?= rp2350-application
LDSCRIPT = $(ROOT_DIR)/application/$(TARGET).ld
TARGET_ELF = $(BUILD_DIR)/$(TARGET).elf
TARGET_BIN = $(BUILD_DIR)/$(TARGET)-no-header.bin
MCUBOOT_IMG0_BIN = $(BUILD_DIR)/$(TARGET).bin

DEP_DIR = $(BUILD_DIR)/dep
DEP_CFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$(*F).d

MACH=cortex-m33
CFLAGS= -g -mcpu=$(MACH) -mthumb -std=gnu11 -Wall -Iinclude -g3 -O0 -Dstatic_assert=_Static_assert -DPICO_DEFAULT_UART=0 -DPICO_DEFAULT_UART_TX_PIN=0 -DPICO_DEFAULT_UART_RX_PIN=1
LDFLAGS= -mcpu=$(MACH) -mthumb -mfloat-abi=soft -T $(TARGET).ld -Wl,--wrap=vprintf,-Map=$(BUILD_DIR)/$(TARGET).map --specs=rdimon.specs -lc -lrdimon -Wl,--entry=Reset_Handler -nostartfiles


all: $(MCUBOOT_IMG0_BIN)

-include $(wildcard $(DEP_DIR)/*.d)

clean:
	rm -rf $(BUILD_DIR)


$(TARGET_ELF): $(OBJ_FILES) $(LDSCRIPT)
	@echo "Linking library"
	@arm-none-eabi-gcc $(CFLAGS) $(LDFLAGS) $(OBJ_FILES) -o $@ -Wl,-lc -Wl,-lgcc
	@echo "Generated $(patsubst $(ROOT_DIR)/%,%,$@)"

$(TARGET_BIN): $(TARGET_ELF)
	@echo "Creating raw binary from ELF..."
	$(Q)arm-none-eabi-objcopy -O binary $< $@

# 2. Sign the Raw Binary with imgtool (Output: $(MCUBOOT_IMG0_BIN))
$(MCUBOOT_IMG0_BIN): $(TARGET_BIN)
	@echo "Signing binary with imgtool..."
	$(Q)python3 $(IMGTOOL_PY) sign \
		--key $(COMMON_SRC_DIR)/root-ec-p256.pem \
		--header-size 256 \
		--version 1.0.0 \
		--confirm \
		--pad-header \
		--align 8 \
		--slot-size 0x3c0000 \
		--overwrite-only \
		$(BUILD_DIR)/rp2350-application-no-header.bin \
		$(BUILD_DIR)/rp2350-application.bin

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(DEP_DIR):
	@mkdir -p $(DEP_DIR)

$(OBJ_FILES): Makefile

$(SRC_FILES): $(MCUBOOT_DIR)

$(BUILD_DIR)/%.o: $(ROOT_DIR)/%.c | $(BUILD_DIR) $(DEP_DIR) $(FREERTOS_PORT_ROOT)
	@echo "Compiling $(patsubst $(ROOT_DIR)/%,%,$<)"
	@mkdir -p $(dir $@)
	$(Q) arm-none-eabi-gcc $(DEP_CFLAGS) $(CFLAGS) $(INCLUDE_PATHS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(ROOT_DIR)/%.S | $(BUILD_DIR)
	@echo "Assembling $(patsubst $(ROOT_DIR)/%,%,$<)"
	@mkdir -p $(dir $@)
	$(Q) arm-none-eabi-gcc $(CFLAGS) $(INCLUDE_PATHS) -x assembler-with-cpp -c -o $@ $<
	