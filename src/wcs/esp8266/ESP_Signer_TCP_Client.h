/**
 * Firebase TCP Client v1.1.20
 *
 * Created April 17, 2022
 *
 * The MIT License (MIT)
 * Copyright (c) 2022 K. Suwatchai (Mobizt)
 *
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

#ifndef ESP_Signer_TCP_Client_H
#define ESP_Signer_TCP_Client_H

#if defined(ESP8266)

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "ESP_Signer_Net.h"
#include "ESP_Signer_Error.h"
#include "./wcs/base/ESP_Signer_TCP_Client_Base.h"

class ESP_Signer_TCP_Client : public ESP_Signer_TCP_Client_Base
{

public:
  ESP_Signer_TCP_Client();
  ~ESP_Signer_TCP_Client();

  void setCACert(const char *caCert);

  bool setCertFile(const char *certFile, mb_fs_mem_storage_type storageType);

  void setBufferSizes(int recv, int xmit);

  bool networkReady();

  void networkReconnect();

  void networkDisconnect();

  esp_signer_tcp_client_type type();

  bool isInitialized();

  int hostByName(const char *name, IPAddress &ip);

  void setTimeout(uint32_t timeoutmSec);

  bool begin(const char *host, uint16_t port, int *response_code);

  int beginUpdate(int len, bool verify = true);

  bool ethLinkUp();

  void ethDNSWorkAround();

private:
  std::unique_ptr<ESP_SIGNER_SSL_CLIENT> wcs = std::unique_ptr<ESP_SIGNER_SSL_CLIENT>(new ESP_SIGNER_SSL_CLIENT());
  uint16_t bsslRxSize = 2048;
  uint16_t bsslTxSize = 512;
  X509List *x509 = nullptr;
  void release();
};

#endif /* ESP8266 */

#endif /* ESP_Signer_TCP_Client_H */