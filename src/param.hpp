/***********************************************************************
 * @file PARAM.hpp
 * PARAM
 * @author :	ch
 * @brief  :
 * @version:	v1
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

#define DEF_DEVICE_ID "" // 实际默认会被替换为 WIFI MAC 地址
#define DEF_RELAY 1
#define DEF_WIFI_SSID "88888888"
#define DEF_WIFI_PWD "88888888"
// #define DEF_MQTT_SERVER "ee836835.ala.cn-hangzhou.emqxsl.cn"
// #define DEF_MQTT_PORT 8883
// #define DEF_MQTT_USERNAME "dianzha202451"
// #define DEF_MQTT_PASSWORD "zhangguile"
#define DEF_MQTT_SERVER "mqtt.dianwuyo.com"
#define DEF_MQTT_PORT 1884
#define DEF_MQTT_USERNAME "dwy_gaiban"
#define DEF_MQTT_PASSWORD "gaibanmenjin"
#define DEF_PUB_TOPIC "pub_topic"               // 全局发布主题
#define DEF_DEVICE_SUB_TOPIC "device_sub_topic" // 设备订阅主题
#define DEF_DEVICE_PUB_TOPIC "device_pub_topic" // 设备发布主题
#define DEF_COUNT_DOWN 60                       // 默认倒计时
#define DEF_AP_SSID "ESP8266-AP"
#define DEF_AP_PWD "88888888"

#define UPDATA_PARAM_PERIOD_MS 1000 // 更新参数周期
/* type definition --------------------------------------------------------------------------------------------------------- */

class Param
{
public:
  enum
  {
    PARAM_BUFSIZE = 40,
  };

  enum
  {
    RELAY_ID = 0,
    WIFI_SSID_ID = 1,
    WIFI_PWD_ID = 2,
    MQTT_SERVER_ID = 3,
    MQTT_PORT_ID = 4,
    MQTT_USERNAME_ID = 5,
    MQTT_PASSWORD_ID = 6,
    DEVICE_ID = 7,
    DEVICE_SUB_TOPIC_ID = 8,
    DEVICE_PUB_TOPIC_ID = 9,
    PUB_TOPIC_ID = 10,
    COUNT_DOWN_ID = 11,
    AP_SSID = 12,
    AP_PWD = 13,
    PARAM_NUM
  };

#pragma pack(1)
  typedef struct
  {
    uint8_t relay;
    uint8_t wifi_ssid[PARAM_BUFSIZE];
    uint8_t wifi_pwd[PARAM_BUFSIZE];
    uint8_t mqtt_server[PARAM_BUFSIZE];
    uint16_t mqtt_port;
    uint8_t mqtt_username[PARAM_BUFSIZE];
    uint8_t mqtt_password[PARAM_BUFSIZE];
    uint8_t device_id[PARAM_BUFSIZE];
    uint8_t pub_topic[PARAM_BUFSIZE];
    uint8_t device_sub_topic[PARAM_BUFSIZE];
    uint8_t device_pub_topic[PARAM_BUFSIZE];
    uint16_t count_down_minute;
    uint8_t ap_ssid[PARAM_BUFSIZE];
    uint8_t ap_pwd[PARAM_BUFSIZE];
  } Entry_t;

  Entry_t cur_data;
  Entry_t last_data;
  Entry_t def_data;

  uint8_t param_size[PARAM_NUM] = {sizeof(cur_data.relay),
                                   sizeof(cur_data.wifi_ssid),
                                   sizeof(cur_data.wifi_pwd),
                                   sizeof(cur_data.mqtt_server),
                                   sizeof(cur_data.mqtt_port),
                                   sizeof(cur_data.mqtt_username),
                                   sizeof(cur_data.mqtt_password),
                                   sizeof(cur_data.device_id),
                                   sizeof(cur_data.pub_topic),
                                   sizeof(cur_data.device_sub_topic),
                                   sizeof(cur_data.device_pub_topic),
                                   sizeof(cur_data.count_down_minute),
                                   sizeof(cur_data.ap_ssid),
                                   sizeof(cur_data.ap_pwd)};
#pragma pack()

  void Init();
  /**
   * @brief 加载参数,如果参数第一次初始化,则加载默认参数
   *
   */
  void LoadAllParam();
  uint8_t WriteParam(uint8_t id, char *strbuf, uint8_t buflen);
  template <typename T, typename std::enable_if<std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value || std::is_same<T, uint32_t>::value ||
                                                std::is_same<T, int8_t>::value || std::is_same<T, int16_t>::value || std::is_same<T, int32_t>::value>::type * = nullptr>
  uint8_t WriteParam(uint8_t id, T intval)
  {
    uint8_t buf[PARAM_BUFSIZE];
    memset(buf, 0, sizeof(buf));
    sprintf((char *)buf, "%d", intval);
    param_tab.WriteParam(id, (uint8_t *)buf, strlen((char *)buf) + 1);

    return 0;
  }

  /**
   * @brief
   *
   * @tparam T it type must be uint8_t and uint16_t and uint32_t
   * @tparam std::enable_if<std::is_same<T, uint8_t>::value ||
   * std::is_same<T, uint16_t>::value ||
   * std::is_same<T, uint32_t>::value>::type
   * @param id
   * @param intval
   * @return uint8_t
   */
  template <typename T, typename std::enable_if<std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value || std::is_same<T, uint32_t>::value ||
                                                std::is_same<T, int8_t>::value || std::is_same<T, int16_t>::value || std::is_same<T, int32_t>::value>::type * = nullptr>
  uint8_t ReadParam(uint8_t id, T *intval)
  {
    uint8_t rbuf[PARAM_BUFSIZE];
    memset(rbuf, 0, sizeof(rbuf));
    uint8_t len = param_tab.ReadParam(id, rbuf, sizeof(T));
    if (len > 0)
    {
      sscanf((char *)rbuf, "%d", intval);
    }
    return len;
  }
  uint8_t ReadParam(uint8_t id, char *strbuf, uint8_t buflen);
  void ResetAllParamToDefault();
  void ResetParamToDefault(uint8_t id);
  String ToString(Entry_t *entry);
  void UpdateParam_pool();

private:
  ParamTable param_tab;
  uint8_t *GetDefParamAddr(uint8_t id);
  uint8_t *GetLastParamAddr(uint8_t id);
  uint8_t *GetCurParamAddr(uint8_t id);
  uint8_t GetParamSize(uint8_t id);
};
/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */

#endif // __PARAM_H_
