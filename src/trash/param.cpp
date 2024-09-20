
/***********************************************************************
 * @file 	:	param.cpp
 * @author	:	CH
 * @brief	:	
 * @Copyright (C)  2024-07-15  .cdWFVCEL. all right reserved
***********************************************************************/

/* include ------------------------------------------------------------------------------------------------- */
#include "include.h"
#include "param.h"
#include "ESP8266WiFi.h"
/* macro definition ------------------------------------------------------------------------------------------------- */
/* type definition ------------------------------------------------------------------------------------------------- */
/* variable declaration ------------------------------------------------------------------------------------------------- */
/* function declaration ------------------------------------------------------------------------------------------------- */
/* variable definition ------------------------------------------------------------------------------------------------- */
/* function implementation ------------------------------------------------------------------------------------------------- */
// Paramer class implement -------------------------------------------------------------------------------------------------
Paramer::Paramer()
{
  // 构造函数的实现
}
void Paramer::ResetParamToDefault()
{
  jdoc.clear();

  String mac_addr = WiFi.macAddress();
  mac_addr.replace(":", "");
  jdoc["drive_no"] = mac_addr.c_str();
  jdoc["ssid"] = "88888888";
  jdoc["password"] = "88888888";
  jdoc["mqtt_server"] = "ee836835.ala.cn-hangzhou.emqxsl.cn";
  jdoc["mqtt_port"] = 8883;
  jdoc["mqtt_username"] = "dianzha202451";
  jdoc["mqtt_password"] = "zhangguile";
  jdoc["group_ctrl_topic"] = "group_control";
  jdoc["group_status_topic"] = "group_status";
  jdoc["pub_ctrl_topic"] = "public_control";
  jdoc["pub_status_topic"] = "public_status";
  jdoc["ctrl_topic"] = jdoc["drive_no"].as<String>() + "/" + "control";
  jdoc["status_topic"] = jdoc["drive_no"].as<String>() + "/" + "status";
  jdoc["modify_config_topic"] = jdoc["drive_no"].as<String>() + "/" + "modify_config";
  jdoc["read_config_topic"] = jdoc["drive_no"].as<String>() + "/" + "read_config";
  jdoc["relay_s"] = false;

  drive_no = jdoc["drive_no"].as<String>();
  ssid = jdoc["ssid"].as<String>();
  password = jdoc["password"].as<String>();
  mqtt_server = jdoc["mqtt_server"].as<String>();
  mqtt_port = jdoc["mqtt_port"].as<uint16_t>();
  mqtt_username = jdoc["mqtt_username"].as<String>();
  mqtt_password = jdoc["mqtt_password"].as<String>();
  group_ctrl_topic = jdoc["group_ctrl_topic"].as<String>();
  group_status_topic = jdoc["group_status_topic"].as<String>();
  pub_ctrl_topic = jdoc["pub_ctrl_topic"].as<String>();
  pub_status_topic = jdoc["pub_status_topic"].as<String>();
  ctrl_topic = jdoc["ctrl_topic"].as<String>();
  status_topic = jdoc["status_topic"].as<String>();
  modify_config_topic = jdoc["modify_config_topic"].as<String>();
  read_config_topic = jdoc["read_config_topic"].as<String>();
  relay_s = jdoc["relay_s"].as<bool>();

  Save();
}

void Paramer::Load()
{
  EEPROM.begin(MB_DATASIZE);
  // uint16_t datalen = 0;
  // datalen = EEPROM.get(0, datalen);
  uint16_t datalen = EEPROM.read(0) << 8 | EEPROM.read(1);
  Serial.printf("datalen:%d\r\n", datalen);
  if (datalen == 0xffff) // first time run
  {
    tpfln("no data in eeprom");
    ResetParamToDefault();
    // Save();
  }
  else if (datalen > MB_DATASIZE)
  {
    tpfln("eeprom data error,reset to default");
    ResetParamToDefault();
    // Save();
  }
  else
  {
    // uint8_t g_tmp_buf[datalen];
    memset(g_tmp_buf, 0, sizeof(g_tmp_buf));
    for (int i = 0; i < datalen; i++)
      g_tmp_buf[i] = EEPROM.read(i + 2);

    // tpfln((char *)g_tmp_buf);

    DeserializationError err = deserializeJson(jdoc, g_tmp_buf);

    if (err)
    {
      uint8_t recnt = 5; // 重试次数
      for (; recnt > 0; recnt--)
      {
        datalen = EEPROM.read(0) << 8 | EEPROM.read(1);
        memset(g_tmp_buf, 0, sizeof(g_tmp_buf));
        for (int i = 0; i < datalen; i++)
          g_tmp_buf[i] = EEPROM.read(i + 2);

        tpfln("retry %d", recnt);
        tpfln((char *)g_tmp_buf);

        err = deserializeJson(jdoc, g_tmp_buf);

        if (!err)
          break;
      }
      if (recnt == 0)
      {
        tpf(F("deserializeJson() failed: "));
        tpfln(err.f_str());
        tpln();
        ResetParamToDefault();
      }

      // Save();
    }
    else
    {
      // tpln();
      drive_no = jdoc["drive_no"].as<String>();
      ssid = jdoc["ssid"].as<String>();
      password = jdoc["password"].as<String>();
      mqtt_server = jdoc["mqtt_server"].as<String>();
      mqtt_port = jdoc["mqtt_port"].as<uint16_t>();
      mqtt_username = jdoc["mqtt_username"].as<String>();
      mqtt_password = jdoc["mqtt_password"].as<String>();
      group_ctrl_topic = jdoc["group_ctrl_topic"].as<String>();
      group_status_topic = jdoc["group_status_topic"].as<String>();
      pub_ctrl_topic = jdoc["pub_ctrl_topic"].as<String>();
      pub_status_topic = jdoc["pub_status_topic"].as<String>();
      ctrl_topic = jdoc["ctrl_topic"].as<String>();
      status_topic = jdoc["status_topic"].as<String>();
      modify_config_topic = jdoc["modify_config_topic"].as<String>();
      read_config_topic = jdoc["read_config_topic"].as<String>();
      relay_s = jdoc["relay_s"].as<bool>();
    }
  }
  EEPROM.end();
}

// 将 doc 转换成字符串
// String json_str;
void Paramer::Save()
{
  EEPROM.begin(MB_DATASIZE);

  jdoc["drive_no"] = drive_no;
  jdoc["ssid"] = ssid;
  jdoc["password"] = password;
  jdoc["mqtt_server"] = mqtt_server;
  jdoc["mqtt_port"] = mqtt_port;
  jdoc["mqtt_username"] = mqtt_username;
  jdoc["mqtt_password"] = mqtt_password;
  jdoc["group_ctrl_topic"] = group_ctrl_topic;
  jdoc["pub_ctrl_topic"] = pub_ctrl_topic;
  jdoc["ctrl_topic"] = ctrl_topic;
  jdoc["status_topic"] = status_topic;
  jdoc["modify_config_topic"] = modify_config_topic;
  jdoc["read_config_topic"] = read_config_topic;
  jdoc["relay_s"] = relay_s;
  jdoc["group_status_topic"] = group_status_topic;

  // uint16_t writen_byte = serializeJson(jdoc, json_str);
  uint16_t writen_byte = serializeJson(jdoc, (char *)g_tmp_buf, sizeof(g_tmp_buf));
  if (writen_byte != 0)
  {
    tpfln("serializeJson success,serialize byte is %d", writen_byte);
    tpfln("save json string to eeprom,the string:%s", g_tmp_buf); // todo 偶尔打印不出来时,其实可能有接收到数据

    EEPROM.write(0, (writen_byte >> 8) & 0x00ff), EEPROM.write(1, writen_byte & 0x00ff); // the first two bytes is the length of data
    for (int i = 0; i < writen_byte; i++)
    {
      EEPROM.write(i + 2, g_tmp_buf[i]);
      ESP.wdtFeed();
    }
    EEPROM.commit();
  }
  else
    tpfln("serializeJson failed,serialize byte is %d", writen_byte);

  EEPROM.end();
}

// void Paramer::Print()
// {
//   String jstr;
//   serializeJson(jdoc, jstr);
//   tpfln(jstr);
// }

String Paramer::GetParamsJsonStr()
{
  String jstr;
  serializeJson(jdoc, jstr);
  return jstr;
}

size_t Paramer::printTo(Print &p) const
{
  size_t written = 0;
  serializeJson(jdoc, g_tmp_buf);
  written += p.print((char *)g_tmp_buf);
  // ... 打印其他成员 ...
  return written;
}
//param.cpp