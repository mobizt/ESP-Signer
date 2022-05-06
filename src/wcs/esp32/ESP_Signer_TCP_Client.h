/**
 * ESP Signer TCP Client v1.0.0
 *
 * Created May 6, 2022
 *
 * The MIT License (MIT)
 * Copyright (c) 2022 K. Suwatchai (Mobizt)
 *
 *
 * Copyright (c) 2015 Markus Sattler. All rights reserved.
 * This file is part of the HTTPClient for Arduino.
 * Port to ESP32 by Evandro Luis Copercini (2017),
 * changed fingerprints to CA verification.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef ESP_Signer_TCP_Client_H
#define ESP_Signer_TCP_Client_H

#if defined(ESP32) 

#include "ESP_Signer_Net.h"
#include "ESP_Signer_Error.h"
#include "mbfs/MB_FS.h"
#include "./wcs/base/ESP_Signer_TCP_Client_Base.h"

extern "C"
{
#include <esp_err.h>
#include <esp_wifi.h>
}

// The derived class to fix the memory leaks issue
// https://github.com/espressif/arduino-esp32/issues/5480
class ESP_Signer_WCS : public WiFiClientSecure
{
public:
  ESP_Signer_WCS(){};
  ~ESP_Signer_WCS(){};

  int _connect(const char *host, uint16_t port, unsigned long timeout)
  {
    _timeout = timeout;
    if (connect(host, port) == 0)
    {
      if (_CA_cert != NULL)
        mbedtls_x509_crt_free(&sslclient->ca_cert);
      return 0;
    }
    _connected = true;
    return 1;
  }
};

class ESP_Signer_TCP_Client : public ESP_Signer_TCP_Client_Base
{

public:
  ESP_Signer_TCP_Client();
  ~ESP_Signer_TCP_Client();

  void setCACert(const char *caCert);

  bool setCertFile(const char *certFile, mb_fs_mem_storage_type storageType);

  void setInsecure();

  bool networkReady();

  void networkReconnect();

  void networkDisconnect();

  esp_signer_tcp_client_type type();

  bool isInitialized();

  int hostByName(const char *name, IPAddress &ip);

  void setTimeout(uint32_t timeoutmSec);

  bool begin(const char *host, uint16_t port, int *response_code);

  bool connect();

private:
  std::unique_ptr<ESP_Signer_WCS> wcs = std::unique_ptr<ESP_Signer_WCS>(new ESP_Signer_WCS());
  char *cert = NULL;

  bool ethLinkUp();

  void release();
};

#endif /* ESP32 */

#endif /* ESP_Signer_TCP_Client_H */
