#pragma once
// Define base address and offsets for GPIO peripherals (check datasheet for exact values)
#define GPIO_BASE 0x40028000 // Example base address
#define GPIO25_STATUS (GPIO_BASE + 0x0c8)
#define GPIO25_CTRL (GPIO_BASE + 0x0cc)

// PAD control
#ifndef PADS_BANK0_BASE
#define PADS_BANK0_BASE 0x40038000
#endif
#define PADS_BANK0_GPIO25_CTRL (PADS_BANK0_BASE + 0x68)

// SIO control
#ifndef SIO_BASE
#define SIO_BASE 0xd0000000
#endif
#define GPIO_OUT_SET (SIO_BASE + 0x018)
#define GPIO_OUT_CLR (SIO_BASE + 0x020)
#define GPIO_OE_SET (SIO_BASE + 0x038)

// Define your LED pin (e.g., GPIO 21)
#define LED_PIN 25