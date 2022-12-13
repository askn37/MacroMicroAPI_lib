/**
 * @file GPS_RMS.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-11
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include "bcddatetime.h"
#include <stddef.h>

struct gpsdata_t {
  struct {
    int32_t latitude;
    int32_t longitude;
  } coordinate;
  bcddatetime_t stamp;
  struct {
    bool coordinate : 1;
    bool stamp : 1;
  } update;
};

class GPS_RMS_Class {
private:
  gpsdata_t gpsdata;
  int indexof (const char* _src, const char _val, size_t _limit);

public:
  bool update (void* _buffer, size_t _limit);
  gpsdata_t getData (void);
  uint8_t calcCheckSum (void* _buffer, size_t _limit);
  bool testCheckSum (void* _buffer, size_t _limit);
  bool parseNMEA (void* _buffer, size_t _limit);
};

// end of code
