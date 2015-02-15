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

void ICACHE_FLASH_ATTR StoreRTC(uint32 addr, uint32 data) {
	system_rtc_mem_write(addr+64,&data,4);
}

uint32 ICACHE_FLASH_ATTR LoadRTC(uint32 addr) {
	uint32 data;
	system_rtc_mem_read(addr+64,&data,4);
	return data;
}
