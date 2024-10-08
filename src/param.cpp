/***********************************************************************
 * @file 	:	param_man.cpp
 * @author	:	CH
 * @brief	:
 * @Copyright (C)  2024-07-22  .cdWFVCEL. all right reserved
 ***********************************************************************/

/* include ----------------------------------------------------------------------------------------------------------------- */
#include "include.h"
#include "param.hpp"

/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */

/* variable declaration ---------------------------------------------------------------------------------------------------- */
extern uint8_t g_tmpbuf[1024];
/* function declaration ---------------------------------------------------------------------------------------------------- */

/* variable definition ----------------------------------------------------------------------------------------------------- */

/* function implementation ------------------------------------------------------------------------------------------------- */
void Param::Init()
{
  param_tab.Init(PARAM_NUM);

  def_data.relay = DEF_RELAY;
  strcpy((char *)def_data.wifi_ssid, DEF_WIFI_SSID);
  strcpy((char *)def_data.wifi_pwd, DEF_WIFI_PWD);
  strcpy((char *)def_data.mqtt_server, DEF_MQTT_SERVER);
  def_data.mqtt_port = DEF_MQTT_PORT;
  strcpy((char *)def_data.mqtt_username, DEF_MQTT_USERNAME);
  strcpy((char *)def_data.mqtt_password, DEF_MQTT_PASSWORD);
  // strcpy((char *)def_data.device_id, DEF_DEVICE_ID);
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  strcpy((char *)def_data.device_id, mac.c_str());

  strcpy((char *)def_data.device_global_pub_topic, DEF_DEVICE_GLOBAL_PUB_TOPIC);
  strcpy((char *)def_data.device_global_sub_topic, DEF_DEVICE_GLOBAL_SUB_TOPIC);

  String def_devece_sub_topic = String(DEF_DEVICE_ID) + "/" + DEF_DEVICE_SUB_TOPIC;
  strcpy((char *)def_data.device_sub_topic, def_devece_sub_topic.c_str());

  // strcpy((char *)def_data.device_pub_topic, DEF_DEVICE_PUB_TOPIC);
  String def_devece_pub_topic = String(DEF_DEVICE_ID) + "/" + DEF_DEVICE_PUB_TOPIC;
  strcpy((char *)def_data.device_pub_topic, def_devece_pub_topic.c_str());

  def_data.count_down_minute = DEF_COUNT_DOWN;
  strcpy((char *)def_data.ap_ssid, DEF_AP_SSID);
  strcpy((char *)def_data.ap_pwd, DEF_AP_PWD);
  def_data.open_door_mode = DEF_OPEN_DOOR_MODE;
  def_data.auto_close_door_time_s = DEF_AUTO_CLOSE_DOOR_TIME_S;
  def_data.soft_ver = DEF_SOFT_VERSION;
  def_data.hard_ver = DEF_HARD_VERSION;
}

void Param::LoadAllParam()
{
  for (int id = 0; id < Param::PARAM_NUM; id++) // read all param from eeprom
  {
    bool is_empty = param_tab.IsEmpty(id);
    if (is_empty)
    {
      tp();
      ResetParamToDefault(id);
    }
    uint8_t *p_param = GetCurParamAddr(id);
    uint8_t param_size = GetParamSize(id);

    param_tab.ReadParam(id, p_param, param_size);
  }
  { // save last data
    memset(&last_data, 0, sizeof(last_data));
    memcpy(&last_data, &cur_data, sizeof(cur_data));
  }
}

String Param::ToString(Entry_t *entry)
{
  String str = "Param:\n";
  str += "relay=", str += String(entry->relay), str += "\n";
  str += "wifi_ssid=", str += (char *)entry->wifi_ssid, str += "\n";
  str += "wifi_pwd=", str += (char *)entry->wifi_pwd, str += "\n";
  str += "mqtt_server=", str += (char *)entry->mqtt_server, str += "\n";
  str += "mqtt_port=", str += String(entry->mqtt_port), str += "\n";
  str += "mqtt_username=", str += (char *)entry->mqtt_username, str += "\n";
  str += "mqtt_password=", str += (char *)entry->mqtt_password, str += "\n";
  str += "device_id=", str += (char *)entry->device_id, str += "\n";
  str += "device_global_pub_topic=", str += (char *)entry->device_global_pub_topic, str += "\n";
  str += "device_global_sub_topic=", str += (char *)entry->device_global_sub_topic, str += "\n";
  str += "device_sub_topic=", str += (char *)entry->device_sub_topic, str += "\n";
  str += "device_pub_topic=", str += (char *)entry->device_pub_topic, str += "\n";
  str += "count_down=", str += String(entry->count_down_minute), str += "\n";
  str += "ap_ssid=", str += (char *)cur_data.ap_ssid, str += "\n";
  str += "ap_pwd=", str += (char *)cur_data.ap_pwd, str += "\n";
  str += "open_door_mode=", str += String(entry->open_door_mode), str += "\n";
  str += "auto_close_door_time_s=", str += String(entry->auto_close_door_time_s), str += "\n";
  str += "soft_ver=", str += String(entry->soft_ver), str += "\n";
  str += "hard_ver=", str += String(entry->hard_ver), str += "\n";
  return str;
}

void Param::ResetAllParamToDefault()
{
  for (int id = 0; id < Param::PARAM_NUM; id++)
  {
    ResetParamToDefault(id);
  }
  // { // save last data
  //   memset(&last_data, 0, sizeof(last_data));
  //   memcpy(&last_data, &cur_data, sizeof(cur_data));
  // }
}

uint8_t *Param::GetDefParamAddr(uint8_t id)
{
  uint16_t offset = 0;
  for (int i = 0; i < id; i++)
  {
    offset += param_size[i];
  }
  return (uint8_t *)&def_data + offset;
}

uint8_t Param::GetParamSize(uint8_t id)
{
  return param_size[id];
}

void Param::ResetParamToDefault(uint8_t id)
{
  uint8_t *def_param_addr = GetDefParamAddr(id);
  uint8_t param_size = GetParamSize(id);
  if (id == DEVICE_ID)
  {
    String device_id_str = WiFi.macAddress();
    device_id_str.replace(":", "");
    memset(def_param_addr, 0, param_size);
    strcpy((char *)def_param_addr, device_id_str.c_str());
  }
  if (id == DEVICE_SUB_TOPIC_ID || id == DEVICE_PUB_TOPIC_ID)
  {
    String mac = WiFi.macAddress();
    mac.replace(":", "");

    String topic_str;
    if(id == DEVICE_SUB_TOPIC_ID)
      topic_str = mac + "/" + DEF_DEVICE_SUB_TOPIC;
    else if(id == DEVICE_PUB_TOPIC_ID)
      topic_str = mac + "/" + DEF_DEVICE_PUB_TOPIC;

    memset(def_param_addr, 0, param_size);
    strncpy((char*)def_param_addr, topic_str.c_str(), param_size - 1);
    def_param_addr[param_size - 1] = '\0'; // Ensure null-termination
  }
  // if (id == 12 || id == 13)
  // {
  //   tpf("def_data address:%p", &def_data);
  //   tpf("def_param_addr:%p, param_size:%d", def_param_addr, param_size);
  //   tpbuf(def_param_addr, param_size);
  // }

  // param_tab.WriteParam(id, def_param_addr, param_size);

  uint16_t offset = def_param_addr - (uint8_t *)&def_data;
  uint8_t *cur_param_addr = (uint8_t *)&cur_data + offset;
  // if (id == 12 || id == 13)
  // {
  //   tpf("offset:%d", offset);
  //   tpf("cur_param_addr:%p", cur_param_addr);
  // }
  memcpy(cur_param_addr, def_param_addr, param_size);
  // tpbuf(cur_param_addr, param_size);
  param_tab.WriteParam(id, def_param_addr, param_size);
}

uint8_t Param::WriteParam(uint8_t id, char *strbuf, uint8_t buflen)
{
  uint8_t _strlen = strlen(strbuf);
  if (strbuf[_strlen] != '\0')
  {
    tp();
    return 1;
  }

  param_tab.WriteParam(id, (uint8_t *)strbuf, _strlen + 1);

  return 0;
}

uint8_t Param::ReadParam(uint8_t id, char *strbuf, uint8_t buflen)
{
  uint8_t len = param_tab.ReadParam(id, (uint8_t *)strbuf, buflen);

  return len;
}

uint8_t *Param::GetCurParamAddr(uint8_t id)
{
  uint16_t offset = 0;
  for (int i = 0; i < id; i++)
  {
    offset += param_size[i];
  }
  return (uint8_t *)&cur_data + offset;
}

uint8_t *Param::GetLastParamAddr(uint8_t id)
{
  uint16_t offset = 0;
  for (int i = 0; i < id; i++)
  {
    offset += param_size[i];
  }
  return (uint8_t *)&last_data + offset;  
}

void Param::UpdateParam_pool()
{
  uint8_t isok = memcmp(&cur_data, &last_data, sizeof(cur_data));
  if (isok != 0)
  {
    tpf("before param:"), tpt(ToString(&last_data));
    for (int id = 0; id < Param::PARAM_NUM; id++)
    {
      uint8_t *p_cur_param = GetCurParamAddr(id);
      uint8_t *p_last_param = GetLastParamAddr(id);
      uint8_t param_size = GetParamSize(id);
      if (memcmp(p_cur_param, p_last_param, param_size) != 0)
      {
        // tp();
        param_tab.WriteParam(id, p_cur_param, param_size);
        // tpbuf(p_cur_param, param_size);
      }
    }
    
    tpf("update param:"), tpt(ToString(&cur_data));
    memcpy(&last_data, &cur_data, sizeof(cur_data));
  }
}