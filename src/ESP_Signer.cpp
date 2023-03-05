/**
 * Google's OAuth2.0 Access token Generation class, Signer.cpp version 1.3.1
 *
 * This library used RS256 for signing algorithm.
 *
 * The signed JWT token will be generated and exchanged with the access token in the final generating process.
 *
 * This library supports ESP8266, ESP32 and Raspberry Pi Pico.
 *
 * Created Match 5, 2023
 *
 * The MIT License (MIT)
 * Copyright (c) 2023 K. Suwatchai (Mobizt)
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

#ifndef ESP_SIGNER_CPP
#define ESP_SIGNER_CPP
#include <Arduino.h>
#include "mbfs/MB_MCU.h"
#include "ESP_Signer.h"

ESP_Signer::ESP_Signer()
{
    authClient.begin(nullptr, &mbfs, &mb_ts, &mb_ts_offset);
    authClient.newClient(&authClient.tcpClient);
}

ESP_Signer::~ESP_Signer()
{
    end();
}

void ESP_Signer::begin(SignerConfig *cfg)
{
    authClient.newClient(&authClient.tcpClient);

    config = cfg;

    if (!config)
        return;

    authClient.reset();

#if defined(ESP32) || defined(ESP8266)
    config->internal.reconnect_wifi = WiFi.getAutoReconnect();
#endif
    config->signer.tokens.token_type = token_type_oauth2_access_token;

    authClient.begin(config, &mbfs, &mb_ts, &mb_ts_offset);
}

void ESP_Signer::end()
{
    authClient.end();
}

String ESP_Signer::accessToken()
{
    if (!config)
        return "";
    return config->internal.auth_token.c_str();
}

bool ESP_Signer::setClock(float gmtOffset)
{
    return TimeHelper::syncClock(&authClient.ntpClient, &mb_ts, &mb_ts_offset, gmtOffset, config);
}

#if defined(ESP_SIGNER_ENABLE_EXTERNAL_CLIENT)
void ESP_Signer::mSetClient(Client *client, ESP_Signer_NetworkConnectionRequestCallback networkConnectionCB,
                           ESP_Signer_NetworkStatusRequestCallback networkStatusCB)
{
    authClient.tcpClient->setClient(client, networkConnectionCB, networkStatusCB);
    authClient.tcpClient->setCACert(nullptr);
}

void ESP_Signer::mSetUDPClient(UDP *client, float gmtOffset)
{
    authClient.udp = client;
    authClient.gmtOffset = gmtOffset;
}
#endif

bool ESP_Signer::waitClockReady()
{
    unsigned long ms = millis();
    while (!setClock(config->internal.gmt_offset) && millis() - ms < 3000)
    {
        Utils::idle();
    }
    return config->internal.clock_rdy;
}

void ESP_Signer::reset()
{
    authClient.reset();
}

bool ESP_Signer::setSecure()
{
    GAuth_TCP_Client *client = authClient.tcpClient;

    if (!client)
        return false;

    client->setConfig(config, &mbfs);

    if (!authClient.reconnect(client))
        return false;

#if (defined(ESP8266) || defined(MB_ARDUINO_PICO))
    if (TimeHelper::getTime(&mb_ts, &mb_ts_offset) > ESP_SIGNER_DEFAULT_TS)
    {
        config->internal.clock_rdy = true;
        client->setClockStatus(true);
    }
#endif

    if (client->getCertType() == esp_signer_cert_type_undefined || cert_updated)
    {

        if (!config->internal.clock_rdy && (config->cert.file.length() > 0 || config->cert.data != NULL || cert_addr > 0))
            TimeHelper::syncClock(&authClient.ntpClient, &mb_ts, &mb_ts_offset, config->internal.gmt_offset, config);

        if (config->cert.file.length() == 0)
        {
            if (cert_addr > 0)
                client->setCACert(reinterpret_cast<const char *>(cert_addr));
            else if (config->cert.data != NULL)
                client->setCACert(config->cert.data);
            else
                client->setCACert(NULL);
        }
        else
        {
            if (!client->setCertFile(config->cert.file.c_str(), (mb_fs_mem_storage_type)config->cert.file_storage))
                client->setCACert(NULL);
        }
        cert_updated = false;
    }
    return true;
}

bool ESP_Signer::isError(MB_String &response)
{
    authClient.initJson();
    bool ret = false;
    if (JsonHelper::setData(authClient.jsonPtr, response, false))
        ret = JsonHelper::parse(authClient.jsonPtr, authClient.resultPtr, esp_signer_gauth_pgm_str_14) || JsonHelper::parse(authClient.jsonPtr, authClient.resultPtr, esp_signer_gauth_pgm_str_14);

    authClient.freeJson();
    return ret;
}

bool ESP_Signer::tokenReady()
{
    return authClient.tokenReady();
};

String ESP_Signer::getTokenType(TokenInfo info)
{
    return authClient.getTokenType(info);
}

String ESP_Signer::getTokenType()
{
    return authClient.getTokenType();
}

String ESP_Signer::getTokenStatus(TokenInfo info)
{
    return authClient.getTokenStatus(info);
}

String ESP_Signer::getTokenStatus()
{
    return authClient.getTokenStatus();
}

String ESP_Signer::getTokenError(TokenInfo info)
{
    return authClient.getTokenError(info);
}

String ESP_Signer::getTokenError()
{
    return authClient.getTokenError();
}

unsigned long ESP_Signer::getExpiredTimestamp()
{
    return authClient.getExpiredTimestamp();
}

void ESP_Signer::refreshToken()
{
    authClient.refreshToken();
}

bool ESP_Signer::setSystemTime(time_t ts)
{
    return authClient.setTime(ts);
}

ESP_Signer Signer = ESP_Signer();

#endif