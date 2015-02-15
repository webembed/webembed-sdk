/*
 * Part of the WebEmbed project
 * Miscellaneous functions for Arduino compatibility
 * See LICENSE for details
 */

#include "Arduino.h"

#include <os_type.h>
#include <limits.h>

unsigned long micros() {
	return system_get_time();
}

unsigned long long int longMicros = 0;
unsigned long lastSystemTime = 0;

unsigned ICACHE_FLASH_ATTR long millis() {
	return longMicros / 1000;
}

os_timer_t milliTimer;
LOCAL void incrMillis() {
	unsigned long mi = micros();
	if(mi < lastSystemTime) {
		longMicros += ((ULONG_MAX - lastSystemTime) + mi);
	} else {
		longMicros += (mi - lastSystemTime);
	}
	lastSystemTime = mi;
}

void ICACHE_FLASH_ATTR init() {
	setupInterrupts();
	os_timer_disarm(&milliTimer);
	os_timer_setfn(&milliTimer, (os_timer_func_t *)incrMillis, NULL);
	os_timer_arm_us(&milliTimer, 1, 1);
}
