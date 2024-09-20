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
void test_mqtt_conn();
void _MQTTRecv_cb(char *topic, uint8_t *payload, uint16_t length);
void test_mqtt_loop();
void test_reconn();
/* variable definition ----------------------------------------------------------------------------------------------------- */
NetClient net;
Param _param;
/* function implementation ------------------------------------------------------------------------------------------------- */

void test_mqtt_conn()
{
  struct
  {
    uint8_t stat;
  } conn_fsm;
  conn_fsm.stat = 0;

  if (conn_fsm.stat == 0)
  { // get param
    // _param.LoadAllParam();

    // _param.WriteParam(Param::WIFI_SSID_ID, (uint8_t *)"88888888", 8);
    // _param.WriteParam(Param::WIFI_PWD_ID, (uint8_t *)"88888888", 8);
    // _param.LoadAllParam();
    // tpf("wifi_ssid=%s\n", _param.wifi_ssid);
    // tpf("wifi_pwd=%s\n", _param.wifi_pwd);

    conn_fsm.stat = 1;
  }
  if (conn_fsm.stat == 1)
  { // connect wifi
    net.Init(_MQTTRecv_cb);
    net.ConnWifi((char *)"88888888", (char *)"88888888");
    uint8_t state = net.GetWifiConn();
    tpf("state=%d\n", state);

    if (state == 1)
    {
      tpf("wifi conn is successful\n");
      conn_fsm.stat = 2;
    }
    else
    {
      tpf("wifi conn is failed\n");
    }
  }
  if (conn_fsm.stat == 2)
  { // connect mqtt
    uint8_t state = net.ConnServer((char *)"ee836835.ala.cn-hangzhou.emqxsl.cn", 8883, (char *)"mqttx_2aaeb33d1", (char *)"dianzha202451", (char *)"zhangguile");
    tpf("state=%d\n", state);
    if (state == 1)
    {
      tpf("mqtt conn is successful\n");
      conn_fsm.stat = 3;
    }
    else
    {
      tpf("mqtt conn is failed\n");
    }
  }
  if (conn_fsm.stat == 3)
  {
    net.Subscribe((char *)"test");
  }
}

void test_mqtt_loop()
{
  net.Loop();
  // logf(".");
  delayMicroseconds(500);
}

void test_reconn()
{
  struct
  {
    uint8_t stat;
  } conn_fsm;
  conn_fsm.stat = 0;

  // uint8_t wifi_stat = net.GetWifiConn();
  // static uint8_t mqtt_stat = net.GetMqttConn();

  // tpf("wifi_stat=%d, mqtt_stat=%d\n", wifi_stat, mqtt_stat);
  // tpf("wifi_stat=%d\n", wifi_stat);
  // tpf("mqtt_stat=%d\n", mqtt_stat);

  tp();

#if 0
  if (wifi_stat == false || mqtt_stat == false)
  {
    if (conn_fsm.stat == 0)
    { // get param
      _param.LoadAllParam();

      _param.WriteParam(Param::WIFI_SSID_ID, (uint8_t *)"88888888", 8);
      _param.WriteParam(Param::WIFI_PWD_ID, (uint8_t *)"88888888", 8);
      _param.LoadAllParam();
      tpf("wifi_ssid=%s\n", _param.wifi_ssid);
      tpf("wifi_pwd=%s\n", _param.wifi_pwd);

      conn_fsm.stat = 1;
    }
    if (conn_fsm.stat == 1)
    { // connect wifi
      net.Init(_MQTTRecv_cb);
      net.ConnWifi((char *)_param.wifi_ssid, (char *)_param.wifi_pwd);
      uint8_t state = net.GetWifiConn();
      tpf("state=%d\n", state);

      if (state == 1)
      {
        tpf("wifi conn is successful\n");
        conn_fsm.stat = 2;
      }
      else
      {
        tpf("wifi conn is failed\n");
      }
    }
    if (conn_fsm.stat == 2)
    { // connect mqtt
      uint8_t state = net.ConnServer((char *)"ee836835.ala.cn-hangzhou.emqxsl.cn", 8883, (char *)"mqttx_2aaeb33d", (char *)"dianzha202451", (char *)"zhangguile");
      tpf("state=%d\n", state);
      if (state == 1)
      {
        tpf("mqtt conn is successful\n");
        conn_fsm.stat = 3;
      }
      else
      {
        tpf("mqtt conn is failed\n");
      }
    }
    if (conn_fsm.stat == 3)
    {
      net.Subscribe((char *)"test");
    }
  }
#endif
}

void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  UNITY_BEGIN(); // IMPORTANT LINE!

  delay(2000);

  Serial.begin(115200);
  tpf("system begin\n");
  net.Init(_MQTTRecv_cb);
  // RUN_TEST(test_mqtt_conn);
  test_mqtt_conn();
  // test_mqtt_conn();
}

void loop()
{
  static uint8_t cnt = 0;

  RUN_TEST(test_mqtt_loop);
  // RUN_TEST(test_reconn);

  // iConnWifi)
  // {
  //   UNITY_END(); // stop unit testinu
  // }
  // UNITY_END(); // stop unit testinu
}

void _MQTTRecv_cb(char *topic, uint8_t *payload, uint16_t length)
{
  tpf("topic=%s\n", topic);
  tpf("payload=%s\n", payload);
}
#endif
// test_tm1620.cpp