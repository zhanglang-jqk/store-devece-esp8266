#if 0
/***********************************************************************
 * @file 	:	test_tts.cpp
 * @author	:	CH
 * @brief	:
 * @Copyright (C)  2024-08-07  .cdWFVCEL. all right reserved
 ***********************************************************************/

/* include ----------------------------------------------------------------------------------------------------------------- */
// #include "include.h"
// #include "test_tts.h"
#include <Arduino.h>
#include <unity.h>
#include <TTS.h>
#include <SoftwareSerial.h>
#include "include.h"
/* macro definition -------------------------------------------------------------------------------------------------------- */
#define RELAY_PIN 5 
/* type definition --------------------------------------------------------------------------------------------------------- */

/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */
void test_pin16();
/* variable definition ----------------------------------------------------------------------------------------------------- */
/* function implementation ------------------------------------------------------------------------------------------------- */
void test_pin16()
{
  static uint8_t state = 0;
  state = !state;
  digitalWrite(14, state);
}

void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  // delay(2000);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  Serial.begin(115200);
  printf("system begin\n");

  // PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
  // GPIO_OUTPUT_SET(14, 1); // 设置 GPIO14 为低电平

  // // 配置 GPIO13 为输出模式
  // PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
  // GPIO_OUTPUT_SET(13, 0); // 设置 GPIO13 为低电平

  // // 配置 GPIO12 为输出模式
  // PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
  // GPIO_OUTPUT_SET(12, 0); // 设置 GPIO12 为低电平

  // UNITY_BEGIN(); // IMPORTANT LINE!
}

void loop()
{
  // RUN_TEST(test_pin16);
  delay(1000);
  tpf("he");
}

// test_tts.cpp
#endif