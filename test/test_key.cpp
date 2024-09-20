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
#include "include.h"
/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */

/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */
void test_key();
void _key();
/* variable definition ----------------------------------------------------------------------------------------------------- */
/* function implementation ------------------------------------------------------------------------------------------------- */

volatile bool interruptFlag = false;

// 在 esp8266 中，中断处理函数必须放在 IRAM 中，否则会出现异常, 必须使用 ICACHE_RAM_ATTR 宏
void ICACHE_RAM_ATTR handleInterrupt()
{
  tp();
  interruptFlag = true;
}

void setup()
{
  delay(2000);
  Serial.begin(115200);
  tpf("Power on..............");
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), handleInterrupt, FALLING);
}

void loop()
{
  if (interruptFlag)
  {
    Serial.println("Interrupt detected");
    interruptFlag = false;
  }
}

// test_tts.cpp
#endif