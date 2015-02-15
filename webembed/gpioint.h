/*
 * Part of the WebEmbed project
 * Arduino-style GPIO interrupts
 * See LICENSE for details
 */

#ifndef GPIOINT_h

#define GPIOINT_h
#include <stdlib.h>
#include <c_types.h>
#include "gpioctl.h"
#include "pin_map.h"
#ifdef __cplusplus
extern "C"{
#endif

#define CHANGE 1
#define RISING 2
#define FALLING 3
#define HIGH_STATE 4
void attachInterrupt(uint8 pin, void(*isr)(), int type);
void detachInterrupt(uint8 pin);
void setupInterrupts();
extern int _interrupt_pin;
extern void (*_gpio_handler)();
#ifdef __cplusplus
}
#endif

#endif
