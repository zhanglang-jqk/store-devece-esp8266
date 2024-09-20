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
// #include <SoftwareSerial.h>
#include <Bonezegei_SoftSerial.h>
/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */

/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */
void test_write_buf();
void test_query_busy_status();
/* variable definition ----------------------------------------------------------------------------------------------------- */
TTS _tts;
// SoftwareSerial mySerial(5, 4); // RX, TX
Bonezegei_SoftSerial mySerial(4, 5); // RX, TX
/* function implementation ------------------------------------------------------------------------------------------------- */

void testTTS_sendVoice_nihao()
{
  char *voice_str = "你好";
  _tts.SendPlayVoice_cmd((uint8_t *)voice_str, strlen(voice_str));

  delay(1000);
}
void testTTS_sendVoice_dajiahao()
{
  char *voice_str = "大家好";
  bool isok = _tts.SendPlayVoice_cmd((uint8_t *)voice_str, strlen(voice_str));
  if (isok)
  {
    printf("send voice success\n");
  }
  else
  {
    printf("send voice failed\n");
  }

  delay(1000);
}
void testTTS_sendVoiceAndVerifyResponse()
{
  char *voice_str = "你好";
  bool ret = _tts.SendPlayVoice_cmd((uint8_t *)voice_str, strlen(voice_str));
  if (ret)
  {
    printf("send voice success\n");
  }
  else
  {
    printf("send voice failed\n");
  }
  TEST_ASSERT_TRUE(ret);
}

void test_write_buf()
{
  // FD 00 06 01 01 C4 E3 BA C3
  uint8_t buf[] = {0xfd, 0x00, 0x06, 0x01, 0x01, 0xc4, 0xe3, 0xba, 0xc3};
  mySerial.write(buf, sizeof(buf));
}

void test_query_busy_status()
{
  const uint16_t WAIT_READ_CH_MS = 1000;
  uint8_t sendbuf[4] = {0xfd, 0x00, 0x01, 0x21};
  mySerial.write(sendbuf, sizeof(sendbuf));
  tpbuf(sendbuf, sizeof(sendbuf));
  uint8_t ret_byte = mySerial.read(WAIT_READ_CH_MS);
  tpf("ret_byte = %x", ret_byte);
  tpf("ret_byte = %d", ret_byte);
}

void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  Serial.begin(115200);
  // printf("system begin\n");
  mySerial.begin(9600); // mySerial.printf("Hello world\n");
  _tts.Init(&mySerial);

  UNITY_BEGIN(); // IMPORTANT LINE!
}

void loop()
{
  // testSoftwareSerial();
  // testTTS_sendVoice_nihao();
  // RUN_TEST(testTTS_sendVoiceAndVerifyResponse);
  // printf("hello world\n");
  // testTTS_sendVoiceAndVerifyResponse();

  // for (int i = 0; i < 10; i++)
  // {
  //   testTTS_sendVoice_dajiahao();
  //   delay(2000);

  //   printf("test %d\n", i);
  //   if (i == 9) UNITY_END(); // stop unit testing
  // }
  // RUN_TEST(test_write_buf);
  RUN_TEST(test_query_busy_status);
  delay(2000);
}

// test_tts.cpp

// 发：FD 00 06 01 01 C4 E3 BA C3
// 收：41
#endif