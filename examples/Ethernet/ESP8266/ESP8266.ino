
/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: k_suwatchai@hotmail.com
 *
 * Github: https://github.com/mobizt
 *
 * Copyright (c) 2023 mobizt
 *
 */

// This example show how to get access token using ESP8266 and ENC28J60 Ethernet module.

/**
 *
 * The ENC28J60 Ethernet module and ESP8266 board, SPI port wiring connection.
 *
 * ESP8266 (Wemos D1 Mini or NodeMCU)        ENC28J60
 *
 * GPIO12 (D6) - MISO                        SO
 * GPIO13 (D7) - MOSI                        SI
 * GPIO14 (D5) - SCK                         SCK
 * GPIO16 (D0) - CS                          CS
 * GND                                       GND
 * 3V3                                       VCC
 *
 */

/**
 * Do not forget to defines the following macros in FS_Config.h
 *
 * For ESP8266 ENC28J60 Ethernet module
 * #define ESP_SIGNER_ENABLE_ESP8266_ENC28J60_ETH
 *
 * For ESP8266 W5100 Ethernet module
 * #define ESP_SIGNER_ENABLE_ESP8266_W5100_ETH
 *
 * For ESP8266 W5500 Ethernet module
 * #define ESP_SIGNER_ENABLE_ESP8266_W5500_ETH
 *
 */
#include <Arduino.h>

#include <ESP_Signer.h>

#include <ENC28J60lwIP.h>
// #include <W5100lwIP.h>
// #include <W5500lwIP.h>

/** These credentials are taken from Service Account key file (JSON)
 * https://cloud.google.com/iam/docs/service-accounts
 */
// See https://github.com/mobizt/ESP-Signer#how-to-create-service-account-private-key

#define PROJECT_ID "The project ID"                                                             // Taken from "project_id" key in JSON file.
#define CLIENT_EMAIL "Client Email"                                                             // Taken from "client_email" key in JSON file.
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\\n-----END PRIVATE KEY-----\n"; // Taken from "private_key" key in JSON file.

#define ETH_CS_PIN 16 // D0

ENC28J60lwIP eth(ETH_CS_PIN);
// Wiznet5100lwIP eth(ETH_CS_PIN);
// Wiznet5500lwIP eth(ETH_CS_PIN);

SignerConfig config;

bool beginReady = false;

void tokenStatusCallback(TokenInfo info);

void begin()
{
    beginReady = true;

    /* Assign the sevice account credentials and private key (required) */
    config.service_account.data.client_email = CLIENT_EMAIL;
    config.service_account.data.project_id = PROJECT_ID;
    config.service_account.data.private_key = PRIVATE_KEY;

    /* Expired period in seconds (optional). Default is 3600 sec.*/
    config.signer.expiredSeconds = 3600;

    /* Seconds to refresh the token before expired (optional). Default is 60 sec.*/
    config.signer.preRefreshSeconds = 60;

    config.token_status_callback = tokenStatusCallback;

    config.signer.tokens.scope = "https://www.googleapis.com/auth/spreadsheets,https://www.googleapis.com/auth/drive,https://www.googleapis.com/auth/drive.file";

    config.spi_ethernet_module.enc28j60 = &eth;

    Signer.begin(&config);
}

void setup()
{

    Serial.begin(115200);
    Serial.println();

#if defined(ESP8266)
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz?
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    eth.setDefault(); // use ethernet for default route
    if (!eth.begin())
    {
        Serial.println("ethernet hardware not found ... sleeping");
        while (1)
        {
            delay(1000);
        }
    }
    else
    {
        Serial.print("connecting ethernet");
        while (!eth.connected())
        {
            Serial.print(".");
            delay(1000);
        }
    }
    Serial.println();
    Serial.print("ethernet IP address: ");
    Serial.println(eth.localIP());
#endif
}

void loop()
{
    if (!beginReady)
    {
        begin();
    }

    // Check to status and also refresh the access token
    bool ready = Signer.tokenReady();
    if (ready)
    {
        int t = Signer.getExpiredTimestamp() - config.signer.preRefreshSeconds - Signer.getCurrentTimestamp();

        Serial.print("Remaining seconds to refresh the token, ");
        Serial.println(t);
        delay(1000);
    }
}

void tokenStatusCallback(TokenInfo info)
{
    if (info.status == esp_signer_token_status_error)
    {
        Signer.printf("Token info: type = %s, status = %s\n", Signer.getTokenType(info).c_str(), Signer.getTokenStatus(info).c_str());
        Signer.printf("Token error: %s\n", Signer.getTokenError(info).c_str());
    }
    else
    {
        Signer.printf("Token info: type = %s, status = %s\n", Signer.getTokenType(info).c_str(), Signer.getTokenStatus(info).c_str());
        if (info.status == esp_signer_token_status_ready)
            Signer.printf("Token: %s\n", Signer.accessToken().c_str());
    }
}
