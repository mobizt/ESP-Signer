# Google OAuth2.0 Access Token generation Arduino Library v1.1.4


This is the library will create the OAuth2.0 access token used in the Google API's http request (REST).

The Service Account credentials i.e. project_id, client_email and private_key which are available in the service account key file will be used.

See [How to Create Service Account Private Key](#how-to-create-service-account-private-key) below.

This library supports ESP8266 and ESP32 MCU from Espressif. 

Supports ethernet in ESP32 using LAN8720, TLK110 and IP101 Ethernet modules and ESP8266 using ENC28J60, W5100 and W5500 Ethernet modules.


## Tested Devices

 * Wemos D1 Mini
 * NodeMCU (ESP8266)
 * ESP-12F
 * LinkNode (ESP8266)
 * Sparkfun ESP32 Thing
 * NodeMCU-32
 * WEMOS LOLIN32
 * TTGO T8 V1.8



## Dependencies


This library required **ESP8266 or ESP32 Core SDK**.

For Arduino IDE, ESP8266 Core SDK can be installed through **Boards Manager**. 

For PlatfoemIO IDE, ESP8266 Core SDK can be installed through **PIO Home** > **Platforms** > **Espressif 8266 or Espressif 32**.



## Installation


For Arduino IDE, download zip file from the repository (Github page) by select **Code** dropdown at the top of repository, select **Download ZIP** 

From Arduino IDE, select menu **Sketch** -> **Include Library** -> **Add .ZIP Library...**.

Choose **ESP-Signer-main.zip** that previously downloaded.

Go to menu **Files** -> **Examples** -> **ESP-Signer-main** and choose one from examples.



## How to Create Service Account Private Key


Go to [Google Cloud Console](https://console.cloud.google.com/projectselector2/iam-admin/settings).

1. Choose or create project to create Service Account.

2. Choose Service Accounts

![Select Project](/media/images/GC_Select_Project.png)

3. Click at ADD SERVICE ACCOUNT.

![Create SA](/media/images/GC_Create_SA.png)

4. Enter the Service account name, 

5. Service account ID and

6. Click at CREATE AND CONTINUE

![Create SA2](/media/images/GC_Create_SA2.png)

7. Select Role.

8. Click at CONTINUE.

![Create SA3](/media/images/GC_Create_SA3.png)

9. Click at DONE.

![Create SA4](/media/images/GC_Create_SA4.png)

10. Choose service account that recently created from the list.

![Create SA5](/media/images/GC_Create_SA5.png)

11. Choose KEYS.

![Create SA6](/media/images/GC_Create_SA6.png)

12. Click ADD KEY and choose Create new key.

![Create SA7](/media/images/GC_Create_SA7.png)

13. Choose JSON for Key type and click CREATE. 


![Create SA8](/media/images/GC_Create_SA8.png)

14. Private key will be created for this service account and downloaded to your computer, click CLOSE. 


![Create SA9](/media/images/GC_Create_SA9.png)

15. Open the .json file that is already downloaded with text editor.

```json
{
  "type": "service_account",
  "project_id": "...",
  "private_key_id": "...",
  "private_key": "-----BEGIN PRIVATE KEY-----\n...\n-----END PRIVATE KEY-----\n",
  "client_email": "...",
  "client_id": "...",
  "auth_uri": "https://accounts.google.com/o/oauth2/auth",
  "token_uri": "https://oauth2.googleapis.com/token",
  "auth_provider_x509_cert_url": "https://www.googleapis.com/oauth2/v1/certs",
  "client_x509_cert_url": "..."
}

```
16. Copy project_id, client_email, private_key_id and private_key from .json file and paste to these defines in the example.

```cpp
#define PROJECT_ID "..." //Taken from "project_id" key in JSON file.
#define CLIENT_EMAIL "..." //Taken from "client_email" key in JSON file.
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\n...\n-----END PRIVATE KEY-----\n"; //Taken from "private_key" key in JSON file.
```

## Usages

```cpp

#include <Arduino.h>
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <ESPSigner.h>

SignerConfig config;

void tokenStatusCallback(TokenInfo info);

void setup()
{

    Serial.begin(115200);
    Serial.println();
    Serial.println();

    WiFi.setAutoReconnect(true);

    WiFi.begin("WiFi SSID", "WIFi PSK");
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
    config.service_account.data.client_email = "...";
    config.service_account.data.project_id = "...";
    config.service_account.data.private_key = "...";

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
        //Token will be refreshed automatically

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

```

See [this](/examples) for complete usages.



 ## Use SRAM/PSRAM in ESP32 and ESP8266


To enable PSRAM in ESP32 module with on-board PSRAM chip, in Arduino IDE

![Enable PSRAM in ESP32](/media/images/ESP32-PSRAM.png)


In PlatformIO in VSCode IDE, add the following build_flags in your project's platformio.ini file

```ini
build_flags = -DBOARD_HAS_PSRAM -mfix-esp32-psram-cache-issue
```

*When config the IDE or add the build flags to use PSRAM in the ESP32 dev boards that do not have on-board PSRAM chip, your device will be crashed (reset).


In ESP8266, to use external Heap from 1 Mbit SRAM 23LC1024, choose the MMU **option 5**, 128K External 23LC1024.

![MMU VM 128K](/media/images/ESP8266_VM.png)

To use external Heap from PSRAM, choose the MMU **option 6**, 1M External 64 MBit PSRAM.

In PlatformIO, **PIO_FRAMEWORK_ARDUINO_MMU_EXTERNAL_128K** or **PIO_FRAMEWORK_ARDUINO_MMU_EXTERNAL_1024K** build flag should be assigned in platformio.ini.

```ini
[env:d1_mini]
platform = espressif8266
build_flags = -D PIO_FRAMEWORK_ARDUINO_MMU_EXTERNAL_128K
board = d1_mini
framework = arduino
monitor_speed = 115200
```

The connection between SRAM/PSRAM and ESP8266

```
23LC1024/ESP-PSRAM64                ESP8266

CS (Pin 1)                          GPIO15
SCK (Pin 6)                         GPIO14
MOSI (Pin 5)                        GPIO13
MISO (Pin 2)                        GPIO12
/HOLD (Pin 7 on 23LC1024 only)      3V3
Vcc (Pin 8)                         3V3
Vcc (Pin 4)                         GND
```


To use SRAM/PSRAM in this library, the macro in file [**FS_Config.h**](src/FS_Config.h) was set.

```cpp
#define ESP_SIGNER_USE_PSRAM
```



## Functions Descriptions



#### Begin the Access token generation.

param **`config`** The pointer to SignerConfig structured data contains the authentication credentials.

```cpp
void begin(SignerConfig *config);
```


#### End the Access token generation.

```cpp
void end();
```


#### Check the token ready state and trying to re-generate the token when expired.

retuen **`Boolean`** of ready state.

```cpp
bool tokenReady();
```


#### Get the generated access token.

retuen **`String`** of OAuth2.0 access token.

```cpp
String accessToken();
```


#### Get the token type string.

param  **`info`** The TokenInfo structured data contains token info.

retuen **`String`** of token type.

```cpp
String getTokenType();

String getTokenType(TokenInfo info);
```


#### Get the token status string.

param  **`info`** The TokenInfo structured data contains token info.

retuen **`String`** of token status.

```cpp
String getTokenStatus();

String getTokenStatus(TokenInfo info);
```


#### Get the token generation error string.

param  **`info`** The TokenInfo structured data contains token info.

retuen **`String`** of token error.

```cpp
String getTokenError();

String getTokenError(TokenInfo info);
```


#### Get the token expiration timestamp (seconds from midnight Jan 1, 1970).

retuen **`unsigned long`** of timestamp.

```cpp
unsigned long getExpiredTimestamp();
```


#### Refresh the access token

```cpp
void refreshToken();
```


#### Set system time with timestamp.

param  **`ts`** timestamp in seconds from midnight Jan 1, 1970.

retuen **`Boolean`** type status indicates the success of the operation.

```cpp
bool setSystemTime(time_t ts);
```



#### SD card config with GPIO pins.

param **`ss`** SPI Chip/Slave Select pin.

param **`sck`** SPI Clock pin.

param **`miso`** SPI MISO pin.

param **`mosi`** SPI MOSI pin.

return **`Boolean`** type status indicates the success of the operation.

```cpp
bool sdBegin( int8_t ss = -1, int8_t sck = -1, int8_t miso = -1, int8_t mosi = -1);
```



#### Initialize the SD_MMC card (ESP32 only).

param **`mountpoint`** The mounting point.

param **`mode1bit`** Allow 1 bit data line (SPI mode).

param **`format_if_mount_failed`** Format SD_MMC card if mount failed.

return **`Boolean`** type status indicates the success of the operation.

```cpp
bool sdMMCBegin(<string> mountpoint = "/sdcard", bool mode1bit = false, bool format_if_mount_failed = false);
```


## License

The MIT License (MIT)

Copyright (c) 2022 K. Suwatchai (Mobizt)


Permission is hereby granted, free of charge, to any person returning a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

