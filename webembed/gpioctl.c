/*
 * Part of the WebEmbed project
 * Arduino-style GPIO interface
 * See LICENSE for details
 */
#include "Arduino.h"
#include "gpioint.h"
#include "gpio.h"
#include "driver/uart.h"
#include "driver/gpio16.h"
#include "pin_map.h"
#include "driver/pwm.h"
#include "ets_sys.h"

void ICACHE_FLASH_ATTR pinMode(uint8_t pin, uint8_t mode) {
	if(!isValidPin(pin)) return;
	if(pin == 16) {
		if(mode == OUTPUT) {
			gpio16_output_conf();
		} else {
			gpio16_input_conf();
		}
	} else {
	    pwm_delete(pin);
	    pwm_start();
	}

	PIN_PULLDWN_DIS(pin_mux[pin]);
	PIN_PULLUP_DIS(pin_mux[pin]);

	//Configure pull resistor setting
	if(mode == INPUT_PULLDOWN)
		PIN_PULLDWN_EN(pin_mux[pin]);
	if(mode == INPUT_PULLUP)
		PIN_PULLUP_EN(pin_mux[pin]);

    if(mode == OUTPUT) {
        GPIO_OUTPUT_SET(pin,0);
    } else {
        GPIO_DIS_OUTPUT(pin);
    }
    PIN_FUNC_SELECT(pin_mux[pin], pin_func[pin]);

    GPIO_REG_WRITE(GPIO_PIN_ADDR(GPIO_ID_PIN(pin)), GPIO_REG_READ(GPIO_PIN_ADDR(GPIO_ID_PIN(pin))) & (~ GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_ENABLE))); //disable open drain;

}

int ICACHE_FLASH_ATTR digitalRead(uint8_t pin) {
	if(!isValidPin(pin)) return LOW;
    if(pin == 16)
    	return 0x1 & gpio16_input_get();
    return 0x1 & GPIO_INPUT_GET(GPIO_ID_PIN(pin));
}

void ICACHE_FLASH_ATTR digitalWrite(uint8_t pin, uint8_t value) {
	if(!isValidPin(pin)) return;
    if(pin == 16)
    	gpio16_output_set(value);
    GPIO_OUTPUT_SET(GPIO_ID_PIN(pin), value);
}

int ICACHE_FLASH_ATTR analogRead() {
	return system_adc_read();
}

void ICACHE_FLASH_ATTR analogWrite(uint8_t pin, int value) {
	if(!pwm_exist(pin)) {
		if(pwm_add(pin)) {
			pwm_set_freq(1000,0);
			pwm_start();
		} else {
			//ets_uart_printf("pwm fail\n");

		}
	} else {
		//ets_uart_printf("pwm ok\n");
	}
	pwm_set_duty(value, pin);
	pwm_start();
}
unsigned ICACHE_FLASH_ATTR long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout) {
	if(!isValidPin(pin)) return 0;
    if(pin == 16) return 0; //pin 16 not supported for this function
    unsigned long waitCount = 0;
    while((0x1 & GPIO_INPUT_GET(GPIO_ID_PIN(pin))) != state) {
    	delayMicroseconds(1);
    	waitCount++;
    	if(waitCount > timeout) return 0;
    }
    unsigned long pulseLength = 0;
    while((0x1 & GPIO_INPUT_GET(GPIO_ID_PIN(pin))) == state) {
    	delayMicroseconds(1);
    	pulseLength++;
    }
    return pulseLength;
}
void ICACHE_FLASH_ATTR shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val) {
	if(!isValidPin(dataPin)) return;
    if(dataPin == 16) return; //pin 16 not supported for this function
	if(!isValidPin(clockPin)) return;
    if(clockPin == 16) return; //pin 16 not supported for this function
    int i;
    for(i = 0; i < 8; i++) {
    	int bit;
        if(bitOrder == MSBFIRST) {
        	bit = 7 - i;
        } else {
        	bit = i;
        }
        GPIO_OUTPUT_SET(GPIO_ID_PIN(dataPin),bitRead(val,bit));
        delayMicroseconds(1);
        GPIO_OUTPUT_SET(GPIO_ID_PIN(clockPin),HIGH);
        delayMicroseconds(2);
        GPIO_OUTPUT_SET(GPIO_ID_PIN(clockPin),LOW);
        delayMicroseconds(1);

    }
}

uint8_t ICACHE_FLASH_ATTR shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
	if(!isValidPin(dataPin)) return 0;
    if(dataPin == 16) return 0; //pin 16 not supported for this function
	if(!isValidPin(clockPin)) return 0;
    if(clockPin == 16) return 0; //pin 16 not supported for this function
    int i;
    int val;
    for(i = 0; i < 8; i++) {
    	int bit;
        if(bitOrder == MSBFIRST) {
        	bit = 7 - i;
        } else {
        	bit = i;
        }
        delayMicroseconds(1);
        GPIO_OUTPUT_SET(GPIO_ID_PIN(clockPin),HIGH);
        delayMicroseconds(1);
        bitWrite(val, bit, 0x1 & GPIO_INPUT_GET(GPIO_ID_PIN(dataPin)));
        delayMicroseconds(1);
        GPIO_OUTPUT_SET(GPIO_ID_PIN(clockPin),LOW);
        delayMicroseconds(1);

    }
    return val;
}

