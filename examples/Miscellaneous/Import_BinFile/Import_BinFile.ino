/**
 * @file Import_BinFile.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-05
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

/* FLASHページ境界に次の定数領域要素を配置する */
// PAGE_ALIGN(".progmem");

/* 定数領域のシンボル FooBin に foo.bin を配置し参照できるようにする */
IMPORT_BINFILE(".progmem", FooBin, __QUOTE(SKETCH_PATH) "/" "foo.bin");

/* C言語へのシンボル割付（型は任意） */
extern const uint8_t FooBin[], FooBin_end[] PROGMEM;

/* 量数 */
const size_t _sizeof_FooBin = FooBin_end - FooBin;

/* .data 領域への読込の場合（SRAM空間） */ /* 部分読込 */
IMPORT_BINFILE_PART(".data", FooBinDat, __QUOTE(SKETCH_PATH) "/" "foo.bin", 0x2c, 0x30);
extern const uint8_t FooBinDat[], FooBinDat_end[];
const size_t _sizeof_FooBinDat = (FooBinDat_end - FooBinDat);

void setup (void) {
  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));

  /* PROGMEM空間の場合 */
  Serial.print(F("FooBin_ADDR=0x")).println(pgm_get_far_address(FooBin), HEX);
  Serial.print(F("sizeof=")).println(_sizeof_FooBin, DEC);
  Serial.printDump(P(FooBin), _sizeof_FooBin, true).println(F("[End]"));

  /* SRAM空間の場合 */
  Serial.print(F("FooBinDat_ADDR=0x")).println((uint16_t)&FooBinDat, HEX);
  Serial.print(F("sizeof=")).println(_sizeof_FooBinDat, DEC);
  Serial.printDump(FooBinDat, _sizeof_FooBinDat, true).println(F("[End]"));
}

// end of code
