/**
 * @file TWI_BUS_Scan.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-26
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#define TWI_PULLUP true

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  #ifdef LED_BUILTIN_INVERT
  pinControlRegister(LED_BUILTIN) = PORT_INVEN_bm;
  #endif

  Serial.begin(CONSOLE_BAUD).println(F("\r<Startup>"));
  Serial.print(F("F_CPU=")).println(F_CPU);
}

void loop (void) {

  /* First TWI Master Bus */
  Serial.ln().println(F("[First TWI BUS Scan]"));
  twi_scan(Wire);

  /* Second TWI Master Bus */
  #ifdef Wire1
  Serial.ln().println(F("[Second TWI BUS Scan]"));
  twi_scan(Wire1);
  #endif

  Serial.ln().println(F("<Press enter to next>"));
  Serial.flush();
  while (Serial.read() != '\n');
}

void twi_scan (TWIM_Class TWIC) {
  bool _found = false;
  TWIC.initiate(TWI_SM, TWI_PULLUP);

  /* option : I2C HUB PCA9548A (0x70) selected channels (all) */
  TWIC.start(0x70, 1).send(0xFF);

  for (uint8_t _addr = 8; _addr < 0x78; _addr++) {
    digitalWrite(LED_BUILTIN, TOGGLE);
    if (TWIC.scan(_addr)) {
      _found = true;
      twi_addr_found(_addr);
    }
  }
  TWIC.end();
  if (!_found) Serial.println(F(" (device not found)")).ln();
}

void twi_addr_found (uint8_t _addr) {
  const PGM_t* _message = nullptr;
  Serial.print(F(" 0x")).print(_addr, ZFILL|HEX, 2).print(F(" : "));
  switch (_addr) {
    case 0x29 :          _message = F("TOF  VL53L0X"); break;
    case 0x3C ... 0x3D : _message = F("OLED SSD1306 or SH1107"); break;
    case 0x3E :          _message = F("LCD  ST7032i"); break;
    case 0x44 ... 0x45 : _message = F("ENV  SHT3x"); break;
    case 0x51 :          _message = F("RTC  PCF86xxx"); break;
    case 0x55 :          _message = F("NTAG NT3H2111_2211 (NXP NTAG I2C Plus)"); break;
    case 0x56 :          _message = F("ENV  QMP6988"); break;
    case 0x5A :          _message = F("NCIR MLX90614"); break;
    case 0x60 :          _message = F("ECC  ATECC608A"); break;
    case 0x68 :          _message = F("RTC  DS130x"); break;
    case 0x70 :          _message = F("ENV  QMP6988 or HUB PCA9548A"); break;
    case 0x71 ... 0x75 : _message = F("HUB  PCA9548A"); break;
    case 0x76 ... 0x77 : _message = F("ENV  BME280 or BMP280 or HUB PCA9548A"); break;
  }
  if (_message != nullptr) Serial.println(P(_message));
  else Serial.ln();
}

// end of code
