/**
 * Google's OAuth2.0 Access token Generation class, Signer.h version 1.0.7
 * 
 * This library use RS256 for signing algorithm.
 * 
 * The signed JWT token will be generated and exchanged with the access token in the final generating process.
 * 
 * This library supports Espressif ESP8266 and ESP32
 * 
 * Created June 5, 2021
 * 
 * This work is a part of Firebase ESP Client library
 * Copyright (c) 2021 K. Suwatchai (Mobizt)
 * 
 * The MIT License (MIT)
 * Copyright (c) 2021 K. Suwatchai (Mobizt)
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
#include "ESPSigner.h"

ESP_Signer::ESP_Signer()
{
}

ESP_Signer::~ESP_Signer()
{
    if (ut)
        delete ut;
}

void ESP_Signer::begin(SignerConfig *cfg)
{
    if (ut)
        delete ut;
    config = cfg;
    ut = new SignerUtils(config);
    std::string().swap(config->signer.tokens.error.message);

    config->_int.esp_signer_reconnect_wifi = WiFi.getAutoReconnect();

    if (config->service_account.json.path.length() > 0)
    {
        if (!parseSAFile())
            config->signer.tokens.status = esp_signer_token_status_uninitialized;
    }

    if (tokenSigninDataReady())
        config->signer.tokens.token_type = esp_signer_token_type_oauth2_access_token;

    if (strlen_P(config->cert.data))
        config->_int.esp_signer_caCert = config->cert.data;

    if (config->cert.file.length() > 0)
    {
        if (config->cert.file_storage == esp_signer_mem_storage_type_sd && !config->_int.esp_signer_sd_rdy)
            config->_int.esp_signer_sd_rdy = ut->sdTest(config->_int.esp_signer_file);
        else if ((config->cert.file_storage == esp_signer_mem_storage_type_flash) && !config->_int.esp_signer_flash_rdy)
            ut->flashTest();
    }

    handleToken();
}

bool ESP_Signer::parseSAFile()
{
    if (config->signer.pk.length() > 0)
        return false;

    if (config->service_account.json.storage_type == esp_signer_mem_storage_type_sd && !config->_int.esp_signer_sd_rdy)
        config->_int.esp_signer_sd_rdy = ut->sdTest(config->_int.esp_signer_file);
    else if (config->service_account.json.storage_type == esp_signer_mem_storage_type_flash && !config->_int.esp_signer_flash_rdy)
        ut->flashTest();

    if (config->_int.esp_signer_sd_rdy || config->_int.esp_signer_flash_rdy)
    {
        if (config->service_account.json.storage_type == esp_signer_mem_storage_type_flash)
        {
            if (FLASH_FS.exists(config->service_account.json.path.c_str()))
                config->_int.esp_signer_file = FLASH_FS.open(config->service_account.json.path.c_str(), "r");
        }
        else
        {
            if (SD_FS.exists(config->service_account.json.path.c_str()))
                config->_int.esp_signer_file = SD_FS.open(config->service_account.json.path.c_str(), "r");
        }

        if (config->_int.esp_signer_file)
        {
            clearSA();
            config->signer.json = new FirebaseJson();
            config->signer.data = new FirebaseJsonData();
            char *tmp = nullptr;

            size_t len = config->_int.esp_signer_file.size();
            char *buf = ut->newS(len + 10);
            if (config->_int.esp_signer_file.available())
            {
                config->_int.esp_signer_file.readBytes(buf, len);
                config->signer.json->setJsonData(buf);
            }
            config->_int.esp_signer_file.close();
            ut->delS(buf);

            tmp = ut->strP(esp_signer_pgm_str_13);
            config->signer.json->get(*config->signer.data, (const char *)tmp);
            ut->delS(tmp);
            if (config->signer.data->success)
            {
                if (ut->strposP(config->signer.data->stringValue.c_str(), esp_signer_pgm_str_14, 0) > -1)
                {
                    tmp = ut->strP(esp_signer_pgm_str_15);
                    config->signer.json->get(*config->signer.data, (const char *)tmp);
                    ut->delS(tmp);
                    if (config->signer.data->success)
                        config->service_account.data.project_id = config->signer.data->stringValue.c_str();
                    tmp = ut->strP(esp_signer_pgm_str_16);
                    config->signer.json->get(*config->signer.data, (const char *)tmp);
                    ut->delS(tmp);
                    if (config->signer.data->success)
                        config->service_account.data.private_key_id = config->signer.data->stringValue.c_str();
                    tmp = ut->strP(esp_signer_pgm_str_17);
                    config->signer.json->get(*config->signer.data, (const char *)tmp);
                    ut->delS(tmp);
                    if (config->signer.data->success)
                    {
                        tmp = ut->newS(config->signer.data->stringValue.length());
                        size_t c = 0;
                        for (size_t i = 0; i < config->signer.data->stringValue.length(); i++)
                        {
                            if (config->signer.data->stringValue[i] == '\\')
                            {
                                delay(0);
                                tmp[c++] = '\n';
                                i++;
                            }
                            else
                                tmp[c++] = config->signer.data->stringValue[i];
                        }
                        config->signer.pk = tmp;
                        config->signer.data->stringValue.clear();
                        ut->delS(tmp);
                    }

                    tmp = ut->strP(esp_signer_pgm_str_18);
                    config->signer.json->get(*config->signer.data, (const char *)tmp);
                    ut->delS(tmp);
                    if (config->signer.data->success)
                        config->service_account.data.client_email = config->signer.data->stringValue.c_str();
                    tmp = ut->strP(esp_signer_pgm_str_19);
                    config->signer.json->get(*config->signer.data, (const char *)tmp);
                    ut->delS(tmp);
                    if (config->signer.data->success)
                        config->service_account.data.client_id = config->signer.data->stringValue.c_str();

                    delete config->signer.json;
                    delete config->signer.data;
                    return true;
                }
            }

            delete config->signer.json;
            delete config->signer.data;
        }
    }

    return false;
}

void ESP_Signer::clearSA()
{
    config->service_account.data.private_key = "";
    config->service_account.data.project_id.clear();
    config->service_account.data.private_key_id.clear();
    config->service_account.data.client_email.clear();
    std::string().swap(config->signer.pk);
}

bool ESP_Signer::tokenSigninDataReady()
{
    if (!config)
        return false;
    return (strlen_P(config->service_account.data.private_key) > 0 || config->signer.pk.length() > 0) && config->service_account.data.client_email.length() > 0 && config->service_account.data.project_id.length() > 0;
}

bool ESP_Signer::handleToken()
{
    if (!config)
        return false;

    //if the time was set (changed) after token has been generated, update its expiration
    if (config->signer.tokens.expires > 0 && config->signer.tokens.expires < ESP_DEFAULT_TS && time(nullptr) > ESP_DEFAULT_TS)
        config->signer.tokens.expires += time(nullptr) - (millis() - config->signer.tokens.last_millis) / 1000 - 60;

    if (config->signer.tokens.token_type == esp_signer_token_type_oauth2_access_token && (time(nullptr) > config->signer.tokens.expires - config->signer.preRefreshSeconds || config->signer.tokens.expires == 0))
    {

        if (config->signer.step == esp_signer_jwt_generation_step_begin)
        {

            if (!config->signer.tokenTaskRunning)
            {
                if (config->service_account.json.path.length() > 0 && config->signer.pk.length() == 0)
                {
                    if (!parseSAFile())
                        config->signer.tokens.status = esp_signer_token_status_uninitialized;
                }

                config->signer.tokens.status = esp_signer_token_status_on_initialize;
                config->signer.tokens.error.code = 0;
                config->signer.tokens.error.message.clear();
                sendTokenStatusCB();

                _token_processing_task_enable = true;
                tokenProcessingTask();
            }
        }
    }

    if (config->signer.tokens.token_type == esp_signer_token_type_undefined)
        setTokenError(ESP_SIGNER_ERROR_TOKEN_NOT_READY);

    return config->signer.tokens.status == esp_signer_token_status_ready;
}

void ESP_Signer::tokenProcessingTask()
{
#if defined(ESP32)

    if (config->signer.tokenTaskRunning)
        return;

    static ESP_Signer *_this = this;

    TaskFunction_t taskCode = [](void *param) {
        _this->config->signer.tokenTaskRunning = true;

        while (_this->_token_processing_task_enable)
        {
            vTaskDelay(20 / portTICK_PERIOD_MS);

            if (_this->config->signer.step == esp_signer_jwt_generation_step_begin)
            {
                _this->ut->setClock(_this->config->time_zone);
                time_t now = time(nullptr);
                _this->config->_int.esp_signer_clock_rdy = now > _this->ut->default_ts;

                if (_this->config->_int.esp_signer_clock_rdy)
                    _this->config->signer.step = esp_signer_jwt_generation_step_encode_header_payload;
            }
            else if (_this->config->signer.step == esp_signer_jwt_generation_step_encode_header_payload)
            {
                if (_this->createJWT())
                    _this->config->signer.step = esp_signer_jwt_generation_step_sign;
            }
            else if (_this->config->signer.step == esp_signer_jwt_generation_step_sign)
            {
                if (_this->createJWT())
                    _this->config->signer.step = esp_signer_jwt_generation_step_exchange;
            }
            else if (_this->config->signer.step == esp_signer_jwt_generation_step_exchange)
            {
                if (_this->requestTokens())
                {
                    _this->config->signer.attempts = 0;
                    _this->_token_processing_task_enable = false;
                    _this->config->signer.step = esp_signer_jwt_generation_step_begin;
                    break;
                }
                else
                {
                    if (_this->config->signer.attempts < _this->config->max_token_generation_retry)
                        _this->config->signer.attempts++;
                    else
                    {
                        _this->config->signer.tokens.error.message.clear();
                        _this->setTokenError(ESP_SIGNER_ERROR_TOKEN_EXCHANGE_MAX_RETRY_REACHED);
                        _this->sendTokenStatusCB();
                        _this->config->signer.attempts = 0;
                        _this->config->signer.step = esp_signer_jwt_generation_step_begin;
                        break;
                    }
                }
            }

            yield();
        }
        _this->config->_int.token_processing_task_handle = NULL;
        _this->config->signer.tokenTaskRunning = false;
        vTaskDelete(NULL);
    };

    char *taskname = ut->strP(esp_signer_pgm_str_20);
    xTaskCreatePinnedToCore(taskCode, taskname, 12000, NULL, 3, &config->_int.token_processing_task_handle, 1);
    ut->delS(taskname);

#elif defined(ESP8266)

    if (_token_processing_task_enable)
    {
        if (config->signer.step == esp_signer_jwt_generation_step_begin)
        {
            config->signer.tokenTaskRunning = true;
            ut->setClock(config->time_zone);
            time_t now = time(nullptr);
            config->_int.esp_signer_clock_rdy = now > ut->default_ts;

            if (config->_int.esp_signer_clock_rdy)
                config->signer.step = esp_signer_jwt_generation_step_encode_header_payload;
        }
        else if (config->signer.step == esp_signer_jwt_generation_step_encode_header_payload)
        {
            if (createJWT())
                config->signer.step = esp_signer_jwt_generation_step_sign;
        }
        else if (config->signer.step == esp_signer_jwt_generation_step_sign)
        {
            if (createJWT())
                config->signer.step = esp_signer_jwt_generation_step_exchange;
        }
        else if (config->signer.step == esp_signer_jwt_generation_step_exchange)
        {
            if (requestTokens())
            {
                config->signer.tokenTaskRunning = false;
                _token_processing_task_enable = false;
                config->signer.attempts = 0;
                config->signer.step = esp_signer_jwt_generation_step_begin;
                return;
            }
            else
            {
                if (config->signer.attempts < config->max_token_generation_retry)
                    config->signer.attempts++;
                else
                {
                    config->signer.tokens.error.message.clear();
                    setTokenError(ESP_SIGNER_ERROR_TOKEN_EXCHANGE_MAX_RETRY_REACHED);
                    sendTokenStatusCB();
                    config->signer.tokenTaskRunning = false;
                    _token_processing_task_enable = false;
                    config->signer.attempts = 0;
                    config->signer.step = esp_signer_jwt_generation_step_begin;
                    return;
                }
            }
        }

        set_scheduled_callback(std::bind(&ESP_Signer::tokenProcessingTask, this));
    }

#endif
}

void ESP_Signer::setTokenError(int code)
{
    if (code != 0)
        config->signer.tokens.status = esp_signer_token_status_error;
    else
    {
        config->signer.tokens.error.message.clear();
        config->signer.tokens.status = esp_signer_token_status_ready;
    }

    config->signer.tokens.error.code = code;

    if (config->signer.tokens.error.message.length() == 0)
    {
        config->_int.esp_signer_processing = false;
        switch (code)
        {
        case ESP_SIGNER_ERROR_TOKEN_SET_TIME:
            ut->appendP(config->signer.tokens.error.message, esp_signer_pgm_str_21, true);
            break;
        case ESP_SIGNER_ERROR_TOKEN_PARSE_PK:
            ut->appendP(config->signer.tokens.error.message, esp_signer_pgm_str_22, true);
            break;
        case ESP_SIGNER_ERROR_TOKEN_CREATE_HASH:
            ut->appendP(config->signer.tokens.error.message, esp_signer_pgm_str_23, true);
            break;
        case ESP_SIGNER_ERROR_TOKEN_SIGN:
            ut->appendP(config->signer.tokens.error.message, esp_signer_pgm_str_24, true);
            break;
        case ESP_SIGNER_ERROR_TOKEN_EXCHANGE:
            ut->appendP(config->signer.tokens.error.message, esp_signer_pgm_str_25, true);
            break;
        case ESP_SIGNER_ERROR_TOKEN_NOT_READY:
            ut->appendP(config->signer.tokens.error.message, esp_signer_pgm_str_26, true);
            break;
        case ESP_SIGNER_ERROR_TOKEN_EXCHANGE_MAX_RETRY_REACHED:
            ut->appendP(config->signer.tokens.error.message, esp_signer_pgm_str_27, true);
            break;
        case ESP_SIGNER_ERROR_HTTPC_ERROR_NOT_CONNECTED:
            ut->appendP(config->signer.tokens.error.message, esp_signer_pgm_str_28);
            break;
        case ESP_SIGNER_ERROR_HTTPC_ERROR_CONNECTION_LOST:
            ut->appendP(config->signer.tokens.error.message, esp_signer_pgm_str_29);
            break;
        case ESP_SIGNER_ERROR_HTTP_CODE_REQUEST_TIMEOUT:
            ut->appendP(config->signer.tokens.error.message, esp_signer_pgm_str_30);
            break;

        default:
            break;
        }
    }
}

bool ESP_Signer::sdBegin(int8_t ss, int8_t sck, int8_t miso, int8_t mosi)
{
    if (config)
    {
        config->_int.sd_config.sck = sck;
        config->_int.sd_config.miso = miso;
        config->_int.sd_config.mosi = mosi;
        config->_int.sd_config.ss = ss;
    }
#if defined(ESP32)
    if (ss > -1)
    {
        SPI.begin(sck, miso, mosi, ss);
        return SD_FS.begin(ss, SPI);
    }
    else
        return SD_FS.begin();
#elif defined(ESP8266)
    if (ss > -1)
        return SD_FS.begin(ss);
    else
        return SD_FS.begin(SD_CS_PIN);
#endif
    return false;
}

bool ESP_Signer::handleSignerError(int code)
{

    switch (code)
    {

    case 1:
        config->signer.tokens.error.message.clear();
        setTokenError(ESP_SIGNER_ERROR_HTTPC_ERROR_NOT_CONNECTED);
        sendTokenStatusCB();
        break;
    case 2:
#if defined(ESP32)
        if (config->signer.wcs->stream())
            config->signer.wcs->stream()->stop();
#elif defined(ESP8266)
        config->signer.wcs->stop();
#endif
        config->signer.tokens.error.message.clear();
        setTokenError(ESP_SIGNER_ERROR_HTTPC_ERROR_CONNECTION_LOST);
        sendTokenStatusCB();
        break;
    case 3:
#if defined(ESP32)
        if (config->signer.wcs->stream())
            config->signer.wcs->stream()->stop();
#elif defined(ESP8266)
        config->signer.wcs->stop();
#endif
        config->signer.tokens.error.message.clear();
        setTokenError(ESP_SIGNER_ERROR_HTTP_CODE_REQUEST_TIMEOUT);
        sendTokenStatusCB();
        break;

    default:
        break;
    }

    if (config->signer.wcs)
        delete config->signer.wcs;
    if (config->signer.json)
        delete config->signer.json;
    if (config->signer.data)
        delete config->signer.data;

    config->_int.esp_signer_processing = false;

    if (code > 0 && code < 4)
    {
        config->signer.tokens.status = esp_signer_token_status_error;
        config->signer.tokens.error.code = code;
        return false;
    }
    else if (code == 0)
    {
        config->signer.tokens.error.message.clear();
        config->signer.tokens.status = esp_signer_token_status_ready;
        config->signer.attempts = 0;
        config->signer.step = esp_signer_jwt_generation_step_begin;
        sendTokenStatusCB();
        return true;
    }

    return false;
}

void ESP_Signer::sendTokenStatusCB()
{
    tokenInfo.status = config->signer.tokens.status;
    tokenInfo.type = config->signer.tokens.token_type;
    tokenInfo.error = config->signer.tokens.error;

    if (config->token_status_callback)
        config->token_status_callback(tokenInfo);
}

bool ESP_Signer::handleTokenResponse()
{
    if (config->_int.esp_signer_reconnect_wifi)
        ut->reconnect(0);

    if (WiFi.status() != WL_CONNECTED)
        return false;

    struct esp_signer_server_response_data_t response;

    unsigned long dataTime = millis();

    int chunkIdx = 0;
    int pChunkIdx = 0;
    int payloadLen = 3200;
    int pBufPos = 0;
    int hBufPos = 0;
    int chunkBufSize = 0;
    int hstate = 0;
    int pstate = 0;
    int chunkedDataState = 0;
    int chunkedDataSize = 0;
    int chunkedDataLen = 0;
    int defaultChunkSize = ESP_SIGNER_DEFAULT_RESPONSE_BUFFER_SIZE;
    char *header = nullptr;
    char *payload = nullptr;
    char *pChunk = nullptr;
    char *tmp = nullptr;
    bool isHeader = false;
#if defined(ESP32)
    WiFiClient *stream = config->signer.wcs->stream();
#elif defined(ESP8266)
    WiFiClient *stream = config->signer.wcs;
#endif
    while (stream->connected() && stream->available() == 0)
    {
        if (!ut->reconnect(dataTime))
        {
            if (stream)
                if (stream->connected())
                    stream->stop();
            return false;
        }

        delay(0);
    }

    bool complete = false;
    int readLen = 0;
    unsigned long datatime = millis();
    while (!complete)
    {

        chunkBufSize = stream->available();

        if (chunkBufSize > 1 || !complete)
        {
            while (!complete)
            {
                if (config->_int.esp_signer_reconnect_wifi)
                    ut->reconnect(0);

                delay(0);
                
                if (WiFi.status() != WL_CONNECTED)
                {
                    if (stream)
                        if (stream->connected())
                            stream->stop();
                    return false;
                }
                chunkBufSize = stream->available();

                if (chunkBufSize > 0)
                {
                    chunkBufSize = defaultChunkSize;

                    if (chunkIdx == 0)
                    {
                        header = ut->newS(chunkBufSize);
                        hstate = 1;
                        int readLen = ut->readLine(stream, header, chunkBufSize);

                        int pos = 0;

                        tmp = ut->getHeader(header, esp_signer_pgm_str_31, esp_signer_pgm_str_32, pos, 0);
                        delay(0);
                        if (tmp)
                        {
                            isHeader = true;
                            hBufPos = readLen;
                            response.httpCode = atoi(tmp);
                            ut->delS(tmp);
                        }
                    }
                    else
                    {
                        delay(0);
                        if (isHeader)
                        {
                            tmp = ut->newS(chunkBufSize);
                            int readLen = ut->readLine(stream, tmp, chunkBufSize);
                            bool headerEnded = false;

                            if (readLen == 1)
                                if (tmp[0] == '\r')
                                    headerEnded = true;

                            if (readLen == 2)
                                if (tmp[0] == '\r' && tmp[1] == '\n')
                                    headerEnded = true;

                            if (headerEnded)
                            {
                                isHeader = false;
                                ut->parseRespHeader(header, response);
                                if (hstate == 1)
                                    ut->delS(header);
                                hstate = 0;
                            }
                            else
                            {
                                memcpy(header + hBufPos, tmp, readLen);
                                hBufPos += readLen;
                            }

                            ut->delS(tmp);
                        }
                        else
                        {
                            if (!response.noContent)
                            {
                                pChunkIdx++;

                                pChunk = ut->newS(chunkBufSize + 1);

                                if (!payload || pstate == 0)
                                {
                                    pstate = 1;
                                    payload = ut->newS(payloadLen + 1);
                                }
                                readLen = 0;
                                if (response.isChunkedEnc)
                                    readLen = ut->readChunkedData(stream, pChunk, chunkedDataState, chunkedDataSize, chunkedDataLen, chunkBufSize);
                                else
                                {
                                    if (stream->available() < chunkBufSize)
                                        chunkBufSize = stream->available();
                                    readLen = stream->readBytes(pChunk, chunkBufSize);
                                }

                                if (readLen > 0)
                                    memcpy(payload + pBufPos, pChunk, readLen);

                                ut->delS(pChunk);
                                pBufPos += readLen;
                                if ((response.isChunkedEnc && readLen < 0) || (!response.isChunkedEnc && stream->available() <= 0))
                                    complete = true;
                            }
                            else
                            {
                                while (stream->available() > 0)
                                    stream->read();
                                if (stream->available() <= 0)
                                    break;
                            }
                        }
                    }
                    chunkIdx++;
                }

                if (millis() - datatime > 5000)
                    complete = true;
            }
        }
    }

    if (hstate == 1)
        ut->delS(header);

    if (stream->connected())
        stream->stop();

    if (payload && !response.noContent)
    {
        config->signer.json->setJsonData(payload);
        ut->delS(payload);
        return true;
    }

    return false;
}

bool ESP_Signer::createJWT()
{

    if (config->signer.step == esp_signer_jwt_generation_step_encode_header_payload)
    {
        config->signer.tokens.status = esp_signer_token_status_on_signing;
        config->signer.tokens.error.code = 0;
        config->signer.tokens.error.message.clear();
        sendTokenStatusCB();

        config->signer.json = new FirebaseJson();
        config->signer.data = new FirebaseJsonData();

        unsigned long now = time(nullptr);

        config->signer.tokens.jwt.clear();

        //header
        char *tmp = ut->strP(esp_signer_pgm_str_33);
        char *tmp2 = ut->strP(esp_signer_pgm_str_34);
        config->signer.json->add(tmp, (const char *)tmp2);
        ut->delS(tmp);
        ut->delS(tmp2);
        tmp2 = ut->strP(esp_signer_pgm_str_35);
        tmp = ut->strP(esp_signer_pgm_str_36);
        config->signer.json->add(tmp, (const char *)tmp2);
        ut->delS(tmp);
        ut->delS(tmp2);

        config->signer.header = config->signer.json->raw();
        size_t len = ut->base64EncLen(config->signer.header.length());
        char *buf = ut->newS(len);
        ut->encodeBase64Url(buf, (unsigned char *)config->signer.header.c_str(), config->signer.header.length());
        config->signer.encHeader = buf;
        ut->delS(buf);
        config->signer.header.clear();
        config->signer.encHeadPayload = config->signer.encHeader;

        //payload
        config->signer.json->clear();
        tmp = ut->strP(esp_signer_pgm_str_37);
        config->signer.json->add(tmp, config->service_account.data.client_email.c_str());
        ut->delS(tmp);
        tmp = ut->strP(esp_signer_pgm_str_38);
        config->signer.json->add(tmp, config->service_account.data.client_email.c_str());
        ut->delS(tmp);
        tmp = ut->strP(esp_signer_pgm_str_39);
        std::string t;
        ut->appendP(t, esp_signer_pgm_str_40);
        if (config->signer.tokens.token_type == esp_signer_token_type_oauth2_access_token)
        {
            ut->appendP(t, esp_signer_pgm_str_41);
            ut->appendP(t, esp_signer_pgm_str_42);
            ut->appendP(t, esp_signer_pgm_str_43);
            ut->appendP(t, esp_signer_pgm_str_44);
            ut->appendP(t, esp_signer_pgm_str_45);
        }

        config->signer.json->add(tmp, t.c_str());
        ut->delS(tmp);

        tmp = ut->strP(esp_signer_pgm_str_46);
        config->signer.json->add(tmp, (int)now);
        ut->delS(tmp);

        tmp = ut->strP(esp_signer_pgm_str_47);

        if (config->signer.expiredSeconds > 3600)
            config->signer.json->add(tmp, (int)(now + 3600));
        else
            config->signer.json->add(tmp, (int)(now + config->signer.expiredSeconds));

        ut->delS(tmp);

        if (config->signer.tokens.token_type == esp_signer_token_type_oauth2_access_token)
        {

            std::string s;

            if (config->signer.tokens.scope.length() > 0)
            {
                std::vector<std::string> scopes = std::vector<std::string>();
                ut->splitTk(config->signer.tokens.scope, scopes, ",");
                for (size_t i = 0; i < scopes.size(); i++)
                {
                    if (s.length() > 0)
                        ut->appendP(s, esp_signer_pgm_str_32);
                    s += scopes[i];
                    scopes[i].clear();
                    std::string().swap(scopes[i]);
                }
                scopes.clear();
            }

            tmp = ut->strP(esp_signer_pgm_str_56);
            config->signer.json->add(tmp, s.c_str());
            ut->delS(tmp);
        }

        config->signer.payload = config->signer.json->raw();
        len = ut->base64EncLen(config->signer.payload.length());
        buf = ut->newS(len);
        ut->encodeBase64Url(buf, (unsigned char *)config->signer.payload.c_str(), config->signer.payload.length());
        config->signer.encPayload = buf;
        ut->delS(buf);
        config->signer.payload.clear();

        ut->appendP(config->signer.encHeadPayload, esp_signer_pgm_str_42);
        config->signer.encHeadPayload += config->signer.encPayload;

        std::string().swap(config->signer.encHeader);
        std::string().swap(config->signer.encPayload);

//create message digest from encoded header and payload
#if defined(ESP32)
        config->signer.hash = new uint8_t[config->signer.hashSize];
        int ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (const unsigned char *)config->signer.encHeadPayload.c_str(), config->signer.encHeadPayload.length(), config->signer.hash);
        if (ret != 0)
        {
            char *tmp = ut->newS(100);
            mbedtls_strerror(ret, tmp, 100);
            config->signer.tokens.error.message = tmp;
            ut->delS(tmp);
            setTokenError(ESP_SIGNER_ERROR_TOKEN_CREATE_HASH);
            sendTokenStatusCB();
            delete[] config->signer.hash;
            return false;
        }
#elif defined(ESP8266)
        config->signer.hash = ut->newS(config->signer.hashSize);
        br_sha256_context mc;
        br_sha256_init(&mc);
        br_sha256_update(&mc, config->signer.encHeadPayload.c_str(), config->signer.encHeadPayload.length());
        br_sha256_out(&mc, config->signer.hash);
#endif

        config->signer.tokens.jwt = config->signer.encHeadPayload;
        ut->appendP(config->signer.tokens.jwt, esp_signer_pgm_str_42);
        std::string().swap(config->signer.encHeadPayload);

        delete config->signer.json;
        delete config->signer.data;
    }
    else if (config->signer.step == esp_signer_jwt_generation_step_sign)
    {
        config->signer.tokens.status = esp_signer_token_status_on_signing;

#if defined(ESP32)
        config->signer.pk_ctx = new mbedtls_pk_context();
        mbedtls_pk_init(config->signer.pk_ctx);

        //parse priv key
        int ret = 0;
        if (config->signer.pk.length() > 0)
            ret = mbedtls_pk_parse_key(config->signer.pk_ctx, (const unsigned char *)config->signer.pk.c_str(), config->signer.pk.length() + 1, NULL, 0);
        else if (strlen_P(config->service_account.data.private_key) > 0)
            ret = mbedtls_pk_parse_key(config->signer.pk_ctx, (const unsigned char *)config->service_account.data.private_key, strlen_P(config->service_account.data.private_key) + 1, NULL, 0);

        if (ret != 0)
        {
            char *tmp = ut->newS(100);
            mbedtls_strerror(ret, tmp, 100);
            config->signer.tokens.error.message = tmp;
            ut->delS(tmp);
            setTokenError(ESP_SIGNER_ERROR_TOKEN_PARSE_PK);
            sendTokenStatusCB();
            mbedtls_pk_free(config->signer.pk_ctx);
            delete[] config->signer.hash;
            delete config->signer.pk_ctx;
            return false;
        }

        //generate RSA signature from private key and message digest
        config->signer.signature = new unsigned char[config->signer.signatureSize];
        size_t retSize = 0;
        config->signer.entropy_ctx = new mbedtls_entropy_context();
        config->signer.ctr_drbg_ctx = new mbedtls_ctr_drbg_context();
        mbedtls_entropy_init(config->signer.entropy_ctx);
        mbedtls_ctr_drbg_init(config->signer.ctr_drbg_ctx);
        mbedtls_ctr_drbg_seed(config->signer.ctr_drbg_ctx, mbedtls_entropy_func, config->signer.entropy_ctx, NULL, 0);

        ret = mbedtls_pk_sign(config->signer.pk_ctx, MBEDTLS_MD_SHA256, (const unsigned char *)config->signer.hash, sizeof(config->signer.hash), config->signer.signature, &retSize, mbedtls_ctr_drbg_random, config->signer.ctr_drbg_ctx);
        if (ret != 0)
        {
            char *tmp = ut->newS(100);
            mbedtls_strerror(ret, tmp, 100);
            config->signer.tokens.error.message = tmp;
            ut->delS(tmp);
            setTokenError(ESP_SIGNER_ERROR_TOKEN_SIGN);
            sendTokenStatusCB();
        }
        else
        {
            config->signer.encSignature.clear();
            size_t len = ut->base64EncLen(config->signer.signatureSize);
            char *buf = ut->newS(len);
            ut->encodeBase64Url(buf, config->signer.signature, config->signer.signatureSize);
            config->signer.encSignature = buf;
            ut->delS(buf);

            config->signer.tokens.jwt += config->signer.encSignature;
            std::string().swap(config->signer.pk);
            std::string().swap(config->signer.encSignature);
        }

        delete[] config->signer.signature;
        delete[] config->signer.hash;
        mbedtls_pk_free(config->signer.pk_ctx);
        mbedtls_entropy_free(config->signer.entropy_ctx);
        mbedtls_ctr_drbg_free(config->signer.ctr_drbg_ctx);
        delete config->signer.pk_ctx;
        delete config->signer.entropy_ctx;
        delete config->signer.ctr_drbg_ctx;

        if (ret != 0)
            return false;
#elif defined(ESP8266)
        //RSA private key
        BearSSL::PrivateKey *pk = nullptr;
        delay(0);
        //parse priv key
        if (config->signer.pk.length() > 0)
            pk = new BearSSL::PrivateKey((const char *)config->signer.pk.c_str());
        else if (strlen_P(config->service_account.data.private_key) > 0)
            pk = new BearSSL::PrivateKey((const char *)config->service_account.data.private_key);

        if (!pk)
        {
            setTokenError(ESP_SIGNER_ERROR_TOKEN_PARSE_PK);
            sendTokenStatusCB();
            return false;
        }

        if (!pk->isRSA())
        {
            setTokenError(ESP_SIGNER_ERROR_TOKEN_PARSE_PK);
            sendTokenStatusCB();
            delete pk;
            return false;
        }

        const br_rsa_private_key *br_rsa_key = pk->getRSA();

        //generate RSA signature from private key and message digest
        config->signer.signature = new unsigned char[config->signer.signatureSize];

        delay(0);
        int ret = br_rsa_i15_pkcs1_sign(BR_HASH_OID_SHA256, (const unsigned char *)config->signer.hash, br_sha256_SIZE, br_rsa_key, config->signer.signature);
        delay(0);
        ut->delS(config->signer.hash);

        size_t len = ut->base64EncLen(config->signer.signatureSize);
        char *buf = ut->newS(len);
        ut->encodeBase64Url(buf, config->signer.signature, config->signer.signatureSize);
        config->signer.encSignature = buf;
        ut->delS(buf);
        delete[] config->signer.signature;
        delete pk;
        //get the signed JWT
        if (ret > 0)
        {
            config->signer.tokens.jwt += config->signer.encSignature;
            std::string().swap(config->signer.pk);
            std::string().swap(config->signer.encSignature);
        }
        else
        {
            setTokenError(ESP_SIGNER_ERROR_TOKEN_SIGN);
            sendTokenStatusCB();
            return false;
        }
#endif
    }

    return true;
}

bool ESP_Signer::requestTokens()
{

    if (config->_int.esp_signer_reconnect_wifi)
        ut->reconnect(0);

    if (WiFi.status() != WL_CONNECTED)
        return false;

    delay(0);

    if (config->signer.tokens.status == esp_signer_token_status_on_request || config->signer.tokens.status == esp_signer_token_status_on_refresh || time(nullptr) < ut->default_ts || config->_int.esp_signer_processing)
        return false;

    config->signer.tokens.status = esp_signer_token_status_on_request;
    config->_int.esp_signer_processing = true;
    config->signer.tokens.error.code = 0;
    config->signer.tokens.error.message.clear();
    sendTokenStatusCB();

#if defined(ESP32)
    config->signer.wcs = new ESP_Signer_HTTPClient32();
    config->signer.wcs->setCACert(nullptr);
#elif defined(ESP8266)
    config->signer.wcs = new WiFiClientSecure();
    config->signer.wcs->setInsecure();
    config->signer.wcs->setBufferSizes(512, 512);
#endif
    config->signer.json = new FirebaseJson();
    config->signer.data = new FirebaseJsonData();

    std::string host;
    ut->appendP(host, esp_signer_pgm_str_48);
    ut->appendP(host, esp_signer_pgm_str_42);
    ut->appendP(host, esp_signer_pgm_str_43);

    delay(0);
#if defined(ESP32)
    config->signer.wcs->begin(host.c_str(), 443);
#elif defined(ESP8266)
    int ret = config->signer.wcs->connect(host.c_str(), 443);
    if (ret == 0)
        return handleSignerError(1);
#endif

    String s;
    std::string req;
    ut->appendP(req, esp_signer_pgm_str_57);
    ut->appendP(req, esp_signer_pgm_str_32);

    if (config->signer.tokens.token_type == esp_signer_token_type_oauth2_access_token)
    {
        char *tmp = ut->strP(esp_signer_pgm_str_58);
        char *tmp2 = ut->strP(esp_signer_pgm_str_59);
        config->signer.json->add(tmp, (const char *)tmp2);
        ut->delS(tmp);
        ut->delS(tmp2);
        tmp = ut->strP(esp_signer_pgm_str_60);
        config->signer.json->add(tmp, config->signer.tokens.jwt.c_str());
        ut->delS(tmp);

        ut->appendP(req, esp_signer_pgm_str_44);
        ut->appendP(req, esp_signer_pgm_str_45);
        ut->appendP(req, esp_signer_pgm_str_61);
        ut->appendP(req, esp_signer_pgm_str_62);
        ut->appendP(req, esp_signer_pgm_str_41);
    }

    ut->appendP(req, esp_signer_pgm_str_42);
    ut->appendP(req, esp_signer_pgm_str_43);

    ut->appendP(req, esp_signer_pgm_str_63);
    ut->appendP(req, esp_signer_pgm_str_64);
    ut->appendP(req, esp_signer_pgm_str_65);
    config->signer.json->toString(s);
    char *tmp = ut->intStr(s.length());
    req += tmp;
    ut->delS(tmp);
    ut->appendP(req, esp_signer_pgm_str_63);
    ut->appendP(req, esp_signer_pgm_str_66);
    ut->appendP(req, esp_signer_pgm_str_67);
    ut->appendP(req, esp_signer_pgm_str_63);
    ut->appendP(req, esp_signer_pgm_str_63);

    req += s.c_str();
#if defined(ESP32)
    config->signer.wcs->setInsecure();
    if (config->signer.wcs->send(req.c_str(), "") < 0)
        return handleSignerError(2);
#elif defined(ESP8266)
    size_t sz = req.length();
    size_t len = config->signer.wcs->print(req.c_str());
    std::string().swap(req);
    if (len != sz)
        return handleSignerError(2);
#endif

    struct esp_signer_auth_token_error_t error;

    if (handleTokenResponse())
    {
        std::string().swap(config->signer.tokens.jwt);
        tmp = ut->strP(esp_signer_pgm_str_68);
        config->signer.json->get(*config->signer.data, tmp);
        ut->delS(tmp);
        if (config->signer.data->success)
        {
            error.code = config->signer.data->intValue;
            config->signer.tokens.status = esp_signer_token_status_error;

            tmp = ut->strP(esp_signer_pgm_str_69);
            config->signer.json->get(*config->signer.data, tmp);
            ut->delS(tmp);
            if (config->signer.data->success)
                error.message = config->signer.data->stringValue.c_str();
        }

        config->signer.tokens.error = error;
        tokenInfo.status = config->signer.tokens.status;
        tokenInfo.type = config->signer.tokens.token_type;
        tokenInfo.error = config->signer.tokens.error;
        if (error.code != 0)
            sendTokenStatusCB();

        if (error.code == 0)
        {
            if (config->signer.tokens.token_type == esp_signer_token_type_oauth2_access_token)
            {
                tmp = ut->strP(esp_signer_pgm_str_70);
                config->signer.json->get(*config->signer.data, tmp);
                ut->delS(tmp);
                if (config->signer.data->success)
                    config->signer.tokens.access_token = config->signer.data->stringValue.c_str();

                tmp = ut->strP(esp_signer_pgm_str_71);
                config->signer.json->get(*config->signer.data, tmp);
                ut->delS(tmp);
                if (config->signer.data->success)
                    config->signer.tokens.auth_type = config->signer.data->stringValue.c_str();

                tmp = ut->strP(esp_signer_pgm_str_72);
                config->signer.json->get(*config->signer.data, tmp);
                ut->delS(tmp);
                if (config->signer.data->success)
                {
                    time_t ts = time(nullptr);
                    unsigned long ms = millis();
                    config->signer.tokens.expires = ts + atoi(config->signer.data->stringValue.c_str());
                    config->signer.tokens.last_millis = ms;
                }
            }
            return handleSignerError(0);
        }
        return handleSignerError(4);
    }

    return handleSignerError(3);
}

void ESP_Signer::checkToken()
{
    if (!config)
        return;

    //if the time was set (changed) after token has been generated, update its expiration
    if (config->signer.tokens.expires > 0 && config->signer.tokens.expires < ESP_DEFAULT_TS && time(nullptr) > ESP_DEFAULT_TS)
        config->signer.tokens.expires += time(nullptr) - (millis() - config->signer.tokens.last_millis) / 1000 - 60;

    if (config->signer.tokens.token_type == esp_signer_token_type_oauth2_access_token && (time(nullptr) > config->signer.tokens.expires - config->signer.preRefreshSeconds || config->signer.tokens.expires == 0))
        handleToken();
}

bool ESP_Signer::tokenReady()
{
    if (!config)
        return false;

    checkToken();
    return config->signer.tokens.status == esp_signer_token_status_ready;
};

String ESP_Signer::accessToken()
{
    if (!config)
        return "";
    return config->signer.tokens.access_token.c_str();
}

String ESP_Signer::getTokenType(TokenInfo info)
{
    if (!config)
        return "";

    std::string s;
    switch (info.type)
    {
    case esp_signer_token_type_undefined:
        ut->appendP(s, esp_signer_pgm_str_49);
        break;
    case esp_signer_token_type_oauth2_access_token:
        ut->appendP(s, esp_signer_pgm_str_50);
        break;
    default:
        break;
    }
    return s.c_str();
}

String ESP_Signer::getTokenType()
{
    return getTokenType(tokenInfo);
}

String ESP_Signer::getTokenStatus(TokenInfo info)
{
    if (!config)
        return "";

    std::string s;
    switch (info.status)
    {
    case esp_signer_token_status_uninitialized:
        ut->appendP(s, esp_signer_pgm_str_51);
        break;

    case esp_signer_token_status_on_initialize:
        ut->appendP(s, esp_signer_pgm_str_52);
        break;
    case esp_signer_token_status_on_signing:
        ut->appendP(s, esp_signer_pgm_str_53);
        break;
    case esp_signer_token_status_on_request:
        ut->appendP(s, esp_signer_pgm_str_54);
        break;
    case esp_signer_token_status_on_refresh:
        ut->appendP(s, esp_signer_pgm_str_55);
        break;
    case esp_signer_token_status_ready:
        ut->appendP(s, esp_signer_pgm_str_112);
        break;
    case esp_signer_token_status_error:
        ut->appendP(s, esp_signer_pgm_str_113);
        break;
    default:
        break;
    }
    return s.c_str();
}

String ESP_Signer::getTokenStatus()
{
    return getTokenStatus(tokenInfo);
}

String ESP_Signer::getTokenError(TokenInfo info)
{
    if (!config)
        return "";

    std::string s;
    ut->appendP(s, esp_signer_pgm_str_114);
    char *tmp = ut->intStr(info.error.code);
    s += tmp;
    ut->delS(tmp);
    ut->appendP(s, esp_signer_pgm_str_115);
    s += info.error.message;
    return s.c_str();
}

String ESP_Signer::getTokenError()
{
    return getTokenError(tokenInfo);
}

unsigned long ESP_Signer::getExpiredTimestamp()
{
    return config->signer.tokens.expires;
}

void ESP_Signer::refreshToken()
{
    config->signer.tokens.expires = 0;
    checkToken();
}

void ESP_Signer::errorToString(int httpCode, std::string &buff)
{
    buff.clear();

    if (config)
    {
        if (config->signer.tokens.status == esp_signer_token_status_error || config->signer.tokens.error.code != 0)
        {
            buff = config->signer.tokens.error.message;
            return;
        }
    }

    switch (httpCode)
    {
    case ESP_SIGNER_ERROR_HTTPC_ERROR_CONNECTION_REFUSED:
        ut->appendP(buff, esp_signer_pgm_str_73);
        return;
    case ESP_SIGNER_ERROR_HTTPC_ERROR_SEND_HEADER_FAILED:
        ut->appendP(buff, esp_signer_pgm_str_74);
        return;
    case ESP_SIGNER_ERROR_HTTPC_ERROR_SEND_PAYLOAD_FAILED:
        ut->appendP(buff, esp_signer_pgm_str_75);
        return;
    case ESP_SIGNER_ERROR_HTTPC_ERROR_NOT_CONNECTED:
        ut->appendP(buff, esp_signer_pgm_str_28);
        return;
    case ESP_SIGNER_ERROR_HTTPC_ERROR_CONNECTION_LOST:
        ut->appendP(buff, esp_signer_pgm_str_29);
        return;
    case ESP_SIGNER_ERROR_HTTPC_ERROR_NO_HTTP_SERVER:
        ut->appendP(buff, esp_signer_pgm_str_76);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_BAD_REQUEST:
        ut->appendP(buff, esp_signer_pgm_str_77);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_NON_AUTHORITATIVE_INFORMATION:
        ut->appendP(buff, esp_signer_pgm_str_78);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_NO_CONTENT:
        ut->appendP(buff, esp_signer_pgm_str_79);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_MOVED_PERMANENTLY:
        ut->appendP(buff, esp_signer_pgm_str_80);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_USE_PROXY:
        ut->appendP(buff, esp_signer_pgm_str_81);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_TEMPORARY_REDIRECT:
        ut->appendP(buff, esp_signer_pgm_str_82);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_PERMANENT_REDIRECT:
        ut->appendP(buff, esp_signer_pgm_str_83);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_UNAUTHORIZED:
        ut->appendP(buff, esp_signer_pgm_str_84);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_FORBIDDEN:
        ut->appendP(buff, esp_signer_pgm_str_85);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_NOT_FOUND:
        ut->appendP(buff, esp_signer_pgm_str_86);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_METHOD_NOT_ALLOWED:
        ut->appendP(buff, esp_signer_pgm_str_87);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_NOT_ACCEPTABLE:
        ut->appendP(buff, esp_signer_pgm_str_88);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_PROXY_AUTHENTICATION_REQUIRED:
        ut->appendP(buff, esp_signer_pgm_str_89);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_REQUEST_TIMEOUT:
        ut->appendP(buff, esp_signer_pgm_str_30);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_LENGTH_REQUIRED:
        ut->appendP(buff, esp_signer_pgm_str_90);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_TOO_MANY_REQUESTS:
        ut->appendP(buff, esp_signer_pgm_str_91);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_REQUEST_HEADER_FIELDS_TOO_LARGE:
        ut->appendP(buff, esp_signer_pgm_str_92);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_INTERNAL_SERVER_ERROR:
        ut->appendP(buff, esp_signer_pgm_str_93);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_BAD_GATEWAY:
        ut->appendP(buff, esp_signer_pgm_str_94);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_SERVICE_UNAVAILABLE:
        ut->appendP(buff, esp_signer_pgm_str_95);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_GATEWAY_TIMEOUT:
        ut->appendP(buff, esp_signer_pgm_str_96);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_HTTP_VERSION_NOT_SUPPORTED:
        ut->appendP(buff, esp_signer_pgm_str_97);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_NETWORK_AUTHENTICATION_REQUIRED:
        ut->appendP(buff, esp_signer_pgm_str_98);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_PRECONDITION_FAILED:
        ut->appendP(buff, esp_signer_pgm_str_99);
        return;
    case ESP_SIGNER_ERROR_HTTPC_ERROR_READ_TIMEOUT:
        ut->appendP(buff, esp_signer_pgm_str_100);
        return;
    case ESP_SIGNER_ERROR_HTTPC_ERROR_CONNECTION_INUSED:
        ut->appendP(buff, esp_signer_pgm_str_101);
        return;
    case ESP_SIGNER_ERROR_BUFFER_OVERFLOW:
        ut->appendP(buff, esp_signer_pgm_str_102);
        return;
    case ESP_SIGNER_ERROR_HTTP_CODE_PAYLOAD_TOO_LARGE:
        ut->appendP(buff, esp_signer_pgm_str_103);
        return;
    case ESP_SIGNER_ERROR_FILE_IO_ERROR:
        ut->appendP(buff, esp_signer_pgm_str_104);
        return;
    case ESP_SIGNER_ERROR_FILE_NOT_FOUND:
        ut->appendP(buff, esp_signer_pgm_str_105);
        return;
    case ESP_SIGNER_ERROR_TOKEN_NOT_READY:
        ut->appendP(buff, esp_signer_pgm_str_26);
        return;
    case ESP_SIGNER_ERROR_UNINITIALIZED:
        ut->appendP(buff, esp_signer_pgm_str_106);
        return;
    default:
        return;
    }
}

bool ESP_Signer::setSystemTime(time_t ts)
{
    return ut->setTimestamp(ts) == 0;
}

ESP_Signer Signer = ESP_Signer();

#endif