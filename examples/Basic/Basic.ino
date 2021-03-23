#include <Arduino.h>
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <ESPSigner.h>

#define WIFI_SSID "WiFi SSID"
#define WIFI_PASSWORD "WIFi PSK"

/** These credentials are taken from Service Account key file (JSON)
 * https://cloud.google.com/iam/docs/service-accounts
*/
#define PROJECT_ID "The project ID" //Taken from "project_id" key in JSON file.
#define CLIENT_EMAIL "Client Email" //Taken from "client_email" key in JSON file.
#define PRIVATE_KEY_ID "Private key ID" //Taken from "private_key_id" in JSON file.
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\\n-----END PRIVATE KEY-----\n"; //Taken from "private_key" key in JSON file.

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

    /* Assign the sevice account credentials and private key (required) */
    config.service_account.data.client_email = CLIENT_EMAIL;
    config.service_account.data.project_id = PROJECT_ID;
    config.service_account.data.private_key = PRIVATE_KEY;

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

    /* Create token */
    Signer.begin(&config);

    //The WiFi connection can be processed before or after Signer.begin

    //Call Signer.getExpiredTimestamp() to get the token expired timestamp (seconds from midnight Jan 1, 1970)

    //Call Signer.refreshToken() to force refresh the token.
}

void loop()
{
    delay(1000);

    /* Check for token generation ready state and also refresh the access token if it expired */
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
