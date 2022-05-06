/**
 * Util class, SignerUtils.h version 1.0.6
 *
 *
 * Created May 6, 2022
 *
 * This work is a part of ESP Signer library
 * Copyright (c) 2022 K. Suwatchai (Mobizt)
 *
 * The MIT License (MIT)
 * Copyright (c)2022 K. Suwatchai (Mobizt)
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

#ifndef SIGNER_UTILS_H
#define SIGNER_UTILS_H

#include <Arduino.h>
#include "ESP_Signer_Const.h"

class SignerUtils
{

public:
    uint16_t ntpTimeout = 20;
    esp_signer_callback_function_t _callback_function = nullptr;
    SignerConfig *config = nullptr;
    MB_FS *mbfs = nullptr;

    SignerUtils(SignerConfig *cfg, MB_FS *mbfs)
    {
        config = cfg;
        this->mbfs = mbfs;
    };

    ~SignerUtils(){};

    void idle()
    {
        delay(0);
    }
    char *strP(PGM_P pgm)
    {
        size_t len = strlen_P(pgm) + 5;
        char *buf = (char *)mbfs->newP(len);
        strcpy_P(buf, pgm);
        buf[strlen_P(pgm)] = 0;
        return buf;
    }

    int strposP(const char *buf, PGM_P beginH, int ofs)
    {
        char *tmp = strP(beginH);
        int p = strpos(buf, tmp, ofs);
        mbfs->delP(&tmp);
        return p;
    }

    bool strcmpP(const char *buf, int ofs, PGM_P beginH)
    {
        char *tmp = nullptr;
        if (ofs < 0)
        {
            int p = strposP(buf, beginH, 0);
            if (p == -1)
                return false;
            ofs = p;
        }
        tmp = strP(beginH);
        char *tmp2 = (char *)mbfs->newP(strlen_P(beginH) + 1);
        memcpy(tmp2, &buf[ofs], strlen_P(beginH));
        tmp2[strlen_P(beginH)] = 0;
        bool ret = (strcasecmp(tmp, tmp2) == 0);
        mbfs->delP(&tmp);
        mbfs->delP(&tmp2);
        return ret;
    }

    char *subStr(const char *buf, PGM_P beginH, PGM_P endH, int beginPos, int endPos)
    {

        char *tmp = nullptr;
        int p1 = strposP(buf, beginH, beginPos);
        if (p1 != -1)
        {
            int p2 = -1;
            if (endPos == 0)
                p2 = strposP(buf, endH, p1 + strlen_P(beginH));

            if (p2 == -1)
                p2 = strlen(buf);

            int len = p2 - p1 - strlen_P(beginH);
            tmp = (char *)mbfs->newP(len + 1);
            memcpy(tmp, &buf[p1 + strlen_P(beginH)], len);
            return tmp;
        }

        return nullptr;
    }

    void strcat_c(char *str, char c)
    {
        for (; *str; str++)
            ;
        *str++ = c;
        *str++ = 0;
    }

    int strpos(const char *haystack, const char *needle, int offset)
    {
        if (!haystack || !needle)
            return -1;

        int hlen = strlen(haystack);
        int nlen = strlen(needle);

        if (hlen == 0 || nlen == 0)
            return -1;

        int hidx = offset, nidx = 0;
        while ((*(haystack + hidx) != '\0') && (*(needle + nidx) != '\0') && hidx < hlen)
        {
            if (*(needle + nidx) != *(haystack + hidx))
            {
                hidx++;
                nidx = 0;
            }
            else
            {
                nidx++;
                hidx++;
                if (nidx == nlen)
                    return hidx - nidx;
            }
        }

        return -1;
    }

    int strpos(const char *haystack, char needle, int offset)
    {
        if (!haystack || needle == 0)
            return -1;

        int hlen = strlen(haystack);

        if (hlen == 0)
            return -1;

        int hidx = offset;
        while ((*(haystack + hidx) != '\0') && hidx < hlen)
        {
            if (needle == *(haystack + hidx))
                return hidx;
            hidx++;
        }

        return -1;
    }

    int rstrpos(const char *haystack, const char *needle, int offset /* start search from this offset to the left string */)
    {
        if (!haystack || !needle)
            return -1;

        int hlen = strlen(haystack);
        int nlen = strlen(needle);

        if (hlen == 0 || nlen == 0)
            return -1;

        int hidx = offset;

        if (hidx >= hlen || offset == -1)
            hidx = hlen - 1;

        int nidx = nlen - 1;

        while (hidx >= 0)
        {
            if (*(needle + nidx) != *(haystack + hidx))
            {
                hidx--;
                nidx = nlen - 1;
            }
            else
            {
                if (nidx == 0)
                    return hidx + nidx;
                nidx--;
                hidx--;
            }
        }

        return -1;
    }

    int rstrpos(const char *haystack, char needle, int offset /* start search from this offset to the left char */)
    {
        if (!haystack || needle == 0)
            return -1;

        int hlen = strlen(haystack);

        if (hlen == 0)
            return -1;

        int hidx = offset;

        if (hidx >= hlen || offset == -1)
            hidx = hlen - 1;

        while (hidx >= 0)
        {
            if (needle == *(haystack + hidx))
                return hidx;
            hidx--;
        }

        return -1;
    }

    void ltrim(MB_String &str, const MB_String &chars = " ")
    {
        size_t pos = str.find_first_not_of(chars);
        if (pos != MB_String::npos)
            str.erase(0, pos);
    }

    void rtrim(MB_String &str, const MB_String &chars = " ")
    {
        size_t pos = str.find_last_not_of(chars);
        if (pos != MB_String::npos)
            str.erase(pos + 1);
    }

    inline MB_String trim(const MB_String &s)
    {
        MB_String chars = " ";
        MB_String str = s;
        ltrim(str, chars);
        rtrim(str, chars);
        return str;
    }

    void substr(MB_String &str, const char *s, int offset, size_t len)
    {
        if (!s)
            return;

        int slen = strlen(s);

        if (slen == 0)
            return;

        int last = offset + len;

        if (offset >= slen || len == 0 || last > slen)
            return;

        for (int i = offset; i < last; i++)
            str += s[i];
    }
    void splitString(const char *str, MB_VECTOR<MB_String> out, const char delim)
    {
        int current = 0, previous = 0;
        current = strpos(str, delim, 0);
        MB_String s;
        while (current != -1)
        {
            s.clear();
            substr(s, str, previous, current - previous);
            trim(s);
            if (s.length() > 0)
                out.push_back(s);

            previous = current + 1;
            current = strpos(str, delim, previous);
            delay(0);
        }

        s.clear();

        if (previous > 0 && current == -1)
            substr(s, str, previous, strlen(str) - previous);
        else
            s = str;

        trim(s);
        if (s.length() > 0)
            out.push_back(s);
        s.clear();
    }

    int url_decode(const char *s, char *dec)
    {
        char *o;
        const char *end = s + strlen(s);
        int c;

        for (o = dec; s <= end; o++)
        {
            c = *s++;
            if (c == '+')
                c = ' ';
            else if (c == '%' && (!ishex(*s++) ||
                                  !ishex(*s++) ||
                                  !sscanf(s - 2, "%2x", &c)))
                return -1;

            if (dec)
                *o = c;
        }

        return o - dec;
    }

    MB_String url_encode(const MB_String &s)
    {
        MB_String ret;
        ret.reserve(s.length() * 3 + 1);
        for (size_t i = 0, l = s.size(); i < l; i++)
        {
            char c = s[i];
            if ((c >= '0' && c <= '9') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= 'a' && c <= 'z') ||
                c == '-' || c == '_' || c == '.' || c == '!' || c == '~' ||
                c == '*' || c == '\'' || c == '(' || c == ')')
            {
                ret += c;
            }
            else
            {
                ret += '%';
                unsigned char d1, d2;
                hexchar(c, d1, d2);
                ret += d1;
                ret += d2;
            }
        }
        ret.shrink_to_fit();
        return ret;
    }

    inline int ishex(int x)
    {
        return (x >= '0' && x <= '9') ||
               (x >= 'a' && x <= 'f') ||
               (x >= 'A' && x <= 'F');
    }

    void hexchar(unsigned char c, unsigned char &hex1, unsigned char &hex2)
    {
        hex1 = c / 16;
        hex2 = c % 16;
        hex1 += hex1 <= 9 ? '0' : 'a' - 10;
        hex2 += hex2 <= 9 ? '0' : 'a' - 10;
    }

    char from_hex(char ch)
    {
        return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
    }

    uint32_t hex2int(const char *hex)
    {
        uint32_t val = 0;
        while (*hex)
        {
            // get current character then increment
            uint8_t byte = *hex++;
            // transform hex character to the 4bit equivalent number, using the ascii table indexes
            if (byte >= '0' && byte <= '9')
                byte = byte - '0';
            else if (byte >= 'a' && byte <= 'f')
                byte = byte - 'a' + 10;
            else if (byte >= 'A' && byte <= 'F')
                byte = byte - 'A' + 10;
            // shift 4 to make space for new digit, and add the 4 bits of the new digit
            val = (val << 4) | (byte & 0xF);
        }
        return val;
    }

    void parseRespHeader(const char *buf, struct esp_signer_server_response_data_t &response)
    {
        int beginPos = 0, pmax = 0, payloadPos = 0;

        char *tmp = nullptr;

        if (response.httpCode != -1)
        {
            payloadPos = beginPos;
            pmax = beginPos;
            tmp = getHeader(buf, esp_signer_pgm_str_3, esp_signer_pgm_str_4, beginPos, 0);
            if (tmp)
            {
                response.connection = tmp;
                mbfs->delP(&tmp);
            }
            if (pmax < beginPos)
                pmax = beginPos;
            beginPos = payloadPos;
            tmp = getHeader(buf, esp_signer_pgm_str_5, esp_signer_pgm_str_4, beginPos, 0);
            if (tmp)
            {
                response.contentType = tmp;
                mbfs->delP(&tmp);
            }

            if (pmax < beginPos)
                pmax = beginPos;
            beginPos = payloadPos;
            tmp = getHeader(buf, esp_signer_pgm_str_6, esp_signer_pgm_str_4, beginPos, 0);
            if (tmp)
            {
                response.contentLen = atoi(tmp);
                mbfs->delP(&tmp);
            }

            if (pmax < beginPos)
                pmax = beginPos;
            beginPos = payloadPos;
            tmp = getHeader(buf, esp_signer_pgm_str_7, esp_signer_pgm_str_4, beginPos, 0);
            if (tmp)
            {
                response.transferEnc = tmp;
                if (stringCompare(tmp, 0, esp_signer_pgm_str_8))
                    response.isChunkedEnc = true;
                mbfs->delP(&tmp);
            }

            if (pmax < beginPos)
                pmax = beginPos;
            beginPos = payloadPos;
            tmp = getHeader(buf, esp_signer_pgm_str_3, esp_signer_pgm_str_4, beginPos, 0);
            if (tmp)
            {
                response.connection = tmp;
                mbfs->delP(&tmp);
            }

            if (pmax < beginPos)
                pmax = beginPos;
            beginPos = payloadPos;
            tmp = getHeader(buf, esp_signer_pgm_str_6, esp_signer_pgm_str_4, beginPos, 0);
            if (tmp)
            {

                response.payloadLen = atoi(tmp);
                mbfs->delP(&tmp);
            }

            if (response.httpCode == ESP_SIGNER_ERROR_HTTP_CODE_OK || response.httpCode == ESP_SIGNER_ERROR_HTTP_CODE_TEMPORARY_REDIRECT || response.httpCode == ESP_SIGNER_ERROR_HTTP_CODE_PERMANENT_REDIRECT || response.httpCode == ESP_SIGNER_ERROR_HTTP_CODE_MOVED_PERMANENTLY || response.httpCode == ESP_SIGNER_ERROR_HTTP_CODE_FOUND)
            {
                if (pmax < beginPos)
                    pmax = beginPos;
                beginPos = payloadPos;
                tmp = getHeader(buf, esp_signer_pgm_str_9, esp_signer_pgm_str_4, beginPos, 0);
                if (tmp)
                {
                    response.location = tmp;
                    mbfs->delP(&tmp);
                }
            }

            if (response.httpCode == ESP_SIGNER_ERROR_HTTP_CODE_NO_CONTENT)
                response.noContent = true;
        }
    }


    char *getHeader(const char *buf, PGM_P beginH, PGM_P endH, int &beginPos, int endPos)
    {

        char *tmp = strP(beginH);
        int p1 = strpos(buf, tmp, beginPos);
        int ofs = 0;
        mbfs->delP(&tmp);
        if (p1 != -1)
        {
            tmp = strP(endH);
            int p2 = -1;
            if (endPos > 0)
                p2 = endPos;
            else if (endPos == 0)
            {
                ofs = strlen_P(endH);
                p2 = strpos(buf, tmp, p1 + strlen_P(beginH) + 1);
            }
            else if (endPos == -1)
            {
                beginPos = p1 + strlen_P(beginH);
            }

            if (p2 == -1)
                p2 = strlen(buf);

            mbfs->delP(&tmp);

            if (p2 != -1)
            {
                beginPos = p2 + ofs;
                int len = p2 - p1 - strlen_P(beginH);
                tmp = (char *)mbfs->newP(len + 1);
                memcpy(tmp, &buf[p1 + strlen_P(beginH)], len);
                return tmp;
            }
        }

        return nullptr;
    }

    void getHeaderStr(const MB_String &in, MB_String &out, PGM_P beginH, PGM_P endH, int &beginPos, int endPos)
    {
        MB_String _in = in;

        char *tmp = strP(beginH);
        int p1 = strpos(in.c_str(), tmp, beginPos);
        int ofs = 0;
        mbfs->delP(&tmp);
        if (p1 != -1)
        {
            tmp = strP(endH);
            int p2 = -1;
            if (endPos > 0)
                p2 = endPos;
            else if (endPos == 0)
            {
                ofs = strlen_P(endH);
                p2 = strpos(in.c_str(), tmp, p1 + strlen_P(beginH) + 1);
            }
            else if (endPos == -1)
            {
                beginPos = p1 + strlen_P(beginH);
            }

            if (p2 == -1)
                p2 = in.length();

            mbfs->delP(&tmp);

            if (p2 != -1)
            {
                beginPos = p2 + ofs;
                int len = p2 - p1 - strlen_P(beginH);
                out = _in.substr(p1 + strlen_P(beginH), len);
            }
        }
    }

    bool stringCompare(const char *buf, int ofs, PGM_P beginH)
    {
        char *tmp = strP(beginH);
        char *tmp2 = (char *)mbfs->newP(strlen_P(beginH) + 1);
        memcpy(tmp2, &buf[ofs], strlen_P(beginH));
        tmp2[strlen_P(beginH)] = 0;
        bool ret = (strcmp(tmp, tmp2) == 0);
        mbfs->delP(&tmp);
        mbfs->delP(&tmp2);
        return ret;
    }

    bool syncClock(float gmtOffset)
    {
        if (!config)
            return false;

        time_t now = time(nullptr);

        config->internal.esp_signer_clock_rdy = now > ESP_DEFAULT_TS;

        if (config->internal.esp_signer_clock_rdy && gmtOffset == config->internal.esp_signer_gmt_offset)
            return true;

        if (!config->internal.esp_signer_clock_rdy || gmtOffset != config->internal.esp_signer_gmt_offset)
        {
            if (gmtOffset != config->internal.esp_signer_gmt_offset)
                config->internal.esp_signer_clock_synched = false;

            if (!config->internal.esp_signer_clock_synched)
                configTime(gmtOffset * 3600, 0, "pool.ntp.org", "time.nist.gov");

            config->internal.esp_signer_clock_synched = true;

            now = time(nullptr);
        }

        config->internal.esp_signer_clock_rdy = now > ESP_DEFAULT_TS;
        if (config->internal.esp_signer_clock_rdy)
            config->internal.esp_signer_gmt_offset = gmtOffset;

        return config->internal.esp_signer_clock_rdy;
    }

    void encodeBase64Url(char *encoded, unsigned char *string, size_t len)
    {
        size_t i;
        char *p = encoded;

        unsigned char *b64enc = (unsigned char *)mbfs->newP(65);
        strcpy_P((char *)b64enc, (char *)esp_signer_base64_table);
        b64enc[62] = '-';
        b64enc[63] = '_';

        for (i = 0; i < len - 2; i += 3)
        {
            *p++ = b64enc[(string[i] >> 2) & 0x3F];
            *p++ = b64enc[((string[i] & 0x3) << 4) | ((int)(string[i + 1] & 0xF0) >> 4)];
            *p++ = b64enc[((string[i + 1] & 0xF) << 2) | ((int)(string[i + 2] & 0xC0) >> 6)];
            *p++ = b64enc[string[i + 2] & 0x3F];
        }

        if (i < len)
        {
            *p++ = b64enc[(string[i] >> 2) & 0x3F];
            if (i == (len - 1))
            {
                *p++ = b64enc[((string[i] & 0x3) << 4)];
            }
            else
            {
                *p++ = b64enc[((string[i] & 0x3) << 4) | ((int)(string[i + 1] & 0xF0) >> 4)];
                *p++ = b64enc[((string[i + 1] & 0xF) << 2)];
            }
        }

        *p++ = '\0';

        mbfs->delP(&b64enc);
    }

    MB_String encodeBase64Str(const unsigned char *src, size_t len)
    {
        return encodeBase64Str((uint8_t *)src, len);
    }

    MB_String encodeBase64Str(uint8_t *src, size_t len)
    {
        unsigned char *out, *pos;
        const unsigned char *end, *in;

        unsigned char *b64enc = (unsigned char *)mbfs->newP(65);
        strcpy_P((char *)b64enc, (char *)esp_signer_base64_table);

        size_t olen;

        olen = 4 * ((len + 2) / 3); /* 3-byte blocks to 4-byte */

        MB_String outStr;
        outStr.resize(olen);
        out = (unsigned char *)&outStr[0];

        end = src + len;
        in = src;
        pos = out;

        while (end - in >= 3)
        {
            *pos++ = b64enc[in[0] >> 2];
            *pos++ = b64enc[((in[0] & 0x03) << 4) | (in[1] >> 4)];
            *pos++ = b64enc[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
            *pos++ = b64enc[in[2] & 0x3f];
            in += 3;
            delay(0);
        }

        if (end - in)
        {

            *pos++ = b64enc[in[0] >> 2];

            if (end - in == 1)
            {
                *pos++ = b64enc[(in[0] & 0x03) << 4];
                *pos++ = '=';
            }
            else
            {
                *pos++ = b64enc[((in[0] & 0x03) << 4) | (in[1] >> 4)];
                *pos++ = b64enc[(in[1] & 0x0f) << 2];
            }

            *pos++ = '=';
        }

        mbfs->delP(&b64enc);
        return outStr;
    }

    size_t base64EncLen(size_t len)
    {
        return ((len + 2) / 3 * 4) + 1;
    }



#if defined(ESP8266)
    void set_scheduled_callback(esp_signer_callback_function_t callback)
    {
        _callback_function = std::move([callback]()
                                       { schedule_function(callback); });
        _callback_function();
    }
#endif

    bool waitIdle(int &httpCode)
    {
#if defined(ESP32)

        unsigned long wTime = millis();
        while (config->internal.esp_signer_processing)
        {
            if (millis() - wTime > 3000)
            {
                httpCode = ESP_SIGNER_ERROR_TCP_ERROR_CONNECTION_INUSED;
                return false;
            }
            delay(0);
        }
#endif
        return true;
    }

    void splitTk(const MB_String &str, MB_VECTOR<MB_String> &tk, const char *delim)
    {
        std::size_t current, previous = 0;
        current = str.find(delim, previous);
        MB_String s;
        while (current != MB_String::npos)
        {
            s = str.substr(previous, current - previous);
            tk.push_back(s);
            previous = current + strlen(delim);
            current = str.find(delim, previous);
        }
        s = str.substr(previous, current - previous);
        tk.push_back(s);
        MB_String().swap(s);
    }

    int setTimestamp(time_t ts)
    {
        struct timeval tm = {ts, 0}; // sec, us
        return settimeofday((const timeval *)&tm, 0);
    }

private:
};

#endif