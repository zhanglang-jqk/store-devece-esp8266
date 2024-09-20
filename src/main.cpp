#if 1
/***********************************************************************
 * @file 	:	main.cpp
 * @author	:	CH
 * @brief	:
 * @Copyright (C)  2024-01-30  .cdWFVCEL. all right reserved
 ***********************************************************************/

/* include ------------------------------------------------------------------------------------------------- */
// #include "main.h"
#include "include.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>

#include "digit_out.h"
#include "led.h"
#include "key.hpp"
#include "debug.h"
#include "net_man.hpp"
#include "param.hpp"

#include "debug.h"
#include "cmd_pkt.h"
#include "TTS.h"
#include "tm1620.h"
#include "bonezegei_softserial.h"
/* macro definition ------------------------------------------------------------------------------------------------- */

#define SOFT_VERSION "1.2"

#define LED_RELAY_STAT 12
#define LED_SIGNAL 13
#define ON_LED_RELAY 0
#define OFF_LED_RELAY 1
#define ON_LED_SIGNAL 0
#define OFF_LED_SIGNAL 1

#define RELAY_GPIO 16
#define KEY_GPIO 2

#define CLK_PIN 14
#define DIO_PIN 13
#define STB_PIN 12

#define TTS_PIN_RX 5
#define TTS_PIN_TX 4
/* type definition ------------------------------------------------------------------------------------------------- */

/* variable declaration ------------------------------------------------------------------------------------------------- */
// LED led;
DOut relay;
Key key;
Ticker ticker;
NetClient net_client;
Param param;
CmdPkt cmd_pkt;
// SoftwareSerial softserial(TTS_PIN_RX, TTS_PIN_TX);
Bonezegei_SoftSerial softserial(TTS_PIN_RX, TTS_PIN_TX);

TTS tts;
TM1620 disp;

uint8_t g_tmpbuf[1024];

struct
{
  enum
  {
    RETRY_WIFI_CONN_CNT = 3,   // WIFI 连接重试次数
    RETRY_MQTT_CONN_CNT = 3,   // MQTT 连接重试次数
    RETRY_MQTT_SUBPUB_CNT = 3, // MQTT 订阅发布重试次数

    LOOP_PERIOD_MS = 50, // 主循环周期

    INIT_STAT,
    CONN_WIFI_STAT,
    CONN_MQTT_SVR_STAT,
    SYS_RESTART_STAT,
    APP_LOOP_STAT,
    MQTT_SUBSCRIPT_STAT,
    WIFI_INFO_CONFIG_STAT,
  };

  uint8_t retry_wifi_cnt = 0;
  uint8_t retry_mqtt_cnt = 0;
  uint8_t retry_mqtt_subpub_cnt = 0;
  uint8_t stat = INIT_STAT;

  uint32_t last_ms_st = 0; // 上次时间戳

  struct
  {
    uint8_t stat;
    uint8_t LOOP_PERIOD_MS = 10; // 10MS
    uint8_t RETRY_TTS_CNT = 3;   // 重试次数
    uint32_t period_last_ms_st;
    // uint32_t wait_response_last_ms; // 上次时间戳
    uint8_t playbuf[32];
    uint16_t playbuf_len;
    bool play_cmd_f;
    uint8_t retry_cnt;
  } play_voice_fsm;

  struct
  {
    bool count_down_start_f;
    uint8_t stat;
    uint32_t count_down_inc_sec;
    // uint32_t count_down_inc_minute;
    uint32_t last_ms_st;
  } count_down_fsm;

  struct
  {
    uint8_t stat;
    uint16_t LOOP_PERIOD_MS = 1000;
    uint32_t last_ms_st;
  } param_fsm;

  struct
  {
    uint8_t stat;
    uint8_t LOOP_PERIOD_MS = 10; // 10MS
    uint32_t last_ms_st;
    bool ap_en; // 0:关闭AP, 1:打开AP
    bool config_finish_f;
  } web_server_fsm;

  struct
  {
    uint8_t stat;
    uint8_t LOOP_PERIOD_MS = 10; // 10MS
    uint32_t last_ms_st;
  } key_fsm;

  struct
  {
    uint8_t stat;
    String ssid, pwd;
    bool cfg_finish_f;
  } wifi_cfg_fsm;

  struct
  {
    // uint8_t stat;
    uint32_t last_ms;
    uint8_t retry_cnt;
    const uint16_t LOOP_PERIOD_MS = 1000;
    const uint8_t MAX_RETRY_CNT = 3;
  } conn_mqtt_svr_fsm;

} ctrl_fsm;
/* function declaration ------------------------------------------------------------------------------------------------- */
void Key_cb(Key::KeyEvent_et key_event);
void MQTTRecv_cb(char *topic, uint8_t *payload, uint16_t length);
void WebServer_cb();

/* function implementation ------------------------------------------------------------------------------------------------- */
void setup()
{
  Serial.begin(115200);
  tpf("Power on..............");

  // led.Init(LED_SIGNAL, OUTPUT, DOut::ON_POLARITY_LOW);
  relay.Init(RELAY_GPIO, DOut::ON_POLARITY_HIGH);
  key.Init(KEY_GPIO, INPUT_PULLUP, Key_cb, Key::KEY_LOW);

  softserial.begin(9600); // TODO 打开这个，就会导致系统无法运行
  tts.Init(&softserial);
  disp.Init(STB_PIN, CLK_PIN, DIO_PIN);
  param.Init();
  net_client.Init(MQTTRecv_cb);
}

void loop()
{
  if (millis() - ctrl_fsm.last_ms_st > ctrl_fsm.LOOP_PERIOD_MS)
  {
    // tp();

    if (ctrl_fsm.stat == ctrl_fsm.INIT_STAT) // 根据已保存的继电器状态控制继电器
    {
      // tp();
      tpf("free stack:%d, free heap:%d", ESP.getFreeContStack(), ESP.getFreeHeap());
      // param.ResetAllParamToDefault();
      param.LoadAllParam();
      tpt(param.ToString(&param.cur_data));

      relay.Set(param.cur_data.relay);

      ctrl_fsm.stat = ctrl_fsm.CONN_WIFI_STAT;
    }
#if 1
    else if (ctrl_fsm.stat == ctrl_fsm.CONN_WIFI_STAT) // connect to Wifi
    {
      // tp();

      NetClient::WiFiConnResult_et wifi_conn_result = (NetClient::WiFiConnResult_et)net_client.ConnWifi((char *)param.cur_data.wifi_ssid, (char *)param.cur_data.wifi_pwd);
      if (wifi_conn_result == NetClient::WIFI_CONNECED)
      {
        tpf("connect to wifi successful");

        ctrl_fsm.retry_wifi_cnt = 0;
        ctrl_fsm.conn_mqtt_svr_fsm.retry_cnt = 0;
        ctrl_fsm.stat = ctrl_fsm.CONN_MQTT_SVR_STAT;
        ctrl_fsm.conn_mqtt_svr_fsm.last_ms = millis();
      }
      else if (wifi_conn_result == NetClient::WIFI_CONNECT_TIMEOUT)
      {
        if (++ctrl_fsm.retry_wifi_cnt > ctrl_fsm.RETRY_WIFI_CONN_CNT)
        {
          ctrl_fsm.stat = ctrl_fsm.SYS_RESTART_STAT;
          tpf("try connect wifi is failed so then restart system");
        }
        tpf("The %d times, try conenct to wifi is failed", ctrl_fsm.retry_wifi_cnt);
      }
    }
    else if (ctrl_fsm.stat == ctrl_fsm.CONN_MQTT_SVR_STAT) // connect to MQTT server
    {
      if (millis() - ctrl_fsm.conn_mqtt_svr_fsm.last_ms >= ctrl_fsm.conn_mqtt_svr_fsm.LOOP_PERIOD_MS)
      {
        tpf("connect mqtt server, server:%s, port:%d, id:%s, username:%s, password:%s", param.cur_data.mqtt_server, param.cur_data.mqtt_port, param.cur_data.device_id,
            param.cur_data.mqtt_username, param.cur_data.mqtt_password);

        bool isok = net_client.ConnServer((char *)param.cur_data.mqtt_server, param.cur_data.mqtt_port, (char *)param.cur_data.device_id, (char *)param.cur_data.mqtt_username,
                                          (char *)param.cur_data.mqtt_password);
        if (isok)
        {
          tpf("connect to mqtt server successful");

          ctrl_fsm.conn_mqtt_svr_fsm.retry_cnt = 0;
          ctrl_fsm.stat = ctrl_fsm.MQTT_SUBSCRIPT_STAT;
        }
        else
        {
          if (++ctrl_fsm.conn_mqtt_svr_fsm.retry_cnt >= ctrl_fsm.conn_mqtt_svr_fsm.MAX_RETRY_CNT)
          {
            ctrl_fsm.stat = ctrl_fsm.CONN_WIFI_STAT;

            tpf("try connect mqtt server is failed so then reconnect wifi");
          }
          tpf("The %d times, try conenct to mqtt server is failed", ctrl_fsm.conn_mqtt_svr_fsm.retry_cnt);
        }

        ctrl_fsm.conn_mqtt_svr_fsm.last_ms = millis();
      }
    }
    else if (ctrl_fsm.stat == ctrl_fsm.MQTT_SUBSCRIPT_STAT)
    {
      tp();
      char *pub_topic = (char *)g_tmpbuf, *device_sub_topic = (char *)g_tmpbuf, *device_pub_topic = (char *)g_tmpbuf;
      pub_topic = (char *)param.cur_data.pub_topic;
      sprintf((char *)device_sub_topic, "%s/%s", param.cur_data.device_id, param.cur_data.device_pub_topic);
      sprintf((char *)device_pub_topic, "%s/%s", param.cur_data.device_id, param.cur_data.device_sub_topic);

      bool isok1 = net_client.Subscribe(pub_topic);
      tpf("subscribe topic is:%s", (char *)pub_topic);
      bool isok2 = net_client.Subscribe(device_pub_topic);
      tpf("subscribe topic is:%s", (char *)device_pub_topic);
      bool isok3 = net_client.Subscribe(device_sub_topic);
      tpf("subscribe topic is:%s", (char *)device_sub_topic);

      /* publish relay state to device_pub_topic  -------------------- */
      memset(g_tmpbuf, 0, sizeof(g_tmpbuf));
      if (relay.GetPinState() == DOut::ON_STATUS)
        sprintf((char *)g_tmpbuf, "{\"device_id\":\"%s\",\"relay\":\"on\"}", param.cur_data.device_id);
      else
        sprintf((char *)g_tmpbuf, "{\"device_id\":\"%s\",\"relay\":\"off\"}", param.cur_data.device_id);
      bool isok4 = net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)g_tmpbuf);

      if (isok1 == false || isok2 == false || isok3 == false || isok4 == false)
      {
        tpf("subscribe is failed");
        if (++ctrl_fsm.retry_mqtt_subpub_cnt > ctrl_fsm.RETRY_MQTT_SUBPUB_CNT)
        {
          net_client.Disconnect();
          ctrl_fsm.stat = ctrl_fsm.RETRY_MQTT_CONN_CNT;
          tpf("retry subscribe is failed,then try reconnect mqtt");
        }
      }
      else
      {
        tpf("subscribe all topics is successful");
        ctrl_fsm.retry_mqtt_subpub_cnt = 0;
        ctrl_fsm.stat = ctrl_fsm.APP_LOOP_STAT;
      }
    }
    else if (ctrl_fsm.stat == ctrl_fsm.SYS_RESTART_STAT)
    {
      tp();
      ESP.restart();
    }
    else if (ctrl_fsm.stat == ctrl_fsm.WIFI_INFO_CONFIG_STAT)
    {
      if (ctrl_fsm.wifi_cfg_fsm.stat == 0)
      {
        tpf("wait for receive wifi info");
        ctrl_fsm.wifi_cfg_fsm.stat = 1;
        // server.send(200, "text/plain", "Received SSID and Password");  // wait for verify that annotation this line whether ok
      }
      else if (ctrl_fsm.wifi_cfg_fsm.stat == 1) // valid wifi info
      {
        logf(".");
        if (ctrl_fsm.web_server_fsm.config_finish_f)
        {
          ln();
          tpf("config wifi infomation is finished");
          net_client.WebServer_stop();
          ctrl_fsm.web_server_fsm.ap_en = false;
          ctrl_fsm.wifi_cfg_fsm.stat = 0;
          ctrl_fsm.stat = ctrl_fsm.CONN_WIFI_STAT;
        }
      }
    }
    else if (ctrl_fsm.stat == ctrl_fsm.APP_LOOP_STAT)
    {
      // tp();
      if (net_client.GetWifiConn() == false)
      {
        tpf("wifi is disconnected, then reconnect to wifi");
        ctrl_fsm.stat = ctrl_fsm.CONN_WIFI_STAT;
      }
      else if (net_client.GetMqttConn() == false)
      {
        tpf("mqtt is disconnected, then reconnect to mqtt server");
        ctrl_fsm.stat = ctrl_fsm.CONN_MQTT_SVR_STAT;
      }

      net_client.Loop();
    }
#endif
    ctrl_fsm.last_ms_st = millis();
  }

#if 0
  if (millis() - ctrl_fsm.play_voice_fsm.period_last_ms_st > ctrl_fsm.play_voice_fsm.LOOP_PERIOD_MS) // paly voice logic
  {
    if (ctrl_fsm.play_voice_fsm.play_cmd_f)
    {
      if (ctrl_fsm.play_voice_fsm.stat == 0) // Play voice cmd
      {
        tpf("try to play voice cmd to TTS modules, the voice is %s, try times is %d", ctrl_fsm.play_voice_fsm.playbuf, ctrl_fsm.play_voice_fsm.retry_cnt);

        bool isok = tts.SendPlayVoice_cmd((uint8_t *)ctrl_fsm.play_voice_fsm.playbuf, ctrl_fsm.play_voice_fsm.playbuf_len);

        if (isok)
          ctrl_fsm.play_voice_fsm.stat = 1;
        else
          ctrl_fsm.play_voice_fsm.stat = 2;
      }
      else if (ctrl_fsm.play_voice_fsm.stat == 1) // play voice cmd is successful
      {
        tpf("play voice is successful");

        uint8_t sbuf[32];
        memset(sbuf, 0, sizeof(sbuf));
        cmd_pkt.BuildJsonPkt((char *)sbuf, CmdPkt::PLAY_VOICE, (char *)"successful");
        net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)sbuf);

        ctrl_fsm.play_voice_fsm.play_cmd_f = false;
        ctrl_fsm.play_voice_fsm.stat = 0;
        ctrl_fsm.play_voice_fsm.retry_cnt = 0;
      }
      else if (ctrl_fsm.play_voice_fsm.stat == 2) // play voice cmd is failed
      {
        tpf("retry paly voice cmd to TTS modules again");

        ctrl_fsm.play_voice_fsm.retry_cnt++;
        if (ctrl_fsm.play_voice_fsm.retry_cnt > ctrl_fsm.play_voice_fsm.RETRY_TTS_CNT)
        {
          tpf("retry play voice is failed that try %d times and then publish message to MQTT server", ctrl_fsm.play_voice_fsm.RETRY_TTS_CNT);
          uint8_t sbuf[32];
          memset(sbuf, 0, sizeof(sbuf));
          cmd_pkt.BuildJsonPkt((char *)sbuf, CmdPkt::PLAY_VOICE, (char *)"failed");
          net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)sbuf);

          ctrl_fsm.play_voice_fsm.play_cmd_f = false;
          ctrl_fsm.play_voice_fsm.stat = 0;
          ctrl_fsm.play_voice_fsm.retry_cnt = 0;
        }
        else
        {
          tpf("paly voice cmd again");
          ctrl_fsm.play_voice_fsm.stat = 0;
        }
      }
    }

    ctrl_fsm.play_voice_fsm.period_last_ms_st = millis();
  }
#endif
#if 0
  { // count down logic
    if (ctrl_fsm.count_down_fsm.stat == 0)
    {
      if (ctrl_fsm.count_down_fsm.count_down_start_f)
      {

        uint8_t hour = param.cur_data.count_down_minute / 60;
        uint8_t hour_10 = hour / 10 % 10, hour_1 = hour % 10;
        uint8_t minute = param.cur_data.count_down_minute % 60;
        uint8_t minute_10 = minute / 10 % 10, minute_1 = minute % 10;

        tpf("count down minute is %d", param.cur_data.count_down_minute);
        tpf("hour_10:%d, hour_1:%d, minute_10:%d, minute_1:%d", hour_10, hour_1, minute_10, minute_1);
        uint8_t valid_num = floor(log10(param.cur_data.count_down_minute) + 1); // note: log10() input value must be greater than 0
        for (int i = 0; i < 4 - valid_num; i++)
        {
          disp.DispSeg(i, TM1620::SEG_NOT_DISPLAY);
        }
        uint8_t skip_num = 4 - valid_num;
        switch (skip_num)
        {
        case 0:
          disp.DispSeg(0, hour_10);
        case 1:
          disp.DispSeg(1, hour_1);
        case 2:
          disp.DispSeg(2, minute_10);
        case 3:
          disp.DispSeg(3, minute_1);
        }

        ctrl_fsm.count_down_fsm.stat = 1;
        ctrl_fsm.count_down_fsm.last_ms_st = millis();
      }
    }
    else if (ctrl_fsm.count_down_fsm.stat == 1)
    {
      if (ctrl_fsm.count_down_fsm.count_down_start_f)
      {
        if (millis() - ctrl_fsm.count_down_fsm.last_ms_st >= 60 * 1000) // 1 minute
        {
          if (param.cur_data.count_down_minute == 0)
          {
            ctrl_fsm.count_down_fsm.count_down_start_f = false;
            ctrl_fsm.count_down_fsm.stat = 0;
          }
          else
          {
            param.cur_data.count_down_minute--;

            uint8_t hour = param.cur_data.count_down_minute / 60;
            uint8_t hour_10 = hour / 10 % 10, hour_1 = hour % 10;
            uint8_t minute = param.cur_data.count_down_minute % 60;
            uint8_t minute_10 = minute / 10 % 10, minute_1 = minute % 10;

            tpf("count down minute is %d", param.cur_data.count_down_minute);
            tpf("hour_10:%d, hour_1:%d, minute_10:%d, minute_1:%d", hour_10, hour_1, minute_10, minute_1);
            uint8_t valid_num = floor(log10(param.cur_data.count_down_minute) + 1); // note: log10() input value must be greater than 0
            for (int i = 0; i < 4 - valid_num; i++)
            {
              disp.DispSeg(i, TM1620::SEG_NOT_DISPLAY);
            }
            uint8_t skip_num = 4 - valid_num;
            switch (skip_num)
            {
            case 0:
              disp.DispSeg(0, hour_10);
            case 1:
              disp.DispSeg(1, hour_1);
            case 2:
              disp.DispSeg(2, minute_10);
            case 3:
              disp.DispSeg(3, minute_1);
            }
          }
          ctrl_fsm.count_down_fsm.last_ms_st = millis();
        }
      }
      else
      {
        disp.DispSeg(0, TM1620::SEG_COLON), disp.DispSeg(1, TM1620::SEG_COLON), disp.DispSeg(2, TM1620::SEG_COLON), disp.DispSeg(3, TM1620::SEG_COLON);
        ctrl_fsm.count_down_fsm.stat = 0;
      }
    }
  }
#endif

  { // param logic
    if (ctrl_fsm.param_fsm.stat == 0)
    {
      ctrl_fsm.param_fsm.last_ms_st = millis();
      ctrl_fsm.param_fsm.stat = 1;
    }
    else if (ctrl_fsm.param_fsm.stat == 1)
    {
      if (millis() - ctrl_fsm.param_fsm.last_ms_st >= ctrl_fsm.param_fsm.LOOP_PERIOD_MS) // 1S
      {
        param.UpdateParam_pool();
      }
    }
  }
#if 1
  { // web server logic
    if (ctrl_fsm.web_server_fsm.stat == 0)
    {
      ctrl_fsm.web_server_fsm.last_ms_st = millis();
      ctrl_fsm.web_server_fsm.stat = 1;
    }
    else if (ctrl_fsm.web_server_fsm.stat == 1)
    {
      if (millis() - ctrl_fsm.web_server_fsm.last_ms_st >= ctrl_fsm.web_server_fsm.LOOP_PERIOD_MS) // 10ms
      {
        if (ctrl_fsm.web_server_fsm.ap_en) net_client.WebServer_pool();
      }
    }
  }
#endif

#if 1
  { // key logic
    if (ctrl_fsm.key_fsm.stat == 0)
    {
      // tp();
      ctrl_fsm.key_fsm.last_ms_st = millis();
      ctrl_fsm.key_fsm.stat = 1;
    }
    else if (ctrl_fsm.key_fsm.stat == 1)
    {
      if (millis() - ctrl_fsm.key_fsm.last_ms_st >= ctrl_fsm.key_fsm.LOOP_PERIOD_MS)
      {
        // tp();
        key.pool();
        ctrl_fsm.key_fsm.last_ms_st = millis();
      }
    }
  }
#endif
}

void Key_cb(Key::KeyEvent_et key_event)
{
  if (key_event == Key::KeyEvent_et::CLICK)
  {
    tpf("key short press");
    bool state = relay.Toggle();
    tpf("relay state is %d", state);
    param.cur_data.relay = (uint8_t)state;
  }
  else if (key_event == Key::KeyEvent_et::LONG_CLICK) // wifi config by hot point
  {
    tpf("key long press");
    net_client.WebServer_start((char *)param.cur_data.ap_ssid, (char *)param.cur_data.ap_pwd, WebServer_cb);
    ctrl_fsm.web_server_fsm.ap_en = true;
    ctrl_fsm.stat = ctrl_fsm.WIFI_INFO_CONFIG_STAT;
  }
  else if (key_event == Key::KeyEvent_et::DOUBLE_CLICK)
  {
    tpf("key double press");
  }
}

void WebServer_cb() // 提交配置信息时,调用此函数
{
  tp();
  ctrl_fsm.wifi_cfg_fsm.ssid = net_client.GetArg((char *)"ssid");
  ctrl_fsm.wifi_cfg_fsm.pwd = net_client.GetArg("password");
  Serial.println("Received SSID: " + ctrl_fsm.wifi_cfg_fsm.ssid);
  Serial.println("Received Password: " + ctrl_fsm.wifi_cfg_fsm.pwd);
  strcpy((char *)param.cur_data.wifi_ssid, ctrl_fsm.wifi_cfg_fsm.ssid.c_str());
  strcpy((char *)param.cur_data.wifi_pwd, ctrl_fsm.wifi_cfg_fsm.pwd.c_str());

  if (ctrl_fsm.wifi_cfg_fsm.ssid.length() != 0 && ctrl_fsm.wifi_cfg_fsm.pwd.length() != 0)
  {
    ctrl_fsm.web_server_fsm.config_finish_f = true;
  }
}

/**
 * @brief Callback function for receiving MQTT messages.
 *
 * This function is called when an MQTT message is received.
 * It parses the received message and performs corresponding actions based on the command.
 *
 * @param topic The topic of the received message.
 * @param payload The payload of the received message.
 * @param length The length of the payload.
//  */
void MQTTRecv_cb(char *topic, uint8_t *payload, uint16_t length)
{
#if 1
  char _payload[length + 1];
  memset(_payload, 0, sizeof(_payload)), memcpy(_payload, payload, length);
  tpf("MQTTRecv_cb, topic:%s, payload:%s", topic, _payload);

  static char device_sub_topic[64];
  memset(device_sub_topic, 0, sizeof(device_sub_topic));
  sprintf(device_sub_topic, "%s/%s", param.cur_data.device_id, param.cur_data.device_sub_topic);

  if (strcmp(topic, device_sub_topic) == 0)
  {
    char cmd_no[5];
    memset(cmd_no, 0, sizeof(cmd_no));
    cmd_pkt.GetCmdNo(cmd_no, payload, length);
    tpf("cmd_no:%s", cmd_no);

    if (strcmp(cmd_no, CmdPkt::MODIFY_MQTT_SERVER_CMD) == 0)
    {
      cmd_pkt.ExtractStrOfBuf((char *)payload, length, (char *)param.cur_data.mqtt_server);
      // param.WriteParam(Param::MQTT_SERVER_ID, param.data.mqtt_server, strlen((char *)param.data.mqtt_server));
    }
    else if (strcmp(cmd_no, CmdPkt::MODIFY_WIFI_SSID_CMD) == 0)
    {
      cmd_pkt.ExtractStrOfBuf((char *)payload, length, (char *)param.cur_data.wifi_ssid);
      // param.WriteParam(Param::WIFI_SSID_ID, param.data.wifi_ssid, strlen((char *)param.data.wifi_ssid));
    }
    else if (strcmp(cmd_no, CmdPkt::MODIFY_WIFI_PWD_CMD) == 0)
    {
      cmd_pkt.ExtractStrOfBuf((char *)payload, length, (char *)param.cur_data.wifi_pwd);
      // param.WriteParam(Param::WIFI_PWD_ID, param.data.wifi_pwd, strlen((char *)param.data.wifi_pwd));
    }
    else if (strcmp(cmd_no, CmdPkt::MODIFY_MQTT_USERNAME_CMD) == 0)
    {
      cmd_pkt.ExtractStrOfBuf((char *)payload, length, (char *)param.cur_data.mqtt_username);
      // param.WriteParam(Param::MQTT_USERNAME_ID, param.data.mqtt_username, strlen((char *)param.data.mqtt_username));
    }
    else if (strcmp(cmd_no, CmdPkt::MODIFY_MQTT_PASSWORD_CMD) == 0)
    {
      cmd_pkt.ExtractStrOfBuf((char *)payload, length, (char *)param.cur_data.mqtt_password);
      // param.WriteParam(Param::MQTT_PASSWORD_ID, param.data.mqtt_password, strlen((char *)param.data.mqtt_password));
    }
    else if (strcmp(cmd_no, CmdPkt::OPEN_RELAY_CMD) == 0)
    {
      param.cur_data.relay = (uint8_t)relay.On();
      tpf("received cmd, open relay");
    }
    else if (strcmp(cmd_no, CmdPkt::CLOSE_RELAY_CMD) == 0)
    {
      param.cur_data.relay = (uint8_t)relay.Off();
      tpf("received cmd, close relay");
    }
    else if (strcmp(cmd_no, CmdPkt::GET_RELAY_STATUS_CMD) == 0)
    {
      char sbuf[32];
      memset(sbuf, 0, sizeof(sbuf));
      if (relay.GetPinState() == DOut::ON_STATUS)
        cmd_pkt.BuildJsonPkt(sbuf, CmdPkt::RELAY_STATUS_CMD, (char *)"on");
      else
        cmd_pkt.BuildJsonPkt(sbuf, CmdPkt::RELAY_STATUS_CMD, (char *)"off");

      net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)sbuf);
    }
    else if (strcmp(cmd_no, CmdPkt::GET_DEVICE_ID_CMD) == 0)
    {
      char sbuf[48];
      memset(sbuf, 0, sizeof(sbuf));
      cmd_pkt.BuildJsonPkt(sbuf, CmdPkt::GET_DEVICE_ID_CMD, (char *)param.cur_data.device_id);

      net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)sbuf);
    }
    else if (strcmp(cmd_no, CmdPkt::MODIFY_MQTT_PORT_CMD) == 0)
    {
      uint16_t port;
      cmd_pkt.ExtractIntOfBuf((char *)payload, length, &port);
      param.cur_data.mqtt_port = port;
    }
    else if (strcmp(cmd_no, CmdPkt::PLAY_VOICE) == 0)
    {
      ctrl_fsm.play_voice_fsm.play_cmd_f = true;
      // cmd_pkt.ExtractStrOfBuf((char *)payload, length, (char *)ctrl_fsm.play_voice_fsm.playbuf);
      ctrl_fsm.play_voice_fsm.playbuf_len = cmd_pkt.ExtractGbkHexStrOfBuf((char *)payload, length, ctrl_fsm.play_voice_fsm.playbuf);
    }
    else if (strcmp(cmd_no, CmdPkt::SET_COUNT_DOWN) == 0)
    {
      uint16_t count_down = 0;
      cmd_pkt.ExtractIntOfBuf((char *)payload, length, &count_down);
      param.cur_data.count_down_minute = count_down;
      tpf("cound down: %d", count_down);
    }
    else if (strcmp(cmd_no, CmdPkt::OPEN_COUNT_DOWN) == 0)
    {
      tpf("open count down");
      ctrl_fsm.count_down_fsm.count_down_start_f = true;
    }
    else if (strcmp(cmd_no, CmdPkt::CLOSE_COUNT_DOWN) == 0)
    {
      tpf("close count down");
      ctrl_fsm.count_down_fsm.count_down_start_f = false;
    }
    else
    {
      tpf("unknow cmd");
    }
  }

#endif
}
#endif

// main.cpp
