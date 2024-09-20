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
#include "net_man.hpp"
/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */

/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */
void test_wifi_conn();
/* variable definition ----------------------------------------------------------------------------------------------------- */
NetClient net;
Param _param;
/* function implementation ------------------------------------------------------------------------------------------------- */

void test_wifi_conn()
{
  _param.LoadAllParam();

  _param.WriteParam(Param::WIFI_SSID_ID, (uint8_t *)"88888888", 8);
  _param.WriteParam(Param::WIFI_PWD_ID, (uint8_t *)"88888888", 8);
  _param.LoadAllParam();

  tpf("wifi_ssid=%s\n", _param.wifi_ssid);
  tpf("wifi_pwd=%s\n", _param.wifi_pwd);

  net.ConnWifi((char *)_param.wifi_ssid, (char *)_param.wifi_pwd);
  uint8_t state = net.GetWifiConn();
  tpf("state=%d\n", state);

  delay(2000);
  TEST_ASSERT_EQUAL(WL_CONNECTED, WiFi.status());
}

void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  UNITY_BEGIN(); // IMPORTANT LINE!

  delay(2000);

  Serial.begin(115200);
  tpf("system begin\n");
}

void loop()
{
  RUN_TEST(test_wifi_conn);

  UNITY_END(); // stop unit testinu
}
// test_tm1620.cpp
#endif