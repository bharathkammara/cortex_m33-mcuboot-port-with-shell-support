//! @brief
//!
//! @file
//! A minimal implementation of logging platform dependencies

#include <stdarg.h>
#include <stdio.h>

#include "hal/uart.h"
// This tells the header to generate the function code here
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

static void prv_log(const char *fmt, va_list *args) {
char log_buf[256];
    
    // 1. Format the string, leaving room for '\n' and '\0'
    // Use sizeof(log_buf) - 2 to ensure there's space for the newline
    int size = stbsp_vsnprintf(log_buf, sizeof(log_buf) - 2, fmt, *args);
    
    if (size < 0) return; // Encoding error

    // 2. Add the newline safely
    log_buf[size] = '\n';
    log_buf[size + 1] = '\0'; // Manually re-terminate the string
    
    // 3. Output via UART
    uart_puts(uart0, log_buf);
}

void example_log(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  prv_log(fmt, &args);
  va_end(args);
}
