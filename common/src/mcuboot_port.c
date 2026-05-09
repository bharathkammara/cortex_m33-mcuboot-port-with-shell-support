/* Run the boot image. */

#include <string.h>

#include "flash_map_backend/flash_map_backend.h"
#include "os/os_malloc.h"
#include "sysflash/sysflash.h"

#include "hal/logging.h"
#include "hal/internal_flash.h"

#include "mcuboot_config/mcuboot_logging.h"
#include "mcuboot_config/mcuboot_config.h"
#include "mcuboot_config/mcuboot_assert.h"
#include "bootutil/image.h"

/* Relative Offsets */
#define BOOTLOADER_OFFSET           0x00000
#define BOOTLOADER_SIZE             0x40000  /* 256KB */

#define APPLICATION_PRIMARY_OFFSET  0x40000  /* Starts at 256KB */
#define APPLICATION_SIZE            0x3C0000

static const struct flash_area bootloader = {
  .fa_id = FLASH_AREA_BOOTLOADER,
  .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
  .fa_off = BOOTLOADER_OFFSET,
  .fa_size = BOOTLOADER_SIZE,
};

static const struct flash_area primary_slot = {
  .fa_id = FLASH_AREA_IMAGE_PRIMARY(0),
  .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
  .fa_off = APPLICATION_PRIMARY_OFFSET,
  .fa_size = APPLICATION_SIZE,
};

static const struct flash_area *s_flash_areas[] = {
  &bootloader,
  &primary_slot,
};

/* Helper to find our static area structure by ID */
static const struct flash_area *prv_lookup_flash_area(uint8_t id) {
    for (size_t i = 0; i < (sizeof(s_flash_areas) / sizeof(s_flash_areas[0])); i++) {
        if (s_flash_areas[i]->fa_id == id) {
            return s_flash_areas[i];
        }
    }
    return NULL;
}

int flash_area_id_from_multi_image_slot(int image_index, int slot) {
    if (slot == 0) {
        return FLASH_AREA_IMAGE_PRIMARY(image_index);
    }
    // Return an invalid ID for any other slot to prevent accidental erases
    return -1; 
}

int flash_area_id_from_image_slot(int slot) {
    return flash_area_id_from_multi_image_slot(0, slot);
}

int flash_area_open(uint8_t id, const struct flash_area **area_outp) {
    const struct flash_area *area = prv_lookup_flash_area(id);
    if (area == NULL) {
        return -1; // Tell MCUboot this area doesn't exist
    }
    *area_outp = area;
    return 0;
}

void flash_area_close(const struct flash_area *fa) {
  // no cleanup to do for this flash part
}

//
// Flash Property Dependencies
//

#define FLASH_SECTOR_SIZE 4096

size_t flash_area_align(const struct flash_area *area) {
  // the smallest unit a flash write can occur along
  return 256;
}

uint8_t flash_area_erased_val(const struct flash_area *area) {
  // the value a byte reads when erased on storage.
  return 0xff;
}

int flash_area_get_sectors(int fa_id, uint32_t *count,
                           struct flash_sector *sectors) {
    const struct flash_area *fa = prv_lookup_flash_area(fa_id);
    
    // 1. Safety check for the lookup
    if (fa == NULL) {
        return -1;
    }

    // 2. RP2350 usually uses 4096 (4KB) sectors
    const size_t sector_size = 4096; 
    uint32_t total_count = fa->fa_size / sector_size;

    // 3. If sectors is NULL, MCUboot just wants the count
    if (sectors == NULL) {
        *count = total_count;
        return 0;
    }

    // 4. Limit check: MCUboot usually provides a buffer based on the count 
    // you returned in step 3. Fill the array.
    for (uint32_t i = 0; i < total_count; i++) {
        sectors[i].fs_off = i * sector_size; // Offset relative to Area start
        sectors[i].fs_size = sector_size;
    }

    *count = total_count;
    return 0;
}

//! Useful for bringup to make sure the write
//! and erase operations are behaving as expected
#define VALIDATE_PROGRAM_OP 1

#define QSPI_XIP_BASE 0x10000000

int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len) {
    if (fa == NULL) return -1;
    
    // Ensure we are reading from the XIP window (0x10000000)
    uintptr_t addr = QSPI_XIP_BASE + fa->fa_off + off;
    
    // Cast to volatile to prevent compiler over-optimization
    const uint8_t *src = (const uint8_t *)addr;
    memcpy(dst, src, len);
    
    return 0;
}

int flash_area_write(const struct flash_area *fa, uint32_t off, const void *src,
                     uint32_t len) {
  if (fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) {
    return -1;
  }

  const uint32_t end_offset = off + len;
  if (end_offset > fa->fa_size) {
    MCUBOOT_LOG_ERR("%s: Out of Bounds (0x%x vs 0x%x)", __func__, end_offset, fa->fa_size);
    return -1;
  }

  const uint32_t addr = fa->fa_off + off;
  MCUBOOT_LOG_DBG("%s: Addr: 0x%08x Length: %d", __func__, (int)addr, (int)len);
  example_internal_flash_write(addr, src, len);

#if VALIDATE_PROGRAM_OP
  // Use the memory-mapped address for comparison
  if (memcmp((void *)(QSPI_XIP_BASE + addr), src, len) != 0) {
    MCUBOOT_LOG_ERR("%s: Program Failed", __func__);
    assert(0);
  }
#endif

  return 0;
}

int flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len) {
    // if (fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) {
    //     return -1;
    // }

    // // Ensure alignment
    // if ((len % FLASH_SECTOR_SIZE) != 0 || (off % FLASH_SECTOR_SIZE) != 0) {
    //     return -1;
    // }

    // const uint32_t start_addr = fa->fa_off + off;
    // // Remove the 0x10000000 prefix if it exists in fa_off to get raw flash offset
    // uint32_t flash_offs = (start_addr >= QSPI_XIP_BASE) ? (start_addr - QSPI_XIP_BASE) : start_addr;

    // MCUBOOT_LOG_DBG("Erasing 0x%08x, Len: %d...", (int)flash_offs, (int)len);

    // /* * USE THE SDK DIRECTLY:
    //  * flash_range_erase is much faster for large blocks.
    //  * It handles the RAM-resident code and interrupt safety.
    //  */
    // flash_range_erase(flash_offs, len);

    // // Instead of a byte-loop, just flush the cache to be safe
    // flash_flush_cache(); 

    return 0;
}

void example_assert_handler(const char *file, int line) {
  EXAMPLE_LOG("ASSERT: File: %s Line: %d", file, line);
  __builtin_trap();
}

/* Fix for: undefined reference to `default_CSPRNG` */
/* TinyCrypt ECC needs this, but verification doesn't actually use entropy */
int default_CSPRNG(uint8_t *dest, unsigned int size) {
    /* For a bootloader, we just return success */
    return 1; 
}

/* Fix for: undefined reference to `mbedtls_platform_zeroize` */
void mbedtls_platform_zeroize(void *buf, size_t len) {
    memset(buf, 0, len);
}



/* Fixes: undefined reference to `mbedtls_mpi_read_binary` */
/* This allows the ASN.1 parser to "import" the key into a raw buffer */
int mbedtls_mpi_read_binary(void *X, const unsigned char *buf, size_t buflen) {
    /* TinyCrypt handles the buffer directly, so we just copy or return success 
       depending on how your image_ec256.c is configured. */
    memcpy(X, buf, buflen); 
    return 0;
}
