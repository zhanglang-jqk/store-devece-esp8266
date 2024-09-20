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
  static inline char *MODIFY_MQTT_SERVER_CMD = "cmd1";
  static inline char *MODIFY_WIFI_SSID_CMD = "cmd2";
  static inline char *MODIFY_WIFI_PWD_CMD = "cmd3";
  static inline char *MODIFY_MQTT_USERNAME_CMD = "cmd4";
  static inline char *MODIFY_MQTT_PASSWORD_CMD = "cmd5";
  static inline char *OPEN_RELAY_CMD = "cmd6";
  static inline char *CLOSE_RELAY_CMD = "cmd7";
  static inline char *GET_RELAY_STATUS_CMD = "cmd8";
  static inline char *GET_DEVICE_ID_CMD = "cmd9";
  static inline char *MODIFY_MQTT_PORT_CMD = "cmd10";
  static inline char *PLAY_VOICE = "cmd11";
  static inline char *RELAY_STATUS_CMD = "cmd12"; // 继电器状态,用于上报
  static inline char *SET_COUNT_DOWN = "cmd13";   // 设置倒计时
  static inline char *GET_COUNT_DOWN = "cmd14";   // 获取倒计时
  static inline char *OPEN_COUNT_DOWN = "cmd15";  // 打开倒计时
  static inline char *CLOSE_COUNT_DOWN = "cmd16"; // 关闭倒计时

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
    tpf("cmdbuf:%s", buf);
    // sscanf((char *)buf, R"({"%*[^:]:"%s"})", strbuf);
    sscanf((char *)buf, R"({"%*[^:]:"%[^"]"})", strbuf);
    tpf("strbuf:%s", strbuf);
  };

  uint8_t ExtractGbkHexStrOfBuf(char *cmdbuf, uint16_t cmdbuf_len, uint8_t *gbkbuf)
  {
    char inbuf[cmdbuf_len + 1];
    memset(inbuf, 0, sizeof(inbuf)), memcpy(inbuf, cmdbuf, cmdbuf_len);
    uint8_t gbkhex_buf[32];
    memset(gbkhex_buf, 0, sizeof(gbkhex_buf));

    // tpf("cmdbuf:%s", inbuf);
    sscanf((char *)inbuf, R"({"%*[^:]:"%[^"]"})", gbkhex_buf);
    // tpf("%s", gbkhex_buf);

    uint8_t byte_buf[3];
    memset(byte_buf, 0, sizeof(byte_buf));
    for (int i = 0; i < strlen((char *)gbkhex_buf) / 2; i++)
    {
      memcpy(byte_buf, gbkhex_buf + i * 2, 2);
      uint8_t hex = std::stoi((char *)byte_buf, nullptr, 16);
      gbkbuf[i] = hex;
    }
    // tpf("strbuf:%s", gbkbuf);
    // tpbuf(gbkbuf, strlen((char *)gbkhex_buf) / 2);

    return strlen((char *)gbkhex_buf) / 2;
  };

  void ExtractIntOfBuf(char *cmdbuf, uint16_t cmdbuf_len, uint16_t *intbuf)
  {
    char buf[cmdbuf_len + 1];
    memset(buf, 0, sizeof(buf)), memcpy(buf, cmdbuf, cmdbuf_len);
    tpf("cmdbuf:%s", buf);
    sscanf((char *)buf, R"({"%*[^:]:%d})", intbuf);
  };

  void BuildJsonPkt(char *json_str, char *cmd_no, char *cmd_str) { sprintf((char *)json_str, R"({"%s":"%s"})", cmd_no, cmd_str); }
  /* variable declaration ---------------------------------------------------------------------------------------------------- */

  /* function declaration ---------------------------------------------------------------------------------------------------- */
};
#endif // __CMD_PKT_H_
