# Google OAuth2.0 Access Token generation Arduino Library


This is the library will create the OAuth2.0 access token used in the Google API's http request (REST).

The Service Account credentials i.e. project_id, client_email and private_key which are available in the service account key file will be used.

See [How to Create Service Account Private Key](#how-to-create-service-account-private-key) below.

This library supports most Arduino devices except for AVR platform. 

This library supports native ethernet for ESP8266 and ESP32 and SPI Ethernet for Raspberry Pi Pico.

And other network interface clients e.g., WiFiClient, EthernetClient and GSMClient are also supported.


## Tested Devices

 * Wemos D1 Mini
 * NodeMCU (ESP8266)
 * ESP-12F
 * LinkNode (ESP8266)
 * Sparkfun ESP32 Thing
 * NodeMCU-32
 * WEMOS LOLIN32
 * TTGO T8 V1.8
 * Arduino MKR WiFi 1010
 * Arduino MKR 1000 WIFI
 * Arduino Nano 33 IoT
 * Arduino MKR Vidor 4000
 * Raspberry Pi Pico (RP2040)




## Dependencies


This library required **ESP8266, ESP32 and Raspberry Pi Pico Arduino Core SDK** to be installed.

To install device SDK, in Arduino IDE, ESP8266, ESP32 and Pico Core SDK can be installed through **Boards Manager**. 

In PlatfoemIO IDE, ESP32 and ESP8266 devices's Core SDK can be installed through **PIO Home** > **Platforms** > **Espressif 8266 or Espressif 32**.


### RP2040 Arduino SDK installation

For Arduino IDE, the Arduino-Pico SDK can be installed from Boards Manager by searching pico and choose Raspberry Pi Pico/RP2040 to install.

For PlatformIO, the Arduino-Pico SDK can be installed via platformio.ini

```ini
[env:rpipicow]
platform = https://github.com/maxgerhardt/platform-raspberrypi.git
board = rpipicow
framework = arduino
board_build.core = earlephilhower
monitor_speed = 115200
board_build.filesystem_size = 1m
```

See this Arduino-Pico SDK [documentation](https://arduino-pico.readthedocs.io/en/latest/) for more information.




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

3. Click at + CREAT SERVICE ACCOUNT.

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

In the following stepts (15-16) for saving the Service Account Credential in flash memory at compile time.

If you want to allow library to read the Service Account JSON key file directly at run time, skip these steps.

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
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>)
#include <WiFiS3.h>
#endif

#include <ESPSigner.h>

#define WIFI_SSID "..."
#define WIFI_PASSWORD "..."

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
WiFiMulti multi;
#endif

SignerConfig config;

void tokenStatusCallback(TokenInfo info);

void setup()
{

  Serial.begin(115200);
  Serial.println();
  Serial.println();

#if defined(ESP32) || defined(ESP8266)
  WiFi.setAutoReconnect(true);
#endif

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
  multi.addAP(WIFI_SSID, WIFI_PASSWORD);
  multi.run();
#else
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
#endif

  Serial.print("Connecting to Wi-Fi");
  unsigned long ms = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
    if (millis() - ms > 10000)
      break;
#endif
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // The WiFi credentials are required for Pico W
  // due to it does not have reconnect feature.
#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
  config.wifi.clearAP();
  config.wifi.addAP(WIFI_SSID, WIFI_PASSWORD);
#endif


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
    int t = Signer.getExpiredTimestamp() - config.signer.preRefreshSeconds - Signer.getCurrentTimestamp();
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



#### Assign external Arduino Client and required callback fumctions.

param **`client`** The pointer to Arduino Client derived class of SSL Client.

param **`networkConnectionCB`** The function that handles the network connection.

param **`networkStatusCB`** The function that handle the network connection status acknowledgement.

Due to the client pointer is assigned, to avoid dangling pointer, client should be existed as long as it was used for transportation.

```cpp
void setExternalClient(Client *client, ESP_Signer_NetworkConnectionRequestCallback networkConnectionCB,
                           ESP_Signer_NetworkStatusRequestCallback networkStatusCB);
```


#### Assign TinyGsm Clients.

param **`client`** The pointer to TinyGsmClient.

param **`modem`** The pointer to TinyGsm modem object. Modem should be initialized and/or set mode before transfering data.

param **`pin`** The SIM pin.

param **`apn`** The GPRS APN (Access Point Name).

param **`user`** The GPRS user.

param **`password`** The GPRS password.

Due to the client and modem pointers are assigned, to avoid dangling pointer, client should be existed as long as it was used for transportation.

```cpp
void setGSMClient(Client *client, void *modem, const char *pin, const char *apn, const char *user, const char *password);
```



####  Set the network status acknowledgement.

param **`status`** The network status.

```cpp
void setNetworkStatus(bool status);
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


#### Get the current timestamp.

return **`timestamp`**

```cpp
uint64_t getCurrentTimestamp();
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


#### Initiate SD card with SPI port configuration.

param **`ss`** The SPI Chip/Slave Select pin.

param **`sck`** The SPI Clock pin.

param **`miso`** The SPI MISO pin.

param **`mosi`** The SPI MOSI pin.

aram **`frequency`** The SPI frequency.

return **`boolean`** The boolean value indicates the success of operation.

```cpp
bool sdBegin(int8_t ss = -1, int8_t sck = -1, int8_t miso = -1, int8_t mosi = -1, uint32_t frequency = 4000000);
```


#### Initiate SD card with SD FS configurations (ESP8266 only).

param **`ss`** SPI Chip/Slave Select pin.

param **`sdFSConfig`** The pointer to SDFSConfig object (ESP8266 only).

return **`boolean`** type status indicates the success of the operation.

```cpp
  bool sdBegin(SDFSConfig *sdFSConfig);
```


#### Initiate SD card with chip select and SPI configuration (ESP32 only).

param **`ss`** The SPI Chip/Slave Select pin.

param **`spiConfig`** The pointer to SPIClass object for SPI configuartion.

param **`frequency`** The SPI frequency.

return **`boolean`** The boolean value indicates the success of operation.

```cpp
bool sdBegin(int8_t ss, SPIClass *spiConfig = nullptr, uint32_t frequency = 4000000);
```


#### Initiate SD card with SdFat SPI and pins configurations (with SdFat included only).

param **`sdFatSPIConfig`** The pointer to SdSpiConfig object for SdFat SPI configuration.

param **`ss`** The SPI Chip/Slave Select pin.

param **`sck`** The SPI Clock pin.

param **`miso`** The SPI MISO pin.

param **`mosi`** The SPI MOSI pin.

return **`boolean`** The boolean value indicates the success of operation.

```cpp
 bool sdBegin(SdSpiConfig *sdFatSPIConfig, int8_t ss = -1, int8_t sck = -1, int8_t miso = -1, int8_t mosi = -1);
```


#### Initiate SD card with SdFat SDIO configuration (with SdFat included only).

param **`sdFatSDIOConfig`** The pointer to SdioConfig object for SdFat SDIO configuration.

return **`boolean`** The boolean value indicates the success of operation.

```cpp
 bool sdBegin(SdioConfig *sdFatSDIOConfig);
```


#### Initialize the SD_MMC card (ESP32 only).

param **`mountpoint`** The mounting point.

param **`mode1bit`** Allow 1 bit data line (SPI mode).

param **`format_if_mount_failed`** Format SD_MMC card if mount failed.

return **`Boolean`** type status indicates the success of the operation.

```cpp
bool sdMMCBegin(const char *mountpoint = "/sdcard", bool mode1bit = false, bool format_if_mount_failed = false);
```


#### Initiate SD card with SdFat SDIO configuration (with SdFat included only).

param **`sdFatSDIOConfig`** The pointer to SdioConfig object for SdFat SDIO configuration.

return **`boolean`** The boolean value indicates the success of operation.

```cpp
 bool sdBegin(SdioConfig *sdFatSDIOConfig);
```

#### Formatted printing on Serial.

```cpp
void printf(const char *format, ...);
```


#### Get free Heap memory.

return **`Free memory amount in byte`**

```cpp
int getFreeHeap();
```

## License

The MIT License (MIT)

Copyright (c) 2023 K. Suwatchai (Mobizt)


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

