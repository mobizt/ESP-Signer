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

/** These credentials are taken from Service Account key file (JSON)
 * https://cloud.google.com/iam/docs/service-accounts
*/
#define PROJECT_ID "The project ID"
#define CLIENT_EMAIL "Client Email"
#define PRIVATE_KEY_ID "e172ba5eb712aea49938280fe3be3b9508dfbe0f"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDQz8GHAQegHm9O\nz/mjEi5DsRW6o7ZtPqv8G2knDESxwesBvzbJxQI/WuVN1Lwn4cI84UFlbBS7p1eK\nE+GRJ8A+tPMa3K1ewCiDBZbiBRLullzhGfEvpPFadoqRWmWYeVrpUGsVp0ZkMWbM\nI6dIInXTT9CeX2CPlrUNzbENq10j+mkcAF7FX+UjoKmfBpTNdS44sxpFz6LbJMrX\nLqdOdGocNROZkYFrZ/UZKRqo1ZnUjpC8rW6kBTcyoonotioZxLU9VPYWEVEUGGcf\nP8ZT86+mVEldN98nUi05XpMhQbbzkVT9S+JYPJIurIx9O59sVukj1D3CBQ1txEIp\nxHFB0IXTAgMBAAECggEAT2NVhDLvh8BaeD/Y/Nc6MLFAfiOlBXbI44I2YcqnyV9d\nLl5ZAbA9sQGpYYRqfs010HihqOJFJa0zOoNhhJL048JfFQzmezoajOnQmiDsuDoB\nmPGIgaE3w7mC6E1Sh/xHhKH2/JYKzKf9mcpKXMIBQvQ68CN57k2Ri2xzDqvirvzi\nVPzD/gUWk9T8YY21N3kfkHFJa1vU6DNqQrRZC6PC1fwGyl1T8ggGNHn/BPas7mPP\nfDrznOiKUQOMTGN45ht+SKyoAwHZvRcnFQsi1tqng5XRkq3DUcTazLWEJkJML65G\nS9X53+Z9knNwuOWHRVi18JVawC0O7WzQBzyFcBMD0QKBgQD/KwIWWBqN8y4rMvo0\na2a6xGapCm4BSbr0HAyMYJXgM/MQW6PGv0qT0Qhm9inLFXtmUI+Me91B5TEOavej\nfDT/dTUaHV8wExc19Bwbon5yfziUeyWkEZ1WEd+Q25P8Gv01t7SIHpey53ucDL9t\nMgIRnZl2ReJE2DirR9Q3OOxkUQKBgQDRfg2zIglOh2CpGzbZBa6E8hVc9ZCXoJXV\ndKu8w+PyyPLDl/i1kUNd6roc6Zh6QT966TEXxlpxovfUznTcdjJJbqVjM0lZUrzq\n3p/Oscq86qP1MfibTv6EUSPLgdsYv5OSnYVqnS2H+QI1uIVb8rBU7KqpwX/vPyAi\nzDZ8Xq/y4wKBgBf35j5LEFN9ID0WkMITxJaOlEYUoLimQFVlvRgCipGAz7gdo4ir\nt0mjPPLO9KYK4oh90L4VdHcYHD+KdLB6nk/QpqhJUgOmB2wd/fXqkY4XvSzVVHfQ\n38KR8zocJg5sSpSdKOwQU5eOfuHtoJ2VMPSOpTei4NoupUQRfzGguzPBAoGBAL9B\njdBU6po/DghVSCWqAkJtoQNVTcSgXyqZcZDMZndo7fmT7QQWsqIzgeaTfRlgKExW\nSPOjqz7Dwe8O7bG4VwCje2qtj/F0j/T9eniB6M3Aih94l7dyzvqN+Vf6HcTiYiAg\nn4VOKhtRQqs44senMUTXLJf+iIYr43LhbXzuFURxAoGBAJruUO5E+HOpX4e4gRGT\nszy9v0zxS4m8Mro6Tdxylk8iondJHnJL515cuC2IwCj5JOj5tSzTFNYjy2+b7R9t\nNJs0uuRZuPxnog2OvsJpICI0PyU1Tq9lxflw46lPXmpNZkFtKmbelVpQlH3UvwuV\nUBegcEvgBSXRPrZUCgKgHVG+\n-----END PRIVATE KEY-----\n";


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

/* Assign the sevice account credentials and private key (required) */
config.service_account.data.client_email = CLIENT_EMAIL;
config.service_account.data.project_id = PROJECT_ID;
config.service_account.data.private_key = PRIVATE_KEY;

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
