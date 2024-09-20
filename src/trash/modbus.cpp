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
      { // �������ݽ���
        // �������ݸ�ʽ
        // �豸��ַ��1�ֽڣ�����ʾ������豸�ĵ�ַ��
        // �����루1�ֽڣ�����ʾִ�еĹ��ܣ�����0x03��������������ֵӦ����0x03��
        // ��ʼ��ַ��2�ֽڣ�����ʾҪ��ȡ�ĵ�һ���Ĵ����ĵ�ַ������Big-Endian�����ֽ���ǰ����˳��洢��
        // �Ĵ���������2�ֽڣ�����ʾҪ��ȡ�ļĴ���������������Big-Endian��˳��洢��
        // CRCУ�飨2�ֽڣ�����ʾǰ�������ֽڵ�CRCУ��ֵ������Little-Endian�����ֽ���ǰ����˳��洢��
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

      { // ��Ӧ����
        // paramer.Load();
        String jstr = paramer.GetParamsJsonStr();

        // ��Ӧ���ĸ�ʽ
        // �豸��ַ��1�ֽڣ�����ʾ��Ӧ���豸�ĵ�ַ
        // �����루1�ֽڣ�����ʾִ�еĹ��ܣ�����03�������Ӧ�����ֵӦ����03��
        // �ֽڼ�����2�ֽڣ�����ʾ�������������ֽڵ���������չΪ2�ֽ�,��׼modbusЭ��Ϊ1�ֽ�
        // ���ݣ�n�ֽڣ�����ʾ��ȡ�ļĴ�����ֵ��ÿ���Ĵ�����ֵռ2�ֽڣ�����Big-Endian�����ֽ���ǰ����˳��洢��
        // CRCУ�飨2�ֽڣ�����ʾǰ�������ֽڵ�CRCУ��ֵ������Little-Endian�����ֽ���ǰ����˳��洢��

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
      { // ��������
        // �������ݸ�ʽ
        // �豸��ַ��1�ֽڣ�����ʾ������豸�ĵ�ַ��
        // �����루1�ֽڣ�����ʾִ�еĹ��ܣ�����0x10��������������ֵӦ����0x10��
        // ��ʼ��ַ��2�ֽڣ�����ʾҪд��ĵ�һ���Ĵ����ĵ�ַ������Big-Endian�����ֽ���ǰ����˳��洢��
        // �Ĵ���������2�ֽڣ�����ʾҪд��ļĴ���������������Big-Endian��˳��洢��
        // �ֽڼ�����2�ֽڣ�����ʾ�������������ֽڵ���������չΪ2�ֽ�,��׼modbusЭ��Ϊ1�ֽ�
        // ���ݣ�n�ֽڣ�����ʾҪд��ļĴ�����ֵ��ÿ���Ĵ�����ֵռ2�ֽڣ�����Big-Endian��˳��洢��
        // CRCУ�飨2�ֽڣ�����ʾǰ�������ֽڵ�CRCУ��ֵ������Little-Endian�����ֽ���ǰ����˳��洢��

        // �������ݽ���
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
      { // д����

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
      { // ��Ӧ����
        // �豸��ַ��1�ֽڣ�����ʾ��Ӧ���豸�ĵ�ַ��
        // �����루1�ֽڣ�����ʾִ�еĹ��ܣ�����0x10���������Ӧ�����ֵӦ����0x10��
        // ��ʼ��ַ��2�ֽڣ�����ʾд��ĵ�һ���Ĵ����ĵ�ַ������Big-Endian�����ֽ���ǰ����˳��洢��
        // �Ĵ���������2�ֽڣ�����ʾд��ļĴ���������������Big-Endian��˳��洢��
        // CRCУ�飨2�ֽڣ�����ʾǰ�������ֽڵ�CRCУ��ֵ������Little-Endian�����ֽ���ǰ����˳��洢

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
    else // error, ��������ʧ��,��Ӧʧ�ܱ���
    {
    error:
      tpln();
      // �豸��ַ��1�ֽڣ�����ʾ��Ӧ���豸�ĵ�ַ��
      // �����루1�ֽڣ�����ʾִ�еĹ��ܣ����ֵӦ����ԭ����Ĺ����� + 0x80�����磬�������Ĺ�������0x03����ô�쳣��Ӧ�Ĺ�����Ӧ����0x83��
      // �쳣�루1�ֽڣ�����ʾ�쳣�����͡��������쳣���У�
      // 0x01���Ƿ������룬��ʾ����Ĺ������豸��֧�֡�
      // 0x02���Ƿ����ݵ�ַ����ʾ��������ݵ�ַ�����ڡ�
      // 0x03���Ƿ�����ֵ����ʾ���������ֵ���Ϸ���
      // 0x04���豸���ϣ���ʾ�豸�ڴ�������ʱ�����˹��ϡ�
      // CRCУ�飨2�ֽڣ�����ʾǰ�������ֽڵ�CRCУ��ֵ������Little-Endian�����ֽ���ǰ����˳��洢��
      uint8_t addr = 0x01;                          // �豸��ַ
      uint8_t _fcode = fcode | 0x80;                // ������
      uint8_t excode = 0x04;                        // �쳣�룬�Ƿ����ݵ�ַ
      uint8_t buf[5] = {addr, fcode, excode, 0, 0}; // ��ʼ��������
      uint16_t crc = modbus_crc16(buf, 3);          // ����CRCУ��ֵ
      buf[3] = crc;                                 // �洢CRCУ��ֵ�ĵ��ֽ�
      buf[4] = crc >> 8;                            // �洢CRCУ��ֵ�ĸ��ֽ�
    }
  }

  return;
}

ModbusSlave::ModbusSlave(HardwareSerial &serial) : _serial(serial)
{
  // ���캯����ʵ��
  _serial.setTimeout(200);
}

// @brief ���� modbus crc16 (LSB)
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