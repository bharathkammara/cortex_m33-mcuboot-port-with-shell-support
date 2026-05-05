# --- Directories ---
ROOT_DIR        := $(CURDIR)
BOOTLOADER_DIR  := $(ROOT_DIR)/bootloader
APP_DIR         := $(ROOT_DIR)/application

# --- Flash Map (Must match your MCUboot config) ---
# RP2350 Flash starts at 0x10000000
BOOT_OFFSET     := 0x10000000
APP_SLOT0_OFFSET := 0x10040000  # 256KB Offset

# --- Binaries ---
BOOT_BIN        := $(BOOTLOADER_DIR)/build/rp2350-bootloader.bin
APP_SIGNED_BIN  := $(APP_DIR)/build/rp2350-application.bin

# --- OpenOCD Configuration ---
OPENOCD_DIR     := $(ROOT_DIR)/openocd
OPENOCD         := $(OPENOCD_DIR)/src/openocd
OCD_INTERFACE   := $(OPENOCD_DIR)/tcl/interface/cmsis-dap.cfg
OCD_TARGET      := $(OPENOCD_DIR)/tcl/target/rp2350.cfg
OCD_FLAGS       := -f $(OCD_INTERFACE) -f $(OCD_TARGET) -c "adapter speed 1000; after 1000"

# --- Targets ---

.PHONY: all bootloader app flash flash-app clean openocd

all: bootloader app

# 1. Build the Bootloader
bootloader:
	@echo "--- Building RP2350 Bootloader ---"
	$(MAKE) -C $(BOOTLOADER_DIR)

# 2. Build and Sign the Application
app:
	@echo "--- Building and Signing Application ---"
	$(MAKE) -C $(APP_DIR)

# 3. Flash Everything (Full System)
flash: all
	@echo "--- Flashing Bootloader and Signed App ---"
	$(OPENOCD) $(OCD_FLAGS) \
		-c "program $(BOOT_BIN) $(BOOT_OFFSET) verify;" \
 		-c "program $(APP_SIGNED_BIN) $(APP_SLOT0_OFFSET) verify;" \
		-c "reset; exit"

# 4. Flash Application Only (Faster for iterative debugging)
flash-app: app
	@echo "--- Flashing Signed App to Slot 0 ---"
	$(OPENOCD) $(OCD_FLAGS) \
		-c "program $(APP_SIGNED_BIN) $(APP_SLOT0_OFFSET) verify;" \
		-c "reset; exit"

clean:
	@echo "--- Cleaning All Projects ---"
	$(MAKE) -C $(BOOTLOADER_DIR) clean
	$(MAKE) -C $(APP_DIR) clean

reset:
	@echo "--- resetting target ---"
	$(OPENOCD) $(OCD_FLAGS) \
		-c "init; reset; exit;"

openocd:
	$(OPENOCD) \
		-f $(OPENOCD_DIR)/tcl/interface/cmsis-dap.cfg \
		-f $(OPENOCD_DIR)/tcl/target/rp2350.cfg \
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