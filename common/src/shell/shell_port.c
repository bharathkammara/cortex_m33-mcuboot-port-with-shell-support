//! @file
//!
//! A port of the tiny shell to a baremetal rp2350 system using the UART as a console

#include "shell_port.h"

#include <stdbool.h>
#include <stddef.h>

#include "cmsis_shim.h"
#include "hal/uart.h"
#include "shell/shell.h"

char ch;
bool flag = false;

void uart_byte_received_from_isr_cb(char c) {
  ch = c;
  flag=true;
}

static int prv_console_putc(char c) {
  uart_putc_raw(uart0, c);
  return 1;
}

void shell_processing_loop(void) {
  const sShellImpl shell_impl = {
    .send_char = prv_console_putc,
  };
  shell_boot(&shell_impl);
  while (1) {
    if (flag==true) {
        if (ch == '\r') {
            shell_receive_char('\n'); 
        } else {
            shell_receive_char(ch);
        }
        flag = false;
    }
  }
}

