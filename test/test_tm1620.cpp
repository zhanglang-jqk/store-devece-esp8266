#if 0
/***********************************************************************
 * @file 	:	test_tm1620.cpp
 * @author	:	CH
 * @brief	:
 * @Copyright (C)  2024-09-06  .cdWFVCEL. all right reserved
 ***********************************************************************/

/* include ----------------------------------------------------------------------------------------------------------------- */
// #include "include.h"
#include <Arduino.h>

#include <unity.h>
#include <TTS.h>
#include <SoftwareSerial.h>
#include "tm1620.h"
/* macro definition -------------------------------------------------------------------------------------------------------- */
#define CLK_PIN 14
#define DIO_PIN 13
#define STB_PIN 12

#define RELAY_PIN 5
/* type definition --------------------------------------------------------------------------------------------------------- */
/* variable declaration ---------------------------------------------------------------------------------------------------- */
TM1620 tm;

/* function declaration ---------------------------------------------------------------------------------------------------- */
void test_pin();
void test_write_byte();
void test_seg_display();
void test_relay();
void test_turn_off_display();
void test_display_();
void test_display_all_number();
void test_display_colon();
void test_display_toggle_colon();
/* variable definition ----------------------------------------------------------------------------------------------------- */

/* function implementation ------------------------------------------------------------------------------------------------- */
void test_turn_off_display()
{
  test_seg_display();
  delay(100);
  tm.TurnOffDispaly();
}

void test_display_all_number()
{
  static uint8_t val = 0;
  if (val++ >= 10)
    val = 0;
  tm.DispSeg(0, val);
  tm.DispSeg(1, val);
  tm.DispSeg(2, val);
  tm.DispSeg(3, val);
}

void test_display_toggle_colon()
{
  TM1620::SEG_COLON;
  tm.DispSeg(1, TM1620::SEG_COLON);
}

void test_display_()
{
  // tm.DispSeg(0, TM1620::SEG_NOT_DISPLAY);
  // tm.DispSeg(1, TM1620::SEG_NOT_DISPLAY);
  // tm.DispSeg(2, TM1620::SEG_NOT_DISPLAY);
  // tm.DispSeg(3, TM1620::SEG_NOT_DISPLAY);
  // delay(100);
  // delay(1000);
  tm.DispSeg(0, TM1620::SEG_VAL_);
  tm.DispSeg(1, TM1620::SEG_VAL_);
  tm.DispSeg(2, TM1620::SEG_VAL_);
  tm.DispSeg(3, TM1620::SEG_VAL_);
}

void test_display_colon()
{
  tm.DispSeg(0, TM1620::SEG_NOT_DISPLAY);
  tm.DispSeg(1, TM1620::SEG_NOT_DISPLAY);
  tm.DispSeg(2, TM1620::SEG_NOT_DISPLAY);
  tm.DispSeg(3, TM1620::SEG_NOT_DISPLAY);

  tm.DispSeg(1, TM1620::SEG_COLON);
}

void test_pin()
{
  // while (1)
  for (int i = 0; i < 50; i++)
  {
    tm.CLK_HIGH();
    tm.DIO_HIGH();
    tm.STB_HIGH();
    delay(100);
    tm.CLK_LOW();
    tm.DIO_LOW();
    tm.STB_LOW();
    delay(100);
  }
}

void test_write_byte()
{
  // uint32_t i = 1;
  // while (i++ != 0)
  // for (int i = 0; i < 10; i++)
  // {
  tm.WriteByte(0x0b);
  delay(1000);
  // }
}

void test_seg_display()
{
  tm.DispSeg(0, 1);
  tm.DispSeg(1, 6);
  tm.DispSeg(2, 8);
  tm.DispSeg(3, 9);
}

void test_relay()
{
  static uint8_t relay_status = 0;
  relay_status = !relay_status;
  digitalWrite(RELAY_PIN, relay_status);
}
void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  UNITY_BEGIN(); // IMPORTANT LINE!

  delay(2000);

  Serial.begin(115200);
  tpf("system begin\n");
  tm.Init(STB_PIN, CLK_PIN, DIO_PIN);
  pinMode(RELAY_PIN, OUTPUT);
}

void loop()
{
  // RUN_TEST(test_pin);
  // RUN_TEST(test_write_byte);
  // RUN_TEST(test_seg_display);
  // RUN_TEST(test_turn_off_display);
  // RUN_TEST(test_display_);
  // RUN_TEST(test_relay);
  // RUN_TEST(test_display_all_number);
  RUN_TEST(test_display_colon);
  // delay(1000);

  // UNITY_END(); // stop unit testinu
}
#endif
// test_tm1620.cpp
