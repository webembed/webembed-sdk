/*
 * Part of the WebEmbed project
 * User-friendly API for various ESP8266 functions
 * See LICENSE for details
 */
#include "WiFi.h"
#include <ESPAPI.h>
#include "driver/uart.h"
void ICACHE_FLASH_ATTR CreateTimer(Timer *timer,  void(*timerFunc)()) {
	os_timer_disarm(timer);
	os_timer_setfn(timer, (os_timer_func_t*)timerFunc, NULL);
}

void ICACHE_FLASH_ATTR EnableDebugMessages() {
	os_install_putc1((void*)uart0_putc);
}

void ICACHE_FLASH_ATTR DisableDebugMessages() {
	os_install_putc1(NULL);
}
