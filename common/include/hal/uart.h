#pragma once

#include <stddef.h>
#include "hardware/uart.h"

void uart_boot(void);
void configure_rx(void);
void uart_tx_blocking(uart_inst_t *uart, const uint8_t *src, size_t len);

extern void uart_byte_received_from_isr_cb(char c);
