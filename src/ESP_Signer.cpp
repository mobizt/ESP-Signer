/**
 * Google's OAuth2.0 Access token Generation class, Signer.cpp version 1.4.1
 *
 * This library used RS256 for signing algorithm.
 *
 * The signed JWT token will be generated and exchanged with the access token in the final generating process.
 *
 * This library supports ESP8266, ESP32 and Raspberry Pi Pico.
 *
 * Created August 13, 2023
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

void ESP_Signer::mSetClient(Client *client, ESP_Signer_NetworkConnectionRequestCallback networkConnectionCB,
                            ESP_Signer_NetworkStatusRequestCallback networkStatusCB)
{
    authClient.tcpClient->setClient(client, networkConnectionCB, networkStatusCB);
    authClient.tcpClient->setCACert(nullptr);
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

uint64_t ESP_Signer::getCurrentTimestamp()
{
    TimeHelper::getTime(&mb_ts, &mb_ts_offset);
    return mb_ts;
}

void ESP_Signer::refreshToken()
{
    authClient.refreshToken();
}

bool ESP_Signer::setSystemTime(time_t ts)
{
    return authClient.setTime(ts);
}

void ESP_Signer::printf(const char *format, ...)
{
    int size = 2048;
    char s[size];
    va_list va;
    va_start(va, format);
    vsnprintf(s, size, format, va);
    va_end(va);
    Serial.print(s);
}

int ESP_Signer::getFreeHeap()
{
#if defined(MB_ARDUINO_ESP)
  return ESP.getFreeHeap();
#elif defined(MB_ARDUINO_PICO)
  return rp2040.getFreeHeap();
#else
  return 0;
#endif
}

ESP_Signer Signer = ESP_Signer();

#endif