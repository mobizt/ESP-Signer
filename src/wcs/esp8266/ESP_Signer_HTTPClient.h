/**
 * HTTP Client wrapper v1.0.0
 * 
 * The MIT License (MIT)
 * Copyright (c) 2021 K. Suwatchai (Mobizt)
 * 
 * Permission is hereby granted, free of charge, to any person returning a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef ESP_SIGNER_HTTPClient_H
#define ESP_SIGNER_HTTPClient_H

#ifdef ESP8266

#include <Arduino.h>
#include <core_version.h>
#include <time.h>

#ifndef ARDUINO_ESP8266_GIT_VER
#error Your ESP8266 Arduino Core SDK is outdated, please update. From Arduino IDE go to Boards Manager and search 'esp8266' then select the latest version.
#endif

//2.6.1 BearSSL bug
#if ARDUINO_ESP8266_GIT_VER == 0x482516e3
#error Due to bugs in BearSSL in ESP8266 Arduino Core SDK version 2.6.1, please update ESP8266 Arduino Core SDK to newer version. The issue was found here https:\/\/github.com/esp8266/Arduino/issues/6811.
#endif

#include <WiFiClientSecure.h>
#include <CertStoreBearSSL.h>
#define ESP_SIGNER_ESP_SSL_CLIENT BearSSL::WiFiClientSecure

#define FS_NO_GLOBALS
#include <FS.h>
#include <SD.h>
#include <LittleFS.h>
#include "FS_Config.h"

#if __has_include(<WiFiEspAT.h>) || __has_include(<espduino.h>)
#error WiFi UART bridge was not supported.
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#define FLASH_FS DEFAULT_FLASH_FS
#define SD_FS DEFAULT_SD_FS

#include "wcs/ESP_Signer_HTTPCode.h"

struct esp_signer_sd_config_info_t
{
  int sck = -1;
  int miso = -1;
  int mosi = -1;
  int ss = -1;
};

class ESP_Signer_HTTPClient
{


public:
  ESP_Signer_HTTPClient();
  ~ESP_Signer_HTTPClient();

  bool begin(const char *host, uint16_t port);

  bool connected(void);

  int send(const char *header, const char *payload);

  bool send(const char *header);

  WiFiClient *stream(void);

  void setCACert(const char *caCert);
  void setCACertFile(const char* caCertFile, uint8_t storageType, struct esp_signer_sd_config_info_t sd_config);
  bool connect(void);


private:
  std::unique_ptr<ESP_SIGNER_ESP_SSL_CLIENT> _wcs = std::unique_ptr<ESP_SIGNER_ESP_SSL_CLIENT>(new ESP_SIGNER_ESP_SSL_CLIENT());
  std::string _host = "";
  uint16_t _port = 0;
  unsigned long timeout = ESP_SIGNER_DEFAULT_TCP_TIMEOUT;

  std::string _CAFile = "";
  uint8_t _CAFileStoreageType = 0;
  int _certType = -1;
  uint8_t _sdPin = 15;
  bool _clockReady = false;
  uint16_t _bsslRxSize = 512;
  uint16_t _bsslTxSize = 512;
  bool fragmentable = false;
  int chunkSize = 1024;
  bool mflnChecked = false;
};

#endif /* ESP8266 */

#endif /* ESP_Signer_HTTPClient_H */