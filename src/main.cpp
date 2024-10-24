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
#include "tm1620.h"
#include "bonezegei_softserial.h"
#include <ArduinoJson.h>
/* macro definition ------------------------------------------------------------------------------------------------- */

/* type definition ------------------------------------------------------------------------------------------------- */

/* variable declaration ------------------------------------------------------------------------------------------------- */
// LED led;
DOut relay;
LED led; // 将 Dout 改为 DOut
Key key;
Ticker ticker;
NetClient net_client;
Param param;
CmdPkt cmd_pkt;

DNSServer dnsServer;
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);

StaticJsonDocument<JSON_DOC_SIZE> g_json_doc; // 回到使用 StaticJsonDocument

uint8_t g_tmpbuf[TMPBUF_SIZE];
String g_str;

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
  struct
  {
    uint8_t stat;
    uint32_t LOOP_PERIOD_MS = 500;
    uint32_t last_ms_st;
    // -------------------------------------------------------------------------------------------------
    enum
    {
      NONE,
      TOGGLE_RELAY,
      OPEN_RELAY,
      CLOSE_RELAY,
      WIFI_CONFIG,
    } event_f;
    // uint32_t event_last_ms_st;
    // uint8_t event_stat;

  } action_fsm;
} ctrl_fsm;
/* function declaration ------------------------------------------------------------------------------------------------- */
void Key_cb(Key::KeyEvent_et key_event);
void MQTTRecv_cb(char *topic, uint8_t *payload, uint16_t length);
void WebServer_cb();
String toStringIp(IPAddress ip);

/* function implementation ------------------------------------------------------------------------------------------------- */
void setup()
{
  Serial.begin(115200);
  tpf("Power on..............");
  g_str.reserve(GSTR_SIZE);

  led.Init(LED_PIN, DOut::ON_POLARITY_LOW);
  relay.Init(RELAY_PIN, DOut::ON_POLARITY_HIGH);
  key.Init(KEY_PIN, INPUT_PULLUP, Key_cb, Key::KEY_LOW);

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
      // param.ResetAllParamToDefault();
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
        param.UpdateVerParam_pool();

        ctrl_fsm.param_fsm.last_ms_st = millis();
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
        if (ctrl_fsm.web_server_fsm.ap_en)
        {
          dnsServer.processNextRequest();
          net_client.WebServer_pool();
        }
        ctrl_fsm.web_server_fsm.last_ms_st = millis();
      }
    }
  }
#endif

#if 1
  {   // key logic
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
  }
#endif

  { // action logic
    if (millis() - ctrl_fsm.action_fsm.last_ms_st >= ctrl_fsm.action_fsm.LOOP_PERIOD_MS)
    {
      if (ctrl_fsm.action_fsm.event_f == ctrl_fsm.action_fsm.TOGGLE_RELAY)
      {
        bool state = relay.Toggle();
        param.cur_data.relay = (uint8_t)state;

        ctrl_fsm.action_fsm.event_f = ctrl_fsm.action_fsm.NONE;

        tpf("relay state is %d", state);
      }
      else if (ctrl_fsm.action_fsm.event_f == ctrl_fsm.action_fsm.OPEN_RELAY)
      {
        param.cur_data.relay = relay.On();
        ctrl_fsm.action_fsm.event_f = ctrl_fsm.action_fsm.NONE;

        tpf("relay on");
      }
      else if (ctrl_fsm.action_fsm.event_f == ctrl_fsm.action_fsm.CLOSE_RELAY)
      {
        param.cur_data.relay = relay.Off();
        ctrl_fsm.action_fsm.event_f = ctrl_fsm.action_fsm.NONE;

        tpf("relay off");
      }
      else if (ctrl_fsm.action_fsm.event_f == ctrl_fsm.action_fsm.WIFI_CONFIG)
      {
        net_client.WebServer_start((char *)param.cur_data.ap_ssid, (char *)param.cur_data.ap_pwd, WebServer_cb);

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

        ctrl_fsm.web_server_fsm.ap_en = true;
        ctrl_fsm.stat = ctrl_fsm.WIFI_INFO_CONFIG_STAT;

        ctrl_fsm.action_fsm.event_f = ctrl_fsm.action_fsm.NONE;
      }

      {                          // Update LED status based on relay status
        if (relay.GetState()) // turn on LED when relay is on
        {
          if (!led.GetState()) led.On();
        }
        else // Turn off LED when relay is off
        {
          if (led.GetState()) led.Off();
        }
      }
      ctrl_fsm.action_fsm.last_ms_st = millis();
    }
  }
#if 1
  if (millis() - ctrl_fsm.public_data_fsm.last_ms > ctrl_fsm.public_data_fsm.LOOP_PERIOD_MS)
  {
    if (net_client.GetMqttConn())
    {
      if (ctrl_fsm.public_data_fsm.conn_svr_baseinfo_event.send_f)
      {
        g_json_doc.clear(); // 清除之前的内容
        JsonObject system_info = g_json_doc["system_info_event"].to<JsonObject>();

        system_info["soft_ver"] = param.cur_data.soft_ver;
        system_info["hard_ver"] = param.cur_data.hard_ver;
        system_info["relay"] = param.cur_data.relay;
        system_info["wifi_ssid"] = param.cur_data.wifi_ssid;
        system_info["wifi_pwd"] = param.cur_data.wifi_pwd;
        system_info["mqtt_server"] = param.cur_data.mqtt_server;
        tpf("mqtt_port:%d", param.cur_data.mqtt_port);
        // system_info["mqtt_port"] = param.cur_data.mqtt_port;
        system_info["mqtt_port"] = static_cast<int>(param.cur_data.mqtt_port); // 必须进行类型转换,可能是 arduino json 不支持 uint16_t 类型
        system_info["mqtt_username"] = param.cur_data.mqtt_username;
        system_info["mqtt_password"] = param.cur_data.mqtt_password;
        system_info["device_id"] = param.cur_data.device_id;
        system_info["device_sub_topic"] = param.cur_data.device_sub_topic;
        system_info["device_pub_topic"] = param.cur_data.device_pub_topic;

        serializeJson(g_json_doc, g_str);

        tpf("publish system info to topic: %s", param.cur_data.device_pub_topic);
        tpf("system info: %s", g_str.c_str());
        net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)g_str.c_str());
        net_client.Publish((char *)param.cur_data.device_global_pub_topic, (char *)g_str.c_str());

        ctrl_fsm.public_data_fsm.conn_svr_baseinfo_event.send_f = false;
      }

      if (ctrl_fsm.public_data_fsm.ask_response.send_f)
      {
        g_json_doc.clear();
        g_json_doc["ask"] = 1;

        serializeJson(g_json_doc, g_str);

        net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)g_str.c_str());
        net_client.Publish((char *)param.cur_data.device_global_pub_topic, (char *)g_str.c_str());
        tpf("publish ask response: %s", g_str.c_str());
        ctrl_fsm.public_data_fsm.ask_response.send_f = false;
      }

      if (ctrl_fsm.public_data_fsm.nask_response.send_f)
      {
        g_json_doc.clear();
        g_json_doc["ask"] = 0;

        serializeJson(g_json_doc, g_str);

        net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)g_str.c_str());
        net_client.Publish((char *)param.cur_data.device_global_pub_topic, (char *)g_str.c_str());
        tpf("publish nask response: %s", g_str.c_str());
        ctrl_fsm.public_data_fsm.nask_response.send_f = false;
      }

      if (ctrl_fsm.public_data_fsm.unknown_response.send_f)
      {
        g_json_doc.clear();
        g_json_doc["unknown_cmd"] = 0;

        serializeJson(g_json_doc, g_str);

        net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)g_str.c_str());
        net_client.Publish((char *)param.cur_data.device_global_pub_topic, (char *)g_str.c_str());
        tpf("publish unknown command response: %s", g_str.c_str());
        ctrl_fsm.public_data_fsm.unknown_response.send_f = false;
      }

      {
        // 检查继电器状态是否发生变化
        static uint8_t last_relay_state = 255; // 初始化为一个不可能的值
        if (param.cur_data.relay != last_relay_state)
        {
          // 继电器状态发生变化，准备上报事件
          g_json_doc.clear();
          g_json_doc["relay_state_change_event"] = param.cur_data.relay;

          serializeJson(g_json_doc, g_str);

          net_client.Publish((char *)param.cur_data.device_pub_topic, (char *)g_str.c_str());
          net_client.Publish((char *)param.cur_data.device_global_pub_topic, (char *)g_str.c_str());
          tpf("publish relay state change event: %s", g_str.c_str());
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
    ctrl_fsm.action_fsm.event_f = ctrl_fsm.action_fsm.TOGGLE_RELAY;
  }
  else if (key_event == Key::KeyEvent_et::LONG_CLICK) // wifi config by hot point
  {
    tpf("key long press");
    ctrl_fsm.action_fsm.event_f = ctrl_fsm.action_fsm.WIFI_CONFIG;
  }
  else if (key_event == Key::KeyEvent_et::DOUBLE_CLICK)
  {
    tpf("key double press");
  }
}

void WebServer_cb()
{
  tp();
  ctrl_fsm.wifi_cfg_fsm.ssid = net_client.GetArg((char *)"ssid");
  ctrl_fsm.wifi_cfg_fsm.pwd = net_client.GetArg((char *)"password");
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

  net_client.WebServer_stop();
  ctrl_fsm.web_server_fsm.ap_en = false;
  ctrl_fsm.wifi_cfg_fsm.stat = 0;
  ctrl_fsm.stat = ctrl_fsm.CONN_WIFI_STAT;
  ctrl_fsm.web_server_fsm.config_finish_f = false;
}

String toStringIp(IPAddress ip)
{
  String res = "";
  for (int i = 0; i < 3; i++)
  {
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
  memset(g_tmpbuf, 0, sizeof(g_tmpbuf));
  strcpy((char *)g_tmpbuf, (char *)payload);
  tpf("MQTTRecv_cb, topic:%s, payload:%s", topic, g_tmpbuf);

#if 1
  g_json_doc.clear(); // 清除之前的内容
  DeserializationError error = deserializeJson(g_json_doc, g_tmpbuf);

  if (error)
  {
    tpf("deserializeJson() failed: %s", error.c_str());
    return;
  }

  if (strcmp(topic, (char *)param.cur_data.device_sub_topic) == 0)
  {
    if (g_json_doc.containsKey("open_relay_cmd"))
    {
      ctrl_fsm.action_fsm.event_f = ctrl_fsm.action_fsm.OPEN_RELAY;
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (g_json_doc.containsKey("close_relay_cmd"))
    {
      ctrl_fsm.action_fsm.event_f = ctrl_fsm.action_fsm.CLOSE_RELAY;
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (g_json_doc.containsKey("set_mqtt_server_cmd"))
    {
      const char *mqtt_server = g_json_doc["set_mqtt_server_cmd"];
      strncpy((char *)param.cur_data.mqtt_server, mqtt_server, sizeof(param.cur_data.mqtt_server));
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (g_json_doc.containsKey("set_wifi_ssid_cmd"))
    {
      const char *wifi_ssid = g_json_doc["set_wifi_ssid_cmd"];
      strncpy((char *)param.cur_data.wifi_ssid, wifi_ssid, sizeof(param.cur_data.wifi_ssid));
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (g_json_doc.containsKey("set_wifi_pwd_cmd"))
    {
      const char *wifi_pwd = g_json_doc["set_wifi_pwd_cmd"];
      strncpy((char *)param.cur_data.wifi_pwd, wifi_pwd, sizeof(param.cur_data.wifi_pwd));
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (g_json_doc.containsKey("set_mqtt_username_cmd"))
    {
      const char *mqtt_username = g_json_doc["set_mqtt_username_cmd"];
      strncpy((char *)param.cur_data.mqtt_username, mqtt_username, sizeof(param.cur_data.mqtt_username));
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (g_json_doc.containsKey("set_mqtt_password_cmd"))
    {
      const char *mqtt_password = g_json_doc["set_mqtt_password_cmd"];
      strncpy((char *)param.cur_data.mqtt_password, mqtt_password, sizeof(param.cur_data.mqtt_password));
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (g_json_doc.containsKey("set_mqtt_port_cmd"))
    {
      uint16_t port = g_json_doc["set_mqtt_port_cmd"];
      param.cur_data.mqtt_port = port;
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (g_json_doc.containsKey("set_device_id_cmd"))
    {
      const char *new_device_id = g_json_doc["set_device_id_cmd"];
      strncpy((char *)param.cur_data.device_id, new_device_id, sizeof(param.cur_data.device_id));
      snprintf((char *)param.cur_data.device_sub_topic, sizeof(param.cur_data.device_sub_topic), "%s/device_sub_topic", param.cur_data.device_id);
      snprintf((char *)param.cur_data.device_pub_topic, sizeof(param.cur_data.device_pub_topic), "%s/device_pub_topic", param.cur_data.device_id);
      tpf("Updated device topics - Sub: %s, Pub: %s", param.cur_data.device_sub_topic, param.cur_data.device_pub_topic);
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (g_json_doc.containsKey("get_system_info_cmd"))
    {
      ctrl_fsm.public_data_fsm.conn_svr_baseinfo_event.send_f = true;
    }
    else if (g_json_doc.containsKey("factory_params_cmd"))
    {
      tpf("received factory params cmd");
      param.ResetAllParamToDefault();
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (g_json_doc.containsKey("restart_cmd"))
    {
      tpf("Restarting system...");
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
      static Ticker restartTicker;
      restartTicker.once(2, []() { ESP.restart(); });
      tpf("System restart scheduled in 2 seconds");
    }
    else if (g_json_doc.containsKey("set_relay_cmd"))
    {
      int relay_state = g_json_doc["set_relay_cmd"];
      if (relay_state == 0)
      {
        param.cur_data.relay = (uint8_t)relay.Off();
        tpf("Set relay off");
      }
      else if (relay_state == 1)
      {
        param.cur_data.relay = (uint8_t)relay.On();
        tpf("Set relay on");
      }
      else
      {
        tpf("Invalid relay state");
      }
      ctrl_fsm.public_data_fsm.ask_response.send_f = true;
    }
    else if (g_json_doc.containsKey("toggle_relay_cmd"))
    {
      param.cur_data.relay = (uint8_t)relay.Toggle();
      tpf("Toggled relay, new state: %s", param.cur_data.relay ? "on" : "off");
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
