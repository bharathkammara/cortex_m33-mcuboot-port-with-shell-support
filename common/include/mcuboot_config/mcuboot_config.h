/* Single Slot Strategy */
#define MCUBOOT_IMAGE_NUMBER 1
#define MCUBOOT_DIRECT_XIP       // Runs code directly from Flash (Standard for RP2350)
#define MCUBOOT_SINGLE_SLOT      // Removes the need for a "Secondary" slot

/* Verification Settings */
#define MCUBOOT_VALIDATE_PRIMARY_SLOT  // Forces check every single boot
#define MCUBOOT_SIGN_EC256         // Use RSA (or MCUBOOT_SIGN_EC256 for Elliptic Curve)
#define MCUBOOT_USE_TINYCRYPT          // Recommended lightweight crypto library

#define MCUBOOT_MAX_IMG_SECTORS 1024
#define MCUBOOT_FLASH_WRITE_BLOCK_SIZE 256
#define MCUBOOT_VERIFY_IMG_ADDRESS

#undef BOOT_NUM_SLOTS
#define BOOT_NUM_SLOTS 1
#define CONF_MCUBOOT_HEADER_SIZE 256
#define FLASH_SECTOR_SIZE 4096
#define MCUBOOT_HAVE_LOGGING 1