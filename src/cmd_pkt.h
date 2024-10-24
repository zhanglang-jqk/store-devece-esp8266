/***********************************************************************
 * @file cmd_pkt.h
 * CMD_PKT
 * @author :	ch
 * @brief  :
 * @version:	v1
 * @Copyright (C)  2024-07-28  .cdWFVCEL. all right reserved
 ***********************************************************************/

#ifndef __CMD_PKT_H_
#define __CMD_PKT_H_
/* include ----------------------------------------------------------------------------------------------------------------- */
// #include "typedef.h"
/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */
class CmdPkt
{
public:
  static inline const char *OPEN_RELAY_CMD = "open_relay_cmd";
  static inline const char *CLOSE_RELAY_CMD = "close_relay_cmd";
  static inline const char *SET_RELAY_CMD = "set_relay_cmd";
  static inline const char *TOGGLE_RELAY_CMD = "toggle_relay_cmd";
  static inline const char *FACTORY_PARAMS_CMD = "factory_params_cmd";
  static inline const char *RESTART_CMD = "restart_cmd";

  static inline const char *SET_MQTT_SERVER_CMD = "set_mqtt_server_cmd";
  static inline const char *SET_MQTT_PORT_CMD = "set_mqtt_port_cmd";
  static inline const char *SET_WIFI_SSID_CMD = "set_wifi_ssid_cmd";
  static inline const char *SET_WIFI_PWD_CMD = "set_wifi_pwd_cmd";
  static inline const char *SET_MQTT_USERNAME_CMD = "set_mqtt_username_cmd";
  static inline const char *SET_MQTT_PASSWORD_CMD = "set_mqtt_password_cmd";
  // static inline const char *SET_COUNT_DOWN_CMD = "set_countdown_cmd";
  // static inline const char *SET_OPEN_DOOR_MODE_CMD = "set_open_door_mode_cmd";
  // static inline const char *SET_AUTO_CLOSE_DOOR_TIME_CMD = "set_auto_close_door_time_cmd";
  static inline const char *SET_DEVECE_ID_CMD = "set_device_id_cmd";
  static inline const char *SET_SOFT_VER_CMD = "set_soft_ver_cmd";
  static inline const char *SET_HARD_VER_CMD = "set_hard_ver_cmd";
  static inline const char *SET_DEVICE_GLOBAL_SUB_TOPIC_CMD = "set_device_global_sub_topic_cmd";
  static inline const char *SET_DEVICE_GLOBAL_PUB_TOPIC_CMD = "set_device_global_pub_topic_cmd";

  static inline const char *GET_SYSTEM_INFO_CMD = "get_system_info_cmd";

  void GetCmdNo(char *cmd_no, uint8_t *payload, uint16_t payload_len)
  {
    char *payload_str[payload_len + 1];
    memset(payload_str, 0, sizeof(payload_str));
    memcpy(payload_str, payload, payload_len);
    sscanf((char *)payload_str, R"({"%[^"]")", cmd_no);
  }

  void ExtractStrOfBuf(char *cmdbuf, uint16_t cmdbuf_len, char *strbuf)
  {
    char buf[cmdbuf_len + 1];
    memset(buf, 0, sizeof(buf)), memcpy(buf, cmdbuf, cmdbuf_len);
    // tpf("cmdbuf:%s", buf);
    // sscanf((char *)buf, R"({"%*[^:]:"%s"})", strbuf);
    sscanf((char *)buf, R"({"%*[^:]: "%[^"]"})", strbuf);
    // tpf("strbuf:%s", strbuf);
  };

  uint8_t ExtractGbkHexStrOfBuf(char *cmdbuf, uint16_t cmdbuf_len, uint8_t *gbkbuf)
  {
    char inbuf[cmdbuf_len + 1];
    memset(inbuf, 0, sizeof(inbuf)), memcpy(inbuf, cmdbuf, cmdbuf_len);
    uint8_t gbkhex_buf[32];
    memset(gbkhex_buf, 0, sizeof(gbkhex_buf));

    tpf("cmdbuf:%s", inbuf);
    sscanf((char *)inbuf, R"({"%*[^:]: "%[^"]"})", gbkhex_buf);
    tpf("%s", gbkhex_buf);

    uint8_t byte_buf[3];
    memset(byte_buf, 0, sizeof(byte_buf));
    for (int i = 0; i < strlen((char *)gbkhex_buf) / 2; i++)
    {
      memcpy(byte_buf, gbkhex_buf + i * 2, 2);
      uint8_t hex = std::stoi((char *)byte_buf, nullptr, 16);
      gbkbuf[i] = hex;
    }
    tpf("strbuf:%s", gbkbuf);
    tpbuf(gbkbuf, strlen((char *)gbkhex_buf) / 2);

    return strlen((char *)gbkhex_buf) / 2;
  };

  void ExtractIntOfBuf(char *cmdbuf, uint16_t cmdbuf_len, uint16_t *intbuf)
  {
    char buf[cmdbuf_len + 1];
    memset(buf, 0, sizeof(buf)), memcpy(buf, cmdbuf, cmdbuf_len);
    tpf("cmdbuf:%s", buf);
    sscanf((char *)buf, R"({"%*[^:]:%d})", intbuf);
  };

  void ExtractIntOfBuf(char *cmdbuf, uint16_t cmdbuf_len, uint8_t *intbuf)
  {
    char buf[cmdbuf_len + 1];
    memset(buf, 0, sizeof(buf)), memcpy(buf, cmdbuf, cmdbuf_len);
    tpf("cmdbuf:%s", buf);
    sscanf((char *)buf, R"({"%*[^:]:%d})", intbuf);
  };

  void BuildJsonPkt(char *json_str, char *cmd_no, char *cmd_str) { sprintf((char *)json_str, R"({"%s":"%s"})", cmd_no, cmd_str); }
  void BuildJsonPkt(char *json_str, char *json_key, uint8_t json_val) { sprintf((char *)json_str, R"({"%s":%d})", json_key, json_val); }
  /* variable declaration ---------------------------------------------------------------------------------------------------- */

  /* function declaration ---------------------------------------------------------------------------------------------------- */
};

#endif // __CMD_PKT_H_
