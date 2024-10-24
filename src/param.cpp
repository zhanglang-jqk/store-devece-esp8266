/***********************************************************************
 * @file 	:	param_man.cpp
 * @author	:	CH
 * @brief	:
 * @Copyright (C)  2024-07-22  .cdWFVCEL. all right reserved
 ***********************************************************************/

/* include ----------------------------------------------------------------------------------------------------------------- */
#include "include.h"
#include "param.hpp"
#include "EEPROM.h"

/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */

/* variable declaration ---------------------------------------------------------------------------------------------------- */
extern uint8_t g_tmpbuf[1024];
/* function declaration ---------------------------------------------------------------------------------------------------- */

/* variable definition ----------------------------------------------------------------------------------------------------- */

/* function implementation ------------------------------------------------------------------------------------------------- */
void Param::Init()
{
  EEPROM.begin(EEPROM_SIZE);
}

void Param::LoadAllParam()
{
  EEPROM.get(EEPROM_ADDRESS, cur_data);

  uint16_t recv_crc16 = cur_data.crc16;
  cur_data.crc16 = 0;
  uint16_t calc_crc16 = crc16((uint8_t *)&cur_data, sizeof(cur_data));
  tpf("recv_crc16:%x,calc_crc16:%x", recv_crc16, calc_crc16);

  if (recv_crc16 != calc_crc16)
  {
    tpf("crc16 is not match, reset all param to default");
    ResetAllParamToDefault();
    EEPROM.get(EEPROM_ADDRESS, cur_data);
  }

  memcpy((uint8_t *)&last_data, (uint8_t *)&cur_data, sizeof(cur_data));
}

String Param::ToString(Entry_t *entry)
{
  String str = "Param:\n";
  str += "soft_ver=" + String(entry->soft_ver) + "\n";
  str += "hard_ver=" + String(entry->hard_ver) + "\n";
  str += "protocol_ver=" + String(entry->protocol_ver) + "\n";
  str += "build_datetime=" + String((char *)entry->build_datetime) + "\n";
  str += "param_ver=" + String(entry->param_ver) + "\n";
  str += "relay=" + String(entry->relay) + "\n";
  str += "wifi_ssid=" + String((char *)entry->wifi_ssid) + "\n";
  str += "wifi_pwd=" + String((char *)entry->wifi_pwd) + "\n";
  str += "mqtt_server=" + String((char *)entry->mqtt_server) + "\n";
  str += "mqtt_port=" + String(entry->mqtt_port) + "\n";
  str += "mqtt_username=" + String((char *)entry->mqtt_username) + "\n";
  str += "mqtt_password=" + String((char *)entry->mqtt_password) + "\n";
  str += "device_id=" + String((char *)entry->device_id) + "\n";
  str += "device_global_pub_topic=" + String((char *)entry->device_global_pub_topic) + "\n";
  str += "device_global_sub_topic=" + String((char *)entry->device_global_sub_topic) + "\n";
  str += "device_sub_topic=" + String((char *)entry->device_sub_topic) + "\n";
  str += "device_pub_topic=" + String((char *)entry->device_pub_topic) + "\n";
  str += "ap_ssid=" + String((char *)entry->ap_ssid) + "\n";
  str += "ap_pwd=" + String((char *)entry->ap_pwd) + "\n";
  return str;
}

void Param::ResetAllParamToDefault()
{
  String device_id_str = WiFi.macAddress();
  device_id_str.replace(":", "");
  strcpy((char *)def_data.device_id, device_id_str.c_str());

  String device_pub_topic_str = device_id_str + "/" + String((char *)def_data.device_pub_topic);
  strcpy((char *)def_data.device_pub_topic, device_pub_topic_str.c_str());

  String device_sub_topic_str = device_id_str + "/" + String((char *)def_data.device_sub_topic);
  strcpy((char *)def_data.device_sub_topic, device_sub_topic_str.c_str());

  memcpy(&cur_data, &def_data, sizeof(def_data));
  uint16_t _crc16 = crc16((uint8_t *)&cur_data, sizeof(cur_data));
  cur_data.crc16 = _crc16;

  EEPROM.put(EEPROM_ADDRESS, cur_data);
  EEPROM.commit();
}

void Param::UpdateParam_pool()
{
  uint8_t isok = memcmp(&cur_data, &last_data, sizeof(cur_data));
  if (isok != 0)
  {
    // for (int id = 0; id < Param::PARAM_NUM; id++)
    // {
    //   uint8_t *p_cur_param = GetCurParamAddr(id);
    //   uint8_t *p_last_param = GetLastParamAddr(id);
    //   uint8_t param_size = GetParamSize(id);
    //   if (memcmp(p_cur_param, p_last_param, param_size) != 0)
    //   {
    //     tpf("Param %d updated:", id);
    //     tpbuf(p_cur_param, param_size);
    //     param_tab.WriteParam(id, p_cur_param, param_size);
    //   }
    // }

    WriteParams();
    tpt(ToString(&cur_data));

    memcpy(&last_data, &cur_data, sizeof(cur_data));
  }
}
void Param::UpdateVerParam_pool() // @brief Update parameters when version info in "version.h" changes
{
  bool ver_changed = false;

  // Check if soft_ver has changed
  if (cur_data.soft_ver != SOFT_VER)
  {
    cur_data.soft_ver = SOFT_VER;
    ver_changed = true;
  }

  // Check if hard_ver has changed
  if (cur_data.hard_ver != HARDWARE_VER)
  {
    cur_data.hard_ver = HARDWARE_VER;
    ver_changed = true;
  }

  // Check if protocol_ver has changed
  if (cur_data.protocol_ver != PROTOCOL_VER)
  {
    cur_data.protocol_ver = PROTOCOL_VER;
    ver_changed = true;
  }

  // Check if build_datetime_ver has changed
  const char *currentBuildDateTime = GetBuildDateTime();
  if (strcmp((const char *)cur_data.build_datetime, currentBuildDateTime) != 0)
  {
    strncpy((char *)cur_data.build_datetime, currentBuildDateTime, sizeof(cur_data.build_datetime) - 1);
    cur_data.build_datetime[sizeof(cur_data.build_datetime) - 1] = '\0'; // Ensure null-termination
    ver_changed = true;
  }

  // Check if param_ver has changed
  if (cur_data.param_ver != PARAM_VER)
  {
    cur_data.param_ver = PARAM_VER;
    ver_changed = true;
  }

  // If any version info has changed, write the updated data to EEPROM
  if (ver_changed)
  {
    WriteParams();
    tpf("Version parameters updated and written to EEPROM");
    tpt(ToString(&cur_data));
  }

  if (ver_changed)
  {
    memcpy(&last_data, &cur_data, sizeof(cur_data));
  }
}

void Param::WriteParams()
{
  cur_data.crc16 = 0;
  cur_data.crc16 = crc16((uint8_t *)&cur_data, sizeof(cur_data));
  EEPROM.put(EEPROM_ADDRESS, cur_data);
  EEPROM.commit();
}
