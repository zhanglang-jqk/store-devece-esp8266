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
  digitalWrite(16, state);
}

void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  Serial.begin(115200);
  printf("system begin\n");
  pinMode(16, OUTPUT);

  UNITY_BEGIN(); // IMPORTANT LINE!
}

void loop()
{
  RUN_TEST(test_pin16);
  delay(1000);
}

// test_tts.cpp
#endif