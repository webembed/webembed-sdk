/*
 * Part of the WebEmbed project
 * User-friendly API for WiFi manipulation
 * See LICENSE for details
 */
#ifndef WIFI_H

#define WIFI_H
#include <stdlib.h>
#include <c_types.h>


#ifdef __cplusplus
extern "C"{
#endif

#include <osapi.h>
#include <ets_sys.h>
#include <ip_addr.h>
#include <user_interface.h>

//Connect to the router based on stored settings
#define WiFiConnect wifi_station_connect
//Disconnect from a router
#define WiFiDisconnect wifi_station_disconnect
//Gets the status of the connection to a router
#define GetWiFiStatus wifi_station_get_connect_status

//Turn off WiFi
#define DisableWiFi() wifi_set_opmode(NULL_MODE)
//Enter Station mode (to connect to a router)
#define EnterStationMode() wifi_set_opmode(STATION_MODE)
//Enter SoftAP mode (to act as a router)
#define EnterSoftAPMode() wifi_set_opmode(SOFTAP_MODE)
//Enter both SoftAP and Station mode
#define EnterStationAndSoftAPMode() wifi_set_opmode(STATIONAP_MODE)

typedef union {
struct {
	uint8 a;
	uint8 b;
	uint8 c;
	uint8 d;
} parts;
uint32 value;
} IPAddress;

//Gets the node's assigned IP address
IPAddress GetStationIPAddress();

#define GetStationModeMAC(mac) wifi_get_macaddr(STATION_IF,mac)
#define GetSoftAPModeMAC(mac) wifi_get_macaddr(SOFTAP_IF,mac)

//If auto connect is enabled, then after user init completes the ESP8266 will connect automatically based on stored settings
#define DisableAutoConnect() wifi_station_set_auto_connect(false);
#define EnableAutoConnect() wifi_station_set_auto_connect(true);

//Sets router SSID and password
void SetWiFiStationConfig(const char *ssid, const char *password);
//Sets SSID, password, channel, authentication mode and whether to hide SSID when acting as an AP
void SetWiFiSoftAPConfig(const char *ssid, const char *password, uint8 channel, AUTH_MODE auth, bool ssid_hidden);

#ifdef __cplusplus
}
#endif

#endif
