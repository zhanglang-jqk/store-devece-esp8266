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
#include "include.h"
#include <Bonezegei_SoftSerial.h>
/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */

/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */
void test_write();
void test_read_write();
void test_write_wait_read();
/* variable definition ----------------------------------------------------------------------------------------------------- */
Bonezegei_SoftSerial SoftSerial(4, 5);
/* function implementation ------------------------------------------------------------------------------------------------- */

void test_write()
{
  char a = 'a';
  SoftSerial.write(a);
}

void test_read_write()
{
  if (SoftSerial.available())
  {
    int ch = SoftSerial.read();
    tpf("ch = %c\n", ch);
    // SoftSerial.write(ch);
  }
}

void test_write_wait_read()
{
  char a = 'a';
  SoftSerial.write(a);

  static struct
  {
    uint8_t stat;
    uint32_t last_ms;
    uint8_t rbuf[32];
    uint8_t idx;
  } ctrl_fsm;

  if (ctrl_fsm.stat == 0)
  {
    ctrl_fsm.last_ms = millis();
    ctrl_fsm.stat = 1;
  }
  if (ctrl_fsm.stat == 1) // wait for read until 2s
  {
    while (millis() - ctrl_fsm.last_ms < 2000)
    {
      if (SoftSerial.available())
      {
        int ch = SoftSerial.read();
        // tpf("ch = %c\n", ch);
        // SoftSerial.write(ch);
        ctrl_fsm.rbuf[ctrl_fsm.idx++] = ch;
      }
      // WDT_FEED();
      ESP.wdtFeed();
    }
    // tpbuf(ctrl_fsm.rbuf, ctrl_fsm.idx);
    tpf("%s", (char *)

    ctrl_fsm.idx = 0;
    memset(ctrl_fsm.rbuf, 0, sizeof(ctrl_fsm.rbuf));
    ctrl_fsm.stat = 0;
  }
}

void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  Serial.begin(115200);
  SoftSerial.begin(9600);
  SoftSerial.print("Hello World\n");

  UNITY_BEGIN(); // IMPORTANT LINE!
}

void loop()
{
  // RUN_TEST(test_write);
  // RUN_TEST(test_read_write);
  RUN_TEST(test_write_wait_read);
  // delay(500);
  // UNITY_END();
}
// test_tts.cpp
#endif