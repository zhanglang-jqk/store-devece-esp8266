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
#include <DNSServer.h>

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

#define RELAY_GPIO 5
#define KEY_GPIO 2

#define CLK_PIN 14
#define DIO_PIN 13
#define STB_PIN 12

#define TTS_PIN_RX 16
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
DNSServer dnsServer;
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);

uint8_t g_tmpbuf[1024];

typedef struct
{
  // uint8_t topic[32];
  // uint8_t payload[32];
  bool send_f;
  // uint8_t u8_arg;
  // uint16_t u16_arg;
  // uint32_t u32_arg;
  // uint64_t u64_arg;
  // uint8_t *u8_ptr_arg;
  // uint16_t *u16_ptr_arg;
  // uint32_t *u32_ptr_arg;
  // uint64_t *u64_ptr_arg;
} PublicData_t;

struct
{
  enum
  {
    RETRY_WIFI_CONN_CNT = 3,   // WIFI 杩炴帴閲嶈瘯娆℃
    RETRY_MQTT_CONN_CNT = 3,   // MQTT 杩炴帴閲嶈瘯娆℃
    RETRY_MQTT_SUBPUB_CNT = 3, // MQTT 璁㈤槄鍙戝竷閲嶈瘯娆℃暟

    LOOP_PERIOD_MS = 50, // 涓诲惊鐜懆?

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

  uint32_t last_ms_st = 0; // 涓婃鏃堕棿?

  struct
  {
    uint8_t stat;
    uint8_t LOOP_PERIOD_MS = 10; // 10MS
    uint8_t RETRY_TTS_CNT = 3;   // 閲嶈瘯娆℃暟
    uint32_t period_last_ms_st;
    // uint32_t wait_response_last_ms; // 涓婃鏃堕棿?
    uint8_t playbuf[32];
    uint16_t playbuf_len;
    bool play_cmd_f;
    uint8_t retry_cnt;
  } PLAY_VOICE_CMD_fsm;

  struct
  {
    bool count_down_start_f;
    uint8_t stat;
    uint32_t count_down_inc_sec;
    // uint32_t count_down_inc_minute;
    uint32_t minute_last_ms_st;
    uint32_t sec_last_ms_st;
    uint8_t disp_colon_f;
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
    bool ap_en; // 0:鍏抽棴AP, 1:鎵撳紑AP
    bool config_finish_f;
  } web_server_fsm;

  struct
  {
    uint8_t stat;
    uint8_t LOOP_PERIOD_MS = 10; // 10MS
    uint32_t last_ms_st;
    // -------------------------------------------------------------------------------------------------
    uint8_t event_f; // 0:none 1:direct toggle door, 2:auto close door 3: wifi info config
    uint32_t event_last_ms_st;
    uint8_t event_stat;
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

  struct
  {
    uint8_t stat;
    uint32_t last_ms;
    const uint16_t LOOP_PERIOD_MS = 500;
    bool disp_f;
  } net_seg_indicate_fsm;

  struct
  {
    uint8_t stat;
    const uint16_t MAIN_SCAN_PERIOD_MS = 500;
    uint32_t main_last_ms;
    bool conn_svr_f;
    bool disp_val_f;
  } seg_disp_fsm;

  struct // TODO wait for implement
  {
    PublicData_t count_down_finish_event;
    PublicData_t relay_state_update_event;
    PublicData_t PLAY_VOICE_CMD_cmd_ok_event;
    PublicData_t PLAY_VOICE_CMD_cmd_err_event;
    PublicData_t conn_svr_baseinfo_event; // then connect to server, then report base info

    PublicData_t query_count_down_response;
    PublicData_t query_relay_state_response;
    PublicData_t query_device_no_response;
    PublicData_t ask_response;  // 纭畾缂?
    PublicData_t nask_response; // 鎷栨嫿缂?
    PublicData_t unknown_response;
    uint32_t last_ms;
    const uint16_t LOOP_PERIOD_MS = 1000;
    uint8_t stat;
  } public_data_fsm;

} ctrl_fsm;
/* function declaration ------------------------------------------------------------------------------------------------- */
void Key_cb(Key::KeyEvent_et key_event);
void MQTTRecv_cb(char *topic, uint8_t *payload, uint16_t length);
void WebServer_cb();
void setupCaptivePortal();
String toStringIp(IPAddress ip);
void handleCommand(uint8_t cmd_type, uint8_t *data, uint16_t len);

/* function implementation ------------------------------------------------------------------------------------------------- */
void setup()
{
  Serial.begin(115200);
  tpf("Power on..............");

  // led.Init(LED_SIGNAL, OUTPUT, DOut::ON_POLARITY_LOW);
  relay.Init(RELAY_GPIO, DOut::ON_POLARITY_HIGH);
  key.Init(KEY_GPIO, INPUT_PULLUP, Key_cb, Key::KEY_LOW);

  softserial.begin(9600); // TODO 鎵撳紑杩欎釜锛屽氨浼氬鑷寸郴缁熸棤娉曡繍琛?
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

    if (ctrl_fsm.stat == ctrl_fsm.INIT_STAT) // 鏍规嵁宸蹭繚瀛樼殑缁х數鍣ㄧ姸鎬佹帶鍒剁户鐢靛?
    {
      // tp();
      tpf("free stack:%d, free heap:%d", ESP.getFreeContStack(), ESP.getFreeHeap());
      param.ResetAllParamToDefault();
      param.LoadAllParam();
      tpt(param.ToString(&param.cur_data));

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

        // ctrl_fsm.public_data_fsm.conn_svr_baseinfo_event.send_f = true;

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
        tpf("connect mqtt server, server:%s, port:%d, id:%s, username:%s, password:%s", param.cur_data.mqtt_server, param.cur_data.mqtt_port, param.cur_data.device_id, param.cur_data.mqtt_username,
            param.cur_data.mqtt_password);

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
      // char *device_global_pub_topic = (char *)g_tmpbuf, *device_sub_topic = (char *)g_tmpbuf, *device_pub_topic = (char *)g_tmpbuf;

      // bool isok1 = net_client.Subscribe(param.cur_data.device_global_pub_topic);
      // tpf("subscribe topic is:%s", (char *)device_global_pub_topic);
      // bool isok2 = net_client.Subscribe(device_pub_topic);
      // tpf("subscribe topic is:%s", (char *)device_pub_topic);
      bool isok1 = net_client.Subscribe((char *)param.cur_data.device_sub_topic);
      bool isok2 = net_client.Subscribe((char *)param.cur_data.device_global_sub_topic);
      tpf("subscribe topic is:%s", (char *)param.cur_data.device_sub_topic);

      /* publish relay state to device_pub_topic  -------------------- */
      // memset(g_tmpbuf, 0, sizeof(g_tmpbuf));
      // if (relay.GetPinState() == DOut::ON_STATUS)
      //   sprintf((char *)g_tmpbuf, "{\"device_id\":\"%s\",\"relay\":\"on\"}", param.cur_data.device_id);
      // else
      //   sprintf((char *)g_tmpbuf, "{\"device_id\":\"%s\",\"relay\":\"off\"}", param.cur_data.device_id);
      // isok = net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)g_tmpbuf);
      ctrl_fsm.public_data_fsm.conn_svr_baseinfo_event.send_f = true;

      if (isok1 == false || isok2 == false)
      // if (isok1 == false)
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
        // param.cur_data.count_down_minute = 10;
        // ctrl_fsm.count_down_fsm.count_down_start_f = true;

        tpf("subscribe all topics is successful");
        ctrl_fsm.retry_mqtt_subpub_cnt = 0;
        ctrl_fsm.stat = ctrl_fsm.APP_LOOP_STAT;

        relay.Set(param.cur_data.relay);
      }
    }
    else if (ctrl_fsm.stat == ctrl_fsm.SYS_RESTART_STAT)
    {
      // tp();
      tpf("reset system for retry to run system");
      ESP.restart();
    }
    else if (ctrl_fsm.stat == ctrl_fsm.WIFI_INFO_CONFIG_STAT)
    {
      if (ctrl_fsm.wifi_cfg_fsm.stat == 0)
      {
        tpf("wait for receive wifi info");
        ctrl_fsm.wifi_cfg_fsm.stat = 1;
      }
      else if (ctrl_fsm.wifi_cfg_fsm.stat == 1) // valid wifi info
      {
        logf(".");
        if (ctrl_fsm.web_server_fsm.config_finish_f)
        {
          ln();
          tpf("config wifi infomation is finished");
          ctrl_fsm.wifi_cfg_fsm.stat = 0;
          ctrl_fsm.stat = ctrl_fsm.CONN_WIFI_STAT;
          ctrl_fsm.web_server_fsm.config_finish_f = false;
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

#if 1
  if (millis() - ctrl_fsm.PLAY_VOICE_CMD_fsm.period_last_ms_st > ctrl_fsm.PLAY_VOICE_CMD_fsm.LOOP_PERIOD_MS) // paly voice logic
  {
    if (ctrl_fsm.PLAY_VOICE_CMD_fsm.play_cmd_f)
    {
      if (ctrl_fsm.PLAY_VOICE_CMD_fsm.stat == 0) // Play voice cmd
      {
        tpf("try to play voice cmd to TTS modules, the voice is %s, try times is %d", ctrl_fsm.PLAY_VOICE_CMD_fsm.playbuf, ctrl_fsm.PLAY_VOICE_CMD_fsm.retry_cnt);

        bool isok = tts.SendPlayVoice_cmd((uint8_t *)ctrl_fsm.PLAY_VOICE_CMD_fsm.playbuf, ctrl_fsm.PLAY_VOICE_CMD_fsm.playbuf_len);

        if (isok)
          ctrl_fsm.PLAY_VOICE_CMD_fsm.stat = 1;
        else
          ctrl_fsm.PLAY_VOICE_CMD_fsm.stat = 2;
      }
      else if (ctrl_fsm.PLAY_VOICE_CMD_fsm.stat == 1) // play voice cmd is successful
      {
        tpf("play voice is successful");

        // 鍙戦€佸彇鍙戝竷鎴愬姛浜嬩欢
        ctrl_fsm.public_data_fsm.PLAY_VOICE_CMD_cmd_ok_event.send_f = true;

        ctrl_fsm.PLAY_VOICE_CMD_fsm.play_cmd_f = false;
        ctrl_fsm.PLAY_VOICE_CMD_fsm.stat = 0;
        ctrl_fsm.PLAY_VOICE_CMD_fsm.retry_cnt = 0;
      }
      else if (ctrl_fsm.PLAY_VOICE_CMD_fsm.stat == 2) // play voice cmd is failed
      {
        tpf("retry paly voice cmd to TTS modules again");

        ctrl_fsm.PLAY_VOICE_CMD_fsm.retry_cnt++;
        if (ctrl_fsm.PLAY_VOICE_CMD_fsm.retry_cnt > ctrl_fsm.PLAY_VOICE_CMD_fsm.RETRY_TTS_CNT)
        {
          // 鍙戦€佸彇鍙戝竷澶辫触浜嬩欢
          ctrl_fsm.public_data_fsm.PLAY_VOICE_CMD_cmd_err_event.send_f = true;

          ctrl_fsm.PLAY_VOICE_CMD_fsm.play_cmd_f = false;
          ctrl_fsm.PLAY_VOICE_CMD_fsm.stat = 0;
          ctrl_fsm.PLAY_VOICE_CMD_fsm.retry_cnt = 0;
        }
        else
        {
          tpf("paly voice cmd again");
          ctrl_fsm.PLAY_VOICE_CMD_fsm.stat = 0;
        }
      }
    }

    ctrl_fsm.PLAY_VOICE_CMD_fsm.period_last_ms_st = millis();
  }
#endif
#if 1 // 鏁扮爜绠℃樉绀洪€昏緫
  if (millis() - ctrl_fsm.seg_disp_fsm.main_last_ms >= ctrl_fsm.seg_disp_fsm.MAIN_SCAN_PERIOD_MS)
  {
    if (net_client.GetMqttConn() == false)
    {
      ctrl_fsm.seg_disp_fsm.stat = 1;
    }

    if (ctrl_fsm.seg_disp_fsm.stat == 0) // it display at power on
    {
      disp.DispSeg(0, TM1620::SEG_ROD), disp.DispSeg(1, TM1620::SEG_ROD), disp.DispSeg(2, TM1620::SEG_ROD), disp.DispSeg(3, TM1620::SEG_ROD);
      ctrl_fsm.seg_disp_fsm.stat = 1;
    }
    else if (ctrl_fsm.seg_disp_fsm.stat == 1) // 鏂綉鏃?,鏁扮爜绠￠棯???"-"鎻愮?
    {
      if (ctrl_fsm.seg_disp_fsm.disp_val_f)
        disp.DispSeg(0, TM1620::SEG_NOT_DISPLAY), disp.DispSeg(1, TM1620::SEG_NOT_DISPLAY), disp.DispSeg(2, TM1620::SEG_NOT_DISPLAY), disp.DispSeg(3, TM1620::SEG_NOT_DISPLAY);
      else
        disp.DispSeg(0, TM1620::SEG_ROD), disp.DispSeg(1, TM1620::SEG_ROD), disp.DispSeg(2, TM1620::SEG_ROD), disp.DispSeg(3, TM1620::SEG_ROD);

      ctrl_fsm.seg_disp_fsm.disp_val_f = !ctrl_fsm.seg_disp_fsm.disp_val_f;

      if (net_client.GetMqttConn() == true)
      {
        disp.DispSeg(0, TM1620::SEG_ROD), disp.DispSeg(1, TM1620::SEG_ROD), disp.DispSeg(2, TM1620::SEG_ROD), disp.DispSeg(3, TM1620::SEG_ROD);
        ctrl_fsm.seg_disp_fsm.stat = 2;
      }
    }
    else if (ctrl_fsm.seg_disp_fsm.stat == 2)
    {
      if (ctrl_fsm.count_down_fsm.stat == 0) // wait for count down enable
      {
        if (ctrl_fsm.count_down_fsm.count_down_start_f) ctrl_fsm.count_down_fsm.stat = 1;
      }
      else if (ctrl_fsm.count_down_fsm.stat == 1) // display count down on number segment
      {

        if ((millis() - ctrl_fsm.count_down_fsm.minute_last_ms_st >= 60 * 1000) || (ctrl_fsm.count_down_fsm.minute_last_ms_st == 0)) // 1 minute
        {
          tp();
          if (ctrl_fsm.count_down_fsm.minute_last_ms_st != 0) param.cur_data.count_down_minute--;

          uint8_t hour = param.cur_data.count_down_minute / 60;
          uint8_t hour_10 = hour / 10 % 10, hour_1 = hour % 10;
          uint8_t minute = param.cur_data.count_down_minute % 60;
          uint8_t minute_10 = minute / 10 % 10, minute_1 = minute % 10;

          tpf("count down minute is %d", param.cur_data.count_down_minute);
          tpf("hour_10:%d, hour_1:%d, minute_10:%d, minute_1:%d", hour_10, hour_1, minute_10, minute_1);

          for (int i = 0; i < 4; i++)
            disp.DispSeg(i, TM1620::SEG_NOT_DISPLAY);

          uint8_t skip_num = 0;
          if (param.cur_data.count_down_minute / 60 >= 10)
            skip_num = 0;
          else if (param.cur_data.count_down_minute / 60 >= 1)
            skip_num = 1;
          else if (param.cur_data.count_down_minute % 60 >= 10)
            skip_num = 2;
          else if (param.cur_data.count_down_minute % 60 >= 1)
            skip_num = 3;
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

          ctrl_fsm.count_down_fsm.minute_last_ms_st = millis();
        }
        if ((millis() - ctrl_fsm.count_down_fsm.sec_last_ms_st >= 1000) || (ctrl_fsm.count_down_fsm.sec_last_ms_st == 0)) // 1s
        {
          uint8_t skip_num = 0;
          if (param.cur_data.count_down_minute / 60 >= 10)
            skip_num = 0;
          else if (param.cur_data.count_down_minute / 60 >= 1)
            skip_num = 1;
          else if (param.cur_data.count_down_minute % 60 >= 10)
            skip_num = 2;
          else if (param.cur_data.count_down_minute % 60 >= 1)
            skip_num = 3;
          // tpf("skip_num:%d", skip_num);
          if (skip_num > 1) // only toggle display ":"
          {
            if (ctrl_fsm.count_down_fsm.disp_colon_f)
            {
              // tp();
              disp.DispSeg(1, TM1620::SEG_NOT_DISPLAY);
            }
            else
            {
              // tp();
              disp.DispSeg(1, TM1620::SEG_COLON);
            }
          }
          else // must be display number and ":"
          {
            uint8_t hour = param.cur_data.count_down_minute / 60;
            uint8_t hour_1 = hour % 10;

            if (ctrl_fsm.count_down_fsm.disp_colon_f)
            {
              // tp();
              disp.DispSegCode(1, disp.GetCodeNum(hour_1)); // only display "number"
            }
            else
            {
              // tp();
              disp.DispSegCode(1, disp.GetCodeNum(hour_1) | 0x80); // display ":" and "number"
            }
          }

          ctrl_fsm.count_down_fsm.disp_colon_f = !ctrl_fsm.count_down_fsm.disp_colon_f;
          ctrl_fsm.count_down_fsm.sec_last_ms_st = millis();
        }

        if (param.cur_data.count_down_minute == 0)
        {
          ctrl_fsm.count_down_fsm.count_down_start_f = false;
        }
        if (ctrl_fsm.count_down_fsm.count_down_start_f == false)
        {
          ctrl_fsm.count_down_fsm.stat = 2;
        }
      }
      // then count down finished or received "count down stop" cmd
      // reset default content to number segment
      if (ctrl_fsm.count_down_fsm.stat == 2)
      {
        // tp();
        disp.DispSeg(0, TM1620::SEG_ROD), disp.DispSeg(1, TM1620::SEG_ROD), disp.DispSeg(2, TM1620::SEG_ROD), disp.DispSeg(3, TM1620::SEG_ROD);
        ctrl_fsm.count_down_fsm.stat = 0;
        ctrl_fsm.count_down_fsm.minute_last_ms_st = 0;
        ctrl_fsm.count_down_fsm.sec_last_ms_st = 0;

        // 鍙戦€佸彇鏁扮爜鏃堕棿浜嬩欢
        ctrl_fsm.public_data_fsm.count_down_finish_event.send_f = true;
      }
    }

    ctrl_fsm.seg_disp_fsm.main_last_ms = millis();
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
        if (ctrl_fsm.web_server_fsm.ap_en) {
          dnsServer.processNextRequest();
          net_client.WebServer_pool();
        }
        ctrl_fsm.web_server_fsm.last_ms_st = millis();
      }
    }
  }
#endif

#if 1
  { // key logic
  { // key pool
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
  { // key event
    if (ctrl_fsm.key_fsm.event_f == 1)  // direct toggle door
    {
        bool state = relay.Toggle();
        tpf("relay state is %d", state);
        param.cur_data.relay = (uint8_t)state;
        ctrl_fsm.key_fsm.event_f = 0;
    }
    else if (ctrl_fsm.key_fsm.event_f == 2)  // auto close door
    {
      if(ctrl_fsm.key_fsm.event_stat == 0)
      {
        param.cur_data.relay = (uint8_t)relay.On();
        ctrl_fsm.key_fsm.event_stat = 1;
      }
      else if(ctrl_fsm.key_fsm.event_stat == 1)
      {
        if(millis() - ctrl_fsm.key_fsm.event_last_ms_st >= param.cur_data.auto_close_door_time_s * 1000)
        {
          param.cur_data.relay = (uint8_t)relay.Off();
          ctrl_fsm.key_fsm.event_stat = 0;
          ctrl_fsm.key_fsm.event_f = 0;
        }
      }
      else if(ctrl_fsm.key_fsm.event_stat == 2) // wifi infomation config
      {
        uint8_t buf[] = {0xbf,0xaa,0xca,0xbc,0xc5,0xe4,0xcd,0xf8};  // 播报,开始配�
        tts.SendPlayVoice_cmd(buf, sizeof(buf));
        net_client.WebServer_start((char *)param.cur_data.ap_ssid, (char *)param.cur_data.ap_pwd, WebServer_cb);
        setupCaptivePortal();
        ctrl_fsm.web_server_fsm.ap_en = true;
        ctrl_fsm.stat = ctrl_fsm.WIFI_INFO_CONFIG_STAT;
        ctrl_fsm.key_fsm.event_f = 0;
      }
    }
    else if (ctrl_fsm.key_fsm.event_f == 3) 
    {
        ctrl_fsm.key_fsm.event_f = 0;
    }
  }  
  }
#endif
#if 1
  if(millis() - ctrl_fsm.public_data_fsm.last_ms > ctrl_fsm.public_data_fsm.LOOP_PERIOD_MS)
  {
    if(net_client.GetMqttConn())
    {
      if(ctrl_fsm.public_data_fsm.conn_svr_baseinfo_event.send_f)
      {
        memset(g_tmpbuf,0,sizeof(g_tmpbuf));
        int result = snprintf((char *)g_tmpbuf, sizeof(g_tmpbuf),
          R"({"system_info_event":{"%s":"%d","%s":"%d","%s":%d,"%s":"%s","%s":"%s","%s":"%s","%s":%d,"%s":"%s","%s":"%s","%s":%d,"%s":%d,"%s":"%s","%s":%d,"%s":%d,"%s":"%s","%s":"%s"}})",
          "soft_ver", param.cur_data.soft_ver,
          "hard_ver", param.cur_data.hard_ver,
          "relay", param.cur_data.relay,
          "wifi_ssid", param.cur_data.wifi_ssid,
          "wifi_pwd", param.cur_data.wifi_pwd,
          "mqtt_server", param.cur_data.mqtt_server,
          "mqtt_port", param.cur_data.mqtt_port,
          "mqtt_username", param.cur_data.mqtt_username,
          "mqtt_password", param.cur_data.mqtt_password,
          "count_down_minute", param.cur_data.count_down_minute,
          "count_down_start_f", ctrl_fsm.count_down_fsm.count_down_start_f,
          "device_id", param.cur_data.device_id,
          "open_door_mode", param.cur_data.open_door_mode,
          "auto_close_door_time_s", param.cur_data.auto_close_door_time_s,
          "device_sub_topic",param.cur_data.device_sub_topic,
          "device_pub_topic",param.cur_data.device_pub_topic
        );
        
        if (result < 0 || result >= sizeof(g_tmpbuf)) {
          // Handle buffer overflow
          tpf("Error: Buffer overflow when generating system info");
          g_tmpbuf[sizeof(g_tmpbuf) - 1] = '\0';  // Ensure null-termination
        }

        tpf("publish system info to topic: %s", param.cur_data.device_pub_topic);
        tpf("system info: %s", (char *)g_tmpbuf);
        net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)g_tmpbuf);
        net_client.Publish((char *)param.cur_data.device_global_pub_topic, (char *)g_tmpbuf);

        ctrl_fsm.public_data_fsm.conn_svr_baseinfo_event.send_f = false;
      }

      if (ctrl_fsm.public_data_fsm.count_down_finish_event.send_f)
      {
        memset(g_tmpbuf, 0, sizeof(g_tmpbuf));
        cmd_pkt.BuildJsonPkt((char *)g_tmpbuf, "countdown_finish_event", (uint8_t)1);
        net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)g_tmpbuf);
        net_client.Publish((char *)param.cur_data.device_global_pub_topic, (char *)g_tmpbuf);
        tpf("publish count down finish event: %s", (char *)g_tmpbuf);
        ctrl_fsm.public_data_fsm.count_down_finish_event.send_f = false;
      }

      if (ctrl_fsm.public_data_fsm.PLAY_VOICE_CMD_cmd_ok_event.send_f)
      {
        memset(g_tmpbuf, 0, sizeof(g_tmpbuf));
        cmd_pkt.BuildJsonPkt((char *)g_tmpbuf, "play_voice_result_event", (uint8_t)1);
        net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)g_tmpbuf);
        net_client.Publish((char *)param.cur_data.device_global_pub_topic, (char *)g_tmpbuf);
        tpf("publish play voice command OK event: %s", (char *)g_tmpbuf);
        ctrl_fsm.public_data_fsm.PLAY_VOICE_CMD_cmd_ok_event.send_f = false;
      }

      if (ctrl_fsm.public_data_fsm.PLAY_VOICE_CMD_cmd_err_event.send_f)
      {
        memset(g_tmpbuf, 0, sizeof(g_tmpbuf));
        cmd_pkt.BuildJsonPkt((char *)g_tmpbuf, "play_voice_result_event", (uint8_t)0);
        net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)g_tmpbuf);
        net_client.Publish((char *)param.cur_data.device_global_pub_topic, (char *)g_tmpbuf);
        tpf("publish play voice command error event: %s", (char *)g_tmpbuf);
        ctrl_fsm.public_data_fsm.PLAY_VOICE_CMD_cmd_err_event.send_f = false;
      }

      if (ctrl_fsm.public_data_fsm.ask_response.send_f)
      {
        memset(g_tmpbuf, 0, sizeof(g_tmpbuf));
        cmd_pkt.BuildJsonPkt((char *)g_tmpbuf, "ask", (uint8_t)1);  // 确认应答
        net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)g_tmpbuf);
        net_client.Publish((char *)param.cur_data.device_global_pub_topic, (char *)g_tmpbuf);
        tpf("publish ask response: %s", (char *)g_tmpbuf);
        ctrl_fsm.public_data_fsm.ask_response.send_f = false;
      }

      if (ctrl_fsm.public_data_fsm.nask_response.send_f)
      {
        memset(g_tmpbuf, 0, sizeof(g_tmpbuf));
        cmd_pkt.BuildJsonPkt((char *)g_tmpbuf, "ask", (uint8_t)0);  // 否认应答
        net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)g_tmpbuf);
        net_client.Publish((char *)param.cur_data.device_global_pub_topic, (char *)g_tmpbuf);
        tpf("publish nask response: %s", (char *)g_tmpbuf);
        ctrl_fsm.public_data_fsm.nask_response.send_f = false;
      }

      if (ctrl_fsm.public_data_fsm.unknown_response.send_f)
      {
        memset(g_tmpbuf, 0, sizeof(g_tmpbuf));
        cmd_pkt.BuildJsonPkt((char *)g_tmpbuf, "unknown_cmd", (uint8_t)0);  // 未知命令应答
        net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)g_tmpbuf);
        net_client.Publish((char *)param.cur_data.device_global_pub_topic, (char *)g_tmpbuf);
        tpf("publish unknown command response: %s", (char *)g_tmpbuf);
        ctrl_fsm.public_data_fsm.unknown_response.send_f = false;
      }

      
      {
      // 检查继电器状态是否发生变化
        static uint8_t last_relay_state = 255; // 初始化为一个不可能的值
        if (param.cur_data.relay != last_relay_state)
        {
          // 继电器状态发生变化，准备上报事件
          memset(g_tmpbuf, 0, sizeof(g_tmpbuf));
          cmd_pkt.BuildJsonPkt((char *)g_tmpbuf, "relay_state_change_event", (uint8_t)param.cur_data.relay);
          net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)g_tmpbuf);
          net_client.Publish((char *)param.cur_data.device_global_pub_topic, (char *)g_tmpbuf);
          tpf("publish relay state change event: %s", (char *)g_tmpbuf);
          // 更新上一次的继电器状态
          last_relay_state = param.cur_data.relay;
        }
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

    if (param.cur_data.open_door_mode == 0) {
      ctrl_fsm.key_fsm.event_f = 1; // direct toggle door
    }
    else if (param.cur_data.open_door_mode == 1) {
      ctrl_fsm.key_fsm.event_stat = 0; // auto close door
      ctrl_fsm.key_fsm.event_last_ms_st = millis();
      ctrl_fsm.key_fsm.event_f = 2; // auto close door
    }
  }
  else if (key_event == Key::KeyEvent_et::LONG_CLICK) // wifi config by hot point
  {
    tpf("key long press");
    ctrl_fsm.key_fsm.event_f = 2; // wifi info config
    ctrl_fsm.key_fsm.event_stat = 2;
  }
  else if (key_event == Key::KeyEvent_et::DOUBLE_CLICK)
  {
    tpf("key double press");
  }
}

void WebServer_cb() 
{
  tp();
  ctrl_fsm.wifi_cfg_fsm.ssid = net_client.GetArg("ssid");
  ctrl_fsm.wifi_cfg_fsm.pwd = net_client.GetArg("password");
  Serial.println("Received SSID: " + ctrl_fsm.wifi_cfg_fsm.ssid);
  Serial.println("Received Password: " + ctrl_fsm.wifi_cfg_fsm.pwd);
  strcpy((char *)param.cur_data.wifi_ssid, ctrl_fsm.wifi_cfg_fsm.ssid.c_str());
  strcpy((char *)param.cur_data.wifi_pwd, ctrl_fsm.wifi_cfg_fsm.pwd.c_str());

  if (ctrl_fsm.wifi_cfg_fsm.ssid.length() != 0 && ctrl_fsm.wifi_cfg_fsm.pwd.length() != 0)
  {
    ctrl_fsm.web_server_fsm.config_finish_f = true;
  }

  tpf("config wifi infomation is finished");
  String response = "{\"status\":\"success\",\"message\":\"WiFi 配置完成\"}";
  NetClient::server.send(200, "application/json", response);
  delay(800), ESP.wdtFeed(), delay(800), ESP.wdtFeed();

  uint8_t buf[] = {0xc5,0xe4,0xcd,0xf8,0xcd,0xea,0xb3,0xc9};  // 播报"配网完成"的GBK编码
  tts.SendPlayVoice_cmd(buf, sizeof(buf));

  net_client.WebServer_stop();
  ctrl_fsm.web_server_fsm.ap_en = false;
  ctrl_fsm.wifi_cfg_fsm.stat = 0;
  ctrl_fsm.stat = ctrl_fsm.CONN_WIFI_STAT;
  ctrl_fsm.web_server_fsm.config_finish_f = false;
}

void setupCaptivePortal() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP((char *)param.cur_data.ap_ssid, (char *)param.cur_data.ap_pwd);

  // Configure DNS server to redirect all domains to the AP's IP
  dnsServer.start(DNS_PORT, "*", apIP);

  NetClient::server.on("/", HTTP_GET, []() {
    NetClient::server.sendHeader("Location", String("http://") + toStringIp(NetClient::server.client().localIP()), true);
    NetClient::server.send(302, "text/plain", "");
  });

  NetClient::server.onNotFound([]() {
    NetClient::server.sendHeader("Location", String("http://") + toStringIp(NetClient::server.client().localIP()), true);
    NetClient::server.send(302, "text/plain", "");
  });
}

String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
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

  // static char device_sub_topic[64];
  // memset(device_sub_topic, 0, sizeof(device_sub_topic));
  // sprintf(device_sub_topic, "%s/%s", param.cur_data.device_id, param.cur_data.device_sub_topic);

  if (strcmp(topic, (char *)param.cur_data.device_sub_topic) == 0)
  {
    char cmd_no[32];
    memset(cmd_no, 0, sizeof(cmd_no));
    cmd_pkt.GetCmdNo(cmd_no, payload, length);
    tpf("cmd_no:%s", cmd_no);

    if (strcmp(cmd_no, CmdPkt::SET_MQTT_SERVER_CMD) == 0)
    {
      cmd_pkt.ExtractStrOfBuf((char *)payload, length, (char *)param.cur_data.mqtt_server);
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;      
    }
    else if (strcmp(cmd_no, CmdPkt::SET_WIFI_SSID_CMD) == 0)
    {
      cmd_pkt.ExtractStrOfBuf((char *)payload, length, (char *)param.cur_data.wifi_ssid);
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;      
    }
    else if (strcmp(cmd_no, CmdPkt::SET_WIFI_PWD_CMD) == 0)
    {
      cmd_pkt.ExtractStrOfBuf((char *)payload, length, (char *)param.cur_data.wifi_pwd);
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;      
    }
    else if (strcmp(cmd_no, CmdPkt::SET_MQTT_USERNAME_CMD) == 0)
    {
      cmd_pkt.ExtractStrOfBuf((char *)payload, length, (char *)param.cur_data.mqtt_username);
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;      
    }
    else if (strcmp(cmd_no, CmdPkt::SET_MQTT_PASSWORD_CMD) == 0)
    {
      cmd_pkt.ExtractStrOfBuf((char *)payload, length, (char *)param.cur_data.mqtt_password);
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;      
    }
    else if (strcmp(cmd_no, CmdPkt::OPEN_RELAY_CMD) == 0)
    {
      // param.cur_data.relay = (uint8_t)relay.On();
      // tpf("received cmd, open relay");

      if (param.cur_data.open_door_mode == 0) {
      ctrl_fsm.key_fsm.event_f = 1; // direct toggle door
      }
      else if (param.cur_data.open_door_mode == 1) {
        ctrl_fsm.key_fsm.event_stat = 0; // auto close door
        ctrl_fsm.key_fsm.event_last_ms_st = millis();
        ctrl_fsm.key_fsm.event_f = 2; // auto close door
      }

      ctrl_fsm.public_data_fsm.ask_response.send_f = true;      
    }
    else if (strcmp(cmd_no, CmdPkt::CLOSE_RELAY_CMD) == 0)
    {
      if (param.cur_data.open_door_mode == 0) {      // direct mode
        param.cur_data.relay = (uint8_t)relay.Off();
        tpf("received cmd, close relay");
        ctrl_fsm.public_data_fsm.ask_response.send_f = true;      
      }else if(param.cur_data.open_door_mode == 1){
        tpf("current mode is auto close door mode, don't direct close door");
        ctrl_fsm.public_data_fsm.nask_response.send_f = false;
      }
    }
    else if (strcmp(cmd_no, CmdPkt::GET_SYSTEM_INFO_CMD) == 0)
    {
      ctrl_fsm.public_data_fsm.conn_svr_baseinfo_event.send_f = true;      
      // ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (strcmp(cmd_no, CmdPkt::SET_MQTT_PORT_CMD) == 0)
    {
      uint16_t port;
      cmd_pkt.ExtractIntOfBuf((char *)payload, length, &port);
      param.cur_data.mqtt_port = port;
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (strcmp(cmd_no, CmdPkt::PLAY_VOICE_CMD) == 0)
    {
      ctrl_fsm.PLAY_VOICE_CMD_fsm.play_cmd_f = true;
      ctrl_fsm.PLAY_VOICE_CMD_fsm.playbuf_len = cmd_pkt.ExtractGbkHexStrOfBuf((char *)payload, length, ctrl_fsm.PLAY_VOICE_CMD_fsm.playbuf);
      tpbuf(ctrl_fsm.PLAY_VOICE_CMD_fsm.playbuf, ctrl_fsm.PLAY_VOICE_CMD_fsm.playbuf_len);
    }
    else if (strcmp(cmd_no, CmdPkt::SET_COUNT_DOWN_CMD) == 0)
    {
      uint16_t count_down = 0;
      cmd_pkt.ExtractIntOfBuf((char *)payload, length, &count_down);
      tpf("count down: %d", count_down);
      param.cur_data.count_down_minute = count_down;
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (strcmp(cmd_no, CmdPkt::OPEN_COUNT_DOWN_CMD) == 0)
    {
      tpf("open count down");
      ctrl_fsm.count_down_fsm.count_down_start_f = true;
      ctrl_fsm.public_data_fsm.ask_response.send_f = true; 
    }
    else if (strcmp(cmd_no, CmdPkt::CLOSE_COUNT_DOWN_CMD) == 0)
    {
      tpf("close count down");
      ctrl_fsm.count_down_fsm.count_down_start_f = false;
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (strcmp(cmd_no, CmdPkt::SET_OPEN_DOOR_MODE_CMD) == 0)
    {
      uint8_t door_mode; cmd_pkt.ExtractIntOfBuf((char *)payload, length, &door_mode);
      param.cur_data.open_door_mode = door_mode;
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (strcmp(cmd_no, CmdPkt::SET_SOFT_VER_CMD) == 0)
    {
      char new_soft_ver[32];
      cmd_pkt.ExtractStrOfBuf((char *)payload, length, new_soft_ver);
      // Assuming there's a software version field in param.cur_data
      strncpy((char *)param.cur_data.soft_ver, new_soft_ver, sizeof(param.cur_data.soft_ver));
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (strcmp(cmd_no, CmdPkt::SET_HARD_VER_CMD) == 0)
    {
      char new_hard_ver[32];
      cmd_pkt.ExtractStrOfBuf((char *)payload, length, new_hard_ver);
      // Assuming there's a hardware version field in param.cur_data
      strncpy((char *)param.cur_data.hard_ver, new_hard_ver, sizeof(param.cur_data.hard_ver));
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (strcmp(cmd_no, CmdPkt::SET_DEVECE_ID_CMD) == 0)
    {
      tp();
      char new_device_id[32];
      cmd_pkt.ExtractStrOfBuf((char *)payload, length, new_device_id);
      // Assuming there's a device ID field in param.cur_data
      strncpy((char *)param.cur_data.device_id, new_device_id, sizeof(param.cur_data.device_id));
      // Update device_sub_topic and device_pub_topic based on the new device_id
      snprintf((char *)param.cur_data.device_sub_topic, sizeof(param.cur_data.device_sub_topic), "%s/device_sub_topic", param.cur_data.device_id);
      snprintf((char *)param.cur_data.device_pub_topic, sizeof(param.cur_data.device_pub_topic), "%s/device_pub_topic", param.cur_data.device_id);
      tpf("Updated device topics - Sub: %s, Pub: %s", param.cur_data.device_sub_topic, param.cur_data.device_pub_topic);
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (strcmp(cmd_no, CmdPkt::FACTORY_PARAMS_CMD) == 0)
    {
      tpf("received factory params cmd");
      // Reset all parameters to factory defaults
      param.ResetAllParamToDefault();
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (strcmp(cmd_no,CmdPkt::RESTART_CMD) == 0)
    {
      tpf("Restarting system...");
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
      // Schedule a system restart using Tick's once function
      static Ticker restartTicker;
      restartTicker.once(2, []() {
        ESP.restart();
      });
      tpf("System restart scheduled in 2 second");
    }
    else if (strcmp(cmd_no, CmdPkt::SET_DEVICE_GLOBAL_PUB_TOPIC_CMD) == 0)
    {
      char new_global_pub_topic[64];
      cmd_pkt.ExtractStrOfBuf((char *)payload, length, new_global_pub_topic);
      strncpy((char *)param.cur_data.device_global_pub_topic, new_global_pub_topic, sizeof(param.cur_data.device_global_pub_topic));
      tpf("Updated global pub topic: %s", param.cur_data.device_global_pub_topic);
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (strcmp(cmd_no, CmdPkt::SET_DEVICE_GLOBAL_SUB_TOPIC_CMD) == 0)
    {
      char new_global_sub_topic[64];
      cmd_pkt.ExtractStrOfBuf((char *)payload, length, new_global_sub_topic);
      strncpy((char *)param.cur_data.device_global_sub_topic, new_global_sub_topic, sizeof(param.cur_data.device_global_sub_topic));
      tpf("Updated global sub topic: %s", param.cur_data.device_global_sub_topic);
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else
    {
      tpf("unknown cmd");
      ctrl_fsm.public_data_fsm.unknown_response.send_f = true;
    }
  }
#endif
}
#endif

// main.cpp
