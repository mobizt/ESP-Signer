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

#ifndef ESP_Signer_TCP_Client_CPP
#define ESP_Signer_TCP_Client_CPP

#if defined(ESP32)

#include "ESP_Signer_TCP_Client.h"

ESP_Signer_TCP_Client::ESP_Signer_TCP_Client()
{
  client = wcs.get();
}

ESP_Signer_TCP_Client::~ESP_Signer_TCP_Client()
{
  release();
}

void ESP_Signer_TCP_Client::setInsecure()
{
#if __has_include(<esp_idf_version.h>)
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(3, 3, 0)
  wcs->setInsecure();
#endif
#endif
}

void ESP_Signer_TCP_Client::setCACert(const char *caCert)
{
  release();

  wcs = std::unique_ptr<ESP_Signer_WCS>(new ESP_Signer_WCS());
  client = wcs.get();

  if (caCert != NULL)
  {
    baseSetCertType(esp_signer_cert_type_data);
    wcs->setCACert(caCert);
  }
  else
  {
    wcs->stop();
    wcs->setCACert(NULL);
    setInsecure();
    baseSetCertType(esp_signer_cert_type_none);
  }
}

bool ESP_Signer_TCP_Client::setCertFile(const char *caCertFile, mb_fs_mem_storage_type storageType)
{
  if (!mbfs)
    return false;

  if (strlen(caCertFile) > 0)
  {
    MB_String filename = caCertFile;
    if (filename.length() > 0)
    {
      if (filename[0] != '/')
        filename.prepend('/');
    }

    int len = mbfs->open(filename, storageType, mb_fs_open_mode_read);
    if (len > -1)
    {

      if (storageType == mb_fs_mem_storage_type_flash)
      {
        fs::File file = mbfs->getFlashFile();
        wcs->loadCACert(file, len);
        mbfs->close(storageType);
        baseSetCertType(esp_signer_cert_type_file);
      }
      else if (storageType == mb_fs_mem_storage_type_sd)
      {

#if defined(MBFS_ESP32_SDFAT_ENABLED)

        if (cert)
          mbfs->delP(&cert);

        cert = (char *)mbfs->newP(len);
        if (mbfs->available(storageType))
          mbfs->read(storageType, (uint8_t *)cert, len);

        mbfs->close(storageType);
        wcs->setCACert((const char *)cert);
        baseSetCertType(esp_signer_cert_type_file);

#elif defined(MBFS_SD_FS)
        fs::File file = mbfs->getSDFile();
        wcs->loadCACert(file, len);
        mbfs->close(storageType);
        baseSetCertType(esp_signer_cert_type_file);

#endif
      }
    }
  }

  return getCertType() == esp_signer_cert_type_file;
}

bool ESP_Signer_TCP_Client::networkReady()
{
  return WiFi.status() == WL_CONNECTED || ethLinkUp();
}

void ESP_Signer_TCP_Client::networkReconnect()
{
  esp_wifi_connect();
}

void ESP_Signer_TCP_Client::networkDisconnect()
{
  WiFi.disconnect();
}

esp_signer_tcp_client_type ESP_Signer_TCP_Client::type()
{
  return esp_signer_tcp_client_type_internal;
}

bool ESP_Signer_TCP_Client::isInitialized() { return true; }

int ESP_Signer_TCP_Client::hostByName(const char *name, IPAddress &ip)
{
  return WiFi.hostByName(name, ip);
}

void ESP_Signer_TCP_Client::setTimeout(uint32_t timeoutmSec)
{
  if (wcs)
    wcs->setTimeout(timeoutmSec / 1000);

  baseSetTimeout(timeoutmSec);
}

bool ESP_Signer_TCP_Client::begin(const char *host, uint16_t port, int *response_code)
{
  this->host = host;
  this->port = port;
  this->response_code = response_code;
  return true;
}

// override the base connect
bool ESP_Signer_TCP_Client::connect()
{
  if (connected())
  {
    flush();
    return true;
  }

  wcs->setTimeout(timeoutMs);

  if (!wcs->_connect(host.c_str(), port, timeoutMs))
    return setError(ESP_SIGNER_ERROR_TCP_ERROR_CONNECTION_REFUSED);

  return connected();
}

bool ESP_Signer_TCP_Client::ethLinkUp()
{
  if (strcmp(ETH.localIP().toString().c_str(), (const char *)MBSTRING_FLASH_MCR("0.0.0.0")) != 0)
  {
    ETH.linkUp();
    return true;
  }
  return false;
}

void ESP_Signer_TCP_Client::release()
{
  if (wcs)
  {
    wcs->stop();
    wcs.reset(nullptr);
    wcs.release();

    if (cert)
      mbfs->delP(&cert);

    baseSetCertType(esp_signer_cert_type_undefined);
  }
}

#endif /* ESP32 */

#endif /* ESP_Signer_TCP_Client_CPP */
