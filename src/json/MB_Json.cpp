/*
 * MB_Json, version 1.0.0
 * 
 * The ESP8266/ESP32 Json Arduino library.
 * 
 * March 15, 2021
 * 
 * Features
 * - None recursive operations
 * - Parse and edit JSON object directly with a specified relative path. 
 * - Prettify JSON string 
 * 
 * 
 * The zserge's JSON object parser library used as part of this library
 * 
 * The MIT License (MIT)
 * Copyright (c) 2020 K. Suwatchai (Mobizt)
 * Copyright (c) 2012–2018, Serge Zaitsev, zaitsev.serge@gmail.com
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

#ifndef MB_Json_CPP
#define MB_Json_CPP
#define MB_JSON_JSMN_STRICT

#include "MB_Json.h"

//Teensy 3.0, 3.2,3.5,3.6, 4.0 and 4.1
#if defined(__arm__) && defined(TEENSYDUINO) && (defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__) || defined(__IMXRT1062__))
extern "C" {
int __exidx_start() { return -1; }
int __exidx_end() { return -1; }
}

#endif

MB_Json::MB_Json()
{
    _init();
}

MB_Json::MB_Json(std::string &data)
{
    _init();
    _setJsonData(data);
}

MB_Json::~MB_Json()
{
    clear();
    _topLevelTkType = MB_JSON_JSMN_OBJECT;
    _parser.reset();
    _parser = nullptr;
    _finalize();
}

void MB_Json::_init()
{
    _finalize();
    _qt = _strP(mb_json_str_2);
    _tab = _strP(mb_json_str_22);
    _brk1 = _strP(mb_json_str_8);
    _brk2 = _strP(mb_json_str_9);
    _brk3 = _strP(mb_json_str_10);
    _brk4 = _strP(mb_json_str_11);
    _cm = _strP(mb_json_str_1);
    _pr2 = _strP(mb_json_str_3);
    _nl = _strP(mb_json_str_24);
    _nll = _strP(mb_json_str_18);
    _pr = _strP(mb_json_str_25);
    _pd = _strP(mb_json_str_4);
    _pf = _strP(mb_json_str_5);
    _fls = _strP(mb_json_str_6);
    _tr = _strP(mb_json_str_7);
    _string = _strP(mb_json_str_12);
    _int = _strP(mb_json_str_13);
    _dbl = _strP(mb_json_str_14);
    _bl = _strP(mb_json_str_15);
    _obj = _strP(mb_json_str_16);
    _arry = _strP(mb_json_str_17);
    _undef = _strP(mb_json_str_19);
    _dot = _strP(mb_json_str_20);
}

void MB_Json::_finalize()
{
    _delS(_qt);
    _delS(_tab);
    _delS(_brk1);
    _delS(_brk2);
    _delS(_brk3);
    _delS(_brk4);
    _delS(_cm);
    _delS(_pr2);
    _delS(_nl);
    _delS(_nll);
    _delS(_pr);
    _delS(_pd);
    _delS(_pf);
    _delS(_fls);
    _delS(_tr);
    _delS(_string);
    _delS(_int);
    _delS(_dbl);
    _delS(_bl);
    _delS(_obj);
    _delS(_arry);
    _delS(_undef);
    _delS(_dot);
}

MB_Json &MB_Json::_setJsonData(std::string &data)
{
    return setJsonData(data.c_str());
}

MB_Json &MB_Json::setJsonData(const String &data)
{
    _topLevelTkType = MB_JSON_JSMN_OBJECT;
    if (data.length() > 0)
    {
        int p1 = _strpos(data.c_str(), _brk1, 0);
        int p2 = _rstrpos(data.c_str(), _brk2, data.length() - 1);
        if (p1 != -1)
            p1 += 1;
        if (p1 != -1 && p2 != -1)
            _rawbuf = data.substring(p1, p2).c_str();
        else
        {
            p1 = _strpos(data.c_str(), _brk3, 0);
            p2 = _rstrpos(data.c_str(), _brk4, data.length() - 1);
            if (p1 != -1)
                p1 += 1;
            if (p1 != -1 && p2 != -1)
            {
                char *_r = _strP(mb_json_str_21);
                _rawbuf = _r;
                _rawbuf += data.c_str();
                _delS(_r);
                _topLevelTkType = MB_JSON_JSMN_ARRAY;
            }
            else
            {
                _rawbuf = data.c_str();
                _topLevelTkType = MB_JSON_JSMN_PRIMITIVE;
            }
        }
    }
    else
        _rawbuf.clear();

    return *this;
}

MB_Json &MB_Json::clear()
{
    std::string().swap(_rawbuf);
    std::string().swap(_jsonData._dbuf);
    std::string().swap(_tbuf);
    clearPathTk();
    _tokens.reset();
    _tokens = nullptr;
    _topLevelTkType = MB_JSON_JSMN_OBJECT;
    return *this;
}

MB_Json &MB_Json::add(const String &key)
{
    _addNull(key.c_str());
    return *this;
}

MB_Json &MB_Json::add(const String &key, const String &value)
{
    _addString(key.c_str(), value.c_str());
    return *this;
}
MB_Json &MB_Json::add(const String &key, const char *value)
{
    _addString(key.c_str(), value);
    return *this;
}
MB_Json &MB_Json::add(const String &key, int value)
{
    _addInt(key.c_str(), value);
    return *this;
}

MB_Json &MB_Json::add(const String &key, unsigned short value)
{
    _addInt(key.c_str(), value);
    return *this;
}

MB_Json &MB_Json::add(const String &key, float value)
{
    _addFloat(key.c_str(), value);
    return *this;
}

MB_Json &MB_Json::add(const String &key, double value)
{
    _addDouble(key.c_str(), value);
    return *this;
}

MB_Json &MB_Json::add(const String &key, bool value)
{
    _addBool(key.c_str(), value);
    return *this;
}

MB_Json &MB_Json::add(const String &key, MB_Json &json)
{
    _addJson(key.c_str(), &json);
    return *this;
}

MB_Json &MB_Json::add(const String &key, MB_JsonArray &arr)
{
    _addArray(key.c_str(), &arr);
    return *this;
}

template <typename T>
MB_Json &MB_Json::add(const String &key, T value)
{
    if (std::is_same<T, int>::value)
        _addInt(key, value);
    else if (std::is_same<T, float>::value)
        _addFloat(key, value);
    else if (std::is_same<T, double>::value)
        _addDouble(key, value);
    else if (std::is_same<T, bool>::value)
        _addBool(key, value);
    else if (std::is_same<T, const char *>::value)
        _addString(key, value);
    else if (std::is_same<T, MB_Json &>::value)
        _addJson(key, &value);
    else if (std::is_same<T, MB_JsonArray &>::value)
        _addArray(key, &value);
    return *this;
}

void MB_Json::_addString(const std::string &key, const std::string &value)
{
    _add(key.c_str(), value.c_str(), key.length(), value.length(), true, true);
}

void MB_Json::_addInt(const std::string &key, int value)
{
    char *buf = intStr(value);
    _add(key.c_str(), buf, key.length(), 60, false, true);
    _delS(buf);
}

void MB_Json::_addFloat(const std::string &key, float value)
{
    char *buf = floatStr(value);
    _trimDouble(buf);
    _add(key.c_str(), buf, key.length(), 60, false, true);
    _delS(buf);
}

void MB_Json::_addDouble(const std::string &key, double value)
{
    char *buf = doubleStr(value);
    _trimDouble(buf);
    _add(key.c_str(), buf, key.length(), 60, false, true);
    _delS(buf);
}

void MB_Json::_addBool(const std::string &key, bool value)
{
    if (value)
        _add(key.c_str(), _tr, key.length(), 6, false, true);
    else
        _add(key.c_str(), _fls, key.length(), 7, false, true);
}

void MB_Json::_addNull(const std::string &key)
{
    _add(key.c_str(), _nll, key.length(), 6, false, true);
}

void MB_Json::_addJson(const std::string &key, MB_Json *json)
{
    std::string s;
    json->_toStdString(s);
    _add(key.c_str(), s.c_str(), key.length(), s.length(), false, true);
    std::string().swap(s);
}

void MB_Json::_addArray(const std::string &key, MB_JsonArray *arr)
{
    String arrStr;
    arr->toString(arrStr);
    _add(key.c_str(), arrStr.c_str(), key.length(), arrStr.length(), false, true);
}

char *MB_Json::floatStr(float value)
{
    char *buf = _newS(36);
    dtostrf(value, 7, 6, buf);
    return buf;
}

char *MB_Json::intStr(int value)
{
    char *buf = _newS(36);
    sprintf(buf, "%d", value);
    return buf;
}

char *MB_Json::boolStr(bool value)
{
    char *buf = nullptr;
    if (value)
        buf = _strP(mb_json_str_7);
    else
        buf = _strP(mb_json_str_6);
    return buf;
}

char *MB_Json::doubleStr(double value)
{
    char *buf = _newS(36);
    dtostrf(value, 12, 9, buf);
    return buf;
}

void MB_Json::_trimDouble(char *buf)
{
    size_t i = strlen(buf) - 1;
    while (buf[i] == '0' && i > 0)
    {
        if (buf[i - 1] == '.')
        {
            i--;
            break;
        }
        if (buf[i - 1] != '0')
            break;
        i--;
    }
    if (i < strlen(buf) - 1)
        buf[i] = '\0';
}

void MB_Json::toString(String &buf, bool prettify)
{
    char *nbuf = _newS(2);
    if (prettify)
        _parse(nbuf, MB_JSON_PRINT_MODE_PRETTY);
    else
        _parse(nbuf, MB_JSON_PRINT_MODE_PLAIN);
    buf = _jsonData._dbuf.c_str();
    std::string()
        .swap(_jsonData._dbuf);
    _delS(nbuf);
}

void MB_Json::_tostr(std::string &s, bool prettify)
{
    char *nbuf = _newS(2);
    if (prettify)
        _parse(nbuf, MB_JSON_PRINT_MODE_PRETTY);
    else
        _parse(nbuf, MB_JSON_PRINT_MODE_PLAIN);
    s = _jsonData._dbuf;
    std::string().swap(_jsonData._dbuf);
    _delS(nbuf);
}

void MB_Json::_toStdString(std::string &s, bool isJson)
{
    s.clear();
    size_t bufSize = 10;
    char *buf = _newS(bufSize);
    if (_topLevelTkType != MB_JSON_JSMN_PRIMITIVE)
    {
        if (isJson)
            strcat(buf, _brk1);
        else
            strcat(buf, _brk3);
    }

    s += buf;
    s += _rawbuf;
    buf = _newS(buf, bufSize);
    if (_topLevelTkType != MB_JSON_JSMN_PRIMITIVE)
    {
        if (isJson)
            strcat(buf, _brk2);
        else
            strcat(buf, _brk4);
    }
    s += buf;
    _delS(buf);
}

MB_Json &MB_Json::_add(const char *key, const char *value, size_t klen, size_t vlen, bool isString, bool isJson)
{
    size_t bufSize = klen + vlen + 1024;
    char *buf = _newS(bufSize);
    if (_rawbuf.length() > 0)
        strcpy_P(buf, mb_json_str_1);
    if (isJson)
    {
        strcat(buf, _qt);
        strcat(buf, key);
        strcat(buf, _qt);
        strcat_P(buf, _pr2);
    }
    if (isString)
        strcat(buf, _qt);
    strcat(buf, value);
    if (isString)
        strcat(buf, _qt);
    _rawbuf += buf;
    _delS(buf);
    return *this;
}

MB_Json &MB_Json::_addArrayStr(const char *value, size_t len, bool isString)
{
    char *nbuf = _newS(2);
    _add(nbuf, value, 0, len, isString, false);
    _delS(nbuf);
    return *this;
}

bool MB_Json::get(MB_JsonData &jsonData, const String &path, bool prettify)
{
    clearPathTk();
    _strToTk(path.c_str(), _pathTk, '/');
    std::string().swap(_jsonData._dbuf);
    std::string().swap(_tbuf);
    if (prettify)
        _parse(path.c_str(), MB_JSON_PRINT_MODE_PRETTY);
    else
        _parse(path.c_str(), MB_JSON_PRINT_MODE_PLAIN);
    if (_jsonData.success)
    {
        if (_jsonData._type == MB_Json::MB_JSON_JSMN_STRING && _jsonData._dbuf.c_str()[0] == '"' && _jsonData._dbuf.c_str()[_jsonData._dbuf.length() - 1] == '"')
            _jsonData.stringValue = _jsonData._dbuf.substr(1, _jsonData._dbuf.length() - 2).c_str();
        else
            _jsonData.stringValue = _jsonData._dbuf.c_str();
    }
    jsonData = _jsonData;
    std::string().swap(_jsonData._dbuf);
    std::string().swap(_tbuf);
    clearPathTk();
    _tokens.reset();
    _tokens = nullptr;
    return _jsonData.success;
}

size_t MB_Json::iteratorBegin(const char *data)
{
    if (data)
        setJsonData(data);
    _mbjs_parse(true);
    std::string s;
    _toStdString(s);
    int bufLen = s.length() + 1024;
    char *buf = _newS(bufLen);
    char *nbuf = _newS(2);
    strcpy(buf, s.c_str());
    std::string().swap(s);
    int depth = -1;
    _collectTk = true;
    _eltk.clear();
    for (uint16_t i = 0; i < _tokenCount; i++)
        _parseToken(i, buf, depth, nbuf, -2, MB_JSON_PRINT_MODE_NONE);
    _el.clear();
    _delS(buf);
    _delS(nbuf);
    return _eltk.size();
}

void MB_Json::iteratorEnd()
{
    _eltk.clear();
    clearPathTk();
    _jsonData.stringValue = "";
    std::string().swap(_jsonData._dbuf);
    std::string().swap(_tbuf);
    clearPathTk();
    _tokens.reset();
    _tokens = nullptr;
}

void MB_Json::iteratorGet(size_t index, int &type, String &key, String &value)
{
    if (_eltk.size() < index + 1)
        return;
    std::string s;
    _toStdString(s);
    int bufLen = s.length() + 1024;
    char *buf = _newS(bufLen);
    strcpy(buf, s.c_str());
    std::string().swap(s);
    if (_eltk[index].type == 0)
    {
        MB_Json::mbjs_tok_t *h = &_tokens.get()[_eltk[index].index];
        size_t len = h->end - h->start + 3;
        char *k = _newS(len);
        strncpy(k, buf + h->start, h->end - h->start);
        MB_Json::mbjs_tok_t *g = &_tokens.get()[_eltk[index].index + 1];
        size_t len2 = g->end - g->start + 3;
        char *v = _newS(len2);
        strncpy(v, buf + g->start, g->end - g->start);
        key = k;
        value = v;
        type = JSON_OBJECT;
        _delS(k);
        _delS(v);
    }
    else if (_eltk[index].type == 1)
    {
        MB_Json::mbjs_tok_t *g = &_tokens.get()[_eltk[index].index];
        size_t len2 = g->end - g->start + 3;
        char *v = _newS(len2);
        strncpy(v, buf + g->start, g->end - g->start);
        value = v;
        key = "";
        type = JSON_ARRAY;
        _delS(v);
    }
    _delS(buf);
}

void MB_Json::_mbjs_parse(bool collectTk)
{
    std::string s;
    _toStdString(s);
    int bufLen = s.length() + 1024;
    char *buf = _newS(bufLen);
    strcpy(buf, s.c_str());
    std::string().swap(s);
    _tokens.reset();
    _collectTk = collectTk;
    _eltk.clear();
    int cnt = mbjs_parse(_parser.get(), buf, bufLen, (MB_Json::mbjs_tok_t *)NULL, 0);
    int cnt2 = 0;
    int a = 0;
    int b = 0;
    for (int i = 0; i < bufLen; i++)
    {
        if (buf[i] == ',')
            a++;
        else if (buf[i] == '[' || buf[i] == '{')
            b++;
    }
    cnt2 = 10 + (2 * (a + 1)) + b;

    if (cnt < cnt2)
        cnt = cnt2;

    _tokens = std::shared_ptr<MB_Json::mbjs_tok_t>(new MB_Json::mbjs_tok_t[cnt + 10]);
    mbjs_init(_parser.get());
    _tokenCount = mbjs_parse(_parser.get(), buf, bufLen, _tokens.get(), cnt + 10);
    _paresRes = true;
    if (_tokenCount < 0)
        _paresRes = false;
    if (_tokenCount < 1 || _tokens.get()[0].type != MB_Json::MB_JSON_JSMN_OBJECT)
        _paresRes = false;
    _jsonData.success = _paresRes;
    _nextToken = 0;
    _nextDepth = 0;
    _tokenMatch = false;
    _refToken = -1;
    _resetParseResult();
    _setElementType();
    _delS(buf);
}

void MB_Json::_setMark(int depth, bool mark)
{
    for (size_t i = 0; i < _el.size(); i++)
    {
        if (_el[i].depth == depth - 1)
        {
            _el[i].omark = mark;
            break;
        }
    }
}

void MB_Json::_setSkip(int depth, bool skip)
{
    for (size_t i = 0; i < _el.size(); i++)
    {
        if (_el[i].depth == depth - 1)
        {
            _el[i].skip = skip;
            break;
        }
    }
}

void MB_Json::_setRef(int depth, bool ref)
{
    for (size_t i = 0; i < _el.size(); i++)
    {
        if (ref)
        {
            if (_el[i].depth == depth - 1)
            {
                _el[i].ref = ref;
                break;
            }
        }
        else
            _el[i].ref = false;
    }
}

void MB_Json::_getTkIndex(int depth, tk_index_t &tk)
{
    tk.oindex = 0;
    tk.olen = 0;
    tk.omark = false;
    tk.type = MB_Json::MB_JSON_JSMN_UNDEFINED;
    tk.depth = -1;
    tk.skip = false;
    tk.ref = false;
    tk.index = -1;
    for (size_t i = 0; i < _el.size(); i++)
    {
        if (_el[i].depth == depth - 1)
        {
            tk.index = _el[i].index;
            tk.omark = _el[i].omark;
            tk.ref = _el[i].ref;
            tk.type = _el[i].type;
            tk.depth = _el[i].depth;
            tk.oindex = _el[i].oindex;
            tk.olen = _el[i].olen;
            tk.skip = _el[i].skip;
            break;
        }
    }
}

bool MB_Json::_updateTkIndex(uint16_t index, int &depth, char *searchKey, int searchIndex, char *replace, MB_JSON_PRINT_MODE printMode, bool advanceCount)
{
    int len = -1;
    bool skip = false;
    bool ref = false;
    for (size_t i = 0; i < _el.size(); i++)
    {
        if (_el[i].depth == depth - 1)
        {
            if (_el[i].type == MB_Json::MB_JSON_JSMN_OBJECT || _el[i].type == MB_Json::MB_JSON_JSMN_ARRAY)
            {
                _el[i].oindex++;
                if (_el[i].oindex >= _el[i].olen)
                {
                    depth = _el[i].depth;
                    len = _el[i].olen;
                    skip = _el[i].skip;
                    if (!_TkRefOk && _el[i].type == MB_Json::MB_JSON_JSMN_OBJECT)
                        ref = _el[i].ref;
                    else if (!_TkRefOk && _el[i].type == MB_Json::MB_JSON_JSMN_ARRAY && searchIndex > -1)
                        ref = _el[i].ref;
                    if (i > 0)
                        _el.erase(_el.begin() + i);
                    else
                        _el.erase(_el.begin());
                    if (printMode != MB_JSON_PRINT_MODE_NONE && !skip)
                    {
                        if (len > 0 && !_arrReplaced)
                        {
                            if (ref)
                                _jsonData._dbuf += _cm;
                            if (_el[i].type == MB_Json::MB_JSON_JSMN_OBJECT)
                            {
                                if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                                    _jsonData._dbuf += _nl;
                                if (printMode == MB_JSON_PRINT_MODE_PRETTY && !ref)
                                {
                                    for (int j = 0; j < depth + 1; j++)
                                        _jsonData._dbuf += _tab;
                                }
                            }
                        }
                        if (ref)
                        {
                            if (!advanceCount)
                                _parseCompleted++;

                            if (!_arrReplaced)
                            {
                                if (_el[i].type == MB_Json::MB_JSON_JSMN_OBJECT)
                                {
                                    if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                                    {
                                        for (int j = 0; j < depth + 2; j++)
                                            _jsonData._dbuf += _tab;
                                    }
                                    _jsonData._dbuf += _qt;
                                    _jsonData._dbuf += searchKey;
                                    _jsonData._dbuf += _qt;
                                    if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                                        _jsonData._dbuf += _pr;
                                    else
                                        _jsonData._dbuf += _pr2;
                                    if (_parseCompleted == (int)_pathTk.size())
                                        _jsonData._dbuf += replace;
                                    else
                                        _insertChilds(replace, printMode);
                                    _arrReplaced = true;
                                    if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                                    {
                                        _jsonData._dbuf += _nl;
                                        for (int j = 0; j < depth + 1; j++)
                                            _jsonData._dbuf += _tab;
                                    }
                                }
                                else
                                {
                                    for (int k = _el[i].oindex - 1; k < searchIndex; k++)
                                    {
                                        if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                                        {
                                            _jsonData._dbuf += _nl;
                                            for (int j = 0; j < depth + 2; j++)
                                                _jsonData._dbuf += _tab;
                                        }
                                        if (k == searchIndex - 1)
                                        {
                                            if (_parseCompleted == (int)_pathTk.size())
                                                _jsonData._dbuf += replace;
                                            else
                                                _insertChilds(replace, printMode);
                                            _arrReplaced = true;
                                        }
                                        else
                                        {
                                            _jsonData._dbuf += _nll;
                                            _jsonData._dbuf += _cm;
                                        }
                                    }
                                }
                            }
                            _setRef(depth, false);
                            if (!advanceCount)
                                _parseCompleted = _pathTk.size();
                        }

                        if (_el[i].type == MB_Json::MB_JSON_JSMN_OBJECT)
                            _jsonData._dbuf += _brk2;
                        else
                        {
                            if (len > 0)
                            {
                                if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                                {
                                    _jsonData._dbuf += _nl;
                                    for (int j = 0; j < depth + 1; j++)
                                        _jsonData._dbuf += _tab;
                                }
                            }
                            _jsonData._dbuf += _brk4;
                        }
                    }
                    return true;
                }
            }
            break;
        }
    }
    return false;
}

bool MB_Json::_updateTkIndex2(std::string &str, uint16_t index, int &depth, char *searchKey, int searchIndex, char *replace, MB_JSON_PRINT_MODE printMode)
{
    int len = -1;
    bool skip = false;
    bool ref = false;
    for (size_t i = 0; i < _el.size(); i++)
    {
        if (_el[i].depth == depth - 1)
        {
            if (_el[i].type == MB_Json::MB_JSON_JSMN_OBJECT || _el[i].type == MB_Json::MB_JSON_JSMN_ARRAY)
            {
                _el[i].oindex++;
                if (_el[i].oindex >= _el[i].olen)
                {
                    depth = _el[i].depth;
                    len = _el[i].olen;
                    skip = _el[i].skip;
                    if (!_TkRefOk && _el[i].type == MB_Json::MB_JSON_JSMN_OBJECT)
                        ref = _el[i].ref;
                    else if (!_TkRefOk && _el[i].type == MB_Json::MB_JSON_JSMN_ARRAY && searchIndex > -1)
                        ref = _el[i].ref;
                    if (i > 0)
                        _el.erase(_el.begin() + i);
                    else
                        _el.erase(_el.begin());
                    if (printMode != MB_JSON_PRINT_MODE_NONE && !skip)
                    {
                        if (len > 0)
                        {
                            if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                                str += _nl;
                            if (_el[i].type == MB_Json::MB_JSON_JSMN_OBJECT)
                            {
                                if (printMode == MB_JSON_PRINT_MODE_PRETTY && !ref)
                                {
                                    for (int j = 0; j < depth + 1; j++)
                                        str += _tab;
                                }
                            }
                            else
                            {
                                if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                                {
                                    for (int j = 0; j < depth + 1; j++)
                                        str += _tab;
                                }
                            }
                        }
                        if (ref)
                            _setRef(depth, false);
                        if (_el[i].type == MB_Json::MB_JSON_JSMN_OBJECT)
                            str += _brk2;
                        else
                            str += _brk4;
                    }
                    return true;
                }
            }
            break;
        }
    }
    return false;
}

bool MB_Json::_updateTkIndex3(uint16_t index, int &depth, char *searchKey, int searchIndex, MB_JSON_PRINT_MODE printMode)
{
    int len = -1;
    bool skip = false;
    bool ref = false;
    for (size_t i = 0; i < _el.size(); i++)
    {
        if (_el[i].depth == depth - 1)
        {
            if (_el[i].type == MB_Json::MB_JSON_JSMN_OBJECT || _el[i].type == MB_Json::MB_JSON_JSMN_ARRAY)
            {
                _el[i].oindex++;
                if (_el[i].oindex >= _el[i].olen)
                {
                    depth = _el[i].depth;
                    len = _el[i].olen;
                    skip = _el[i].skip;
                    if (!_TkRefOk && _el[i].type == MB_Json::MB_JSON_JSMN_OBJECT)
                        ref = _el[i].ref;
                    else if (!_TkRefOk && _el[i].type == MB_Json::MB_JSON_JSMN_ARRAY && searchIndex > -1)
                        ref = _el[i].ref;
                    if (i > 0)
                        _el.erase(_el.begin() + i);
                    else
                        _el.erase(_el.begin());
                    if (depth < _skipDepth)
                        return false;
                    if (printMode != MB_JSON_PRINT_MODE_NONE && skip)
                    {
                        if (len > 0)
                        {
                            if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                                _jsonData._dbuf += _nl;
                            if (_el[i].type == MB_Json::MB_JSON_JSMN_OBJECT)
                            {
                                if (printMode == MB_JSON_PRINT_MODE_PRETTY && !ref)
                                {
                                    for (int j = 0; j < depth + 1 - (_skipDepth + 1); j++)
                                        _jsonData._dbuf += _tab;
                                }
                            }
                            else
                            {
                                if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                                {
                                    for (int j = 0; j < depth + 1 - (_skipDepth + 1); j++)
                                        _jsonData._dbuf += _tab;
                                }
                            }
                        }
                        if (ref)
                            _setRef(depth, false);

                        if (_el[i].type == MB_Json::MB_JSON_JSMN_OBJECT)
                            _jsonData._dbuf += _brk2;
                        else
                            _jsonData._dbuf += _brk4;
                    }
                    return true;
                }
            }
            break;
        }
    }
    return false;
}

void MB_Json::_insertChilds(char *data, MB_JSON_PRINT_MODE printMode)
{
    std::string str = "";
    for (int i = _pathTk.size() - 1; i > _parseCompleted - 1; i--)
    {
        if (_isArrTk(i))
        {
            std::string _str;
            _addArrNodes(_str, str, i, data, printMode);
            str = _str;
            std::string().swap(_str);
        }
        else
        {
            std::string _str;
            _addObjNodes(_str, str, i, data, printMode);
            str = _str;
            std::string().swap(_str);
        }
    }
    if ((int)_pathTk.size() == _parseCompleted)
        str = data;
    _jsonData._dbuf += str;
    std::string().swap(str);
}

void MB_Json::_addArrNodes(std::string &str, std::string &str2, int index, char *data, MB_JSON_PRINT_MODE printMode)
{

    int i = _getArrIndex(index);
    str += _brk3;
    if (printMode == MB_JSON_PRINT_MODE_PRETTY)
        str += _nl;
    for (int k = 0; k <= i; k++)
    {
        if (printMode == MB_JSON_PRINT_MODE_PRETTY)
        {
            for (int j = 0; j < index + 1; j++)
                str += _tab;
        }
        if (k == i)
        {
            if (index == (int)_pathTk.size() - 1)
                str += data;
            else
                str += str2;
        }
        else
        {
            str += _nll;
            str += _cm;
        }

        if (printMode == MB_JSON_PRINT_MODE_PRETTY)
            str += _nl;
    }

    if (printMode == MB_JSON_PRINT_MODE_PRETTY)
    {
        for (int j = 0; j < index; j++)
            str += _tab;
    }
    str += _brk4;
}

void MB_Json::_addObjNodes(std::string &str, std::string &str2, int index, char *data, MB_JSON_PRINT_MODE printMode)
{
    str += _brk1;
    if (printMode == MB_JSON_PRINT_MODE_PRETTY)
    {
        str += _nl;
        for (int j = 0; j < index + 1; j++)
            str += _tab;
    }
    str += _qt;
    str += _pathTk[index].tk.c_str();
    str += _qt;
    if (printMode == MB_JSON_PRINT_MODE_PRETTY)
        str += _pr;
    else
        str += _pr2;
    if (index == (int)_pathTk.size() - 1)
        str += data;
    else
        str += str2;
    if (printMode == MB_JSON_PRINT_MODE_PRETTY)
    {
        str += _nl;
        for (int j = 0; j < index; j++)
            str += _tab;
    }
    str += _brk2;
}

void MB_Json::_parseToken(uint16_t &i, char *buf, int &depth, char *searchKey, int searchIndex, MB_JSON_PRINT_MODE printMode)
{
    tk_index_t tk;
    _getTkIndex(depth, tk);
    MB_Json::mbjs_tok_t *h = &_tokens.get()[i];
    bool oskip = false;
    bool ex = false;
    size_t resLen = _jsonData._dbuf.length();
    if (searchIndex == -2)
        tk.skip = true;
    delay(0);
    if (searchIndex > -1)
    {
        tk_index_t tk2;
        int depth2 = depth - 1;
        _getTkIndex(depth2, tk2);
        if (tk.type == MB_Json::MB_JSON_JSMN_ARRAY && _parseDepth == depth && tk2.oindex == _parentIndex)
        {
            if (tk.oindex == searchIndex)
            {
                _nextToken = i;
                _nextDepth = depth;
                _parentIndex = tk.oindex;

                if ((int)_pathTk.size() != _parseDepth + 1)
                {
                    _tokenMatch = true;
                    _parseCompleted++;
                }
                else
                {
                    if (!_TkRefOk)
                    {
                        _parseCompleted++;
                        _refTkIndex = i + 1;
                        _refToken = i + 1;
                        _TkRefOk = true;
                        char *dat1 = _newS(h->end - h->start + 10);
                        strncpy(dat1, buf + h->start, h->end - h->start);
                        _jsonData.stringValue = dat1;
                        _delS(dat1);
                        _jsonData._type = h->type;
                        _jsonData._k_start = h->start;
                        _jsonData._start = h->start;
                        _jsonData._end = h->end;
                        _jsonData._tokenIndex = i;
                        _jsonData._depth = depth;
                        _jsonData._len = h->size;
                        _jsonData.success = true;
                        _setElementType();
                        if (printMode != MB_JSON_PRINT_MODE_NONE)
                            _jsonData.stringValue = "";
                        else
                        {
                            std::string().swap(_jsonData._dbuf);
                            std::string().swap(_tbuf);
                            _tokenMatch = true;
                            ex = true;
                        }
                    }
                }
            }
            else
            {
                if (tk.oindex + 1 == tk.olen)
                {
                    _setRef(depth - 1, false);
                    _setRef(depth, true);
                }
            }
        }
    }
    else
    {
        char *key = _newS(h->end - h->start + 10);
        strncpy(key, buf + h->start, h->end - h->start);
        if (tk.type != MB_Json::MB_JSON_JSMN_UNDEFINED && _parseDepth == depth)
        {
            if (strcmp(searchKey, key) == 0)
            {
                _nextToken = i + 1;
                _nextDepth = depth;
                _parentIndex = tk.oindex;
                if ((int)_pathTk.size() != _parseDepth + 1)
                {
                    _tokenMatch = true;
                    _parseCompleted++;
                }
                else
                {
                    if (!_TkRefOk)
                    {
                        _parseCompleted++;
                        _refTkIndex = i + 1;
                        _refToken = i + 1;
                        _TkRefOk = true;
                        h = &_tokens.get()[i + 1];
                        char *dat2 = _newS(h->end - h->start + 10);
                        strncpy(dat2, buf + h->start, h->end - h->start);
                        _jsonData.stringValue = dat2;
                        _delS(dat2);
                        _jsonData._type = h->type;
                        _jsonData._k_start = h->start;
                        _jsonData._start = h->start;
                        _jsonData._end = h->end;
                        _jsonData._tokenIndex = i;
                        _jsonData._depth = depth;
                        _jsonData._len = h->size;
                        _jsonData.success = true;
                        _setElementType();
                        if (printMode != MB_JSON_PRINT_MODE_NONE)
                            _jsonData.stringValue = "";
                        else
                        {
                            std::string().swap(_jsonData._dbuf);
                            std::string().swap(_tbuf);
                            _tokenMatch = true;
                            ex = true;
                        }
                    }
                }
            }
            else
            {
                if (tk.oindex + 1 == tk.olen)
                {
                    _setRef(depth - 1, false);
                    _setRef(depth, true);
                }
            }
        }
        _delS(key);
    }
    if (ex)
        return;
    if (_refTkIndex == i + 1)
    {
        if (tk.type == MB_Json::MB_JSON_JSMN_OBJECT)
            oskip = true;
        tk.skip = true;
        _skipDepth = depth;
    }
    h = &_tokens.get()[i];
    if (h->type == MB_Json::MB_JSON_JSMN_OBJECT || h->type == MB_Json::MB_JSON_JSMN_ARRAY)
    {
        if (printMode != MB_JSON_PRINT_MODE_NONE && (tk.skip || _refTkIndex == i + 1))
        {
            if (!tk.omark && i > 0 && resLen > 0)
            {
                if (tk.oindex > 0)
                    _jsonData._dbuf += _cm;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY && h->size >= 0)
                    _jsonData._dbuf += _nl;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY && h->size >= 0)
                {
                    for (int j = 0; j < depth - (_skipDepth + 1); j++)
                        _jsonData._dbuf += _tab;
                    _jsonData._dbuf += _tab;
                }
            }
            if (h->type == MB_Json::MB_JSON_JSMN_OBJECT)
                _jsonData._dbuf += _brk1;
            else
                _jsonData._dbuf += _brk3;
        }
        el_t e;
        e.index = i;
        e.olen = h->size;
        e.type = h->type;
        e.oindex = 0;
        e.depth = depth;
        e.omark = false;
        e.ref = false;
        if (_refToken != -1)
            e.skip = true;
        else
            e.skip = tk.skip;
        _el.push_back(e);
        depth++;
        if (h->size == 0)
        {
            while (_updateTkIndex3(i, depth, searchKey, searchIndex, printMode))
            {
                delay(0);
            }
        }
    }
    else
    {
        char *tmp = _newS(h->end - h->start + 10);
        if (buf[h->start - 1] != '"')
            strncpy(tmp, buf + h->start, h->end - h->start);
        else
            strncpy(tmp, buf + h->start - 1, h->end - h->start + 2);
        if (h->size > 0)
        {
            if (printMode != MB_JSON_PRINT_MODE_NONE && tk.skip && !oskip)
            {
                if (tk.oindex > 0)
                    _jsonData._dbuf += _cm;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                    _jsonData._dbuf += _nl;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY && h->size > 0)
                {
                    for (int j = 0; j < depth - (_skipDepth + 1); j++)
                        _jsonData._dbuf += _tab;
                    _jsonData._dbuf += _tab;
                }
                _jsonData._dbuf += tmp;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                    _jsonData._dbuf += _pr;
                else
                    _jsonData._dbuf += _pr2;
            }
            if (_collectTk)
            {
                eltk_t el;
                el.index = i;
                el.type = 0;
                _eltk.push_back(el);
            }
            tmp = _newS(tmp, h->end - h->start + 10);
            strncpy(tmp, buf + h->start, h->end - h->start);
            h = &_tokens.get()[i + 1];
            if (h->type != MB_Json::MB_JSON_JSMN_OBJECT && h->type != MB_Json::MB_JSON_JSMN_ARRAY)
            {
                _delS(tmp);
                tmp = _newS(h->end - h->start + 10);
                strncpy(tmp, buf + h->start, h->end - h->start);
                if (printMode != MB_JSON_PRINT_MODE_NONE && tk.skip)
                {
                    if (buf[h->start - 1] != '"')
                        strncpy(tmp, buf + h->start, h->end - h->start);
                    else
                        strncpy(tmp, buf + h->start - 1, h->end - h->start + 2);
                    _jsonData._dbuf += tmp;
                }
                i++;
                while (_updateTkIndex3(i, depth, searchKey, searchIndex, printMode))
                {
                    delay(0);
                }
            }
            else
            {
                if (_refToken == i + 1)
                {
                    _setSkip(depth, true);
                }
                _setMark(depth, true);
            }
        }
        else
        {
            if (printMode != MB_JSON_PRINT_MODE_NONE && tk.skip)
            {
                if (tk.oindex > 0 && resLen > 0)
                {
                    _jsonData._dbuf += _cm;
                }
                if (printMode == MB_JSON_PRINT_MODE_PRETTY && resLen > 0)
                    _jsonData._dbuf += _nl;

                if (printMode == MB_JSON_PRINT_MODE_PRETTY && tk.olen > 0 && resLen > 0)
                {
                    for (int j = 0; j < depth - (_skipDepth + 1); j++)
                        _jsonData._dbuf += _tab;
                    _jsonData._dbuf += _tab;
                }
                _jsonData._dbuf += tmp;
            }
            while (_updateTkIndex3(i, depth, searchKey, searchIndex, printMode))
            {
                delay(0);
            }
            if (_collectTk)
            {
                eltk_t el;
                el.index = i;
                el.type = 1;
                _eltk.push_back(el);
            }
        }
        _delS(tmp);

        if (_refToken == -1 && _skipDepth == depth)
            _setSkip(depth, false);
    }
    _nextToken = i + 1;
    _refToken = -1;
}

void MB_Json::_compileToken(uint16_t &i, char *buf, int &depth, char *searchKey, int searchIndex, MB_JSON_PRINT_MODE printMode, char *replace, int refTokenIndex, bool removeTk)
{
    if (_tokenMatch)
        return;
    tk_index_t tk;
    _getTkIndex(depth, tk);
    MB_Json::mbjs_tok_t *h = &_tokens.get()[i];
    bool insertFlag = false;
    bool ex = false;
    delay(0);
    if (searchIndex > -1)
    {
        tk_index_t tk2;
        int depth2 = depth - 1;
        _getTkIndex(depth2, tk2);
        if (tk.type == MB_Json::MB_JSON_JSMN_ARRAY && _parseDepth == depth && tk2.oindex == _parentIndex)
        {
            if (tk.oindex == searchIndex)
            {
                _nextToken = i;
                _nextDepth = depth;
                _parentIndex = tk.oindex;
                if ((int)_pathTk.size() != _parseDepth + 1)
                {
                    _tokenMatch = true;
                    _parseCompleted++;
                    _refTkIndex = i + 1;
                }
                else
                {
                    if (!_TkRefOk)
                    {
                        _parseCompleted++;
                        _refTkIndex = i + 1;
                        _refToken = i + 1;
                        _TkRefOk = true;
                        single_child_parent_t p = _findSCParent(depth);
                        if (p.success)
                        {
                            _remTkIndex = p.index + 1;
                            _remFirstTk = p.firstTk;
                            _remLastTk = p.lastTk;
                        }
                        else
                        {
                            _remTkIndex = i + 1;
                            _remFirstTk = tk.oindex == 0;
                            _remLastTk = tk.oindex + 1 == tk.olen;
                        }
                    }
                }
            }
            else
            {
                if (tk.oindex + 1 == tk.olen)
                {
                    _setRef(depth - 1, false);
                    _setRef(depth, true);
                }
            }
        }
    }
    else
    {
        char *key = _newS(h->end - h->start + 10);
        strncpy(key, buf + h->start, h->end - h->start);
        if (tk.type != MB_Json::MB_JSON_JSMN_UNDEFINED && _parseDepth == depth)
        {
            if (strcmp(searchKey, key) == 0)
            {
                _nextToken = i + 1;
                _nextDepth = depth;
                _parentIndex = tk.oindex;
                if ((int)_pathTk.size() != _parseDepth + 1)
                {
                    _tokenMatch = true;
                    _parseCompleted++;
                    _refTkIndex = i + 1;
                }
                else
                {
                    if (!_TkRefOk)
                    {
                        _parseCompleted++;
                        _refTkIndex = i + 1;
                        _refToken = i + 1;
                        _TkRefOk = true;
                        single_child_parent_t p = _findSCParent(depth);
                        if (p.success)
                        {
                            _remTkIndex = p.index + 1;
                            _remFirstTk = p.firstTk;
                            _remLastTk = p.lastTk;
                        }
                        else
                        {
                            _remTkIndex = i + 1;
                            _remFirstTk = tk.oindex == 0;
                            _remLastTk = tk.oindex + 1 == tk.olen;
                        }
                    }
                }
            }
            else
            {
                if (tk.oindex + 1 == tk.olen)
                {
                    _setRef(depth - 1, false);
                    _setRef(depth, true);
                }
            }
        }
        else
        {
            if (_tokenCount == 1 && h->size == 0 && !removeTk)
            {
                _insertChilds(replace, printMode);
                _nextToken = i + 1;
                _nextDepth = 0;
                _parseCompleted = _pathTk.size();
                _tokenMatch = true;
                ex = true;
            }
        }
        _delS(key);
    }
    if (ex)
        return;

    h = &_tokens.get()[i];
    if (h->type == MB_Json::MB_JSON_JSMN_OBJECT || h->type == MB_Json::MB_JSON_JSMN_ARRAY)
    {
        if (printMode != MB_JSON_PRINT_MODE_NONE && !tk.skip)
        {
            if (!tk.omark && i > 0)
            {
                if (tk.oindex > 0)
                    _jsonData._dbuf += _cm;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY && h->size >= 0)
                    _jsonData._dbuf += _nl;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY && h->size >= 0)
                {
                    for (int j = 0; j < depth; j++)
                        _jsonData._dbuf += _tab;
                    _jsonData._dbuf += _tab;
                }
            }
            if (_refToken == -1)
            {
                if (h->type == MB_Json::MB_JSON_JSMN_OBJECT)
                    _jsonData._dbuf += _brk1;
                else
                    _jsonData._dbuf += _brk3;
            }
            else if (_refToken != -1 && searchIndex > -1)
                _jsonData._dbuf += replace;
        }
        el_t e;
        e.index = i;
        e.olen = h->size;
        e.type = h->type;
        e.oindex = 0;
        e.depth = depth;
        e.omark = false;
        e.ref = false;
        if (_refToken != -1)
            e.skip = true;
        else
            e.skip = tk.skip;
        _el.push_back(e);
        depth++;
        if (h->size == 0)
        {
            while (_updateTkIndex(i, depth, searchKey, searchIndex, replace, printMode, removeTk))
            {
                delay(0);
            }
        }
    }
    else
    {
        if (_refTkIndex == refTokenIndex && refTokenIndex > -1)
        {
            _refToken = refTokenIndex;
            _refTkIndex = -1;
            insertFlag = true;
        }
        char *tmp = _newS(h->end - h->start + 10);
        if (buf[h->start - 1] != '"')
            strncpy(tmp, buf + h->start, h->end - h->start);
        else
            strncpy(tmp, buf + h->start - 1, h->end - h->start + 2);
        if (h->size > 0)
        {
            if (printMode != MB_JSON_PRINT_MODE_NONE && !tk.skip)
            {
                if (tk.oindex > 0)
                    _jsonData._dbuf += _cm;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                    _jsonData._dbuf += _nl;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY && h->size > 0)
                {
                    for (int j = 0; j < depth; j++)
                        _jsonData._dbuf += _tab;
                    _jsonData._dbuf += _tab;
                }
                _jsonData._dbuf += tmp;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                    _jsonData._dbuf += _pr;
                else
                    _jsonData._dbuf += _pr2;
            }
            tmp = _newS(tmp, h->end - h->start + 10);
            strncpy(tmp, buf + h->start, h->end - h->start);
            h = &_tokens.get()[i + 1];
            if (h->type != MB_Json::MB_JSON_JSMN_OBJECT && h->type != MB_Json::MB_JSON_JSMN_ARRAY)
            {
                tmp = _newS(tmp, h->end - h->start + 10);
                strncpy(tmp, buf + h->start, h->end - h->start);

                if (printMode != MB_JSON_PRINT_MODE_NONE && !tk.skip)
                {
                    if (buf[h->start - 1] != '"')
                        strncpy(tmp, buf + h->start, h->end - h->start);
                    else
                        strncpy(tmp, buf + h->start - 1, h->end - h->start + 2);
                    if (_refToken == i + 1)
                    {
                        if (!insertFlag)
                            _jsonData._dbuf += replace;
                        else
                            _insertChilds(replace, printMode);
                    }
                    else
                        _jsonData._dbuf += tmp;
                }
                i++;
                while (_updateTkIndex(i, depth, searchKey, searchIndex, replace, printMode, removeTk))
                {
                    delay(0);
                }
            }
            else
            {
                if (_refToken == i + 1)
                {
                    _setSkip(depth, true);
                    _skipDepth = depth;
                    if (!insertFlag)
                        _jsonData._dbuf += replace;
                    else
                        _insertChilds(replace, printMode);
                    if (printMode != MB_JSON_PRINT_MODE_NONE && (depth > 0 || tk.oindex == tk.olen - 1))
                    {
                        if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                            _jsonData._dbuf += _nl;
                        if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                        {
                            for (int j = 0; j < depth; j++)
                                _jsonData._dbuf += _tab;
                        }
                        _jsonData._dbuf += _brk2;
                    }
                }
                _setMark(depth, true);
            }
        }
        else
        {
            if (printMode != MB_JSON_PRINT_MODE_NONE && !tk.skip)
            {
                if (tk.oindex > 0)
                    _jsonData._dbuf += _cm;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                    _jsonData._dbuf += _nl;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY && tk.olen > 0)
                {
                    for (int j = 0; j < depth; j++)
                        _jsonData._dbuf += _tab;
                    _jsonData._dbuf += _tab;
                }

                if (_refToken == i + 1 && !_arrInserted)
                {
                    if (!insertFlag)
                        _jsonData._dbuf += replace;
                    else
                        _insertChilds(replace, printMode);
                    _arrInserted = true;
                }
                else
                    _jsonData._dbuf += tmp;
            }
            while (_updateTkIndex(i, depth, searchKey, searchIndex, replace, printMode, removeTk))
            {
                delay(0);
            }
        }
        _delS(tmp);

        if (_refToken == -1 && _skipDepth == depth)
            _setSkip(depth, false);
    }
    _nextToken = i + 1;
    _refToken = -1;
}

void MB_Json::_removeToken(uint16_t &i, char *buf, int &depth, char *searchKey, int searchIndex, MB_JSON_PRINT_MODE printMode, char *replace, int refTokenIndex, bool removeTk)
{
    bool ncm = false;
    tk_index_t tk;
    _getTkIndex(depth, tk);
    MB_Json::mbjs_tok_t *h = &_tokens.get()[i];
    delay(0);
    if (refTokenIndex == i && refTokenIndex > -1)
        ncm = _remFirstTk;
    if (refTokenIndex != i || (refTokenIndex == i && _remLastTk))
        _jsonData._dbuf += _tbuf;
    _tbuf.clear();
    bool flag = tk.oindex > 0 && !ncm && _jsonData._dbuf.c_str()[_jsonData._dbuf.length() - 1] != '{' && _jsonData._dbuf.c_str()[_jsonData._dbuf.length() - 1] != '[';
    if (refTokenIndex == i + 1 && refTokenIndex > -1)
    {
        _refToken = refTokenIndex;
        _refTkIndex = -1;
        tk.skip = true;
    }
    h = &_tokens.get()[i];
    if (h->type == MB_Json::MB_JSON_JSMN_OBJECT || h->type == MB_Json::MB_JSON_JSMN_ARRAY)
    {
        if (printMode != MB_JSON_PRINT_MODE_NONE && !tk.skip)
        {
            if (!tk.omark && i > 0)
            {
                if (flag)
                    _tbuf += _cm;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY && h->size >= 0)
                    _tbuf += _nl;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY && h->size >= 0)
                {
                    for (int j = 0; j < depth; j++)
                        _tbuf += _tab;
                    _tbuf += _tab;
                }
            }
            if (_refToken == -1)
            {
                if (h->type == MB_Json::MB_JSON_JSMN_OBJECT)
                    _tbuf += _brk1;
                else
                    _tbuf += _brk3;
            }
            else if (_refToken != -1 && searchIndex > -1)
                _tbuf += replace;
        }
        el_t e;
        e.index = i;
        e.olen = h->size;
        e.type = h->type;
        e.oindex = 0;
        e.depth = depth;
        e.omark = false;
        e.ref = false;
        if (_refToken != -1)
            e.skip = true;
        else
            e.skip = tk.skip;
        _el.push_back(e);
        depth++;
        if (h->size == 0)
        {
            while (_updateTkIndex2(_tbuf, i, depth, searchKey, searchIndex, replace, printMode))
            {
                delay(0);
            }
        }
    }
    else
    {
        char *tmp = _newS(h->end - h->start + 10);
        if (buf[h->start - 1] != '"')
            strncpy(tmp, buf + h->start, h->end - h->start);
        else
            strncpy(tmp, buf + h->start - 1, h->end - h->start + 2);
        if (h->size > 0)
        {
            if (printMode != MB_JSON_PRINT_MODE_NONE && !tk.skip)
            {
                if (flag)
                    _tbuf += _cm;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                    _tbuf += _nl;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY && h->size > 0)
                {
                    for (int j = 0; j < depth; j++)
                        _tbuf += _tab;
                    _tbuf += _tab;
                }
                _tbuf += tmp;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                    _tbuf += _pr;
                else
                    _tbuf += _pr2;
            }
            tmp = _newS(tmp, h->end - h->start + 10);
            strncpy(tmp, buf + h->start, h->end - h->start);
            h = &_tokens.get()[i + 1];
            if (h->type != MB_Json::MB_JSON_JSMN_OBJECT && h->type != MB_Json::MB_JSON_JSMN_ARRAY)
            {
                tmp = _newS(tmp, h->end - h->start + 10);
                strncpy(tmp, buf + h->start, h->end - h->start);
                if (printMode != MB_JSON_PRINT_MODE_NONE && !tk.skip)
                {
                    if (buf[h->start - 1] != '"')
                        strncpy(tmp, buf + h->start, h->end - h->start);
                    else
                        strncpy(tmp, buf + h->start - 1, h->end - h->start + 2);
                    _tbuf += tmp;
                }
                i++;
                while (_updateTkIndex2(_tbuf, i, depth, searchKey, searchIndex, replace, printMode))
                {
                    delay(0);
                }
            }
            else
            {
                if (_refToken == i + 1)
                {
                    _setSkip(depth, true);
                    _skipDepth = depth;
                    _tbuf += replace;
                    if (printMode != MB_JSON_PRINT_MODE_NONE && (depth > 0 || tk.oindex == tk.olen - 1))
                    {
                        if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                            _tbuf += _nl;
                        if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                        {
                            for (int j = 0; j < depth; j++)
                                _tbuf += _tab;
                        }
                        _tbuf += _brk2;
                    }
                }
                _setMark(depth, true);
            }
        }
        else
        {
            if (printMode != MB_JSON_PRINT_MODE_NONE && !tk.skip)
            {
                if (flag)
                    _tbuf += _cm;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY)
                    _tbuf += _nl;
                if (printMode == MB_JSON_PRINT_MODE_PRETTY && tk.olen > 0)
                {
                    for (int j = 0; j < depth; j++)
                        _tbuf += _tab;
                    _tbuf += _tab;
                }
                _tbuf += tmp;
            }
            while (_updateTkIndex2(_tbuf, i, depth, searchKey, searchIndex, replace, printMode))
            {
                delay(0);
            }
        }
        _delS(tmp);

        if (_refToken == -1 && _skipDepth == depth)
            _setSkip(depth, false);
    }
    _nextToken = i + 1;
    _refToken = -1;
    _lastTk.olen = tk.olen;
    _lastTk.oindex = tk.oindex;
    _lastTk.type = tk.type;
    _lastTk.depth = tk.depth;
    _lastTk.index = tk.index;
    _lastTk.skip = tk.skip;
}

MB_Json::single_child_parent_t MB_Json::_findSCParent(int depth)
{
    single_child_parent_t res;
    res.index = -1;
    res.firstTk = false;
    res.lastTk = false;
    res.success = false;
    for (int i = depth; i >= 0; i--)
    {
        bool match = false;
        for (size_t j = 0; j < _el.size(); j++)
        {
            if (_el[j].depth == i - 1 && _el[i].olen == 1)
            {
                match = true;
                res.index = _el[i].index;
                res.firstTk = _el[j].oindex == 0;
                res.lastTk = _el[j].oindex + 1 == _el[j].olen;
                res.success = true;
            }
        }
        if (!match)
            break;
    }
    return res;
}

void MB_Json::_get(const char *key, int depth, int index)
{
    _tokenMatch = false;
    if (_paresRes)
    {
        std::string s;
        _toStdString(s);
        int bufLen = s.length() + 1024;
        char *buf = _newS(bufLen);
        strcpy(buf, s.c_str());
        std::string().swap(s);

        if (_jsonData.success)
        {
            _jsonData._dbuf.clear();
            _parseDepth = depth;
            if (_nextToken < 0)
                _nextToken = 0;
            for (uint16_t i = _nextToken; i < _tokenCount; i++)
            {
                _parseToken(i, buf, _nextDepth, (char *)key, index, MB_JSON_PRINT_MODE_NONE);
                if (_tokenMatch)
                    break;
            }
        }
        _delS(buf);
        if (!_tokenMatch)
        {
            _paresRes = false;
            _jsonData.success = false;
            _resetParseResult();
        }
    }
}

void MB_Json::_strToTk(const std::string &str, std::vector<path_tk_t> &tk, char delim)
{
    std::size_t current, previous = 0;
    current = str.find(delim);
    std::string s;
    while (current != std::string::npos)
    {
        s = str.substr(previous, current - previous);
        _trim(s);
        if (s.length() > 0)
        {
            path_tk_t tk_t;
            tk_t.tk = s;
            tk.push_back(tk_t);
        }

        previous = current + 1;
        current = str.find(delim, previous);
        delay(0);
    }
    s = str.substr(previous, current - previous);
    _trim(s);
    if (s.length() > 0)
    {
        path_tk_t tk_t;
        tk_t.tk = s;
        tk.push_back(tk_t);
    }
    std::string().swap(s);
}

void MB_Json::_ltrim(std::string &str, const std::string &chars)
{
    str.erase(0, str.find_first_not_of(chars));
}

void MB_Json::_rtrim(std::string &str, const std::string &chars)
{
    str.erase(str.find_last_not_of(chars) + 1);
}

void MB_Json::_trim(std::string &str, const std::string &chars)
{
    _ltrim(str, chars);
    _rtrim(str, chars);
}

void MB_Json::_parse(const char *path, MB_JSON_PRINT_MODE printMode)
{
    clearPathTk();
    std::string _path;

    if (_topLevelTkType == MB_JSON_JSMN_ARRAY)
    {
        char *_root = _strP(mb_json_str_26);
        char *_slash = _strP(mb_json_str_27);
        _path = _root;
        _path += _slash;
        _path += path;
        _delS(_root);
        _delS(_slash);
    }
    else
        _path = path;

    _strToTk(_path.c_str(), _pathTk, '/');
    _mbjs_parse();
    std::string().swap(_path);
    if (!_jsonData.success)
        return;
    _jsonData.success = false;
    char *nbuf = _newS(2);
    int len = _pathTk.size();
    _nextDepth = -1;
    _nextToken = 0;
    _skipDepth = -1;
    _parentIndex = -1;
    _TkRefOk = false;
    _parseCompleted = 0;
    _arrReplaced = false;
    _refTkIndex = -1;
    _remTkIndex = -1;
    _remFirstTk = false;
    _remLastTk = false;
    _el.clear();
    _eltk.clear();
    if (len == 0)
    {
        _parse(nbuf, 0, -2, printMode);
        _jsonData.success = true;
    }
    else
    {
        for (int i = 0; i < len; i++)
        {
            if (_isStrTk(i))
                _parse(_pathTk[i].tk.c_str(), i, -1, printMode);
            else if (_isArrTk(i))
                _parse(nbuf, i, _getArrIndex(i), printMode);
            else
                _parse(_pathTk[i].tk.c_str(), i, -1, printMode);
        }
        _jsonData.success = _parseCompleted == len;
    }
    _el.clear();
    _eltk.clear();
    _delS(nbuf);
    clearPathTk();
    std::string().swap(_tbuf);
    _tokens.reset();
    _tokens = nullptr;
}

void MB_Json::clearPathTk()
{
    size_t len = _pathTk.size();
    for (size_t i = 0; i < len; i++)
        std::string().swap(_pathTk[i].tk);
    for (size_t i = 0; i < len; i++)
        _pathTk.erase(_pathTk.end());
    _pathTk.clear();
    std::vector<path_tk_t>().swap(_pathTk);
}

void MB_Json::_parse(const char *key, int depth, int index, MB_JSON_PRINT_MODE printMode)
{
    _tokenMatch = false;
    if (_paresRes)
    {
        std::string s;
        _toStdString(s);
        int bufLen = s.length() + 1024;
        char *buf = _newS(bufLen);
        strcpy(buf, s.c_str());
        std::string().swap(s);
        _parseDepth = depth;
        if (_nextToken < 0)
            _nextToken = 0;

        for (uint16_t i = _nextToken; i < _tokenCount; i++)
        {

            int oDepth = _nextDepth;

            _parseToken(i, buf, _nextDepth, (char *)key, index, printMode);

            if (index > -1 && oDepth == _nextDepth && _tokenMatch)
            {
                _tokenMatch = false;
                break;
            }

            if (oDepth > _nextDepth && index == -1)
            {
                if (_nextDepth > -1 && _nextDepth < (int)_pathTk.size())
                {
                    if (_pathTk[_nextDepth].matched)
                    {
                        _tokenMatch = false;
                        break;
                    }
                }
            }

            if (_tokenMatch)
            {
                _pathTk[depth].matched = true;
                break;
            }
        }

        _delS(buf);
        if (!_tokenMatch)
        {
            _paresRes = false;
            _jsonData.success = false;
        }
    }
}

void MB_Json::_compile(const char *key, int depth, int index, const char *replace, MB_JSON_PRINT_MODE printMode, int refTokenIndex, bool removeTk)
{
    _tokenMatch = false;
    if (_paresRes)
    {
        std::string s;
        _toStdString(s);
        int bufLen = s.length() + 1024;
        char *buf = _newS(bufLen);
        strcpy(buf, s.c_str());
        std::string().swap(s);
        _parseDepth = depth;
        if (_nextToken < 0)
            _nextToken = 0;
        for (uint16_t i = _nextToken; i < _tokenCount; i++)
        {
            _compileToken(i, buf, _nextDepth, (char *)key, index, printMode, (char *)replace, refTokenIndex, removeTk);
            if (_tokenMatch)
                break;
        }
        _delS(buf);
        if (!_tokenMatch)
        {
            _paresRes = false;
            _jsonData.success = false;
        }
    }
}

void MB_Json::_remove(const char *key, int depth, int index, const char *replace, int refTokenIndex, bool removeTk)
{
    if (_paresRes)
    {
        std::string s;
        _toStdString(s);
        int bufLen = s.length() + 1024;
        char *buf = _newS(bufLen);
        strcpy(buf, s.c_str());
        std::string().swap(s);
        _parseDepth = depth;
        if (_nextToken < 0)
            _nextToken = 0;
        for (uint16_t i = _nextToken; i < _tokenCount; i++)
        {
            _removeToken(i, buf, _nextDepth, (char *)key, index, MB_JSON_PRINT_MODE_PLAIN, (char *)replace, refTokenIndex, removeTk);
        }
        _delS(buf);
    }
}

bool MB_Json::_isArrTk(int index)
{
    if (index < (int)_pathTk.size())
        return _pathTk[index].tk.c_str()[0] == '[' && _pathTk[index].tk.c_str()[_pathTk[index].tk.length() - 1] == ']';
    else
        return false;
}
bool MB_Json::_isStrTk(int index)
{
    if (index < (int)_pathTk.size())
        return _pathTk[index].tk.c_str()[0] == '"' && _pathTk[index].tk.c_str()[_pathTk[index].tk.length() - 1] == '"';
    else
        return false;
}

int MB_Json::_getArrIndex(int index)
{
    int res = -1;
    if (index < (int)_pathTk.size())
    {
        res = atoi(_pathTk[index].tk.substr(1, _pathTk[index].tk.length() - 2).c_str());
        if (res < 0)
            res = 0;
    }
    return res;
}

void MB_Json::set(const String &path)
{
    _setNull(path.c_str());
}

void MB_Json::set(const String &path, const String &value)
{
    _setString(path.c_str(), value.c_str());
}

void MB_Json::set(const String &path, const char *value)
{
    _setString(path.c_str(), value);
}

void MB_Json::set(const String &path, int value)
{
    _setInt(path.c_str(), value);
}

void MB_Json::set(const String &path, unsigned short value)
{
    _setInt(path.c_str(), value);
}

void MB_Json::set(const String &path, float value)
{
    _setFloat(path.c_str(), value);
}

void MB_Json::set(const String &path, double value)
{
    _setDouble(path.c_str(), value);
}

void MB_Json::set(const String &path, bool value)
{
    _setBool(path.c_str(), value);
}

void MB_Json::set(const String &path, MB_Json &json)
{
    _setJson(path.c_str(), &json);
}

void MB_Json::set(const String &path, MB_JsonArray &arr)
{
    _setArray(path.c_str(), &arr);
}

template <typename T>
bool MB_Json::set(const String &path, T value)
{
    if (std::is_same<T, int>::value)
        return _setInt(path, value);
    else if (std::is_same<T, float>::value)
        return _setFloat(path, value);
    else if (std::is_same<T, double>::value)
        return _setDouble(path, value);
    else if (std::is_same<T, bool>::value)
        return _setBool(path, value);
    else if (std::is_same<T, const char *>::value)
        return _setString(path, value);
    else if (std::is_same<T, MB_Json &>::value)
        return _setJson(path, &value);
    else if (std::is_same<T, MB_JsonArray &>::value)
        return _setArray(path, &value);
}

void MB_Json::_setString(const std::string &path, const std::string &value)
{
    char *tmp = _newS(value.length() + 1024);
    strcpy(tmp, _qt);
    strcat(tmp, value.c_str());
    strcat(tmp, _qt);
    _set(path.c_str(), tmp);
    _delS(tmp);
    std::string().swap(_jsonData._dbuf);
}

void MB_Json::_setInt(const std::string &path, int value)
{
    char *tmp = intStr(value);
    _set(path.c_str(), tmp);
    _delS(tmp);
    std::string().swap(_jsonData._dbuf);
}

void MB_Json::_setFloat(const std::string &path, float value)
{
    char *tmp = floatStr(value);
    _trimDouble(tmp);
    _set(path.c_str(), tmp);
    _delS(tmp);
    std::string().swap(_jsonData._dbuf);
}

void MB_Json::_setDouble(const std::string &path, double value)
{
    char *tmp = doubleStr(value);
    _trimDouble(tmp);
    _set(path.c_str(), tmp);
    _delS(tmp);
    std::string().swap(_jsonData._dbuf);
}

void MB_Json::_setBool(const std::string &path, bool value)
{
    if (value)
        _set(path.c_str(), _tr);
    else
        _set(path.c_str(), _fls);
    std::string().swap(_jsonData._dbuf);
}

void MB_Json::_setNull(const std::string &path)
{
    _set(path.c_str(), _nll);
    std::string().swap(_jsonData._dbuf);
}

void MB_Json::_setJson(const std::string &path, MB_Json *json)
{
    std::string s;
    json->_toStdString(s);
    _set(path.c_str(), s.c_str());
    std::string().swap(s);
}

void MB_Json::_setArray(const std::string &path, MB_JsonArray *arr)
{
    std::string s;
    arr->_toStdString(s);
    _set(path.c_str(), s.c_str());
    std::string().swap(s);
}

void MB_Json::_set(const char *path, const char *data)
{
    clearPathTk();
    std::string _path;

    if (_topLevelTkType == MB_JSON_JSMN_ARRAY)
    {
        char *_root = _strP(mb_json_str_26);
        char *_slash = _strP(mb_json_str_27);
        _path = _root;
        _path += _slash;
        _path += path;
        _delS(_root);
        _delS(_slash);
    }
    else
        _path = path;

    _strToTk(_path.c_str(), _pathTk, '/');
    _mbjs_parse();
    std::string().swap(_path);
    if (!_jsonData.success)
        return;
    _jsonData.success = false;
    char *nbuf = _newS(2);
    int len = _pathTk.size();
    _nextDepth = -1;
    _nextToken = 0;
    _skipDepth = -1;
    _parentIndex = -1;
    _TkRefOk = false;
    _parseCompleted = 0;
    _arrReplaced = false;
    _arrInserted = false;
    _refTkIndex = -1;
    _remTkIndex = -1;
    _remFirstTk = false;
    _remLastTk = false;
    _el.clear();
    _eltk.clear();
    for (int i = 0; i < len; i++)
    {
        if (_isStrTk(i))
            _compile(_pathTk[i].tk.c_str(), i, -1, data, MB_JSON_PRINT_MODE_PLAIN);
        else if (_isArrTk(i))
            _compile(nbuf, i, _getArrIndex(i), data, MB_JSON_PRINT_MODE_PLAIN);
        else
            _compile(_pathTk[i].tk.c_str(), i, -1, data, MB_JSON_PRINT_MODE_PLAIN);
    }
    _el.clear();
    _eltk.clear();
    if (_parseCompleted != len)
    {
        std::string().swap(_jsonData._dbuf);
        std::string().swap(_tbuf);
        int refTokenIndex = _refTkIndex;
        _nextDepth = -1;
        _nextToken = 0;
        _skipDepth = -1;
        _parentIndex = -1;
        _TkRefOk = false;
        _parseCompleted = 0;
        _arrReplaced = false;
        _refTkIndex = -1;
        _tokenMatch = false;
        _paresRes = true;
        for (int i = 0; i < len; i++)
        {
            if (_isStrTk(i))
                _compile(_pathTk[i].tk.c_str(), i, -1, data, MB_JSON_PRINT_MODE_PLAIN, refTokenIndex);
            else if (_isArrTk(i))
                _compile(nbuf, i, _getArrIndex(i), data, MB_JSON_PRINT_MODE_PLAIN, refTokenIndex);
            else
                _compile(_pathTk[i].tk.c_str(), i, -1, data, MB_JSON_PRINT_MODE_PLAIN, refTokenIndex);
        }
        _el.clear();
        _eltk.clear();
    }
    _delS(nbuf);
    if (_jsonData._dbuf.length() >= 2)
    {
        _jsonData.success = true;
        _rawbuf = _jsonData._dbuf.substr(1, _jsonData._dbuf.length() - 2);
    }
    else
        _rawbuf.clear();
    clearPathTk();
    std::string().swap(_jsonData._dbuf);
    std::string().swap(_tbuf);
    _tokens.reset();
    _tokens = nullptr;
}

bool MB_Json::remove(const String &path)
{
    clearPathTk();
    std::string _path;

    if (_topLevelTkType == MB_JSON_JSMN_ARRAY)
    {
        char *_root = _strP(mb_json_str_26);
        char *_slash = _strP(mb_json_str_27);
        _path = _root;
        _path += _slash;
        _path += path.c_str();
        _delS(_root);
        _delS(_slash);
    }
    else
        _path = path.c_str();

    _strToTk(_path.c_str(), _pathTk, '/');
    _mbjs_parse();
    std::string().swap(_path);
    if (!_jsonData.success)
        return false;

    _jsonData.success = false;
    char *nbuf = _newS(2);
    int len = _pathTk.size();
    _nextDepth = -1;
    _nextToken = 0;
    _skipDepth = -1;
    _parentIndex = -1;
    _TkRefOk = false;
    _parseCompleted = 0;
    _arrReplaced = false;
    _refTkIndex = -1;
    _remTkIndex = -1;
    _remFirstTk = false;
    _remLastTk = false;
    _el.clear();
    _eltk.clear();
    for (int i = 0; i < len; i++)
    {
        if (_isStrTk(i))
            _compile(_pathTk[i].tk.c_str(), i, -1, nbuf, MB_JSON_PRINT_MODE_NONE, -1, true);
        else if (_isArrTk(i))
            _compile(nbuf, i, _getArrIndex(i), nbuf, MB_JSON_PRINT_MODE_NONE, -1, true);
        else
            _compile(_pathTk[i].tk.c_str(), i, -1, nbuf, MB_JSON_PRINT_MODE_NONE, -1, true);
    }
    _el.clear();
    _eltk.clear();
    std::string().swap(_jsonData._dbuf);
    int refTokenIndex = _remTkIndex;
    if (_parseCompleted == len)
    {
        _nextDepth = -1;
        _nextToken = 0;
        _skipDepth = -1;
        _parentIndex = -1;
        _TkRefOk = false;
        _parseCompleted = 0;
        _arrReplaced = false;
        _refTkIndex = -1;
        _tokenMatch = false;
        _paresRes = true;
        _jsonData.success = true;
        _lastTk.skip = false;
        _lastTk.olen = 0;
        _lastTk.oindex = 0;
        if (_isStrTk(len - 1))
            _remove(_pathTk[len - 1].tk.c_str(), -1, -1, nbuf, refTokenIndex, true);
        else
            _remove(nbuf, -1, _getArrIndex(len - 1), nbuf, refTokenIndex, true);
        _jsonData._dbuf += _tbuf;
        _el.clear();
        _eltk.clear();
    }

    _delS(nbuf);
    if (_jsonData._dbuf.length() >= 2)
        _rawbuf = _jsonData._dbuf.substr(1, _jsonData._dbuf.length() - 2);
    else
        _rawbuf.clear();
    clearPathTk();
    std::string().swap(_jsonData._dbuf);
    std::string().swap(_tbuf);
    _tokens.reset();
    _tokens = nullptr;
    return _jsonData.success;
}

void MB_Json::_resetParseResult()
{
    _jsonData._type = 0;
    _jsonData.type = "";
    _jsonData.typeNum = 0;
    _jsonData.stringValue = "";
    _jsonData._dbuf = "";
    _jsonData.intValue = 0;
    _jsonData.floatValue = 0;
    _jsonData.doubleValue = 0;
    _jsonData.boolValue = false;
}

void MB_Json::_setElementType()
{
    bool typeSet = false;
    char *buf = _newS(1024);
    char *tmp = _newS(1024);
    char *tmp2 = nullptr;
    if (_jsonData._type == MB_Json::MB_JSON_JSMN_PRIMITIVE)
    {
        tmp2 = _newS(tmp2, _jsonData.stringValue.length() + 1);
        strcpy(tmp2, _jsonData.stringValue.c_str());
    }
    switch (_jsonData._type)
    {
    case MB_Json::MB_JSON_JSMN_UNDEFINED:
        strcpy(buf, _undef);
        _jsonData.typeNum = JSON_UNDEFINED;
        break;
    case MB_Json::MB_JSON_JSMN_OBJECT:
        strcpy(buf, _obj);
        _jsonData.typeNum = JSON_OBJECT;
        break;
    case MB_Json::MB_JSON_JSMN_ARRAY:
        strcpy(buf, _arry);
        _jsonData.typeNum = JSON_ARRAY;
        break;
    case MB_Json::MB_JSON_JSMN_STRING:
        strcpy(buf, _string);
        _jsonData.typeNum = JSON_STRING;
        break;
    case MB_Json::MB_JSON_JSMN_PRIMITIVE:
        if (!typeSet && strcmp(tmp2, _tr) == 0)
        {
            typeSet = true;
            strcpy(buf, _bl);
            _jsonData.typeNum = JSON_BOOL;
            _jsonData.boolValue = true;
            _jsonData.floatValue = 1.0f;
            _jsonData.doubleValue = 1.0;
            _jsonData.intValue = 1;
        }
        else
        {
            if (!typeSet && strcmp(tmp2, _fls) == 0)
            {
                typeSet = true;
                strcpy(buf, _bl);
                _jsonData.typeNum = JSON_BOOL;
                _jsonData.boolValue = false;
                _jsonData.floatValue = 0.0f;
                _jsonData.doubleValue = 0.0;
                _jsonData.intValue = 0;
            }
        }

        if (!typeSet && strcmp(tmp2, _nll) == 0)
        {
            typeSet = true;
            strcpy(buf, _nll);
            _jsonData.typeNum = JSON_NULL;
        }
        if (!typeSet)
        {
            typeSet = true;
            strcpy(tmp, _dot);
            double d = atof(tmp2);
            if (d > 0x7fffffff)
            {
                strcpy(buf, _dbl);
                _jsonData.floatValue = (float)d;
                _jsonData.doubleValue = d;
                _jsonData.intValue = atoi(tmp2);
                _jsonData.boolValue = atof(tmp2) > 0 ? true : false;
                _jsonData.typeNum = JSON_DOUBLE;
            }
            else
            {
                if (_strpos(tmp2, tmp, 0) > -1)
                {
                    strcpy(buf, _dbl);
                    _jsonData.floatValue = (float)d;
                    _jsonData.doubleValue = d;
                    _jsonData.intValue = atoi(tmp2);
                    _jsonData.boolValue = atof(tmp2) > 0 ? true : false;
                    _jsonData.typeNum = JSON_FLOAT;
                }
                else
                {
                    _jsonData.intValue = atoi(tmp2);
                    _jsonData.floatValue = atof(tmp2);
                    _jsonData.doubleValue = atof(tmp2);
                    _jsonData.boolValue = atof(tmp2) > 0 ? true : false;
                    strcpy(buf, _int);
                    _jsonData.typeNum = JSON_INT;
                }
            }
        }
        break;
    default:
        break;
    }
    _jsonData.type = buf;
    _delS(buf);
    _delS(tmp);
    if (tmp2)
        _delS(tmp2);
}

int MB_Json::_strpos(const char *haystack, const char *needle, int offset)
{
    size_t len = strlen(haystack);
    size_t len2 = strlen(needle);
    if (len == 0 || len < len2 || len2 == 0)
        return -1;
    char *_haystack = _newS(len - offset + 1);
    _haystack[len - offset] = 0;
    strncpy(_haystack, haystack + offset, len - offset);
    char *p = strstr(_haystack, needle);
    int r = -1;
    if (p)
        r = p - _haystack + offset;
    _delS(_haystack);
    return r;
}

int MB_Json::_rstrpos(const char *haystack, const char *needle, int offset)
{
    size_t len = strlen(haystack);
    size_t len2 = strlen(needle);
    if (len == 0 || len < len2 || len2 == 0)
        return -1;
    char *_haystack = _newS(len - offset + 1);
    _haystack[len - offset] = 0;
    strncpy(_haystack, haystack + offset, len - offset);
    char *p = _rstrstr(_haystack, needle);
    int r = -1;
    if (p)
        r = p - _haystack + offset;
    _delS(_haystack);
    return r;
}

char *MB_Json::_rstrstr(const char *haystack, const char *needle)
{
    size_t needle_length = strlen(needle);
    const char *haystack_end = haystack + strlen(haystack) - needle_length;
    const char *p;
    size_t i;
    for (p = haystack_end; p >= haystack; --p)
    {
        for (i = 0; i < needle_length; ++i)
        {
            if (p[i] != needle[i])
                goto next;
        }
        return (char *)p;
    next:;
    }
    return 0;
}

void MB_Json::_delS(char *p)
{
    if (p != nullptr)
        delete[] p;
}

char *MB_Json::_newS(size_t len)
{
    char *p = new char[len];
    memset(p, 0, len);
    return p;
}

char *MB_Json::_newS(char *p, size_t len)
{
    _delS(p);
    p = _newS(len);
    return p;
}

char *MB_Json::_newS(char *p, size_t len, char *d)
{
    _delS(p);
    p = _newS(len);
    strcpy(p, d);
    return p;
}

char *MB_Json::_strP(PGM_P pgm)
{
    size_t len = strlen_P(pgm) + 1;
    char *buf = _newS(len);
    strcpy_P(buf, pgm);
    buf[len - 1] = 0;
    return buf;
}

/**
 * Allocates a fresh unused token from the token pool.
 */
MB_Json::mbjs_tok_t *MB_Json::mbjs_alloc_token(mbjs_parser *parser,
                                                         MB_Json::mbjs_tok_t *tokens, size_t num_tokens)
{
    MB_Json::mbjs_tok_t *tok;
    if (parser->toknext >= num_tokens)
    {
        return NULL;
    }
    tok = &tokens[parser->toknext++];
    tok->start = tok->end = -1;
    tok->size = 0;
#ifdef MB_JSON_JSMN_PARENT_LINKS
    tok->parent = -1;
#endif
    return tok;
}

/**
 * Fills token type and boundaries.
 */
void MB_Json::mbjs_fill_token(mbjs_tok_t *token, mbjs_type_t type,
                                   int start, int end)
{
    token->type = type;
    token->start = start;
    token->end = end;
    token->size = 0;
}

/**
 * Fills next available token with JSON primitive.
 */
int MB_Json::mbjs_parse_primitive(mbjs_parser *parser, const char *js,
                                       size_t len, mbjs_tok_t *tokens, size_t num_tokens)
{
    mbjs_tok_t *token;
    int start;

    start = parser->pos;

    for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++)
    {
        switch (js[parser->pos])
        {
#ifndef MB_JSON_JSMN_STRICT
        /* In strict mode primitive must be followed by "," or "}" or "]" */
        case ':':
#endif
        case '\t':
        case '\r':
        case '\n':
        case ' ':
        case ',':
        case ']':
        case '}':
            goto found;
        }
        if (js[parser->pos] < 32 || js[parser->pos] >= 127)
        {
            parser->pos = start;
            return MB_JSON_JSMN_ERROR_INVAL;
        }
    }
#ifdef MB_JSON_JSMN_STRICT
    /* In strict mode primitive must be followed by a comma/object/array */
    parser->pos = start;
    return MB_JSON_JSMN_ERROR_PART;
#endif

found:
    if (tokens == NULL)
    {
        parser->pos--;
        return 0;
    }
    token = mbjs_alloc_token(parser, tokens, num_tokens);
    if (token == NULL)
    {
        parser->pos = start;
        return MB_JSON_JSMN_ERROR_NOMEM;
    }
    mbjs_fill_token(token, MB_JSON_JSMN_PRIMITIVE, start, parser->pos);
#ifdef MB_JSON_JSMN_PARENT_LINKS
    token->parent = parser->toksuper;
#endif
    parser->pos--;
    return 0;
}

/**
 * Fills next token with JSON string.
 */
int MB_Json::mbjs_parse_string(mbjs_parser *parser, const char *js,
                                    size_t len, mbjs_tok_t *tokens, size_t num_tokens)
{
    mbjs_tok_t *token;

    int start = parser->pos;

    parser->pos++;

    /* Skip starting quote */
    for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++)
    {
        char c = js[parser->pos];

        /* Quote: end of string */
        if (c == '\"')
        {
            if (tokens == NULL)
            {
                return 0;
            }
            token = mbjs_alloc_token(parser, tokens, num_tokens);
            if (token == NULL)
            {
                parser->pos = start;
                return MB_JSON_JSMN_ERROR_NOMEM;
            }
            mbjs_fill_token(token, MB_JSON_JSMN_STRING, start + 1, parser->pos);
#ifdef MB_JSON_JSMN_PARENT_LINKS
            token->parent = parser->toksuper;
#endif
            return 0;
        }

        /* Backslash: Quoted symbol expected */
        if (c == '\\' && parser->pos + 1 < len)
        {
            int i;
            parser->pos++;
            switch (js[parser->pos])
            {
            /* Allowed escaped symbols */
            case '\"':
            case '/':
            case '\\':
            case 'b':
            case 'f':
            case 'r':
            case 'n':
            case 't':
                break;
            /* Allows escaped symbol \uXXXX */
            case 'u':
                parser->pos++;
                for (i = 0; i < 4 && parser->pos < len && js[parser->pos] != '\0'; i++)
                {
                    /* If it isn't a hex character we have an error */
                    if (!((js[parser->pos] >= 48 && js[parser->pos] <= 57) || /* 0-9 */
                          (js[parser->pos] >= 65 && js[parser->pos] <= 70) || /* A-F */
                          (js[parser->pos] >= 97 && js[parser->pos] <= 102)))
                    { /* a-f */
                        parser->pos = start;
                        return MB_JSON_JSMN_ERROR_INVAL;
                    }
                    parser->pos++;
                }
                parser->pos--;
                break;
            /* Unexpected symbol */
            default:
                parser->pos = start;
                return MB_JSON_JSMN_ERROR_INVAL;
            }
        }
    }
    parser->pos = start;
    return MB_JSON_JSMN_ERROR_PART;
}

/**
 * Parse JSON string and fill tokens.
 */
int MB_Json::mbjs_parse(mbjs_parser *parser, const char *js, size_t len,
                             mbjs_tok_t *tokens, unsigned int num_tokens)
{
    int r;
    int i;
    mbjs_tok_t *token;
    int count = parser->toknext;

    for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++)
    {
        char c;
        mbjs_type_t type;

        c = js[parser->pos];
        switch (c)
        {
        case '{':
        case '[':
            count++;
            if (tokens == NULL)
            {
                break;
            }
            token = mbjs_alloc_token(parser, tokens, num_tokens);
            if (token == NULL)
                return MB_JSON_JSMN_ERROR_NOMEM;
            if (parser->toksuper != -1)
            {
                tokens[parser->toksuper].size++;
#ifdef MB_JSON_JSMN_PARENT_LINKS
                token->parent = parser->toksuper;
#endif
            }
            token->type = (c == '{' ? MB_JSON_JSMN_OBJECT : MB_JSON_JSMN_ARRAY);
            token->start = parser->pos;
            parser->toksuper = parser->toknext - 1;
            if (parser->pos > 0)
                if (js[parser->pos - 1] == '{' && js[parser->pos] == '[')
                    return MB_JSON_JSMN_ERROR_INVAL;
            break;
        case '}':
        case ']':
            if (tokens == NULL)
                break;
            type = (c == '}' ? MB_JSON_JSMN_OBJECT : MB_JSON_JSMN_ARRAY);
#ifdef MB_JSON_JSMN_PARENT_LINKS
            if (parser->toknext < 1)
            {
                return MB_JSON_JSMN_ERROR_INVAL;
            }
            token = &tokens[parser->toknext - 1];
            for (;;)
            {
                if (token->start != -1 && token->end == -1)
                {
                    if (token->type != type)
                    {
                        return MB_JSON_JSMN_ERROR_INVAL;
                    }
                    token->end = parser->pos + 1;
                    parser->toksuper = token->parent;
                    break;
                }
                if (token->parent == -1)
                {
                    if (token->type != type || parser->toksuper == -1)
                    {
                        return MB_JSON_JSMN_ERROR_INVAL;
                    }
                    break;
                }
                token = &tokens[token->parent];
            }
#else
            for (i = parser->toknext - 1; i >= 0; i--)
            {
                token = &tokens[i];
                if (token->start != -1 && token->end == -1)
                {
                    if (token->type != type)
                    {
                        return MB_JSON_JSMN_ERROR_INVAL;
                    }
                    parser->toksuper = -1;
                    token->end = parser->pos + 1;
                    break;
                }
            }
            /* Error if unmatched closing bracket */
            if (i == -1)
                return MB_JSON_JSMN_ERROR_INVAL;
            for (; i >= 0; i--)
            {
                token = &tokens[i];
                if (token->start != -1 && token->end == -1)
                {
                    parser->toksuper = i;
                    break;
                }
            }
#endif
            break;
        case '\"':
            r = mbjs_parse_string(parser, js, len, tokens, num_tokens);
            if (r < 0)
                return r;
            count++;
            if (parser->toksuper != -1 && tokens != NULL)
                tokens[parser->toksuper].size++;
            break;
        case '\t':
        case '\r':
        case '\n':
        case ' ':
            break;
        case ':':
            parser->toksuper = parser->toknext - 1;
            break;
        case ',':
            if (tokens != NULL && parser->toksuper != -1 &&
                tokens[parser->toksuper].type != MB_JSON_JSMN_ARRAY &&
                tokens[parser->toksuper].type != MB_JSON_JSMN_OBJECT)
            {
#ifdef MB_JSON_JSMN_PARENT_LINKS
                parser->toksuper = tokens[parser->toksuper].parent;
#else
                for (i = parser->toknext - 1; i >= 0; i--)
                {
                    if (tokens[i].type == MB_JSON_JSMN_ARRAY || tokens[i].type == MB_JSON_JSMN_OBJECT)
                    {
                        if (tokens[i].start != -1 && tokens[i].end == -1)
                        {
                            parser->toksuper = i;
                            break;
                        }
                    }
                }
#endif
            }
            break;
#ifdef MB_JSON_JSMN_STRICT
        /* In strict mode primitives are: numbers and booleans */
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 't':
        case 'f':
        case 'n':

            /* And they must not be keys of the object */
            if (tokens != NULL && parser->toksuper != -1)
            {
                mbjs_tok_t *t = &tokens[parser->toksuper];
                if (t->type == MB_JSON_JSMN_OBJECT ||
                    (t->type == MB_JSON_JSMN_STRING && t->size != 0))
                {
                    return MB_JSON_JSMN_ERROR_INVAL;
                }
            }
#else
        /* In non-strict mode every unquoted value is a primitive */
        default:
#endif

            r = mbjs_parse_primitive(parser, js, len, tokens, num_tokens);
            if (r < 0)
                return r;
            count++;
            if (parser->toksuper != -1 && tokens != NULL)
                tokens[parser->toksuper].size++;
            break;

#ifdef MB_JSON_JSMN_STRICT
        /* Unexpected char in strict mode */
        default:
            return MB_JSON_JSMN_ERROR_INVAL;
#endif
        }
    }

    if (tokens != NULL)
    {
        for (i = parser->toknext - 1; i >= 0; i--)
        {
            /* Unmatched opened object or array */
            if (tokens[i].start != -1 && tokens[i].end == -1)
            {
                return MB_JSON_JSMN_ERROR_PART;
            }
        }
    }

    return count;
}

/**
 * Creates a new parser based over a given  buffer with an array of tokens
 * available.
 */
void MB_Json::mbjs_init(mbjs_parser *parser)
{
    parser->pos = 0;
    parser->toknext = 0;
    parser->toksuper = -1;
}

MB_JsonArray::MB_JsonArray()
{
    _init();
}
MB_JsonArray::~MB_JsonArray()
{
    _finalize();
    std::string().swap(_jbuf);
};

void MB_JsonArray::_init()
{
    _finalize();

    _pd = _strP(mb_json_str_4);
    _pf = _strP(mb_json_str_5);
    _fls = _strP(mb_json_str_6);
    _tr = _strP(mb_json_str_7);
    _brk3 = _strP(mb_json_str_10);
    _brk4 = _strP(mb_json_str_11);
    _nll = _strP(mb_json_str_18);
    _root = _strP(mb_json_str_21);
    _root2 = _strP(mb_json_str_26);
    _qt = _strP(mb_json_str_2);
    _slash = _strP(mb_json_str_27);
}

int MB_JsonArray::_strpos(const char *haystack, const char *needle, int offset)
{
    size_t len = strlen(haystack);
    size_t len2 = strlen(needle);
    if (len == 0 || len < len2 || len2 == 0)
        return -1;
    char *_haystack = _newS(len - offset + 1);
    _haystack[len - offset] = 0;
    strncpy(_haystack, haystack + offset, len - offset);
    char *p = strstr(_haystack, needle);
    int r = -1;
    if (p)
        r = p - _haystack + offset;
    _delS(_haystack);
    return r;
}

int MB_JsonArray::_rstrpos(const char *haystack, const char *needle, int offset)
{
    size_t len = strlen(haystack);
    size_t len2 = strlen(needle);
    if (len == 0 || len < len2 || len2 == 0)
        return -1;
    char *_haystack = _newS(len - offset + 1);
    _haystack[len - offset] = 0;
    strncpy(_haystack, haystack + offset, len - offset);
    char *p = _rstrstr(_haystack, needle);
    int r = -1;
    if (p)
        r = p - _haystack + offset;
    _delS(_haystack);
    return r;
}

char *MB_JsonArray::_rstrstr(const char *haystack, const char *needle)
{
    size_t needle_length = strlen(needle);
    const char *haystack_end = haystack + strlen(haystack) - needle_length;
    const char *p;
    size_t i;
    for (p = haystack_end; p >= haystack; --p)
    {
        for (i = 0; i < needle_length; ++i)
        {
            if (p[i] != needle[i])
                goto next;
        }
        return (char *)p;
    next:;
    }
    return 0;
}

void MB_JsonArray::_delS(char *p)
{
    if (p != nullptr)
        delete[] p;
}

char *MB_JsonArray::_newS(size_t len)
{
    char *p = new char[len];
    memset(p, 0, len);
    return p;
}

char *MB_JsonArray::_newS(char *p, size_t len)
{
    _delS(p);
    p = _newS(len);
    return p;
}

char *MB_JsonArray::_newS(char *p, size_t len, char *d)
{
    _delS(p);
    p = _newS(len);
    strcpy(p, d);
    return p;
}

char *MB_JsonArray::_strP(PGM_P pgm)
{
    size_t len = strlen_P(pgm) + 1;
    char *buf = _newS(len);
    strcpy_P(buf, pgm);
    buf[len - 1] = 0;
    return buf;
}

void MB_JsonArray::_finalize()
{
    _delS(_pd);
    _delS(_pf);
    _delS(_fls);
    _delS(_tr);
    _delS(_brk3);
    _delS(_brk4);
    _delS(_nll);
    _delS(_root);
    _delS(_root2);
    _delS(_qt);
    _delS(_slash);
}

MB_JsonArray &MB_JsonArray::add()
{
    _addNull();
    return *this;
}

MB_JsonArray &MB_JsonArray::add(const String &value)
{
    _addString(value.c_str());
    return *this;
}
MB_JsonArray &MB_JsonArray::add(const char *value)
{
    _addString(value);
    return *this;
}
MB_JsonArray &MB_JsonArray::add(int value)
{
    _addInt(value);
    return *this;
}

MB_JsonArray &MB_JsonArray::add(unsigned short value)
{
    _addInt(value);
    return *this;
}

MB_JsonArray &MB_JsonArray::add(float value)
{
    _addFloat(value);
    return *this;
}

MB_JsonArray &MB_JsonArray::add(double value)
{
    _addDouble(value);
    return *this;
}

MB_JsonArray &MB_JsonArray::add(bool value)
{
    _addBool(value);
    return *this;
}

MB_JsonArray &MB_JsonArray::add(MB_Json &json)
{
    _addJson(&json);
    return *this;
}

MB_JsonArray &MB_JsonArray::add(MB_JsonArray &arr)
{
    _addArray(&arr);
    return *this;
}

template <typename T>
MB_JsonArray &MB_JsonArray::add(T value)
{
    if (std::is_same<T, int>::value)
        _addInt(value);
    else if (std::is_same<T, float>::value)
        _addFloat(value);
    else if (std::is_same<T, double>::value)
        _addDouble(value);
    else if (std::is_same<T, bool>::value)
        _addBool(value);
    else if (std::is_same<T, const char *>::value)
        _addString(value);
    else if (std::is_same<T, MB_Json &>::value)
        _addJson(&value);
    else if (std::is_same<T, MB_JsonArray &>::value)
        _addArray(&value);
    return *this;
}

void MB_JsonArray::_addString(const std::string &value)
{
    _arrLen++;
    _json._addArrayStr(value.c_str(), value.length(), true);
}

void MB_JsonArray::_addInt(int value)
{
    _arrLen++;
    char *buf = intStr(value);
    sprintf(buf, _pd, value);
    _json._addArrayStr(buf, 60, false);
    _delS(buf);
}

void MB_JsonArray::_addFloat(float value)
{
    _arrLen++;
    char *buf = floatStr(value);
    _trimDouble(buf);
    _json._addArrayStr(buf, 60, false);
    _delS(buf);
}

void MB_JsonArray::_addDouble(double value)
{
    _arrLen++;
    char *buf = doubleStr(value);
    _trimDouble(buf);
    _json._addArrayStr(buf, 60, false);
    _delS(buf);
}

void MB_JsonArray::_addBool(bool value)
{
    _arrLen++;
    if (value)
        _json._addArrayStr(_tr, 6, false);
    else
        _json._addArrayStr(_fls, 7, false);
}

void MB_JsonArray::_addNull()
{
    _arrLen++;
    _json._addArrayStr(_nll, 6, false);
}

void MB_JsonArray::_addJson(MB_Json *json)
{
    _arrLen++;
    std::string s;
    json->_toStdString(s);
    _json._addArrayStr(s.c_str(), s.length(), false);
    std::string().swap(s);
}

void MB_JsonArray::_addArray(MB_JsonArray *arr)
{
    _arrLen++;
    String arrStr;
    arr->toString(arrStr);
    _json._addArrayStr(arrStr.c_str(), arrStr.length(), false);
}

MB_JsonArray &MB_JsonArray::setJsonArrayData(const String &data)
{
    int start_pos = data.indexOf('[');
    int end_pos = data.indexOf(']');

    if (start_pos != -1 && end_pos != -1 && start_pos != end_pos)
    {
        char *r = _strP(mb_json_str_21);
        _json._rawbuf = r;
        _json._rawbuf += data.c_str();
        _delS(r);
        r = _strP(mb_json_str_26);
        MB_JsonData data;
        _json.get(data, r);
        _delS(r);
        data.getArray(*this);
        data.stringValue= "";
    }
    return *this;
}

bool MB_JsonArray::get(MB_JsonData &jsonData, const String &path)
{
    return _get(jsonData, path.c_str());
}

bool MB_JsonArray::get(MB_JsonData &jsonData, int index)
{
    char *tmp = intStr(index);
    std::string path = "";
    path += _brk3;
    path += tmp;
    path += _brk4;
    bool ret = _get(jsonData, path.c_str());
    std::string().swap(path);
    _delS(tmp);
    return ret;
}

bool MB_JsonArray::_get(MB_JsonData &jsonData, const char *path)
{
    _json._toStdString(_jbuf, false);
    _json._rawbuf = _root;
    _json._rawbuf += _jbuf;
    std::string path2 = _root2;
    path2 += _slash;
    path2 += path;
    _json.clearPathTk();
    _json._strToTk(path2.c_str(), _json._pathTk, '/');
    if (!_json._isArrTk(1))
    {
        _json._jsonData.success = false;
        goto ex_;
    }
    if (_json._getArrIndex(1) < 0)
    {
        _json._jsonData.success = false;
        goto ex_;
    }
    _json._parse(path2.c_str(), MB_Json::MB_JSON_PRINT_MODE_NONE);
    if (_json._jsonData.success)
    {
        _json._rawbuf = _jbuf.substr(1, _jbuf.length() - 2).c_str();
        if (_json._jsonData._type == MB_Json::MB_JSON_JSMN_STRING && _json._jsonData.stringValue.c_str()[0] == '"' && _json._jsonData.stringValue.c_str()[_json._jsonData.stringValue.length() - 1] == '"')
            _json._jsonData.stringValue = _json._jsonData.stringValue.substring(1, _json._jsonData.stringValue.length() - 1).c_str();
        jsonData = _json._jsonData;
    }
ex_:
    _json.clearPathTk();
    _json._tokens.reset();
    _json._tokens = nullptr;
    return _json._jsonData.success;
}

size_t MB_JsonArray::size()
{
    return _arrLen;
}

char *MB_JsonArray::floatStr(float value)
{
    char *buf = _newS(36);
    dtostrf(value, 7, 6, buf);
    return buf;
}

char *MB_JsonArray::intStr(int value)
{
    char *buf = _newS(36);
    sprintf(buf, "%d", value);
    return buf;
}

char *MB_JsonArray::boolStr(bool value)
{
    char *buf = nullptr;
    if (value)
        buf = _strP(mb_json_str_7);
    else
        buf = _strP(mb_json_str_6);
    return buf;
}

char *MB_JsonArray::doubleStr(double value)
{
    char *buf = _newS(36);
    dtostrf(value, 12, 9, buf);
    return buf;
}

void MB_JsonArray::_trimDouble(char *buf)
{
    size_t i = strlen(buf) - 1;
    while (buf[i] == '0' && i > 0)
    {
        if (buf[i - 1] == '.')
        {
            i--;
            break;
        }
        if (buf[i - 1] != '0')
            break;
        i--;
    }
    if (i < strlen(buf) - 1)
        buf[i] = '\0';
}

void MB_JsonArray::toString(String &buf, bool prettify)
{
    char *tmp = _newS(1024);
    std::string().swap(_json._jsonData._dbuf);
    std::string().swap(_json._tbuf);
    _json._toStdString(_jbuf, false);
    _json._rawbuf = _root;
    _json._rawbuf += _jbuf;
    if (prettify)
        _json._parse(_root2, MB_Json::MB_JSON_PRINT_MODE_PRETTY);
    else
        _json._parse(_root2, MB_Json::MB_JSON_PRINT_MODE_PLAIN);
    std::string().swap(_json._tbuf);
    std::string().swap(_jbuf);
    _json.clearPathTk();
    _json._tokens.reset();
    _json._tokens = nullptr;
    _delS(tmp);
    _json._rawbuf = _json._jsonData._dbuf.substr(1, _json._jsonData._dbuf.length() - 2);
    buf = _json._jsonData._dbuf.c_str();
    std::string().swap(_json._jsonData._dbuf);
}

MB_JsonArray &MB_JsonArray::clear()
{
    _json.clear();
    std::string().swap(_jbuf);
    _json._jsonData.success = false;
    _json._jsonData.stringValue = "";
    _json._jsonData.boolValue = false;
    _json._jsonData.doubleValue = 0;
    _json._jsonData.intValue = 0;
    _json._jsonData.floatValue = 0;
    _json._jsonData._len = 0;
    _arrLen = 0;
    return *this;
}
void MB_JsonArray::_set2(int index, const char *value, bool isStr)
{
    char *tmp = _newS(50);
    std::string path = _brk3;
    sprintf(tmp, "%d", index);
    path += tmp;
    path += _brk4;
    _set(path.c_str(), value, isStr);
    std::string().swap(path);
    _delS(tmp);
}

void MB_JsonArray::_set(const char *path, const char *value, bool isStr)
{
    _json._jsonData.success = false;
    _json._toStdString(_jbuf, false);
    _json._rawbuf = _root;
    _json._rawbuf += _jbuf;
    char *tmp2 = _newS(strlen(value) + 10);
    if (isStr)
        strcpy_P(tmp2, _qt);
    strcat(tmp2, value);
    if (isStr)
        strcat_P(tmp2, _qt);
    std::string path2 = _root2;
    path2 += _slash;
    path2 += path;
    _json.clearPathTk();
    _json._strToTk(path2, _json._pathTk, '/');
    if (!_json._isArrTk(1))
        goto ex_2;
    if (_json._getArrIndex(1) < 0)
        goto ex_2;
    _json._set(path2.c_str(), tmp2);
    _delS(tmp2);
    std::string().swap(path2);
    if (_json._jsonData.success)
    {
        std::string().swap(_json._jsonData._dbuf);
        std::string().swap(_json._tbuf);
        _json._parse(_root2, MB_Json::MB_JSON_PRINT_MODE_PLAIN);
        if (_json._jsonData.success)
        {
            _arrLen = _json._jsonData._len;
            _json._rawbuf = _json._jsonData._dbuf.substr(1, _json._jsonData._dbuf.length() - 2);
        }
    }
    else
        _json._rawbuf = _jbuf.substr(1, _jbuf.length() - 2);
ex_2:
    std::string().swap(_json._jsonData._dbuf);
    std::string().swap(_json._tbuf);
    std::string().swap(_jbuf);
    _json.clearPathTk();
    _json._tokens.reset();
    _json._tokens = nullptr;
}

void MB_JsonArray::set(int index)
{
    return _setNull(index);
}

void MB_JsonArray::set(const String &path)
{
    _setNull(path);
}

void MB_JsonArray::set(int index, const String &value)
{
    _setString(index, value.c_str());
}

void MB_JsonArray::set(const String &path, const String &value)
{
    _setString(path, value.c_str());
}

void MB_JsonArray::set(int index, const char *value)
{
    _setString(index, value);
}

void MB_JsonArray::set(const String &path, const char *value)
{
    _setString(path, value);
}

void MB_JsonArray::set(int index, int value)
{
    _setInt(index, value);
}

void MB_JsonArray::set(int index, unsigned short value)
{
    _setInt(index, value);
}

void MB_JsonArray::set(const String &path, int value)
{
    _setInt(path, value);
}

void MB_JsonArray::set(const String &path, unsigned short value)
{
    _setInt(path, value);
}

void MB_JsonArray::set(int index, float value)
{
    _setFloat(index, value);
}

void MB_JsonArray::set(const String &path, float value)
{
    _setFloat(path, value);
}

void MB_JsonArray::set(int index, double value)
{
    _setDouble(index, value);
}

void MB_JsonArray::set(const String &path, double value)
{
    _setDouble(path, value);
}

void MB_JsonArray::set(int index, bool value)
{
    _setBool(index, value);
}

void MB_JsonArray::set(const String &path, bool value)
{
    _setBool(path, value);
}

void MB_JsonArray::set(int index, MB_Json &json)
{
    _setJson(index, &json);
}

void MB_JsonArray::set(const String &path, MB_Json &json)
{
    _setJson(path, &json);
}

void MB_JsonArray::set(int index, MB_JsonArray &arr)
{
    _setArray(index, &arr);
}

void MB_JsonArray::set(const String &path, MB_JsonArray &arr)
{
    _setArray(path, &arr);
}

template <typename T>
void MB_JsonArray::set(int index, T value)
{
    if (std::is_same<T, int>::value)
        _setInt(index, value);
    else if (std::is_same<T, float>::value)
        _setFloat(index, value);
    else if (std::is_same<T, double>::value)
        _setDouble(index, value);
    else if (std::is_same<T, bool>::value)
        _setBool(index, value);
    else if (std::is_same<T, const char *>::value)
        _setString(index, value);
    else if (std::is_same<T, MB_Json &>::value)
        _setJson(index, &value);
    else if (std::is_same<T, MB_JsonArray &>::value)
        _setArray(index, &value);
}

template <typename T>
void MB_JsonArray::set(const String &path, T value)
{
    if (std::is_same<T, int>::value)
        _setInt(path, value);
    else if (std::is_same<T, float>::value)
        _setFloat(path, value);
    else if (std::is_same<T, double>::value)
        _setDouble(path, value);
    else if (std::is_same<T, bool>::value)
        _setBool(path, value);
    else if (std::is_same<T, const char *>::value)
        _setString(path, value);
    else if (std::is_same<T, MB_Json &>::value)
        _setJson(path, &value);
    else if (std::is_same<T, MB_JsonArray &>::value)
        _setArray(path, &value);
}

void MB_JsonArray::_setString(int index, const std::string &value)
{
    _set2(index, value.c_str(), true);
}

void MB_JsonArray::_setString(const String &path, const std::string &value)
{
    _set(path.c_str(), value.c_str(), true);
}

void MB_JsonArray::_setInt(int index, int value)
{
    char *tmp = intStr(value);
    _set2(index, tmp, false);
    _delS(tmp);
}

void MB_JsonArray::_setInt(const String &path, int value)
{
    char *tmp = intStr(value);
    _set(path.c_str(), tmp, false);
    _delS(tmp);
}

void MB_JsonArray::_setFloat(int index, float value)
{
    char *tmp = floatStr(value);
    _trimDouble(tmp);
    _set2(index, tmp, false);
    _delS(tmp);
}

void MB_JsonArray::_setFloat(const String &path, float value)
{
    char *tmp = floatStr(value);
    _trimDouble(tmp);
    _set(path.c_str(), tmp, false);
    _delS(tmp);
}

void MB_JsonArray::_setDouble(int index, double value)
{
    char *tmp = doubleStr(value);
    _trimDouble(tmp);
    _set2(index, tmp, false);
    _delS(tmp);
}

void MB_JsonArray::_setDouble(const String &path, double value)
{
    char *tmp = doubleStr(value);
    _trimDouble(tmp);
    _set(path.c_str(), tmp, false);
    _delS(tmp);
}

void MB_JsonArray::_setBool(int index, bool value)
{
    if (value)
        _set2(index, _tr, false);
    else
        _set2(index, _fls, false);
}

void MB_JsonArray::_setBool(const String &path, bool value)
{
    if (value)
        _set(path.c_str(), _tr, false);
    else
        _set(path.c_str(), _fls, false);
}

void MB_JsonArray::_setNull(int index)
{
    _set2(index, _nll, false);
}

void MB_JsonArray::_setNull(const String &path)
{
    _set(path.c_str(), _nll, false);
}

void MB_JsonArray::_setJson(int index, MB_Json *json)
{
    std::string s;
    json->_toStdString(s);
    _set2(index, s.c_str(), false);
    std::string().swap(s);
}

void MB_JsonArray::_setJson(const String &path, MB_Json *json)
{
    std::string s;
    json->_toStdString(s);
    _set(path.c_str(), s.c_str(), false);
    std::string().swap(s);
}

void MB_JsonArray::_setArray(int index, MB_JsonArray *arr)
{
    std::string s;
    arr->_toStdString(s);
    _set2(index, s.c_str(), false);
    std::string().swap(s);
}

void MB_JsonArray::_setArray(const String &path, MB_JsonArray *arr)
{
    std::string s;
    arr->_toStdString(s);
    _set(path.c_str(), s.c_str(), false);
    std::string().swap(s);
}

bool MB_JsonArray::remove(int index)
{
    char *tmp = intStr(index);
    std::string path = "";
    path += _brk3;
    path += tmp;
    path += _brk4;
    bool ret = _remove(path.c_str());
    std::string().swap(path);
    _delS(tmp);
    return ret;
}

bool MB_JsonArray::remove(const String &path)
{
    return _remove(path.c_str());
}

bool MB_JsonArray::_remove(const char *path)
{
    _json._toStdString(_jbuf, false);
    _json._rawbuf = _root;
    _json._rawbuf += _jbuf;
    char *tmp2 = _newS(2);
    std::string path2 = _root2;
    path2 += _slash;
    path2 += path;
    _json._jsonData.success = _json.remove(path2.c_str());
    _delS(tmp2);
    std::string().swap(path2);
    bool success = _json._jsonData.success;
    if (_json._jsonData.success)
    {
        std::string().swap(_json._jsonData._dbuf);
        std::string().swap(_json._tbuf);
        _json._parse(_root2, MB_Json::MB_JSON_PRINT_MODE_PLAIN);
        if (_json._jsonData.success)
        {
            _arrLen = _json._jsonData._len;
            _json._rawbuf = _json._jsonData._dbuf.substr(1, _json._jsonData._dbuf.length() - 2);
        }
    }
    else
        _json._rawbuf = _jbuf.substr(1, _jbuf.length() - 2);

    if (_json._rawbuf.length() == 0)
    {
        _json._jsonData.success = success;
        _arrLen = 0;
    }

    return _json._jsonData.success;
}

void MB_JsonArray::_toStdString(std::string &s)
{
    _json._toStdString(s, false);
}

MB_JsonData::MB_JsonData()
{
}

MB_JsonData::~MB_JsonData()
{
    std::string().swap(_dbuf);
}

bool MB_JsonData::getArray(MB_JsonArray &jsonArray)
{
    if (typeNum != MB_Json::JSON_ARRAY || !success)
        return false;
    char *tmp = new char[1024];
    memset(tmp, 0, 1024);
    char *nbuf = new char[2];
    memset(nbuf, 0, 2);
    strcpy_P(tmp, mb_json_str_21);
    jsonArray._json._toStdString(jsonArray._jbuf, false);
    jsonArray._json._rawbuf = tmp;
    jsonArray._json._rawbuf += stringValue.c_str();
    memset(tmp, 0, 1024);
    strcpy_P(tmp, mb_json_str_26);
    std::string().swap(jsonArray._json._jsonData._dbuf);
    std::string().swap(jsonArray._json._tbuf);
    jsonArray._json._parse(tmp, MB_Json::MB_JSON_PRINT_MODE_PLAIN);
    jsonArray._json._rawbuf = jsonArray._json._jsonData._dbuf.substr(1, jsonArray._json._jsonData._dbuf.length() - 2).c_str();
    jsonArray._arrLen = jsonArray._json._jsonData._len;
    delete[] tmp;
    delete[] nbuf;
    return jsonArray._json._jsonData.success;
}

bool MB_JsonData::getJSON(MB_Json &json)
{
    if (typeNum != MB_Json::JSON_OBJECT || !success)
        return false;
    json.setJsonData(stringValue);
    json._mbjs_parse();
    return json._jsonData.success;
}

#endif