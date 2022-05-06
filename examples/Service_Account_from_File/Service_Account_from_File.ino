#include <Arduino.h>
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <ESPSigner.h>

// For SD card configuration and mounting
#include <ESP_Signer_SD_helper.h> // See src/ESP_Signer_SD_helper.h

#define WIFI_SSID "WiFi SSID"
#define WIFI_PASSWORD "WIFi PSK"


SignerConfig config;

void tokenStatusCallback(TokenInfo info);

void setup()
{

    Serial.begin(115200);
    Serial.println();
    Serial.println();

    WiFi.setAutoReconnect(true);

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

    // Mount SD card.
    SD_Card_Mounting(); // See src/ESP_Signer_SD_helper.h

    /* The file systems for flash and SD/SDMMC can be changed in FS_Config.h. */

    /** Assign the sevice account JSON file and the file storage type (required) 
     * https://cloud.google.com/iam/docs/service-accounts
    */

    // See https://github.com/mobizt/ESP-Signer#how-to-create-service-account-private-key

    config.service_account.json.path = "/service_account_file.json"; //change this for your json file
    config.service_account.json.storage_type = esp_signer_mem_storage_type_sd; //or esp_signer_mem_storage_type_sd

    /** Expired period in seconds (optional). 
     * Default is 3600 sec.
     * This may not afftect the expiry time of generated access token.
    */
    config.signer.expiredSeconds = 3600;

    /* Seconds to refresh the token before expiry time (optional). Default is 60 sec.*/
    config.signer.preRefreshSeconds = 60;


    /** Assign the API scopes (required) 
     * Use space or comma to separate the scope.
    */
    config.signer.tokens.scope = "https://www.googleapis.com/auth/cloud-platform, https://www.googleapis.com/auth/userinfo.email";

    /** Assign the callback function for token ggeneration status (optional) */
    config.token_status_callback = tokenStatusCallback;

    //To set the device time without NTP time acquisition.
    //Signer.setSystemTime(<timestamp>);
    

    /* Create token */
    Signer.begin(&config);

    //The WiFi connection can be processed before or after Signer.begin

    //Call Signer.getExpiredTimestamp() to get the token expired timestamp (seconds from midnight Jan 1, 1970)

    //Call Signer.refreshToken() to force refresh the token.
}

void loop()
{

    /* Check for token generation ready state and also refresh the access token if it expired */
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
