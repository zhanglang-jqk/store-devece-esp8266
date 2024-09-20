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
#include <EEPROM.h>
/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */

/* variable declaration ---------------------------------------------------------------------------------------------------- */
Param _param;
/* function declaration ---------------------------------------------------------------------------------------------------- */
/* variable definition ----------------------------------------------------------------------------------------------------- */
void test_writeparam_readparam_int();
void test_writeparam_readparam_str();
void test_resetparamtodefault();
void test_reset_all_param_to_default();
void test_first_power_on();
void test_param_pool();
void test_all_param();
/* function implementation ------------------------------------------------------------------------------------------------- */

void test_writeparam_readparam_int()
{
  uint8_t relay_st = 100;
  _param.WriteParam(Param::RELAY_ID, relay_st);
  relay_st = 0;
  _param.ReadParam(Param::RELAY_ID, &relay_st);

  tpf("relay_st:%d", relay_st);
}

void test_writeparam_readparam_str()
{
  char *str = "hello";
  _param.WriteParam(Param::WIFI_SSID_ID, str, strlen(str) + 1);
  char strbuf[Param::PARAM_BUFSIZE];
  memset(strbuf, 0, sizeof(strbuf));
  _param.ReadParam(Param::WIFI_SSID_ID, (char *)strbuf, sizeof(strbuf));

  tpf("strbuf:%s", strbuf);
}

void test_resetparamtodefault()
{
  char *str = "hello";
  _param.WriteParam(Param::WIFI_SSID_ID, str, strlen(str) + 1);
  char strbuf[Param::PARAM_BUFSIZE];
  memset(strbuf, 0, sizeof(strbuf));
  _param.ReadParam(Param::WIFI_SSID_ID, (char *)strbuf, sizeof(strbuf));

  tpf("strbuf:%s", strbuf);

  _param.ResetParamToDefault(Param::WIFI_SSID_ID);
  memset(strbuf, 0, sizeof(strbuf));
  _param.ReadParam(Param::WIFI_SSID_ID, (char *)strbuf, sizeof(strbuf));
  tpf("strbuf:%s", strbuf);
}

void test_reset_all_param_to_default()
{
  char *str = "hello";
  _param.WriteParam(Param::WIFI_SSID_ID, str, strlen(str) + 1);
  char strbuf[Param::PARAM_BUFSIZE];
  memset(strbuf, 0, sizeof(strbuf));
  _param.ReadParam(Param::WIFI_SSID_ID, (char *)strbuf, sizeof(strbuf));

  tpf("strbuf:%s", strbuf);

  _param.ResetAllParamToDefault();
  memset(strbuf, 0, sizeof(strbuf));
  _param.ReadParam(Param::WIFI_SSID_ID, (char *)strbuf, sizeof(strbuf));
  tpf("strbuf:%s", strbuf);
}

void test_first_power_on()
{
  for (int i = 0; i < ParamTable::EEPROM_SIZE; i++)
  {
    EEPROM.write(i, 0xf1);
  }
  EEPROM.commit();

  _param.LoadAllParam();
  // const char *tostr = _param.ToString().c_str();
  // tpf("tostr:%s", tostr);
  // Serial.println(_param.ToString());
  tpf("wifi_ssid:%s", _param.cur_data.wifi_ssid);
  tpt(_param.ToString());
}

void test_param_pool()
{
  static struct
  {
    uint8_t stat;
    uint32_t last_ms;
  } ctrl_fsm;

  // for (int i = 0; i < ParamTable::EEPROM_SIZE; i++)
  // {
  //   EEPROM.write(i, 0xf1);
  // }
  // EEPROM.commit();

  if (ctrl_fsm.stat == 0)
  {
    _param.LoadAllParam();
    tpt(_param.ToString());
    ctrl_fsm.last_ms = millis();
    ctrl_fsm.stat = 1;
  }
  else if (ctrl_fsm.stat == 1)
  {
    if (millis() - ctrl_fsm.last_ms > 5000)
    {
      tpf("manual modify param");
      _param.cur_data.relay = 0x55;
    }
  }
  _param.UpdateParam_pool();
}

void test_all_param()
{
  _param.LoadAllParam();
  tpt(_param.ToString());
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
  // RUN_TEST(test_writeparam_readparam_int);
  // RUN_TEST(test_writeparam_readparam_str);
  // RUN_TEST(test_resetparamtodefault);
  // RUN_TEST(test_reset_all_param_to_default);
  // RUN_TEST(test_first_power_on);

  RUN_TEST(test_param_pool);
  delay(1000);
  // RUN_TEST(test_all_param);
  UNITY_END(); // stop unit testinu
}

// test_tm1620.cpp
#endif
