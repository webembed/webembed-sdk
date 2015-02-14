/*
 * Part of the WebEmbed project
 * Arduino-style GPIO interface
 * See LICENSE for details
 */

#ifndef Arduino_h

#define Arduino_h
#include <stdlib.h>
#include <c_types.h>
#ifdef __cplusplus
extern "C"{
#endif

#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define INPUT_PULLDOWN 3

#define HIGH 1
#define LOW 0

void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t value);
int digitalRead(uint8_t pin);
int analogRead();
void analogWrite(uint8_t pin, int duty);

#ifdef __cplusplus
}
#endif

#endif
