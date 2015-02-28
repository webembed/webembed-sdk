/*
	The hello world c++ demo
*/

#include <ets_sys.h>
#include <osapi.h>
#include <os_type.h>
#include <gpio.h>
#include <vector>
#include <c_types.h>
#include "routines.h"
#include "Arduino.h"
#include "WiFi.h"
#include "WebServer.h"
#include "ESPAPI.h"
#include "flashfs.h"

#define DELAY 3000 /* milliseconds */

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
static const char *header = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";
static const char *pagePartA = "<html><body><h1>Hello World!</h1>";
static const char *pagePartB = "<p><a href='/up'>INC</a> <a href='/down'>DEC</a></p></body></html>";

int pwmValue = 500;

int ICACHE_FLASH_ATTR CGITest(WebRequest *req, const void *arg) {
	req->sendData(header);
	req->sendData(pagePartA);
	char buf[128];
	int l = os_sprintf(buf,"<p>Current value: %d</p>",pwmValue);
	req->sendData(buf, l);

	req->sendData(pagePartB);
	return CGI_DONE;
}


static const char *redir = "HTTP/1.0 302 Found\r\nLocation: /\r\n\r\n";
int ICACHE_FLASH_ATTR CGISet(WebRequest *req, const void *arg) {
	int a = 0;
	if(os_strcmp(req->url,"/up") == 0) a = 100;
	if(os_strcmp(req->url,"/down") == 0) a = -100;

	os_printf("set by %d\n",a);
	pwmValue += a;
	if(pwmValue >= 1024) pwmValue = 1023;
	if(pwmValue < 0) pwmValue = 0;
	analogWrite(2, (pwmValue*pwmValue)/1023);
	req->sendData(redir);
	return CGI_DONE;
}

struct FlashCGIData {
	FlashFile file;
	uint32 pos;
};

char pagebuffer[2048];
int ICACHE_FLASH_ATTR FlashCGI(WebRequest *req, const void *arg) {
	FlashCGIData *data;
	if(req->handlerData == NULL) {

		data = new FlashCGIData();
		req->handlerData = data;
		data->pos = 0;
		if(!FindFile(req->url+1,&(data->file))) {
			if(req->url[os_strlen(req->url)-1] == '/') {
				char newUrl[150];
				os_strncpy(newUrl,req->url,130);
				os_strcat(newUrl,"index.html");
				if(!FindFile(newUrl+1,&(data->file))) {
					return CGI_ERROR_NOTFOUND;
				}
			} else {
				return CGI_ERROR_NOTFOUND;
			}
		}
		req->sendData("HTTP/1.0 200 OK\r\n\r\n");
		return CGI_MORE_DATA;
	} else {
		data = (FlashCGIData *)req->handlerData;
	}

	if((data->pos + 2048) <= data->file.fileLength) {
		ReadFromFile(&(data->file),data->pos,2048,pagebuffer);
		data->pos += 2048;
		req->fastSend(pagebuffer,2048);
		os_printf("Sent max of 2048 bytes, pos=%d\n",data->pos);
		return CGI_MORE_DATA;
	} else {
		uint32 remainingData = data->file.fileLength - data->pos;
		ReadFromFile(&(data->file),data->pos,PadTo4ByteAligned(remainingData),pagebuffer);
		data->pos += remainingData;
		req->fastSend(pagebuffer,remainingData);
		os_printf("Sent %d bytes, pos=%d\n",remainingData, data->pos);
		delete data;
		return CGI_DONE;
	}
}

// =============================================================================================
// User code
// =============================================================================================
int value, intrCount;
bool messsagesDisabled = false;
WebServer server;
LOCAL void ICACHE_FLASH_ATTR hello_cb(void *arg)
{
/*	static int counter = 0;
	ets_uart_printf("Hello World #%d val=%d!\r\n", counter++, value);
	analogWrite(2,value);
	value = (value + 10) % 1024;*/
	//ets_uart_printf("Interrupt count: %d, m=%lu, u=%lu\n",intrCount,millis(),micros());

/*	uint32 rtcCount = LoadRTC(0);
	ets_uart_printf("\nrtc count:%d\n",rtcCount);
	rtcCount++;
	StoreRTC(0,rtcCount);*/

	if(GetWiFiStatus() == STATION_GOT_IP) {
		IPAddress addr = GetStationIPAddress();
		os_printf("IP addr: %d.%d.%d.%d\n",addr.parts.a,addr.parts.b,addr.parts.c,addr.parts.d);
		os_printf("Free heap memory: %d\n",system_get_free_heap_size());


	} else {
		os_printf("WiFi status: %d\n", GetWiFiStatus());
	}

}


A a;

void interruptHandler() {
	intrCount++;
}


PageHandler p = {"/",CGITest,NULL};
PageHandler pinc = {"/up",CGISet,NULL};
PageHandler pdec = {"/down",CGISet,NULL};

PageHandler f = {"*",FlashCGI,NULL};

extern "C" void user_init(void)
{
	do_global_ctors();
	init();
	pinMode(0,INPUT_PULLUP);
	pinMode(2,OUTPUT);
	digitalWrite(2, HIGH);
	value = 0;
	intrCount = 0;
	// Configure the UART
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
	EnableDebugMessages();
	a.print();
	// Set up a timer to send the message
	// os_timer_disarm(ETSTimer *ptimer)
	os_timer_disarm(&hello_timer);
	// os_timer_setfn(ETSTimer *ptimer, ETSTimerFunc *pfunction, void *parg)
	os_timer_setfn(&hello_timer, (os_timer_func_t *)hello_cb, (void *)0);
	// void os_timer_arm(ETSTimer *ptimer,uint32_t milliseconds, bool repeat_flag)
	os_timer_arm(&hello_timer, DELAY, 1);
	pwm_init(1000,0);
	analogWrite(2,pwmValue);
//	setupInterrupts();
//	attachInterrupt(0, interruptHandler, FALLING);


	EnterStationMode();
	//SetWiFiStationConfig("ASUS","XXXXXX");
	EnableAutoConnect();
	FlashFile file;
	if(GetFirstFile(&file,0x20000)) {
		do {
			os_printf("File: %s, size: %x\n",file.name,file.fileLength);
		} while(GetNextFile(&file));

	} else {
		os_printf("Failed to init flashfs\n");
	}

	server.pages.push_back(f);


	server.begin(80);
}
