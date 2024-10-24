/***********************************************************************
 * @file PARAM.hpp
 * PARAM
 * @author :	ch
 * @brief  :
 * @version:	v2
 * @Copyright (C)  2024-07-22  .cdWFVCEL. all right reserved
 ***********************************************************************/

#ifndef __PARAM_H_
#define __PARAM_H_
/* include ----------------------------------------------------------------------------------------------------------------- */
#include "include.h"
#include "param_tab.hpp"
#include <type_traits>
/* macro definition -------------------------------------------------------------------------------------------------------- */
// ip：mqtt.dianwuyo.com
// 端口：1884
// 账号：dwy_gaiban
// 密码：gaibanmenjin

#define DEF_DEVICE_ID "888888888888" // 实际默认会被替换为 WIFI MAC 地址
#define DEF_RELAY 1
#define DEF_WIFI_SSID "88888888"
#define DEF_WIFI_PWD "88888888"
// #define DEF_MQTT_SERVER "ee836835.ala.cn-hangzhou.emqxsl.cn"
// #define DEF_MQTT_PORT 8883
// #define DEF_MQTT_USERNAME "dianzha202451"
// #define DEF_MQTT_PASSWORD "zhangguile"

// #define DEF_MQTT_SERVER "mqtt.dianwuyo.com"
// #define DEF_MQTT_PORT 1884
// #define DEF_MQTT_USERNAME "dwy_gaiban"
// #define DEF_MQTT_PASSWORD "gaibanmenjin"

// ip：106.75.224.82
// 端口：1884
// 账号：dwy_gaiban
// 密码：gaibanmenjin
#define DEF_MQTT_SERVER "106.75.224.82"
#define DEF_MQTT_PORT 1884
#define DEF_MQTT_USERNAME "dwy_gaiban"
#define DEF_MQTT_PASSWORD "gaibanmenjin"

#define DEF_DEVICE_GLOBAL_PUB_TOPIC "device_global_pub_topic" // 全局发布主题
#define DEF_DEVICE_GLOBAL_SUB_TOPIC "device_global_sub_topic" // 全局订阅主题
#define DEF_DEVICE_SUB_TOPIC "device_sub_topic"               // 设备订阅主题,it will be replaced by device_id + device_sub_topic
#define DEF_DEVICE_PUB_TOPIC "device_pub_topic"               // 设备发布主题,it will be replaced by device_id + device_pub_topic
#define DEF_COUNT_DOWN 60                                     // 默认倒计时
#define DEF_AP_SSID "ESP8266-AP"
// #define DEF_OPEN_DOOR_MODE 0
// #define DEF_AUTO_CLOSE_DOOR_TIME_S 10
#define DEF_AP_PWD "88888888"
#define DEF_SOFT_VERSION 1
#define DEF_HARD_VERSION 1
#define DEF_PROTOCOL_VER 1
#define DEF_PARAM_VER 1
#define DEF_BUILD_DATETIME_VER "240101120203"

#define UPDATA_PARAM_PERIOD_MS 1000 // 更新参数周期
#define EEPROM_ADDRESS 0
#define PARAM_OF_BYTE 40
#define EEPROM_SIZE 2048
/* type definition --------------------------------------------------------------------------------------------------------- */

class Param
{
public:
#pragma pack(1)

  typedef struct
  {
    uint8_t soft_ver = DEF_SOFT_VERSION;
    uint8_t hard_ver = DEF_HARD_VERSION;
    uint8_t protocol_ver = DEF_PROTOCOL_VER;
    uint8_t build_datetime[PARAM_OF_BYTE] = DEF_BUILD_DATETIME_VER;
    uint8_t param_ver = DEF_PARAM_VER;
    uint8_t relay = DEF_RELAY;
    uint8_t wifi_ssid[PARAM_OF_BYTE] = DEF_WIFI_SSID;
    uint8_t wifi_pwd[PARAM_OF_BYTE] = DEF_WIFI_PWD;
    uint8_t mqtt_server[PARAM_OF_BYTE] = DEF_MQTT_SERVER;
    uint16_t mqtt_port = DEF_MQTT_PORT;
    uint8_t mqtt_username[PARAM_OF_BYTE] = DEF_MQTT_USERNAME;
    uint8_t mqtt_password[PARAM_OF_BYTE] = DEF_MQTT_PASSWORD;
    uint8_t device_id[PARAM_OF_BYTE];
    uint8_t device_global_pub_topic[PARAM_OF_BYTE] = DEF_DEVICE_GLOBAL_PUB_TOPIC;
    uint8_t device_global_sub_topic[PARAM_OF_BYTE] = DEF_DEVICE_GLOBAL_SUB_TOPIC;
    uint8_t device_sub_topic[PARAM_OF_BYTE] = DEF_DEVICE_SUB_TOPIC;
    uint8_t device_pub_topic[PARAM_OF_BYTE] = DEF_DEVICE_PUB_TOPIC;
    uint8_t ap_ssid[PARAM_OF_BYTE] = DEF_AP_SSID;
    uint8_t ap_pwd[PARAM_OF_BYTE] = DEF_AP_PWD;

    uint16_t crc16 = 0;
  } Entry_t;

  Entry_t cur_data;
  Entry_t last_data;
  Entry_t def_data;
#pragma pack()

  void Init();
  /**
   * @brief 加载参数,如果参数第一次初始化,则加载默认参数
   *
   */
  void LoadAllParam();

  void ResetAllParamToDefault();
  void WriteParams();
  void UpdateParam_pool();
  void UpdateVerParam_pool(); // @brief then version will be updated of "version.h" and then updata Param whit write to EEPROM
  String ToString(Entry_t *entry);

private:
};
/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */

#endif // __PARAM_H_
