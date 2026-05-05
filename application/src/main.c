#include <stdio.h>

#include "cmsis_shim.h"
#include "hal/uart.h"
#include "hal/logging.h"
#include "hal/gpio.h"

#include "shell_port.h"

#include "bootutil/bootutil.h"
#include "bootutil/image.h"


#include <string.h>

//! A very naive implementation of the newlib _sbrk dependency function
void* _sbrk(int incr);
void* _sbrk(int incr) {
  static uint32_t s_index = 0;
  static uint8_t s_newlib_heap[2048] __attribute__((aligned(8)));

  if ((s_index + (uint32_t)incr) <= sizeof(s_newlib_heap)) {
    EXAMPLE_LOG("Out of Memory!");
    return 0;
  }

  void* result = (void*)&s_newlib_heap[s_index];
  s_index += (uint32_t)incr;
  return result;
}

void led_init(void)
{
    *(unsigned int *)(GPIO25_CTRL) = 0x05;
    *(unsigned int *)(PADS_BANK0_GPIO25_CTRL) = 0x34;
    *(unsigned int *)(GPIO_OE_SET) = 0x01U << 25;

}

int app_main(void) {
  
  EXAMPLE_LOG("==configuring uart rx ==");
  configure_rx();
  // succesfully completed init, mark image as stable
  boot_set_confirmed();
  EXAMPLE_LOG("==configuring led ==");
  led_init();

  EXAMPLE_LOG("==Main Application Booted==");
  shell_processing_loop();

  __builtin_unreachable();
}
