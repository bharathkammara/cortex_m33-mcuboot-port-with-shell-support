//! @file
//! In the interest of brevity, an _extremely_ barebones port for the NRF52 UART Peripheral

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "cmsis_shim.h"

#include "hardware/uart.h"
#include "hardware/xosc.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"

#include "hardware/regs/addressmap.h"
#include "hardware/regs/m33.h"

void uart_byte_received_from_isr_cb(char c);

#define UART_ID uart0
#define BAUD_RATE 115200

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1

// static uint8_t s_rx_recv_buf[4];

void prv_enable_nvic(void) {
    // 1. Set the Vector Table Offset Register (VTOR) 
    // This points to the Bootloader's own vector table at the start of flash
    volatile uint32_t *vtor = (volatile uint32_t *)(PPB_BASE + M33_VTOR_OFFSET);
    *vtor = 0x10000000; 

    // 2. Clear any pending interrupts left over from the boot ROM
    for (int i = 0; i < 2; i++) {
        *((volatile uint32_t *)(PPB_BASE + M33_NVIC_ICPR0_OFFSET + (i * 4))) = 0xFFFFFFFF;
    }

    // 3. Set priority grouping (optional, but good practice for M33)
    // Application Binary Interface usually expects 0
    volatile uint32_t *aicr = (volatile uint32_t *)(PPB_BASE + M33_AIRCR_OFFSET);
    *aicr = (0x05FA << 16) | (*aicr & ~0x700); 
}

void uart_boot(void) {
    
  // Set the TX and RX pins by using the function select on the GPIO
  // Set datasheet for more information on function select
  gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_TX_PIN));
  gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_RX_PIN));

  // Set up our UART with a basic baud rate.
  xosc_init();
  for (volatile int i = 0; i < 2000; i++) __asm("nop");
  
  clock_configure(clk_peri, 0, 
                CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_XOSC_CLKSRC, 
                12 * MHZ, 12 * MHZ);
  clock_set_reported_hz(clk_peri, 12 * MHZ);
  
  uart_init(UART_ID, BAUD_RATE);  
}

void uart_tx_blocking(uart_inst_t *uart, const uint8_t *src, size_t len) {
    for (size_t i = 0; i < len; i++) {
        // This function waits for the hardware FIFO to have space
        // then pushes the character into the DR (Data Register)
        uart_putc_raw(uart, src[i]);
    }
}


