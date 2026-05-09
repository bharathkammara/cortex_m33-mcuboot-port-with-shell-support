# --- Directories ---
ROOT_DIR        := $(CURDIR)
EXTERNAL_DIR    := $(ROOT_DIR)/external
BOOTLOADER_DIR  := $(ROOT_DIR)/bootloader
APP_DIR         := $(ROOT_DIR)/application
COMMON_DIR      := $(ROOT_DIR)/common

# Define these so the setup target knows where to go
PICO_SDK_BASE_DIR := $(EXTERNAL_DIR)/pico-sdk
MCUBOOT_BASE_DIR  := $(EXTERNAL_DIR)/mcuboot
OPENOCD_BASE_DIR  := $(EXTERNAL_DIR)/openocd

# --- Flash Map ---
BOOT_OFFSET      := 0x10000000
APP_SLOT0_OFFSET := 0x10040000 

# --- Binaries ---
BOOT_BIN        := $(BOOTLOADER_DIR)/build/rp2350-bootloader.bin
APP_SIGNED_BIN  := $(APP_DIR)/build/rp2350-application.bin

# --- OpenOCD Configuration ---
# Use the locally cloned OpenOCD if built, otherwise system path
OPENOCD         := $(OPENOCD_BASE_DIR)/src/openocd
OCD_INTERFACE   := $(OPENOCD_BASE_DIR)/tcl/interface/cmsis-dap.cfg
OCD_TARGET      := $(OPENOCD_BASE_DIR)/tcl/target/rp2350.cfg
OCD_TCL_DIR     := $(OPENOCD_BASE_DIR)/tcl
OCD_FLAGS       := -s $(OCD_TCL_DIR) -f $(OCD_INTERFACE) -f $(OCD_TARGET) -c "adapter speed 1000"

# --- Targets ---

.PHONY: all setup bootloader app flash flash-app clean reset openocd

# Ensure setup is run or directories exist before building
all: bootloader app

bootloader:
	@echo "--- Building RP2350 Bootloader ---"
	$(MAKE) -C $(BOOTLOADER_DIR)

app:
	@echo "--- Building and Signing Application ---"
	$(MAKE) -C $(APP_DIR)

# Fixed Setup Target
setup:
	@mkdir -p $(EXTERNAL_DIR)
	@if [ ! -d "$(PICO_SDK_BASE_DIR)/.git" ]; then \
		echo "Cloning Pico SDK..."; \
		git clone --recurse-submodules https://github.com/raspberrypi/pico-sdk.git $(PICO_SDK_BASE_DIR); \
		cp $(PICO_SDK_BASE_DIR)/src/common/pico_base_headers/include/pico/version.h.in $(PICO_SDK_BASE_DIR)/src/common/pico_base_headers/include/pico/version.h; \
	fi
	@if [ ! -d "$(MCUBOOT_BASE_DIR)/.git" ]; then \
		echo "Cloning MCUboot..."; \
		git clone --recurse-submodules https://github.com/mcu-tools/mcuboot.git $(MCUBOOT_BASE_DIR); \
		cd $(MCUBOOT_BASE_DIR) && git checkout e512181 && git submodule update --recursive; \
		cp $(COMMON_DIR)/src/loader.c $(MCUBOOT_BASE_DIR)/boot/bootutil/src/loader.c; \
	fi
	@if [ ! -d "$(OPENOCD_BASE_DIR)/.git" ]; then \
		echo "Cloning OpenOCD..."; \
		git clone https://github.com/raspberrypi/openocd.git $(OPENOCD_BASE_DIR); \
		cd $(OPENOCD_BASE_DIR) && ./bootstrap; \
		cd $(OPENOCD_BASE_DIR) && ./configure --enable-cmsis-dap --disable-werror CFLAGS="-Wno-cast-align -O2 -g"; \
		$(MAKE) -C $(OPENOCD_BASE_DIR) -j4; \
	fi

flash: all
	@echo "--- Flashing Everything ---"
	$(OPENOCD) $(OCD_FLAGS) \
		-c "init; program $(BOOT_BIN) $(BOOT_OFFSET) verify; program $(APP_SIGNED_BIN) $(APP_SLOT0_OFFSET) verify; reset; exit"

clean:
	$(MAKE) -C $(BOOTLOADER_DIR) clean
	$(MAKE) -C $(APP_DIR) clean

reset:
	@echo "--- resetting target ---"
	$(OPENOCD) $(OCD_FLAGS) \
		-c "init; reset; exit;"


openocd:
	$(OPENOCD) $(OCD_FLAGS) \
		-c "adapter speed 5000"

debug-all:
	@echo "Starting Dual-Target Debug Session..."
	$(Q)arm-none-eabi-gdb \
		-ex "set logging file gdb_session.log" \
		-ex "set logging on" \
		-ex "target extended-remote :3333" \
		-ex "monitor reset init" \
		-ex "file $(BOOTLOADER_DIR)/build/rp2350-bootloader.elf" \
		-ex "add-symbol-file $(APP_DIR)/build/rp2350-application.elf $(APP_SLOT0_OFFSET)" \
		-ex "break main" \
		-ex "break Reset_Handler" \
		-ex "continue" \
		-tui