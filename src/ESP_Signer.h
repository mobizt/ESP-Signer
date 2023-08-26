/**
 * Google's OAuth2.0 Access token Generation class, Signer.h version 1.4.1
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

#ifndef ESP_SIGNER_H
#define ESP_SIGNER_H

#include <Arduino.h>
#include "mbfs/MB_MCU.h"
#include "ESP_Signer_Helper.h"
#include "auth/GAuth_OAuth2_Client.h"

class ESP_Signer
{

public:
    ESP_Signer();
    ~ESP_Signer();

    /** Assign external Arduino Client and required callback fumctions.
     *
     * @param client The pointer to Arduino Client derived class of SSL Client.
     * @param networkConnectionCB The function that handles the network connection.
     * @param networkStatusCB The function that handle the network connection status acknowledgement.
     * 
     * Due to the client pointer is assigned, to avoid dangling pointer, 
     * client should be existed as long as it was used for transportation.
     */
    void setExternalClient(Client *client, ESP_Signer_NetworkConnectionRequestCallback networkConnectionCB,
                           ESP_Signer_NetworkStatusRequestCallback networkStatusCB)
    {
        mSetClient(client, networkConnectionCB, networkStatusCB);
    }

    /** Assign TinyGsm Clients.
     *
     * @param client The pointer to TinyGsmClient.
     * @param modem The pointer to TinyGsm modem object. Modem should be initialized and/or set mode before transfering data
     * @param pin The SIM pin.
     * @param apn The GPRS APN (Access Point Name).
     * @param user The GPRS user.
     * @param password The GPRS password.
     * 
     * Due to the client and modem pointers are assigned, to avoid dangling pointer, 
     * client and modem should be existed as long as it was used for transportation.
     */
    void setGSMClient(Client *client, void *modem = nullptr, const char *pin = nullptr, const char *apn = nullptr, const char *user = nullptr, const char *password = nullptr)
    {
        authClient.tcpClient->setGSMClient(client, modem, pin, apn, user, password);
    }

    /** Set the network status acknowledgement.
     *
     * @param status The network status.
     */
    void setNetworkStatus(bool status)
    {
        authClient.tcpClient->setNetworkStatus(status);
    }

    /**
     * Begin the Access token generation
     *
     * @param config The pointer to SignerConfig structured data contains the authentication credentials
     *
     */
    void begin(SignerConfig *config);

    /**
     * End the Access token generation
     *
     */
    void end();

    /**
     * Check the token ready state and trying to re-generate the token when expired.
     *
     * @return Boolean of ready state.
     *
     */
    bool tokenReady();

    /**
     * Get the generated access token.
     *
     * @return String of OAuth2.0 access token.
     *
     */
    String accessToken();

    /**
     * Get the token type string.
     *
     * @param info The TokenInfo structured data contains token info.
     * @return token type String.
     *
     */
    String getTokenType();
    String getTokenType(TokenInfo info);

    /**
     * Get the token status string.
     *
     * @param info The TokenInfo structured data contains token info.
     * @return token status String.
     *
     */
    String getTokenStatus();
    String getTokenStatus(TokenInfo info);

    /**
     * Get the token generation error string.
     *
     * @param info The TokenInfo structured data contains token info.
     * @return token generation error String.
     *
     */
    String getTokenError();
    String getTokenError(TokenInfo info);

    /**
     * Get the token expiration timestamp (seconds from midnight Jan 1, 1970).
     *
     * @return timestamp.
     *
     */
    unsigned long getExpiredTimestamp();

    /**
     * Get the current timestamp.
     *
     * @return timestamp.
     *
     */
    uint64_t getCurrentTimestamp();

    /**
     * Refresh the access token
     *
     */
    void refreshToken();

    /** Set system time with timestamp.
     *
     * @param ts timestamp in seconds from midnight Jan 1, 1970.
     * @return Boolean type status indicates the success of the operation.
     */
    bool setSystemTime(time_t ts);

#if defined(MBFS_SD_FS) && defined(MBFS_CARD_TYPE_SD)

    /** Initiate SD card with SPI port configuration.
     *
     * @param ss SPI Chip/Slave Select pin.
     * @param sck SPI Clock pin.
     * @param miso SPI MISO pin.
     * @param mosi SPI MOSI pin.
     * @param frequency The SPI frequency
     * @return Boolean type status indicates the success of the operation.
     */
    bool sdBegin(int8_t ss = -1, int8_t sck = -1, int8_t miso = -1, int8_t mosi = -1, uint32_t frequency = 4000000)
    {
        return mbfs.sdBegin(ss, sck, miso, mosi, frequency);
    }

#if defined(ESP8266)

    /** Initiate SD card with SD FS configurations (ESP8266 only).
     *
     * @param ss SPI Chip/Slave Select pin.
     * @param sdFSConfig The pointer to SDFSConfig object (ESP8266 only).
     * @return Boolean type status indicates the success of the operation.
     */
    bool sdBegin(SDFSConfig *sdFSConfig)
    {
        return mbfs.sdFatBegin(sdFSConfig);
    }

#endif

#if defined(ESP32)
    /** Initiate SD card with chip select and SPI configuration (ESP32 only).
     *
     * @param ss SPI Chip/Slave Select pin.
     * @param spiConfig The pointer to SPIClass object for SPI configuartion.
     * @param frequency The SPI frequency.
     * @return Boolean type status indicates the success of the operation.
     */
    bool sdBegin(int8_t ss, SPIClass *spiConfig = nullptr, uint32_t frequency = 4000000)
    {
        return mbfs.sdSPIBegin(ss, spiConfig, frequency);
    }
#endif

#if defined(MBFS_ESP32_SDFAT_ENABLED) || defined(MBFS_SDFAT_ENABLED)
    /** Initiate SD card with SdFat SPI and pins configurations (with SdFat included only).
     *
     * @param sdFatSPIConfig The pointer to SdSpiConfig object for SdFat SPI configuration.
     * @param ss SPI Chip/Slave Select pin.
     * @param sck SPI Clock pin.
     * @param miso SPI MISO pin.
     * @param mosi SPI MOSI pin.
     * @return Boolean type status indicates the success of the operation.
     */
    bool sdBegin(SdSpiConfig *sdFatSPIConfig, int8_t ss = -1, int8_t sck = -1, int8_t miso = -1, int8_t mosi = -1)
    {
        return mbfs.sdFatBegin(sdFatSPIConfig, ss, sck, miso, mosi);
    }

    /** Initiate SD card with SdFat SDIO configuration (with SdFat included only).
     *
     * @param sdFatSDIOConfig The pointer to SdioConfig object for SdFat SDIO configuration.
     * @return Boolean type status indicates the success of the operation.
     */
    bool sdBegin(SdioConfig *sdFatSDIOConfig)
    {
        return mbfs.sdFatBegin(sdFatSDIOConfig);
    }

#endif

#endif

#if defined(ESP32) && defined(MBFS_SD_FS) && defined(MBFS_CARD_TYPE_SD_MMC)
    /** Initialize the SD_MMC card (ESP32 only).
     *
     * @param mountpoint The mounting point.
     * @param mode1bit Allow 1 bit data line (SPI mode).
     * @param format_if_mount_failed Format SD_MMC card if mount failed.
     * @return The boolean value indicates the success of operation.
     */
    bool sdMMCBegin(const char *mountpoint = "/sdcard", bool mode1bit = false, bool format_if_mount_failed = false)
    {
        return mbfs.sdMMCBegin(mountpoint, mode1bit, format_if_mount_failed);
    }
#endif

    /**
     * Formatted printing on Serial.
     *
     */
    void printf(const char *format, ...);

    /** Get free Heap memory.
     *
     * @return Free memory amount in byte.
     */
    int getFreeHeap();

protected:
    SignerConfig *config = nullptr;

    GAuth_OAuth2_Client authClient;
    MB_FS mbfs;
    uint32_t mb_ts = 0;
    uint32_t mb_ts_offset = 0;
    void mSetClient(Client *client, ESP_Signer_NetworkConnectionRequestCallback networkConnectionCB,
                    ESP_Signer_NetworkStatusRequestCallback networkStatusCB);
};

extern ESP_Signer Signer;

#endif