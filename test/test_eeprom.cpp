#if 0
#include <Arduino.h>
#include <unity.h>
#include <TTS.h>
#include <SoftwareSerial.h>
#include "param.hpp"
#include "param_tab.hpp"

void test_load_param()
{
  ParamTable param_tab;
  param_tab.Init(1);

  uint8_t wbuf[4] = {0x01, 0x02, 0x03, 0x04};
  param_tab.WriteParam(0, wbuf, 4);

  uint8_t rbuf[4] = {0};
  uint8_t rlen = 0;
  bool ret = param_tab.ReadParam(0, rbuf, &rlen);

  TEST_ASSERT_TRUE(ret);
  TEST_ASSERT_EQUAL(4, rlen);
  TEST_ASSERT_EQUAL(0x01, rbuf[0]);
  TEST_ASSERT_EQUAL(0x02, rbuf[1]);
  TEST_ASSERT_EQUAL(0x03, rbuf[2]);
  TEST_ASSERT_EQUAL(0x04, rbuf[3]);
}

void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);
  Serial.begin(115200);
  UNITY_BEGIN(); // IMPORTANT LINE!
}

void loop()
{
  RUN_TEST(test_load_param);
  // test_load_param();
  tpf("hello world\n");
  UNITY_END();
}
#endif