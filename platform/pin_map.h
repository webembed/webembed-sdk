
#ifndef __PIN_MAP_H__
#define __PIN_MAP_H__

#include "c_types.h"
#include "user_config.h"
#include "gpio.h"
#include "ets_sys.h"

#define GPIO_PIN_NUM 17

#define UNDEFINED_PIN 0

//extern uint8_t pin_num[GPIO_PIN_NUM];
extern uint8_t pin_func[GPIO_PIN_NUM];
extern uint32_t pin_mux[GPIO_PIN_NUM];
#if/*def GPIO_INTERRUPT_ENABLE*/ 0
extern GPIO_INT_TYPE pin_int_type[GPIO_PIN_NUM];
#endif

bool isValidPin(uint8_t pin);

#endif // #ifndef __PIN_MAP_H__
