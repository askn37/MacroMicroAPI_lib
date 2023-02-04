/**
 * @file DOSFont8x16.cpp
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-24
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <FontSets.h>

const uint8_t moderndos_8x16 [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x20
  0x00, 0x00, 0x00, 0x18, 0x3c, 0x3c, 0x3c, 0x18,  0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, // 0x21 !
  0x00, 0x00, 0x66, 0x66, 0x22, 0x44, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x22 "
  0x00, 0x00, 0x00, 0x00, 0x00, 0x6c, 0x6c, 0xfe,  0x6c, 0x6c, 0xfe, 0x6c, 0x6c, 0x00, 0x00, 0x00, // 0x23 #
  0x00, 0x00, 0x10, 0x10, 0x7c, 0xd6, 0xd6, 0x70,  0x38, 0x1c, 0xd6, 0xd6, 0x7c, 0x10, 0x10, 0x00, // 0x24 $
  0x00, 0x00, 0x00, 0x60, 0x92, 0x96, 0x6c, 0x18,  0x30, 0x6c, 0xd2, 0x92, 0x0c, 0x00, 0x00, 0x00, // 0x25 %
  0x00, 0x00, 0x00, 0x38, 0x6c, 0x6c, 0x38, 0x30,  0x76, 0xdc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, // 0x26 &
  0x00, 0x00, 0x18, 0x18, 0x08, 0x10, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x27 '
  0x00, 0x00, 0x00, 0x0c, 0x18, 0x30, 0x30, 0x30,  0x30, 0x30, 0x30, 0x18, 0x0c, 0x00, 0x00, 0x00, // 0x28 (
  0x00, 0x00, 0x00, 0x30, 0x18, 0x0c, 0x0c, 0x0c,  0x0c, 0x0c, 0x0c, 0x18, 0x30, 0x00, 0x00, 0x00, // 0x29 )
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6c, 0x38,  0xfe, 0x38, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x2a *
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18,  0x7e, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x2b +
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x30, 0x30, 0x10, 0x20, 0x00, // 0x2c ,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x2d -
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, // 0x2e .
  0x00, 0x00, 0x00, 0x00, 0x02, 0x06, 0x0c, 0x18,  0x30, 0x60, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, // 0x2f /

  0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xce, 0xde,  0xf6, 0xe6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, // 0x30 0
  0x00, 0x00, 0x00, 0x18, 0x38, 0x78, 0x18, 0x18,  0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x00, 0x00, // 0x31 1
  0x00, 0x00, 0x00, 0x7c, 0xc6, 0x06, 0x0c, 0x18,  0x30, 0x60, 0xc0, 0xc6, 0xfe, 0x00, 0x00, 0x00, // 0x32 2
  0x00, 0x00, 0x00, 0x7c, 0xc6, 0x06, 0x06, 0x3c,  0x06, 0x06, 0x06, 0xc6, 0x7c, 0x00, 0x00, 0x00, // 0x33 3
  0x00, 0x00, 0x00, 0x0c, 0x1c, 0x3c, 0x6c, 0xcc,  0xcc, 0xfe, 0x0c, 0x0c, 0x1e, 0x00, 0x00, 0x00, // 0x34 4
  0x00, 0x00, 0x00, 0xfe, 0xc0, 0xc0, 0xc0, 0xfc,  0x06, 0x06, 0x06, 0xc6, 0x7c, 0x00, 0x00, 0x00, // 0x35 5
  0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc0, 0xc0, 0xfc,  0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, // 0x36 6
  0x00, 0x00, 0x00, 0xfe, 0xc6, 0x06, 0x06, 0x0c,  0x18, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, // 0x37 7
  0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c,  0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, // 0x38 8
  0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0x7e,  0x06, 0x06, 0x06, 0xc6, 0x7c, 0x00, 0x00, 0x00, // 0x39 9
  0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00,  0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, // 0x3a :
  0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00,  0x00, 0x00, 0x18, 0x18, 0x08, 0x10, 0x00, 0x00, // 0x3b ;
  0x00, 0x00, 0x00, 0x00, 0x06, 0x0c, 0x18, 0x30,  0x60, 0x30, 0x18, 0x0c, 0x06, 0x00, 0x00, 0x00, // 0x3c <
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00,  0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x3d =
  0x00, 0x00, 0x00, 0x00, 0x60, 0x30, 0x18, 0x0c,  0x06, 0x0c, 0x18, 0x30, 0x60, 0x00, 0x00, 0x00, // 0x3e >
  0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0x06, 0x0c,  0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, // 0x3f ?

  0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x9d, 0xa5,  0xa5, 0xad, 0xb6, 0x40, 0x3c, 0x00, 0x00, 0x00, // 0x40 @
  0x00, 0x00, 0x00, 0x38, 0x6c, 0xc6, 0xc6, 0xc6,  0xfe, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, // 0x41 A
  0x00, 0x00, 0x00, 0xfc, 0x66, 0x66, 0x66, 0x7c,  0x66, 0x66, 0x66, 0x66, 0xfc, 0x00, 0x00, 0x00, // 0x42 B
  0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc0, 0xc0,  0xc0, 0xc0, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, // 0x43 C
  0x00, 0x00, 0x00, 0x7c, 0x66, 0x66, 0x66, 0x66,  0x66, 0x66, 0x66, 0x66, 0x7c, 0x00, 0x00, 0x00, // 0x44 D
  0x00, 0x00, 0x00, 0xfe, 0x66, 0x62, 0x68, 0x78,  0x78, 0x68, 0x62, 0x66, 0xfe, 0x00, 0x00, 0x00, // 0x45 E
  0x00, 0x00, 0x00, 0xfe, 0x66, 0x62, 0x68, 0x78,  0x78, 0x68, 0x60, 0x60, 0xf0, 0x00, 0x00, 0x00, // 0x46 F
  0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc0, 0xc0,  0xce, 0xc6, 0xc6, 0xc6, 0x7e, 0x00, 0x00, 0x00, // 0x47 G
  0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xfe,  0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, // 0x48 H
  0x00, 0x00, 0x00, 0x3c, 0x18, 0x18, 0x18, 0x18,  0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, // 0x49 I
  0x00, 0x00, 0x00, 0x1e, 0x0c, 0x0c, 0x0c, 0x0c,  0x0c, 0x0c, 0xcc, 0xcc, 0x78, 0x00, 0x00, 0x00, // 0x4a J
  0x00, 0x00, 0x00, 0xe6, 0x66, 0x66, 0x6c, 0x78,  0x78, 0x6c, 0x66, 0x66, 0xe6, 0x00, 0x00, 0x00, // 0x4b K
  0x00, 0x00, 0x00, 0xf0, 0x60, 0x60, 0x60, 0x60,  0x60, 0x60, 0x62, 0x66, 0xfe, 0x00, 0x00, 0x00, // 0x4c L
  0x00, 0x00, 0x00, 0x82, 0xc6, 0xee, 0xfe, 0xfe,  0xd6, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, // 0x4d M
  0x00, 0x00, 0x00, 0x86, 0xc6, 0xe6, 0xf6, 0xfe,  0xde, 0xce, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, // 0x4e N
  0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6,  0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, // 0x4f O

  0x00, 0x00, 0x00, 0xfc, 0x66, 0x66, 0x66, 0x66,  0x7c, 0x60, 0x60, 0x60, 0xfc, 0x00, 0x00, 0x00, // 0x50 P
  0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6,  0xc6, 0xc6, 0xd6, 0xde, 0x7c, 0x06, 0x00, 0x00, // 0x51 Q
  0x00, 0x00, 0x00, 0xfc, 0x66, 0x66, 0x66, 0x66,  0x7c, 0x6c, 0x66, 0x66, 0xe6, 0x00, 0x00, 0x00, // 0x52 R
  0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0x60, 0x38,  0x0c, 0x06, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, // 0x53 S
  0x00, 0x00, 0x00, 0x7e, 0x7e, 0x5a, 0x18, 0x18,  0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, // 0x54 T
  0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,  0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, // 0x55 U
  0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,  0xc6, 0xc6, 0x6c, 0x38, 0x10, 0x00, 0x00, 0x00, // 0x56 V
  0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,  0xd6, 0xfe, 0xee, 0xc6, 0x82, 0x00, 0x00, 0x00, // 0x57 W
  0x00, 0x00, 0x00, 0xc6, 0xc6, 0x6c, 0x7c, 0x38,  0x38, 0x7c, 0x6c, 0xc6, 0xc6, 0x00, 0x00, 0x00, // 0x58 X
  0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3c,  0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, // 0x59 Y
  0x00, 0x00, 0x00, 0xfe, 0xc6, 0x86, 0x0c, 0x18,  0x30, 0x60, 0xc2, 0xc6, 0xfe, 0x00, 0x00, 0x00, // 0x5a Z
  0x00, 0x00, 0x00, 0x3c, 0x30, 0x30, 0x30, 0x30,  0x30, 0x30, 0x30, 0x30, 0x3c, 0x00, 0x00, 0x00, // 0x5b [
  0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x60, 0x30,  0x18, 0x0c, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, // 0x5c \_
  0x00, 0x00, 0x00, 0x3c, 0x0c, 0x0c, 0x0c, 0x0c,  0x0c, 0x0c, 0x0c, 0x0c, 0x3c, 0x00, 0x00, 0x00, // 0x5d ]
  0x10, 0x38, 0x6c, 0xc6, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x5e ^
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, // 0x5f _

  0x00, 0x00, 0x18, 0x18, 0x10, 0x08, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x60 `
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x0c,  0x7c, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, // 0x61 a
  0x00, 0x00, 0x00, 0xe0, 0x60, 0x60, 0x7c, 0x66,  0x66, 0x66, 0x66, 0x66, 0x7c, 0x00, 0x00, 0x00, // 0x62 b
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6,  0xc0, 0xc0, 0xc0, 0xc6, 0x7c, 0x00, 0x00, 0x00, // 0x63 c
  0x00, 0x00, 0x00, 0x1c, 0x0c, 0x0c, 0x7c, 0xcc,  0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, // 0x64 d
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6,  0xc6, 0xfe, 0xc0, 0xc6, 0x7c, 0x00, 0x00, 0x00, // 0x65 e
  0x00, 0x00, 0x00, 0x1c, 0x36, 0x30, 0x7c, 0x30,  0x30, 0x30, 0x30, 0x30, 0x78, 0x00, 0x00, 0x00, // 0x66 f
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0xcc,  0xcc, 0xcc, 0xcc, 0x7c, 0x0c, 0xcc, 0x78, 0x00, // 0x67 g
  0x00, 0x00, 0x00, 0xe0, 0x60, 0x60, 0x6c, 0x76,  0x66, 0x66, 0x66, 0x66, 0xe6, 0x00, 0x00, 0x00, // 0x68 h
  0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x38, 0x18,  0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, // 0x69 i
  0x00, 0x00, 0x00, 0x0c, 0x0c, 0x00, 0x1c, 0x0c,  0x0c, 0x0c, 0x0c, 0x0c, 0xcc, 0xcc, 0x78, 0x00, // 0x6a j
  0x00, 0x00, 0x00, 0xe0, 0x60, 0x60, 0x66, 0x6c,  0x78, 0x78, 0x6c, 0x66, 0xe6, 0x00, 0x00, 0x00, // 0x6b k
  0x00, 0x00, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18,  0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, // 0x6c l
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xec, 0xfe,  0xd6, 0xd6, 0xd6, 0xd6, 0xc6, 0x00, 0x00, 0x00, // 0x6d m
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdc, 0x66,  0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, // 0x6e n
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6,  0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, // 0x6f o

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdc, 0x66,  0x66, 0x66, 0x66, 0x7c, 0x60, 0x60, 0xf0, 0x00, // 0x70 p
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xcc,  0xcc, 0xcc, 0xcc, 0x7c, 0x0c, 0x0c, 0x1e, 0x00, // 0x71 q
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xde, 0x76,  0x60, 0x60, 0x60, 0x60, 0xf0, 0x00, 0x00, 0x00, // 0x72 r
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6,  0x60, 0x38, 0x0c, 0xc6, 0x7c, 0x00, 0x00, 0x00, // 0x73 s
  0x00, 0x00, 0x00, 0x10, 0x30, 0x30, 0xfc, 0x30,  0x30, 0x30, 0x30, 0x34, 0x18, 0x00, 0x00, 0x00, // 0x74 t
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcc,  0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, // 0x75 u
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0xc6,  0xc6, 0xc6, 0x6c, 0x38, 0x10, 0x00, 0x00, 0x00, // 0x76 v
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0xd6,  0xd6, 0xd6, 0xd6, 0xfe, 0x6c, 0x00, 0x00, 0x00, // 0x77 w
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0xc6,  0x6c, 0x38, 0x6c, 0xc6, 0xc6, 0x00, 0x00, 0x00, // 0x78 x
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0xc6,  0xc6, 0xc6, 0xc6, 0x7e, 0x06, 0x0c, 0xf8, 0x00, // 0x79 y
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x8c,  0x18, 0x30, 0x60, 0xc2, 0xfe, 0x00, 0x00, 0x00, // 0x7a z
  0x00, 0x00, 0x00, 0x0e, 0x18, 0x18, 0x18, 0x70,  0x18, 0x18, 0x18, 0x18, 0x0e, 0x00, 0x00, 0x00, // 0x7b {
  0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x00,  0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, // 0x7c |
  0x00, 0x00, 0x00, 0x70, 0x18, 0x18, 0x18, 0x0e,  0x18, 0x18, 0x18, 0x18, 0x70, 0x00, 0x00, 0x00, // 0x7d }
  0x76, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x7e ~
  0x00, 0x18, 0x24, 0x24, 0x18, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // 0x7f
};

const uint8_t moderndos_8x16_vh [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 20 ' '
  0x00, 0x00, 0x70, 0xf8, 0xf8, 0x70, 0x00, 0x00,  0x00, 0x00, 0x00, 0x1b, 0x1b, 0x00, 0x00, 0x00, // 21 '!'
  0x00, 0x2c, 0x1c, 0x00, 0x00, 0x2c, 0x1c, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 22 '"'
  0x80, 0xe0, 0xe0, 0x80, 0xe0, 0xe0, 0x80, 0x00,  0x04, 0x1f, 0x1f, 0x04, 0x1f, 0x1f, 0x04, 0x00, // 23 '#'
  0x60, 0xf0, 0x90, 0xfc, 0x10, 0x70, 0x60, 0x00,  0x0c, 0x1c, 0x11, 0x7f, 0x13, 0x1e, 0x0c, 0x00, // 24 '$'
  0x30, 0x48, 0x48, 0xb0, 0xc0, 0x60, 0x30, 0x00,  0x0c, 0x06, 0x03, 0x0d, 0x12, 0x12, 0x0c, 0x00, // 25 '%'
  0x00, 0x30, 0xf8, 0xc8, 0x78, 0x30, 0x00, 0x00,  0x0e, 0x1f, 0x11, 0x13, 0x0e, 0x1f, 0x11, 0x00, // 26 '&'
  0x00, 0x00, 0x00, 0x2c, 0x1c, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 27 '''
  0x00, 0x00, 0xe0, 0xf0, 0x18, 0x08, 0x00, 0x00,  0x00, 0x00, 0x07, 0x0f, 0x18, 0x10, 0x00, 0x00, // 28 '('
  0x00, 0x00, 0x08, 0x18, 0xf0, 0xe0, 0x00, 0x00,  0x00, 0x00, 0x10, 0x18, 0x0f, 0x07, 0x00, 0x00, // 29 ')'
  0x00, 0x40, 0xc0, 0x80, 0xc0, 0x40, 0x00, 0x00,  0x01, 0x05, 0x07, 0x03, 0x07, 0x05, 0x01, 0x00, // 2a '*'
  0x00, 0x00, 0x00, 0xc0, 0xc0, 0x00, 0x00, 0x00,  0x00, 0x01, 0x01, 0x07, 0x07, 0x01, 0x01, 0x00, // 2b '+'
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x58, 0x38, 0x00, 0x00, 0x00, 0x00, // 2c ', '
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, // 2d '-'
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, // 2e '.'
  0x00, 0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x00,  0x0c, 0x06, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, // 2f '/'

  0xf0, 0xf8, 0x08, 0x88, 0xc8, 0xf8, 0xf0, 0x00,  0x0f, 0x1f, 0x13, 0x11, 0x10, 0x1f, 0x0f, 0x00, // 30 '0'
  0x00, 0x20, 0x30, 0xf8, 0xf8, 0x00, 0x00, 0x00,  0x00, 0x10, 0x10, 0x1f, 0x1f, 0x10, 0x10, 0x00, // 31 '1'
  0x10, 0x18, 0x08, 0x88, 0xc8, 0x78, 0x30, 0x00,  0x1c, 0x1e, 0x13, 0x11, 0x10, 0x18, 0x18, 0x00, // 32 '2'
  0x10, 0x18, 0x88, 0x88, 0x88, 0xf8, 0x70, 0x00,  0x08, 0x18, 0x10, 0x10, 0x10, 0x1f, 0x0f, 0x00, // 33 '3'
  0x80, 0xc0, 0x60, 0x30, 0xf8, 0xf8, 0x00, 0x00,  0x03, 0x03, 0x02, 0x12, 0x1f, 0x1f, 0x12, 0x00, // 34 '4'
  0xf8, 0xf8, 0x88, 0x88, 0x88, 0x88, 0x08, 0x00,  0x08, 0x18, 0x10, 0x10, 0x10, 0x1f, 0x0f, 0x00, // 35 '5'
  0xf0, 0xf8, 0x88, 0x88, 0x88, 0x98, 0x10, 0x00,  0x0f, 0x1f, 0x10, 0x10, 0x10, 0x1f, 0x0f, 0x00, // 36 '6'
  0x18, 0x18, 0x08, 0x08, 0x88, 0xf8, 0x78, 0x00,  0x00, 0x00, 0x1e, 0x1f, 0x01, 0x00, 0x00, 0x00, // 37 '7'
  0x70, 0xf8, 0x88, 0x88, 0x88, 0xf8, 0x70, 0x00,  0x0f, 0x1f, 0x10, 0x10, 0x10, 0x1f, 0x0f, 0x00, // 38 '8'
  0x70, 0xf8, 0x88, 0x88, 0x88, 0xf8, 0xf0, 0x00,  0x08, 0x18, 0x10, 0x10, 0x10, 0x1f, 0x0f, 0x00, // 39 '9'
  0x00, 0x00, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x0c, 0x0c, 0x00, 0x00, 0x00, // 3a ':'
  0x00, 0x00, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x2c, 0x1c, 0x00, 0x00, 0x00, // 3b ';'
  0x00, 0x00, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x00,  0x00, 0x01, 0x03, 0x06, 0x0c, 0x18, 0x10, 0x00, // 3c '<'
  0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00,  0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00, // 3d '='
  0x00, 0x10, 0x30, 0x60, 0xc0, 0x80, 0x00, 0x00,  0x00, 0x10, 0x18, 0x0c, 0x06, 0x03, 0x01, 0x00, // 3e '>'
  0x30, 0x38, 0x08, 0x08, 0x88, 0xf8, 0x70, 0x00,  0x00, 0x00, 0x00, 0x1b, 0x1b, 0x00, 0x00, 0x00, // 3f '?'

  0xc0, 0x20, 0x90, 0x50, 0x50, 0xd0, 0x20, 0xC0,  0x07, 0x08, 0x17, 0x14, 0x12, 0x17, 0x04, 0x03, // 40 '@'
  0xe0, 0xf0, 0x18, 0x08, 0x18, 0xf0, 0xe0, 0x00,  0x1f, 0x1f, 0x01, 0x01, 0x01, 0x1f, 0x1f, 0x00, // 41 'A'
  0x08, 0xf8, 0xf8, 0x88, 0x88, 0xf8, 0x70, 0x00,  0x10, 0x1f, 0x1f, 0x10, 0x10, 0x1f, 0x0f, 0x00, // 42 'B'
  0xf0, 0xf8, 0x08, 0x08, 0x08, 0x38, 0x30, 0x00,  0x0f, 0x1f, 0x10, 0x10, 0x10, 0x1c, 0x0c, 0x00, // 43 'C'
  0x00, 0xf8, 0xf8, 0x08, 0x08, 0xf8, 0xf0, 0x00,  0x00, 0x1f, 0x1f, 0x10, 0x10, 0x1f, 0x0f, 0x00, // 44 'D'
  0x08, 0xf8, 0xf8, 0x88, 0xc8, 0x18, 0x38, 0x00,  0x10, 0x1f, 0x1f, 0x11, 0x13, 0x18, 0x1c, 0x00, // 45 'E'
  0x08, 0xf8, 0xf8, 0x88, 0xc8, 0x18, 0x38, 0x00,  0x10, 0x1f, 0x1f, 0x11, 0x03, 0x00, 0x00, 0x00, // 46 'F'
  0xf0, 0xf8, 0x08, 0x08, 0x08, 0x38, 0x30, 0x00,  0x0f, 0x1f, 0x10, 0x10, 0x11, 0x1f, 0x1f, 0x00, // 47 'G'
  0xf8, 0xf8, 0x80, 0x80, 0x80, 0xf8, 0xf8, 0x00,  0x1f, 0x1f, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x00, // 48 'H'
  0x00, 0x00, 0x08, 0xf8, 0xf8, 0x08, 0x00, 0x00,  0x00, 0x00, 0x10, 0x1f, 0x1f, 0x10, 0x00, 0x00, // 49 'I'
  0x00, 0x00, 0x00, 0x08, 0xf8, 0xf8, 0x08, 0x00,  0x0c, 0x1c, 0x10, 0x10, 0x1f, 0x0f, 0x00, 0x00, // 4a 'J'
  0x08, 0xf8, 0xf8, 0x80, 0xc0, 0x78, 0x38, 0x00,  0x10, 0x1f, 0x1f, 0x01, 0x03, 0x1e, 0x1c, 0x00, // 4b 'K'
  0x08, 0xf8, 0xf8, 0x08, 0x00, 0x00, 0x00, 0x00,  0x10, 0x1f, 0x1f, 0x10, 0x10, 0x18, 0x1c, 0x00, // 4c 'L'
  0xf8, 0xf0, 0xe0, 0xc0, 0xe0, 0xf0, 0xf8, 0x00,  0x1f, 0x1f, 0x00, 0x01, 0x00, 0x1f, 0x1f, 0x00, // 4d 'M'
  0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0xf8, 0xf8, 0x00,  0x1f, 0x1f, 0x00, 0x01, 0x03, 0x1f, 0x1f, 0x00, // 4e 'N'
  0xf0, 0xf8, 0x08, 0x08, 0x08, 0xf8, 0xf0, 0x00,  0x0f, 0x1f, 0x10, 0x10, 0x10, 0x1f, 0x0f, 0x00, // 4f 'O'

  0x08, 0xf8, 0xf8, 0x08, 0x08, 0xf8, 0xf0, 0x00,  0x10, 0x1f, 0x1f, 0x11, 0x11, 0x11, 0x00, 0x00, // 50 'P'
  0xf0, 0xf8, 0x08, 0x08, 0x08, 0xf8, 0xf0, 0x00,  0x0f, 0x1f, 0x10, 0x1c, 0x18, 0x3f, 0x2f, 0x00, // 51 'Q'
  0x08, 0xf8, 0xf8, 0x08, 0x08, 0xf8, 0xf0, 0x00,  0x10, 0x1f, 0x1f, 0x01, 0x03, 0x1f, 0x1c, 0x00, // 52 'R'
  0x30, 0x78, 0xc8, 0x88, 0x88, 0x38, 0x30, 0x00,  0x0c, 0x1c, 0x10, 0x10, 0x11, 0x1f, 0x0e, 0x00, // 53 'S'
  0x00, 0x38, 0x18, 0xf8, 0xf8, 0x18, 0x38, 0x00,  0x00, 0x00, 0x10, 0x1f, 0x1f, 0x10, 0x00, 0x00, // 54 'T'
  0xf8, 0xf8, 0x00, 0x00, 0x00, 0xf8, 0xf8, 0x00,  0x0f, 0x1f, 0x10, 0x10, 0x10, 0x1f, 0x0f, 0x00, // 55 'U'
  0xf8, 0xf8, 0x00, 0x00, 0x00, 0xf8, 0xf8, 0x00,  0x03, 0x07, 0x0c, 0x18, 0x0c, 0x07, 0x03, 0x00, // 56 'V'
  0xf8, 0xf8, 0x00, 0x00, 0x00, 0xf8, 0xf8, 0x00,  0x1f, 0x0f, 0x06, 0x03, 0x06, 0x0f, 0x1f, 0x00, // 57 'W'
  0x18, 0x78, 0xe0, 0xc0, 0xe0, 0x78, 0x18, 0x00,  0x18, 0x1e, 0x07, 0x03, 0x07, 0x1e, 0x18, 0x00, // 58 'X'
  0x00, 0x78, 0xf8, 0x80, 0x80, 0xf8, 0x78, 0x00,  0x00, 0x00, 0x10, 0x1f, 0x1f, 0x10, 0x00, 0x00, // 59 'Y'
  0x38, 0x18, 0x08, 0x88, 0xc8, 0x78, 0x38, 0x00,  0x1c, 0x1e, 0x13, 0x11, 0x10, 0x18, 0x1c, 0x00, // 5a 'Z'
  0x00, 0x00, 0xf8, 0xf8, 0x08, 0x08, 0x00, 0x00,  0x00, 0x00, 0x1f, 0x1f, 0x10, 0x10, 0x00, 0x00, // 5b '['
  0x30, 0x60, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x01, 0x03, 0x06, 0x0c, 0x00, // 5c '\'
  0x00, 0x00, 0x08, 0x08, 0xf8, 0xf8, 0x00, 0x00,  0x00, 0x00, 0x10, 0x10, 0x1f, 0x1f, 0x00, 0x00, // 5d ']'
  0x08, 0x0c, 0x06, 0x03, 0x06, 0x0c, 0x08, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 5e '^'
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, // 5f '_'

  0x00, 0x00, 0x00, 0x1c, 0x2c, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 60 '`'
  0x00, 0x40, 0x40, 0x40, 0xc0, 0x80, 0x00, 0x00,  0x0e, 0x1f, 0x11, 0x11, 0x0f, 0x1f, 0x10, 0x00, // 61 'a'
  0x08, 0xf8, 0xf8, 0x40, 0x40, 0xc0, 0x80, 0x00,  0x00, 0x1f, 0x1f, 0x10, 0x10, 0x1f, 0x0f, 0x00, // 62 'b'
  0x80, 0xc0, 0x40, 0x40, 0x40, 0xc0, 0x80, 0x00,  0x0f, 0x1f, 0x10, 0x10, 0x10, 0x18, 0x08, 0x00, // 63 'c'
  0x80, 0xc0, 0x40, 0x48, 0xf8, 0xf8, 0x00, 0x00,  0x0f, 0x1f, 0x10, 0x10, 0x0f, 0x1f, 0x10, 0x00, // 64 'd'
  0x80, 0xc0, 0x40, 0x40, 0x40, 0xc0, 0x80, 0x00,  0x0f, 0x1f, 0x12, 0x12, 0x12, 0x1b, 0x0b, 0x00, // 65 'e'
  0x00, 0x40, 0xf0, 0xf8, 0x48, 0x58, 0x10, 0x00,  0x00, 0x10, 0x1f, 0x1f, 0x10, 0x00, 0x00, 0x00, // 66 'f'
  0x80, 0xc0, 0x40, 0x40, 0x80, 0xc0, 0x40, 0x00,  0x27, 0x6f, 0x48, 0x48, 0x7f, 0x3f, 0x00, 0x00, // 67 'g'
  0x08, 0xf8, 0xf8, 0x80, 0x40, 0xc0, 0x80, 0x00,  0x10, 0x1f, 0x1f, 0x00, 0x00, 0x1f, 0x1f, 0x00, // 68 'h'
  0x00, 0x00, 0x40, 0xd8, 0xd8, 0x00, 0x00, 0x00,  0x00, 0x00, 0x10, 0x1f, 0x1f, 0x10, 0x00, 0x00, // 69 'i'
  0x00, 0x00, 0x00, 0x40, 0xd8, 0xd8, 0x00, 0x00,  0x30, 0x70, 0x40, 0x40, 0x7f, 0x3f, 0x00, 0x00, // 6a 'j'
  0x08, 0xf8, 0xf8, 0x00, 0x80, 0xc0, 0x40, 0x00,  0x10, 0x1f, 0x1f, 0x03, 0x07, 0x1c, 0x18, 0x00, // 6b 'k'
  0x00, 0x00, 0x08, 0xf8, 0xf8, 0x00, 0x00, 0x00,  0x00, 0x00, 0x10, 0x1f, 0x1f, 0x10, 0x00, 0x00, // 6c 'l'
  0xc0, 0xc0, 0xc0, 0x80, 0xc0, 0xc0, 0x80, 0x00,  0x1f, 0x1f, 0x00, 0x0f, 0x00, 0x1f, 0x1f, 0x00, // 6d 'm'
  0x40, 0xc0, 0x80, 0x40, 0x40, 0xc0, 0x80, 0x00,  0x00, 0x1f, 0x1f, 0x00, 0x00, 0x1f, 0x1f, 0x00, // 6e 'n'
  0x80, 0xc0, 0x40, 0x40, 0x40, 0xc0, 0x80, 0x00,  0x0f, 0x1f, 0x10, 0x10, 0x10, 0x1f, 0x0f, 0x00, // 6f 'o'

  0x40, 0xc0, 0x80, 0x40, 0x40, 0xc0, 0x80, 0x00,  0x40, 0x7f, 0x7f, 0x48, 0x08, 0x0f, 0x07, 0x00, // 70 'p'
  0x80, 0xc0, 0x40, 0x40, 0xc0, 0xc0, 0x00, 0x00,  0x07, 0x0f, 0x08, 0x48, 0x7f, 0x7f, 0x40, 0x00, // 71 'q'
  0x40, 0xc0, 0x80, 0xc0, 0x40, 0xc0, 0xc0, 0x00,  0x10, 0x1f, 0x1f, 0x10, 0x00, 0x00, 0x00, 0x00, // 72 'r'
  0x80, 0xc0, 0x40, 0x40, 0x40, 0xc0, 0x80, 0x00,  0x08, 0x19, 0x13, 0x12, 0x16, 0x1c, 0x08, 0x00, // 73 's'
  0x40, 0x40, 0xf0, 0xf8, 0x40, 0x40, 0x00, 0x00,  0x00, 0x00, 0x0f, 0x1f, 0x10, 0x08, 0x00, 0x00, // 74 't'
  0xc0, 0xc0, 0x00, 0x00, 0xc0, 0xc0, 0x00, 0x00,  0x0f, 0x1f, 0x10, 0x10, 0x0f, 0x1f, 0x10, 0x00, // 75 'u'
  0xc0, 0xc0, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0x00,  0x03, 0x07, 0x0c, 0x18, 0x0c, 0x07, 0x03, 0x00, // 76 'v'
  0xc0, 0xc0, 0x00, 0x80, 0x00, 0xc0, 0xc0, 0x00,  0x0f, 0x1f, 0x18, 0x0f, 0x18, 0x1f, 0x0f, 0x00, // 77 'w'
  0xc0, 0xc0, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0x00,  0x18, 0x1d, 0x07, 0x02, 0x07, 0x1d, 0x18, 0x00, // 78 'x'
  0xc0, 0xc0, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0x00,  0x47, 0x4f, 0x48, 0x48, 0x68, 0x3f, 0x1f, 0x00, // 79 'y'
  0xc0, 0x40, 0x40, 0x40, 0xc0, 0xc0, 0x40, 0x00,  0x18, 0x1c, 0x16, 0x13, 0x11, 0x10, 0x18, 0x00, // 7a 'z'
  0x00, 0x80, 0x80, 0xf0, 0x78, 0x08, 0x08, 0x00,  0x00, 0x00, 0x00, 0x0f, 0x1f, 0x10, 0x10, 0x00, // 7b '{'
  0x00, 0x00, 0x00, 0x78, 0x78, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x1e, 0x1e, 0x00, 0x00, 0x00, // 7c '|'
  0x00, 0x08, 0x08, 0x78, 0xf0, 0x80, 0x80, 0x00,  0x00, 0x10, 0x10, 0x1f, 0x0f, 0x00, 0x00, 0x00, // 7d '}'
  0x02, 0x03, 0x01, 0x03, 0x02, 0x03, 0x01, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 7e '~'
  0x00, 0x00, 0x0c, 0x12, 0x12, 0x0c, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // 7f
};

// end of code
