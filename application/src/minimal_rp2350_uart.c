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
#include "hardware/irq.h"
#include "hardware/resets.h"

#include "hardware/regs/addressmap.h"
#include "hardware/regs/m33.h"

#include "hal/logging.h"
#include "hal/gpio.h"

void uart_byte_received_from_isr_cb(char c);

#define UART_ID uart0
#define BAUD_RATE 115200

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1

void on_uart_rx() {
    *(unsigned int *)(GPIO_OUT_SET) = 0x01U << 25;
    while(uart_is_readable(UART_ID)) {
        uint8_t ch = uart_getc(UART_ID);
        uart_byte_received_from_isr_cb(ch);
    }
    uart_get_hw(uart0)->icr = 0x7FF;
    *(unsigned int *)(GPIO_OUT_CLR) = 0x01U << 25;
}


void uart_boot(void) {
  // Set the TX and RX pins by using the function select on the GPIO
  // Set datasheet for more information on function select
  gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_TX_PIN));
  gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_RX_PIN));

  // Set up our UART with a basic baud rate.
  xosc_init();
  
  clock_configure(clk_peri, 0, 
                CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_XOSC_CLKSRC, 
                12 * MHZ, 12 * MHZ);
  uart_init(UART_ID, BAUD_RATE);
  uart_set_fifo_enabled(UART_ID, false);
  uart_get_hw(uart0)->imsc = (UART_UARTIMSC_RXIM_BITS | UART_UARTIMSC_OEIM_BITS);
  
}

void configure_rx(void)
{
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
    EXAMPLE_LOG("Configuring uart rx irq:%d", UART_IRQ);
    irq_set_enabled(UART_IRQ, true);
    uart_set_irq_enables(UART_ID, true, false);
}

void uart_tx_blocking(uart_inst_t *uart, const uint8_t *src, size_t len) {
    for (size_t i = 0; i < len; i++) {
        // This function waits for the hardware FIFO to have space
        // then pushes the character into the DR (Data Register)
        uart_putc_raw(uart0, src[i]);
    }
}


