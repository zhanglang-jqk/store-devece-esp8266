/***********************************************************************
 * @file 	:	modbus.cpp
 * @author	:	CH
 * @brief	:
 * @Copyright (C)  2024-07-16  .cdWFVCEL. all right reserved
 ***********************************************************************/

/* include ------------------------------------------------------------------------------------------------- */
#include "include.h"
#include "modbus.hpp"
/* macro definition ------------------------------------------------------------------------------------------------- */
/* type definition ------------------------------------------------------------------------------------------------- */
/* variable declaration ------------------------------------------------------------------------------------------------- */
/* function declaration ------------------------------------------------------------------------------------------------- */
/* variable definition ------------------------------------------------------------------------------------------------- */
/* function implementation ------------------------------------------------------------------------------------------------- */

void ModbusSlave::poll()
{
  if (_serial.available())
  {
    uint16_t cnt = _serial.readBytes(g_tmp_buf, sizeof(g_tmp_buf)); // get a data frame
    // for (int i = 0; i < cnt; i++)
    //   Serial.printf("%02x ", recv_buf[i]);

    uint8_t addr = g_tmp_buf[0];
    uint8_t fcode = g_tmp_buf[1];

    if (addr == 0x01 && fcode == 0x03) // read mutimul register
    {
      { // 接收数据解析
        // 接收数据格式
        // 设备地址（1字节）：表示请求的设备的地址。
        // 功能码（1字节）：表示执行的功能，对于0x03功能码的请求，这个值应该是0x03。
        // 起始地址（2字节）：表示要读取的第一个寄存器的地址，按照Big-Endian（高字节在前）的顺序存储。
        // 寄存器数量（2字节）：表示要读取的寄存器的数量，按照Big-Endian的顺序存储。
        // CRC校验（2字节）：表示前面所有字节的CRC校验值，按照Little-Endian（低字节在前）的顺序存储。
        uint16_t start_ret = g_tmp_buf[2] << 8 | g_tmp_buf[3];
        uint8_t reg_cnt = g_tmp_buf[4] << 8 | g_tmp_buf[5];
        uint16_t recv_crc = g_tmp_buf[6] << 8 | g_tmp_buf[7];

        uint16_t calc_crc = modbus_crc16(g_tmp_buf, 6);

        if (recv_crc != calc_crc)
        {
          Serial.printf("recv_crc:%04x, calc_crc:%04x\r\n", recv_crc, calc_crc);
          goto error;
        }
      }

      { // 响应数据
        // paramer.Load();
        String jstr = paramer.GetParamsJsonStr();

        // 响应报文格式
        // 设备地址（1字节）：表示响应的设备的地址
        // 功能码（1字节）：表示执行的功能，对于03功能码的应答，这个值应该是03。
        // 字节计数（2字节）：表示接下来的数据字节的数量。扩展为2字节,标准modbus协议为1字节
        // 数据（n字节）：表示读取的寄存器的值，每个寄存器的值占2字节，按照Big-Endian（高字节在前）的顺序存储。
        // CRC校验（2字节）：表示前面所有字节的CRC校验值，按照Little-Endian（低字节在前）的顺序存储。

        uint8_t addr = 0x01;
        uint8_t fcode = 0x03;
        uint16_t byte_size = MB_DATASIZE;

        uint16_t idx = 0;
        memset(g_tmp_buf, 0, sizeof(g_tmp_buf));
        g_tmp_buf[idx++] = addr, g_tmp_buf[idx++] = fcode, g_tmp_buf[idx++] = (byte_size >> 8) & 0x00ff, g_tmp_buf[idx++] = byte_size;
        memcpy(&g_tmp_buf[idx], jstr.c_str(), jstr.length()), idx += jstr.length();
        uint16_t crc = modbus_crc16(g_tmp_buf, idx);
        g_tmp_buf[idx++] = crc, g_tmp_buf[idx++] = crc >> 8;

        _serial.write(g_tmp_buf, MB_DATASIZE);
      }
    }
    else if (addr == 0x01 && fcode == 0x10) // write mutimul register
    {
      // tpln();
      { // 接收数据
        // 接收数据格式
        // 设备地址（1字节）：表示请求的设备的地址。
        // 功能码（1字节）：表示执行的功能，对于0x10功能码的请求，这个值应该是0x10。
        // 起始地址（2字节）：表示要写入的第一个寄存器的地址，按照Big-Endian（高字节在前）的顺序存储。
        // 寄存器数量（2字节）：表示要写入的寄存器的数量，按照Big-Endian的顺序存储。
        // 字节计数（2字节）：表示接下来的数据字节的数量。扩展为2字节,标准modbus协议为1字节
        // 数据（n字节）：表示要写入的寄存器的值，每个寄存器的值占2字节，按照Big-Endian的顺序存储。
        // CRC校验（2字节）：表示前面所有字节的CRC校验值，按照Little-Endian（低字节在前）的顺序存储。

        // 接收数据解析
        uint16_t start_ret = g_tmp_buf[2] << 8 | g_tmp_buf[3];
        uint8_t reg_cnt = g_tmp_buf[4] << 8 | g_tmp_buf[5];
        uint16_t byte_cnt = g_tmp_buf[6] << 8 | g_tmp_buf[7];

        uint16_t recv_crc = g_tmp_buf[8 + byte_cnt] << 8 | g_tmp_buf[8 + byte_cnt + 1];

        uint16_t calc_crc = modbus_crc16(g_tmp_buf, 1 + 1 + 2 + 2 + 2 + byte_cnt);

        if (recv_crc != calc_crc)
        {
          Serial.printf("recv_crc:%04x, calc_crc:%04x\r\n", recv_crc, calc_crc);
          goto error;
        }
      }
      { // 写数据

        uint8_t *payload = g_tmp_buf + 8;
        uint8_t len = g_tmp_buf[6];

        DeserializationError err = deserializeJson(tmp_jdoc, payload);
        if (err)
        {
          tpf(F("deserializeJson() failed: "));
          // err.f_str();
          // tpfln(err.f_str());
          // Serial.printf(paramer);
          tpfln(err.f_str());
          goto error;
        }
        if (tmp_jdoc["drive_no"].as<String>() != "")
        {
          paramer.drive_no = tmp_jdoc["drive_no"].as<String>();
        }
        if (tmp_jdoc["ssid"].as<String>() != "") paramer.ssid = tmp_jdoc["ssid"].as<String>();
        if (tmp_jdoc["password"].as<String>() != "") paramer.password = tmp_jdoc["password"].as<String>();
        if (tmp_jdoc["mqtt_server"].as<String>() != "") paramer.mqtt_server = tmp_jdoc["mqtt_server"].as<String>();
        if (tmp_jdoc["mqtt_port"].as<String>() != "") paramer.mqtt_port = tmp_jdoc["mqtt_port"].as<uint16_t>();
        if (tmp_jdoc["group_ctrl_topic"].as<String>() != "") paramer.group_ctrl_topic = tmp_jdoc["group_ctrl_topic"].as<String>();
        if (tmp_jdoc["group_status_topic"].as<String>() != "") paramer.group_status_topic = tmp_jdoc["group_status_topic"].as<String>();
        if (tmp_jdoc["mqtt_username"].as<String>() != "") paramer.mqtt_username = tmp_jdoc["mqtt_username"].as<String>();
        if (tmp_jdoc["mqtt_password"].as<String>() != "") paramer.mqtt_password = tmp_jdoc["mqtt_password"].as<String>();

        paramer.Save();
        // paramer.Print();
      }
      { // 响应报文
        // 设备地址（1字节）：表示响应的设备的地址。
        // 功能码（1字节）：表示执行的功能，对于0x10功能码的响应，这个值应该是0x10。
        // 起始地址（2字节）：表示写入的第一个寄存器的地址，按照Big-Endian（高字节在前）的顺序存储。
        // 寄存器数量（2字节）：表示写入的寄存器的数量，按照Big-Endian的顺序存储。
        // CRC校验（2字节）：表示前面所有字节的CRC校验值，按照Little-Endian（低字节在前）的顺序存储

        memset(g_tmp_buf, 0, sizeof(g_tmp_buf));
        // g_tmp_buf[0] = 0x01, g_tmp_buf[1] = 0x10, g_tmp_buf[2] = 0x0, g_tmp_buf[3] = 0x0;
        uint8_t addr = 0x01, fcode = 0x10, start_ret = 0x0, reg_cnt = 127;
        g_tmp_buf[0] = addr, g_tmp_buf[1] = fcode, g_tmp_buf[2] = (start_ret >> 8), g_tmp_buf[3] = start_ret, g_tmp_buf[4] = (reg_cnt >> 8), g_tmp_buf[5] = reg_cnt;
        uint16_t crc = modbus_crc16(g_tmp_buf, 6);
        g_tmp_buf[6] = crc, g_tmp_buf[7] = crc >> 8;
        // for (int i = 0; i < 8; i++)
        //   Serial.printf("%02x ", g_tmp_buf[i]);
        _serial.write(g_tmp_buf, 8);
      }
    }
    else // error, 解析报文失败,响应失败报文
    {
    error:
      tpln();
      // 设备地址（1字节）：表示响应的设备的地址。
      // 功能码（1字节）：表示执行的功能，这个值应该是原请求的功能码 + 0x80。例如，如果请求的功能码是0x03，那么异常响应的功能码应该是0x83。
      // 异常码（1字节）：表示异常的类型。常见的异常码有：
      // 0x01：非法功能码，表示请求的功能码设备不支持。
      // 0x02：非法数据地址，表示请求的数据地址不存在。
      // 0x03：非法数据值，表示请求的数据值不合法。
      // 0x04：设备故障，表示设备在处理请求时发生了故障。
      // CRC校验（2字节）：表示前面所有字节的CRC校验值，按照Little-Endian（低字节在前）的顺序存储。
      uint8_t addr = 0x01;                          // 设备地址
      uint8_t _fcode = fcode | 0x80;                // 功能码
      uint8_t excode = 0x04;                        // 异常码，非法数据地址
      uint8_t buf[5] = {addr, fcode, excode, 0, 0}; // 初始化缓冲区
      uint16_t crc = modbus_crc16(buf, 3);          // 计算CRC校验值
      buf[3] = crc;                                 // 存储CRC校验值的低字节
      buf[4] = crc >> 8;                            // 存储CRC校验值的高字节
    }
  }

  return;
}

ModbusSlave::ModbusSlave(HardwareSerial &serial) : _serial(serial)
{
  // 构造函数的实现
  _serial.setTimeout(200);
}

// @brief 计算 modbus crc16 (LSB)
uint16_t ModbusSlave::modbus_crc16(uint8_t *buf, uint16_t len)
{
  uint16_t crc = 0xFFFF;
  for (uint16_t pos = 0; pos < len; pos++)
  {
    crc ^= (uint16_t)buf[pos]; // XOR byte into least sig. byte of crc

    for (int i = 8; i != 0; i--)
    { // Loop over each bit
      if ((crc & 0x0001) != 0)
      {            // If the LSB is set
        crc >>= 1; // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else         // Else LSB is not set
        crc >>= 1; // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;
}
// modbus.cpp