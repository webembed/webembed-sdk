/*
 * Part of the WebEmbed project
 * Arduino-style GPIO interface
 * See LICENSE for details
 */
#include "gpioctl.h"
#include "gpio.h"
#include "driver/uart.h"
#include "driver/gpio16.h"
#include "pin_map.h"
#include "driver/pwm.h"
#include "ets_sys.h"

void pinMode(uint8_t pin, uint8_t mode) {
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

	ETS_GPIO_INTR_DISABLE();

    if(mode == OUTPUT) {
        GPIO_OUTPUT_SET(pin,0);
    } else {
        GPIO_DIS_OUTPUT(pin);
    }
    PIN_FUNC_SELECT(pin_mux[pin], pin_func[pin]);
	//disable interrupt
	gpio_pin_intr_state_set(GPIO_ID_PIN(pin), GPIO_PIN_INTR_DISABLE);
	 //clear interrupt status
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(pin));
    GPIO_REG_WRITE(GPIO_PIN_ADDR(GPIO_ID_PIN(pin)), GPIO_REG_READ(GPIO_PIN_ADDR(GPIO_ID_PIN(pin))) & (~ GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_ENABLE))); //disable open drain;
	#ifdef GPIO_INTERRUPT_ENABLE
		pin_int_type[pin] = GPIO_PIN_INTR_DISABLE;
	#endif

    ETS_GPIO_INTR_ENABLE();
}

int digitalRead(uint8_t pin) {
	if(!isValidPin(pin)) return LOW;
    if(pin == 16)
    	return 0x1 & gpio16_input_get();
    return 0x1 & GPIO_INPUT_GET(GPIO_ID_PIN(pin));
}

void digitalWrite(uint8_t pin, uint8_t value) {
	if(!isValidPin(pin)) return;
    if(pin == 16)
    	gpio16_output_set(value);
    GPIO_OUTPUT_SET(GPIO_ID_PIN(pin), value);
}

int analogRead() {
	return system_adc_read();
}

void analogWrite(uint8_t pin, int value) {
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
