/***********************************************************************
 * @file param.h
 * PARAM
 * @author :	ch
 * @brief  :	
 * @version:	v1.0
 * @Copyright (C)  2024-07-15  .cdWFVCEL. all right reserved
***********************************************************************/

#ifndef __PARAM_H_
#define __PARAM_H_
/* include -------------------------------------------------------------------------------------------------*/
// #include "typedef.h"
#include "include.h"
#include <ArduinoJson.h>

/* macro definition -------------------------------------------------------------------------------------------------*/
#define MB_DATASIZE 1100 // modbus 1帧数据最大的字节数决定,扩展为1000字节(modbus默认仅支持255字节)
/* type definition -------------------------------------------------------------------------------------------------*/
class Paramer : public Printable
{
public:
  String drive_no;
  String ssid;
  String password;
  String mqtt_server;
  uint16_t mqtt_port;
  String mqtt_username;
  String mqtt_password;
  String group_ctrl_topic;
  String group_status_topic;
  String pub_ctrl_topic;
  String pub_status_topic;
  String ctrl_topic;
  String status_topic;
  String modify_config_topic;
  String read_config_topic;
  bool relay_s; // ture: relay on, false: relay off

  Paramer();
  void ResetParamToDefault();
  void Load();
  void Save();
  // void Print();
  String GetParamsJsonStr();
  size_t printTo(Print &p) const override;
  // private:
  StaticJsonDocument<MB_DATASIZE> jdoc; // 方便保存到EEPROM中
};
/* variable declaration -------------------------------------------------------------------------------------------------*/
/* function declaration -------------------------------------------------------------------------------------------------*/

#endif // __PARAM_H_
