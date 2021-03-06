/*
 * Part of the WebEmbed project
 * User-friendly API for various ESP8266 functions
 * See LICENSE for details
 */
#ifndef ESPAPI_H

#define ESPAPI_H
#include <stdlib.h>
#include <c_types.h>
#include <misc_includes.h>

#ifdef __cplusplus
extern "C"{
#endif

#include <osapi.h>
#include <ets_sys.h>
#include <user_interface.h>

typedef os_timer_t Timer;

//Create a new timer. Pass a reference to a timer struct and the timer function to call
void CreateTimer(Timer *timer,  void(*timerFunc)());

#define TIMER_ONCE 0
#define TIMER_REPEAT 1
//Starts a timer, given a timer object (see CreateTimer), an interval in ms and whether or not to repeat
#define StartTimer os_timer_arm

//Stops a timer, given a timer object
#define StopTimer os_timer_disarm

//Print debug messages to serial console
void EnableDebugMessages();

//Stop printing debug messages to serial console
void DisableDebugMessages();



//Enter deep sleep mode for a given number of seconds (max 35 mins)
//If time is set to zero, then the ESP8266 will only wake if there is a falling edge on the RST pin
//To wake after a given amount of time, GPIO16 and RESET must be tied together
#define DeepSleep(t) system_deep_sleep((uint32)(t*1000000))

//Gets the unique chip ID
#define GetChipID() system_get_chip_id()

//Gets microseconds from RTC (counter continues during deep sleep)
#define microsRTC() (system_get_rtc_time()*(system_rtc_clock_cali_proc()/4096.0))

//Gets milliseconds from RTC (counter continues during deep sleep
#define millisRTC() (system_get_rtc_time()*(system_rtc_clock_cali_proc()/4096.0)/1000)

//User RTC memory can be used to store variables during deep sleep
//This function stores a 32-bit variable in RTC memory. Address refers to a 32-bit block and goes from 0 to 127
void StoreRTC(uint32 addr, uint32 data);

//This function fetches a 32-bit variable from RTC memory. Address refers to a 32-bit block and goes from 0 to 127
uint32 LoadRTC(uint32 addr);

//Reboot chip
#define Reset system_restart

//Reset all settings - saved wireless network, etc
#define ResetSettings system_restore



#ifdef __cplusplus
}
#endif

#endif
