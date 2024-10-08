/***********************************************************************
 * @file 	:	param_tab.cpp
 * @author	:	CH
 * @brief	:
 * @Copyright (C)  2024-07-21  .cdWFVCEL. all right reserved
 ***********************************************************************/

/* include ----------------------------------------------------------------------------------------------------------------- */
#include "include.h"
#include "param_tab.hpp"
#include <EEPROM.h>
#include "param.hpp"
#include <ESP8266WiFi.h>
/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */

/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */

/* variable definition ----------------------------------------------------------------------------------------------------- */

/* function implementation ------------------------------------------------------------------------------------------------- */
void ParamTable::Init(uint8_t valid_params_num)
{
  EEPROM.begin(EEPROM_SIZE);
}

void ParamTable::WriteParam(uint8_t id, uint8_t *wbuf, uint8_t len)
{
  if (len > MAX_BYTE_OF_PARAM)
  {
    return;
  }

  uint8_t checksum = Checksum(wbuf, len);

  for (uint16_t i = 0; i < len + 3; i++)
  {
    if (i == 0) // 数据头,固定为 0x7f
    {
      EEPROM.write(id * MAX_BYTE_OF_PARAM + i, 0x7f);
    }
    else if (i == 1) // 数据长度
    {
      EEPROM.write(id * MAX_BYTE_OF_PARAM + i, len);
    }
    else if (i - 2 < len) // 有效数据
    {
      EEPROM.write(id * MAX_BYTE_OF_PARAM + i, wbuf[i - 2]);
    }
    else if (i == len + 2) // 数据尾,校验和
    {
      EEPROM.write(id * MAX_BYTE_OF_PARAM + i, checksum);
    }
  }
  EEPROM.commit();
}

int8_t ParamTable::ReadParam(uint8_t id, uint8_t *rbuf, uint8_t rlen)
{
  if (rlen > MAX_BYTE_OF_PARAM)
  {
    return -1;
  }

  // tp();
  uint8_t checksum = 0;
  uint8_t len = 0;
  // tpf("id=%d", id);
  // tpf("read_id=%d", id * MAX_BYTE_OF_PARAM);
  uint8_t item_len = EEPROM.read(id * MAX_BYTE_OF_PARAM + 1) + 3;
  if (item_len > MAX_BYTE_OF_PARAM)
  {
    return -2;
  }
  // tpf("rlen=%d", item_len);
  for (uint16_t i = 0; i < item_len; i++)
  {
    uint8_t data = EEPROM.read(id * MAX_BYTE_OF_PARAM + i);
    // tpf("data=%02x ", data);
    if (i == 0)
    {
      if (data != 0x7f)
        return false;
    }
    else if (i == 1)
    {
      len = data;
    }
    else if (i - 2 < len)
    {
      rbuf[i - 2] = data;
      // tpf("rbuf[%d]=%02x", i - 2, rbuf[i - 2]);
    }
    else if (i == item_len - 1)
    {
      checksum = data;
    }
  }

  if (checksum != Checksum(rbuf, len))
  {
    return -3;
  }

  return len;
}

uint8_t ParamTable::Checksum(uint8_t *buf, uint8_t len)
{
  uint32_t checksum = 0;
  for (uint8_t i = 0; i < len; i++)
  {
    checksum += buf[i];
  }
  return checksum & 0x000000ff;
}

bool ParamTable::IsEmpty(uint8_t id)
{
#if 0
  uint8_t tbuf[MAX_BYTE_OF_PARAM];
  memset(tbuf, 0, sizeof(tbuf));

  for (uint8_t i = 0; i < MAX_BYTE_OF_PARAM; i++)
  {
    tbuf[i] = EEPROM.read(id * MAX_BYTE_OF_PARAM + i);
  }

  for (uint8_t i = 0; i < MAX_BYTE_OF_PARAM; i++)
  {
    if (tbuf[i] != 0xff)
    {
      return false;
    }
  }
  return true;
#endif
  uint8_t tbuf[MAX_BYTE_OF_PARAM];
  memset(tbuf, 0, sizeof(tbuf));
  int8_t isok = ReadParam(id, tbuf, MAX_BYTE_OF_PARAM);
  // tpf("isok=%d", isok);
  if (isok <= 0)
  {
    // tp();
    return true;
  }
  // tp();
  return false;
}

// param_tab.cpp