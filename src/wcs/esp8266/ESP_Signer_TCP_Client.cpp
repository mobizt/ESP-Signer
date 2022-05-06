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

#ifndef ESP_Signer_TCP_Client_CPP
#define ESP_Signer_TCP_Client_CPP

#if defined(ESP8266) 

#include "ESP_Signer_TCP_Client.h"

ESP_Signer_TCP_Client::ESP_Signer_TCP_Client()
{
  client = wcs.get();
}

ESP_Signer_TCP_Client::~ESP_Signer_TCP_Client()
{
  release();
}

void ESP_Signer_TCP_Client::setCACert(const char *caCert)
{

  release();

  wcs = std::unique_ptr<ESP_SIGNER_SSL_CLIENT>(new ESP_SIGNER_SSL_CLIENT());

  client = wcs.get();

  if (caCert)
  {
    x509 = new X509List(caCert);
    wcs->setTrustAnchors(x509);
    baseSetCertType(esp_signer_cert_type_data);
  }
  else
  {
    wcs->setInsecure();
    baseSetCertType(esp_signer_cert_type_none);
  }

  wcs->setBufferSizes(bsslRxSize, bsslTxSize);
}

bool ESP_Signer_TCP_Client::setCertFile(const char *caCertFile, mb_fs_mem_storage_type storageType)
{

  if (!mbfs)
    return false;

  if (clockReady && strlen(caCertFile) > 0)
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

      uint8_t *der = (uint8_t *)mbfs->newP(len);
      if (mbfs->available(storageType))
        mbfs->read(storageType, der, len);
      mbfs->close(storageType);
      wcs->setTrustAnchors(new X509List(der, len));
      mbfs->delP(&der);
      baseSetCertType(esp_signer_cert_type_file);
    }
  }

  wcs->setBufferSizes(bsslRxSize, bsslTxSize);

  return getCertType() == esp_signer_cert_type_file;
}

void ESP_Signer_TCP_Client::setBufferSizes(int recv, int xmit)
{
  bsslRxSize = recv;
  bsslTxSize = xmit;
}

bool ESP_Signer_TCP_Client::networkReady()
{
  return WiFi.status() == WL_CONNECTED || ethLinkUp();
}

void ESP_Signer_TCP_Client::networkReconnect()
{
  WiFi.reconnect();
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
    wcs->setTimeout(timeoutmSec);

  baseSetTimeout(timeoutmSec);
}

bool ESP_Signer_TCP_Client::begin(const char *host, uint16_t port, int *response_code)
{

  this->host = host;
  this->port = port;
  this->response_code = response_code;

  ethDNSWorkAround();

  wcs->setBufferSizes(bsslRxSize, bsslTxSize);

  return true;
}

bool ESP_Signer_TCP_Client::ethLinkUp()
{

  if (!eth)
    return false;

  bool ret = false;
#if defined(ESP8266) && defined(ESP8266_CORE_SDK_V3_X_X)

#if defined(INC_ENC28J60_LWIP)
  if (eth->enc28j60)
  {
    ret = eth->enc28j60->status() == WL_CONNECTED;
    goto ex;
  }
#endif
#if defined(INC_W5100_LWIP)
  if (eth->w5100)
  {
    ret = eth->w5100->status() == WL_CONNECTED;
    goto ex;
  }
#endif
#if defined(INC_W5100_LWIP)
  if (eth->w5500)
  {
    ret = eth->w5500->status() == WL_CONNECTED;
    goto ex;
  }
#endif

  return ret;

ex:
  // workaround for ESP8266 Ethernet
  delayMicroseconds(0);
#endif

  return ret;
}

void ESP_Signer_TCP_Client::ethDNSWorkAround()
{
  if (!eth)
    return;

#if defined(ESP8266_CORE_SDK_V3_X_X)

#if defined(INC_ENC28J60_LWIP)
  if (eth->enc28j60)
    goto ex;
#endif
#if defined(INC_W5100_LWIP)
  if (eth->w5100)
    goto ex;
#endif
#if defined(INC_W5100_LWIP)
  if (eth->w5500)
    goto ex;
#endif

  return;

ex:
  WiFiClient _client;
  _client.connect(host.c_str(), port);
  _client.stop();
#endif
}

void ESP_Signer_TCP_Client::release()
{
  if (wcs)
  {
    wcs->stop();
    wcs.reset(nullptr);
    wcs.release();

    if (x509)
      delete x509;

    baseSetCertType(esp_signer_cert_type_undefined);
  }
}

#endif /* ESP8266 */

#endif /* ESP_Signer_TCP_Client_CPP */