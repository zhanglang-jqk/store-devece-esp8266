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
// void test_softserial_read_write();
void test_write();
void test_read();
/* variable definition ----------------------------------------------------------------------------------------------------- */
SoftwareSerial mySerial(5, 4); // RX, TX
/* function implementation ------------------------------------------------------------------------------------------------- */
void test_softserial_read_write()
{
  uint8_t rbuf[10];
  memset(rbuf, 0, sizeof(rbuf));
  mySerial.read(rbuf, sizeof(rbuf));
  tpbuf(rbuf, sizeof(rbuf));
}

void test_write()
{
  mySerial.write("aaa");
}

void test_read()
{
  if (mySerial.available())
  {
    tp();
    uint8_t rbuf[10];
    memset(rbuf, 0, sizeof(rbuf));
    mySerial.readBytes(rbuf, sizeof(rbuf));
    tpbuf(rbuf, sizeof(rbuf));
  }
}
void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  Serial.begin(115200);
  printf("system begin\n");
  mySerial.begin(115200); // mySerial.printf("Hello world\n");
  pinMode(16, INPUT);

  UNITY_BEGIN(); // IMPORTANT LINE!
}

void loop()
{
  // RUN_TEST(test_softserial_read_write);
  // RUN_TEST(test_write);
  RUN_TEST(test_read);
  delay(1000);
}

// test_tts.cpp
#endif