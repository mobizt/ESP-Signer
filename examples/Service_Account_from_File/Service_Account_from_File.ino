#include <Arduino.h>
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include "ESPSigner.h"

#define WIFI_SSID "WiFi SSID"
#define WIFI_PASSWORD "WIFi PSK"

#define PROJECT_HOST "The project host without cheme (https://)"


SignerConfig config;

void tokenStatusCallback(TokenInfo info);

void setup()
{

    Serial.begin(115200);
    Serial.println();
    Serial.println();

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    /* Assign the project host (required) */
    config.host = PROJECT_HOST;

    /* The file systems for flash and SD/SDMMC can be changed in FS_Config.h. */

    /** Assign the sevice account JSON file and the file storage type (required) 
     * https://cloud.google.com/iam/docs/service-accounts
    */
    config.service_account.json.path = "/service_account_file.json"; //change this for your json file
    config.service_account.json.storage_type = esp_signer_mem_storage_type_flash; //or esp_signer_mem_storage_type_sd

    /** Assign the API scopes (required) 
     * Use space or comma to separate the scope.
    */
    config.signer.tokens.scope = "https://www.googleapis.com/auth/cloud-platform, https://www.googleapis.com/auth/userinfo.email";

    /** Assign the callback function for token ggeneration status (optional) */
    config.token_status_callback = tokenStatusCallback;

    /* Create token */
    Signer.begin(&config);
}

void loop()
{
    delay(1000);

    /* Check to status and also refresh the access token */
    bool ready = Signer.tokenReady();
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
