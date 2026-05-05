/* rpi pico2 rp2350 Single-Slot Layout */
MEMORY
{
  BOOTLOADER (rx) : ORIGIN = 0x10000000, LENGTH = 256K

  /* Start exactly where the bootloader ends */
  SLOT_0 (rx)     : ORIGIN = 0x10040100, LENGTH = 3840K

  SRAM (rwx)      : ORIGIN = 0x20000000, LENGTH = 512K
}
