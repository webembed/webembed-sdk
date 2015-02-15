/*
 * Part of the WebEmbed project
 * Arduino-style GPIO interrupts
 * See LICENSE for details
 */
#include "gpioint.h"
void (*_gpio_handler)() = NULL;
int _interrupt_pin = -1;

LOCAL void gpioint_handler(uint8_t val) {
	ETS_GPIO_INTR_DISABLE();

	if(_interrupt_pin != -1) {
		if(_gpio_handler != NULL) {
			(*_gpio_handler)();
		}
	}
	//clear interrupt status
    uint32 gpio_status;
    gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);
    ETS_GPIO_INTR_ENABLE();

}

void attachInterrupt(uint8 pin, void(*isr)(), int type) {
	if(!isValidPin(pin)) return;
	if(pin == 16) return;
	if(_interrupt_pin != -1) {
		detachInterrupt(_interrupt_pin);
	}
	ETS_GPIO_INTR_DISABLE();

	GPIO_INT_TYPE intrType;
    switch(type) {
    case LOW:
    	intrType = GPIO_PIN_INTR_LOLEVEL;
    	break;
    case CHANGE:
        intrType = GPIO_PIN_INTR_ANYEGDE;
        break;
    case RISING:
    	intrType = GPIO_PIN_INTR_POSEDGE;
    	break;
    case FALLING:
    	intrType = GPIO_PIN_INTR_NEGEDGE;
    	break;
    case HIGH_STATE:
    	intrType = GPIO_PIN_INTR_HILEVEL;
    	break;
    default:
    	intrType = GPIO_PIN_INTR_DISABLE;
    	break;
    }

    _interrupt_pin = pin;
    _gpio_handler = isr;

    gpio_pin_intr_state_set(GPIO_ID_PIN(pin), intrType);

	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(pin));

    ETS_GPIO_INTR_ENABLE();

}

void detachInterrupt(uint8 pin) {
	if(!isValidPin(pin)) return;
	if(pin == 16) return;
	ETS_GPIO_INTR_DISABLE();
	gpio_pin_intr_state_set(GPIO_ID_PIN(pin),GPIO_PIN_INTR_DISABLE);
	if(pin == _interrupt_pin) {
		_interrupt_pin = -1;
		_gpio_handler = NULL;
	}
    ETS_GPIO_INTR_ENABLE();

}

void setupInterrupts() {
	ETS_GPIO_INTR_ATTACH(gpioint_handler, 0);
}

