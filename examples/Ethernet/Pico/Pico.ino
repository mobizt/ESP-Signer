
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

// This example shows how to use Raspberry Pi Pico to create OAuth2 access token via ethernet using external SSL client
// This example is for Raspberri Pi Pico and W5500 Ethernet module.

/**
 *
 * The W5500 Ethernet module and RPI2040 Pico board, SPI 0 port wiring connection.
 *
 * Raspberry Pi Pico                        W5500
 *
 * GPIO 16 - SPI 0 MISO                     SO
 * GPIO 19 - SPI 0 MOSI                     SI
 * GPIO 18 - SPI 0 SCK                      SCK
 * GPIO 17 - SPI 0 CS                       CS
 * GPIO 20 - W5500 Reset                    Reset
 * GND                                      GND
 * 3V3                                      VCC
 *
 */

/**
 * Do not forget to defines the following macro in FS_Config.h
 *
 * #define ESP_SIGNER_ENABLE_EXTERNAL_CLIENT
 *
 */
#include <Arduino.h>

#include <ESP_Signer.h>

// https://github.com/mobizt/ESP_SSLClient
#include <ESP_SSLClient.h>

#include <Ethernet.h>

/** These credentials are taken from Service Account key file (JSON)
 * https://cloud.google.com/iam/docs/service-accounts
 */
// See https://github.com/mobizt/ESP-Signer#how-to-create-service-account-private-key

#define PROJECT_ID "The project ID"                                                             // Taken from "project_id" key in JSON file.
#define CLIENT_EMAIL "Client Email"                                                             // Taken from "client_email" key in JSON file.
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\\n-----END PRIVATE KEY-----\n"; // Taken from "private_key" key in JSON file.

#define WIZNET_RESET_PIN 20       // Connect W5500 Reset pin to GPIO 20 of Raspberry Pi Pico
#define WIZNET_CS_PIN PIN_SPI0_SS // Connect W5500 CS pin to SPI 0's SS (GPIO 17) of Raspberry Pi Pico

/* 5. Define MAC */
uint8_t Eth_MAC[] = {0x02, 0xF0, 0x0D, 0xBE, 0xEF, 0x01};

SignerConfig config;

bool beginReady = false;

void tokenStatusCallback(TokenInfo info);

// Define the basic client
// The network interface devices that can be used to handle SSL data should
// have large memory buffer up to 1k - 2k or more, otherwise the SSL/TLS handshake
// will fail.
EthernetClient basic_client;

// This is the wrapper client that utilized the basic client for io and
// provides the mean for the data encryption and decryption before sending to or after read from the io.
// The most probable failures are related to the basic client itself that may not provide the buffer
// that large enough for SSL data.
// The SSL client can do nothing for this case, you should increase the basic client buffer memory.
ESP_SSLClient ssl_client;

// UDP Client for NTP Time synching
EthernetUDP udpClient;

void begin();

void ResetEthernet()
{
    Serial.println("Resetting WIZnet W5500 Ethernet Board...  ");
    pinMode(WIZNET_RESET_PIN, OUTPUT);
    digitalWrite(WIZNET_RESET_PIN, HIGH);
    delay(200);
    digitalWrite(WIZNET_RESET_PIN, LOW);
    delay(50);
    digitalWrite(WIZNET_RESET_PIN, HIGH);
    delay(200);
}

void networkConnection()
{
    Ethernet.init(WIZNET_CS_PIN);

    ResetEthernet();

    Serial.println("Starting Ethernet connection...");
    Ethernet.begin(Eth_MAC);

    unsigned long to = millis();

    while (Ethernet.linkStatus() == LinkOFF || millis() - to < 2000)
    {
        delay(100);
    }

    if (Ethernet.linkStatus() == LinkON)
    {
        Serial.print("Connected with IP ");
        Serial.println(Ethernet.localIP());
    }
    else
    {
        Serial.println("Can't connected");
    }
}

// Define the callback function to handle server status acknowledgement
void networkStatusRequestCallback()
{
    // Set the network status
    Signer.setNetworkStatus(Ethernet.linkStatus() == LinkON);
}

void setup()
{

    Serial.begin(115200);
    Serial.println();

    networkConnection();

    ssl_client.setClient(&basic_client);
    ssl_client.setInsecure();
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
        int t = Signer.getExpiredTimestamp() - config.signer.preRefreshSeconds - time(nullptr);

        Serial.print("Remaining seconds to refresh the token, ");
        Serial.println(t);
        delay(1000);
    }
}

void tokenStatusCallback(TokenInfo info)
{
    if (info.status == esp_signer_token_status_error)
    {
        Serial.printf("Token info: type = %s, status = %s\n", Signer.getTokenType(info).c_str(), Signer.getTokenStatus(info).c_str());
        Serial.printf("Token error: %s\n", Signer.getTokenError(info).c_str());
    }
    else
    {
        Serial.printf("Token info: type = %s, status = %s\n", Signer.getTokenType(info).c_str(), Signer.getTokenStatus(info).c_str());
        if (info.status == esp_signer_token_status_ready)
            Serial.printf("Token: %s\n", Signer.accessToken().c_str());
    }
}

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

    /* Assign the pointer to global defined external SSL Client object and required callback functions */
    Signer.setExternalClient(&ssl_client, networkConnection, networkStatusRequestCallback);

    /* Assign UDP client and gmt offset for NTP time synching when using external SSL client */
    Signer.setUDPClient(&udpClient, 3);

    Signer.begin(&config);
}