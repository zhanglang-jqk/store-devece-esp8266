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
void test_pin15();
/* variable definition ----------------------------------------------------------------------------------------------------- */
/* function implementation ------------------------------------------------------------------------------------------------- */
void test_pin16()
{
  static uint8_t state = 0;
  state = !state;
  digitalWrite(14, state);
}

void test_pin15()
{
  static uint8_t state = 0;
  state = !state;
  digitalWrite(15, state);
}

void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);
  // pinMode(RELAY_PIN, OUTPUT);
  // digitalWrite(RELAY_PIN, LOW);
  Serial.begin(115200);
  printf("system begin\n");
  pinMode(15, OUTPUT);
  digitalWrite(15, LOW);
  UNITY_BEGIN(); // IMPORTANT LINE!
}

void loop()
{
  // RUN_TEST(test_pin16);
  // RUN_TEST(test_pin15);
  delay(1000);
}

// test_tts.cpp
#endif