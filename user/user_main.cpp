/*
	The hello world c++ demo
*/

#include <ets_sys.h>
#include <osapi.h>
#include <os_type.h>
#include <gpio.h>
#include <vector>
#include "routines.h"
#include "Arduino.h"
#define DELAY 1000 /* milliseconds */

// =============================================================================================
// C includes and declarations
// =============================================================================================
extern "C"
{
#include "driver/uart.h"
#include "driver/pwm.h"


// declare lib methods
extern int ets_uart_printf(const char *fmt, ...);
void ets_timer_disarm(ETSTimer *ptimer);
void ets_timer_setfn(ETSTimer *ptimer, ETSTimerFunc *pfunction, void *parg);
void ets_timer_arm_new(ETSTimer *ptimer,uint32_t milliseconds, bool repeat_flag, bool);

}//extern "C"


// =============================================================================================
// Example class. It show that the global objects shall be initialyzed by user
// (see user_init method)
// =============================================================================================

class A
{
public:
	A()
	: a(5)
	, b(6)
   , vec()
	{
      vec.push_back( 7 );
	}

	A( int k, int m )
	: a(k)
	, b(m)
	, vec()
	{
      vec.push_back( 10 );
	}

	void print()
	{
	   if ( 0 < vec.size() )
	   {
	      ets_uart_printf( "a = %d, b = %d, vec[0]\n", a, b, vec[0] );
	   }
	   else
	   {
         ets_uart_printf( "a = %d, b = %d, vec is empty\n", a, b );
	   }
	}

private:
	int a;
   int b;
   std::vector< int > vec;
};

// =============================================================================================
// global objects
// =============================================================================================

LOCAL os_timer_t hello_timer;



// =============================================================================================
// User code
// =============================================================================================
int value, intrCount;
LOCAL void ICACHE_FLASH_ATTR hello_cb(void *arg)
{
/*	static int counter = 0;
	ets_uart_printf("Hello World #%d val=%d!\r\n", counter++, value);
	analogWrite(2,value);
	value = (value + 10) % 1024;*/
	ets_uart_printf("Interrupt count: %d\n",intrCount);
}


A a;

void interruptHandler() {
	intrCount++;
}

extern "C" void user_init(void)
{
	do_global_ctors();
	setupInterrupts();
	pinMode(0,INPUT_PULLUP);
	pinMode(2,OUTPUT);
	digitalWrite(2, HIGH);
	value = 0;
	intrCount = 0;
	// Configure the UART
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
	a.print();
	// Set up a timer to send the message
	// os_timer_disarm(ETSTimer *ptimer)
	os_timer_disarm(&hello_timer);
	// os_timer_setfn(ETSTimer *ptimer, ETSTimerFunc *pfunction, void *parg)
	os_timer_setfn(&hello_timer, (os_timer_func_t *)hello_cb, (void *)0);
	// void os_timer_arm(ETSTimer *ptimer,uint32_t milliseconds, bool repeat_flag)
	os_timer_arm(&hello_timer, DELAY, 1);
//	pwm_init(1000,0);
	setupInterrupts();
	attachInterrupt(0, interruptHandler, FALLING);

}
