/***********************************************************************
 * @file modbus.h
 * MODBUS
 * @author :	ch
 * @brief  :	
 * @version:	v1.0
 * @Copyright (C)  2024-07-16  .cdWFVCEL. all right reserved
***********************************************************************/

#ifndef __MODBUS_H_
#define __MODBUS_H_

/* include -------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include <Print.h>
#include <HardwareSerial.h>

/* macro definition -------------------------------------------------------------------------------------------------*/
/* type definition -------------------------------------------------------------------------------------------------*/
class ModbusSlave
{
public:
  HardwareSerial &_serial;
  // uint8_t recv_buf[MB_DATASIZE + 10]; // 1帧数据最大的字节数决定
  ModbusSlave(HardwareSerial &serial);
  // @brief 计算 modbus crc16
  uint16_t modbus_crc16(uint8_t *buf, uint16_t len);
  void poll();
};
/* variable declaration -------------------------------------------------------------------------------------------------*/
/* function declaration -------------------------------------------------------------------------------------------------*/

#endif // __MODBUS_H_
