/*
 * Part of the WebEmbed project
 * User-friendly API for various ESP8266 functions
 * See LICENSE for details
 */
#ifndef ESPAPI_H

#define ESPAPI_H
#include <stdlib.h>
#include <c_types.h>


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

//Needed to enable use of os_printf
int os_printf_plus(const char *format, ...)  __attribute__ ((format (printf, 1, 2)));


#ifdef __cplusplus
}
#endif

#endif
