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

#define WiFiConnect wifi_station_connect
#define WiFiDisconnect wifi_station_disconnect
#define GetWiFiStatus wifi_station_get_connect_status

#define DisableWiFi() wifi_set_opmode(NULL_MODE)
#define EnterStationMode() wifi_set_opmode(STATION_MODE)
#define EnterSoftAPMode() wifi_set_opmode(SOFTAP_MODE)
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

IPAddress GetStationIPAddress();

#define GetStationModeMAC(mac) wifi_get_macaddr(STATION_IF,mac)
#define GetSoftAPModeMAC(mac) wifi_get_macaddr(SOFTAP_IF,mac)

#define DisableAutoConnect() wifi_station_set_auto_connect(false);
#define EnableAutoConnect() wifi_station_set_auto_connect(true);


void SetWiFiStationConfig(const char *ssid, const char *password);
void SetWiFiSoftAPConfig(const char *ssid, const char *password, uint8 channel, AUTH_MODE auth, bool ssid_hidden);

#ifdef __cplusplus
}
#endif

#endif
