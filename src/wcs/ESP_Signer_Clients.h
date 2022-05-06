#ifndef ESP_Line_Notify_ClientS_H
#define ESP_Line_Notify_ClientS_H

#include "ESP_Signer_Net.h"


#if defined(ESP32)
#include <WiFi.h>
#include "./wcs/esp32/ESP_Signer_TCP_Client.h"
#define ESP_SIGNER_TCP_CLIENT ESP_Signer_TCP_Client
#elif defined(ESP8266)
#include <Schedule.h>
#include <ets_sys.h>
#include <ESP8266WiFi.h>
#include "./wcs/esp8266/ESP_Signer_TCP_Client.h"
#define ESP_SIGNER_TCP_CLIENT ESP_Signer_TCP_Client
#define FS_NO_GLOBALS
#endif

#endif