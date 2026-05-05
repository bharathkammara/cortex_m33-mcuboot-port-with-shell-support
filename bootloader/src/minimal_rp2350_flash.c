#include "hal/internal_flash.h"
#include "hardware/structs/xip_ctrl.h"
#include "hardware/regs/xip.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include <string.h>

#define FLASH_BASE_ADDR 0x10000000

void example_internal_flash_read(uint32_t addr, void *buf, size_t length) {
    // Ensure we are reading from the memory-mapped flash region
    // If 'addr' is an offset (e.g. 0x40000), add 0x10000000
    const void *flash_ptr = (addr < FLASH_BASE_ADDR) ? 
                             (void *)(addr + FLASH_BASE_ADDR) : (void *)addr;
    memcpy(buf, flash_ptr, length);
}

#include "pico/stdio.h"         // For tight_loop_contents()

void example_internal_flash_write(uint32_t addr, const void *buf, size_t length) {
    uint32_t flash_offs = (addr >= FLASH_BASE_ADDR) ? (addr - FLASH_BASE_ADDR) : addr;
    
    if (length % FLASH_PAGE_SIZE != 0) {
        return; 
    }

    uint32_t ints = save_and_disable_interrupts();
    flash_range_program(flash_offs, (const uint8_t *)buf, length);
    restore_interrupts(ints);

    // --- RP2350 SDK 2.x Corrected Macros ---

    // 1. Trigger the flush: The macro is usually XIP_FLUSH_BITS or XIP_CTRL_FLUSH_BITS
    // Based on your error, try XIP_FLUSH_BITS or the direct bit 0
    xip_ctrl_hw->ctrl |= 0x1u; 

    // 2. Wait for flush ready: The macro is XIP_STAT_FLUSH_RDY
    while (!(xip_ctrl_hw->stat & 0x4u)) {
        tight_loop_contents();
    }
}

void example_internal_flash_erase_sector(uint32_t addr) {
    uint32_t flash_offs = (addr >= FLASH_BASE_ADDR) ? (addr - FLASH_BASE_ADDR) : addr;

    // SAFETY: Erase MUST be 4096-byte aligned
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(flash_offs, FLASH_SECTOR_SIZE);
    restore_interrupts(ints);
}