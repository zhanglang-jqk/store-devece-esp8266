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
#include "param.hpp"
/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */

/* variable declaration ---------------------------------------------------------------------------------------------------- */
Param _param;
/* function declaration ---------------------------------------------------------------------------------------------------- */
void test_load_param();
void test_refresh_param();
/* variable definition ----------------------------------------------------------------------------------------------------- */

/* function implementation ------------------------------------------------------------------------------------------------- */
void test_load_param()
{
  _param.LoadAllParam();

  logf("relay=%d\n", _param.relay);
  logf("wifi_ssid=%s\n", _param.wifi_ssid);
  logf("wifi_pwd=%s\n", _param.wifi_pwd);
  logf("mqtt_server=%s\n", _param.mqtt_server);
  logf("mqtt_port=%d\n", _param.mqtt_port);
  logf("mqtt_username=%s\n", _param.mqtt_username);
  logf("mqtt_password=%s\n", _param.mqtt_password);
  logf("device_id=%s\n", _param.device_id);
  logf("device_global_pub_topic=%s\n", _param.device_global_pub_topic);
  logf("device_sub_topic=%s\n", _param.device_sub_topic);
  logf("device_pub_topic=%s\n", _param.device_pub_topic);
  logf("count_down=%d\n", _param.count_down);

  TEST_ASSERT_EQUAL(1, _param.relay);
  TEST_ASSERT_EQUAL_STRING("88888888", _param.wifi_ssid);
}

void test_refresh_param()
{
  // ESP.getFreeHeap();
  _param.LoadAllParam();

  uint8_t relay_val = 2;
  _param.WriteParam(Param::RELAY_ID, (uint8_t *)&relay_val, 1);
  _param.WriteParam(Param::WIFI_SSID_ID, (uint8_t *)"12345678", 8);

  static uint8_t val;
  static uint8_t buf[Param::PARAM_BUFSIZE]; // TODO if it no static, it will cause stack overflow
  _param.ReadParam(Param::RELAY_ID, &val);
  _param.ReadParam(Param::WIFI_SSID_ID, buf);

  logf("relay=%d\n", _param.relay);
  logf("wifi_ssid=%s\n", _param.wifi_ssid);

  // TEST_ASSERT_EQUAL(2, val);
  // TEST_ASSERT_EQUAL_STRING("12345678", buf);
}

void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  UNITY_BEGIN(); // IMPORTANT LINE!

  delay(2000);

  Serial.begin(115200);
  tpf("system begin\n");
  _param.Init();
}

void loop()
{
  // RUN_TEST(test_load_param);
  RUN_TEST(test_refresh_param);

  UNITY_END(); // stop unit testinu
}
// test_tm1620.cpp
#endif
