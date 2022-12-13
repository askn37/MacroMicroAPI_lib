/**
 * @file GPS_RMS.cpp
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-11
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "../GPS_RMS.h"
#include <api/btools.h>
#include <string.h>

int GPS_RMS_Class::indexof (const char* _src, const char _val, size_t _limit) {
    size_t _index = 0;
    while (_index < _limit) {
        if (_val == _src[_index]) return _index;
        _index++;
    }
    return -1;
}

bool GPS_RMS_Class::update (void* _buffer, size_t _limit) {
    int _index;
    uint8_t* _p = (uint8_t*)_buffer;
    /* '$' を探す */
    while ((_index = indexof((const char*)_p, '$', _limit)) >= 0) {
        /* その位置からヘッダが一致するか調べる */
        if (strncmp((const char*)_p + 3, "RMC,", 4) == 0
            /* 改行を検出 */
            && (_index = indexof((const char*)_p, '\n', _limit) ) >= 0
            /* 見つけたらチェックサムをテスト */
            && testCheckSum(_p, _index - 1)) {
            /* パーサーへ渡す */
            return parseNMEA(_p, _index);
        }
        _limit--;
        _p++;
    }
    return false;
}

gpsdata_t GPS_RMS_Class::getData (void) { return gpsdata; }

uint8_t GPS_RMS_Class::calcCheckSum (void* _buffer, size_t _limit) {
    return bcc8((uint8_t*)_buffer, _limit) ^ ('$' ^ '*');
}

bool GPS_RMS_Class::testCheckSum (void* _buffer, size_t _limit) {
    return calcCheckSum(_buffer, _limit - 2) == stob((char*)((uint16_t)_buffer + _limit - 2));
}

bool GPS_RMS_Class::parseNMEA (void* _buffer, size_t _limit) {
    uint8_t* _ptr = (uint8_t*)_buffer;
    uint8_t* _end = (uint8_t*)_buffer + _limit;
    uint8_t _column = 0;
    gpsdata.update.stamp = false;
    gpsdata.update.coordinate = false;
    while (_ptr < _end) {
        if (',' != *_ptr++) continue;
        _column++;
        if (',' == *_ptr) continue;
        if (_column == 1) { /* UTCTime */
            /* ミリ秒単位部が'.000'でない場合は衛星と同期できていないと見做す */
            if (_BCDT(_ptr[6])->dword != 0x3030302E) break;
            _BCDT(gpsdata.stamp.time)->col.wday   = 0;
            _BCDT(gpsdata.stamp.time)->col.hour   = stob((char*)&_ptr[0]);
            _BCDT(gpsdata.stamp.time)->col.minute = stob((char*)&_ptr[2]);
            _BCDT(gpsdata.stamp.time)->col.second = stob((char*)&_ptr[4]);
        }
        else if (_column == 2) { /* status */
            if ('A' == *_ptr++) gpsdata.update.coordinate = true;
        }
        else if (_column == 4) { /* uLat */
            /* 南緯は負 */
            if ('S' == *_ptr++) gpsdata.coordinate.latitude = -gpsdata.coordinate.latitude;
        }
        else if (_column == 6) { /* uLat */
            /* 西経は負 */
            if ('W' == *_ptr++) gpsdata.coordinate.longitude = -gpsdata.coordinate.longitude;
        }
        else if (_column == 9) { /* Date */
            _BCDD(gpsdata.stamp.date)->col.day   = stob((char*)&_ptr[0]);
            _BCDD(gpsdata.stamp.date)->col.month = stob((char*)&_ptr[2]);
            _BCDD(gpsdata.stamp.date)->bytes[2]  = stob((char*)&_ptr[4]);
            _BCDD(gpsdata.stamp.date)->bytes[3]  = 0x20;
            gpsdata.update.stamp = true;
            return true;
        }
        else if (gpsdata.update.coordinate && (_column == 3 || _column == 5)) { /* Lat or Lon */
            uint32_t _num1 = 0, _num2 = 0;
            while (*_ptr >= '0' && '9' >= *_ptr) _num1 = _num1 * 10 + (*_ptr++ - '0');
            /* 小数点以下 */
            if ('.' == *_ptr++) {
                // uint8_t _d = 0;
                while (*_ptr >= '0' && '9' >= *_ptr) {
                    _num2 = _num2 * 10 + (*_ptr++ - '0');
                    // _d++;
                }
                // /* 精度5桁以外なら補正 */
                // if (_d == 4) _num2 *= 10;
                // if (_d == 6) _num2 /= 10;
            }
            _num2 += (_num1 % 100) * 100000 + (_num1 / 100) * 6000000;
            if (_column == 3)      gpsdata.coordinate.latitude  = _num2;
            else if (_column == 5) gpsdata.coordinate.longitude = _num2;
        }
    }
    return false;
}

// end of code
