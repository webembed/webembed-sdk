/*
 * Part of the WebEmbed project
 * User-friendly API for WiFi manipulation
 * See LICENSE for details
 */
#include "WiFi.h"
#include <c_types.h>
struct station_config staConfig;
void ICACHE_FLASH_ATTR SetWiFiStationConfig(const char *ssid,const char *password) {
	strncpy(staConfig.ssid, ssid, 32);
	strncpy(staConfig.password, password, 64);
	wifi_station_set_config(&staConfig);
}

struct softap_config apConfig;
void ICACHE_FLASH_ATTR SetWiFiSoftAPConfig(const char *ssid,const char *password, uint8 channel, AUTH_MODE auth, bool ssid_hidden) {
	strncpy(apConfig.ssid, ssid, 32);
	strncpy(apConfig.password, password, 64);
	apConfig.ssid_len = strnlen(ssid,32);
	apConfig.channel = channel;
	apConfig.authmode = auth;
	apConfig.ssid_hidden = ssid_hidden;
	wifi_softap_set_config(&apConfig);
}

struct ip_info ipInfo;
IPAddress ICACHE_FLASH_ATTR GetStationIPAddress() {
    wifi_get_ip_info(STATION_IF,&ipInfo);
    IPAddress addr;
    addr.value = ipInfo.ip.addr;
    return addr;
}
