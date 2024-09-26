/***********************************************************************
 * @file tm1620.h
 * TM1620
 * @author :	ch
 * @brief  :
 * @version:	v1
 * @Copyright (C)  2024-07-30  .cdWFVCEL. all right reserved
 ***********************************************************************/

#ifndef __TM1620_H_
#define __TM1620_H_
/* include ----------------------------------------------------------------------------------------------------------------- */
// #include "typedef.h"
// #include "include.h"
#include "Arduino.h"
/* macro definition -------------------------------------------------------------------------------------------------------- */

#define MAX_TUBE_BRIGHTNESS 0x02 // �̶���������
// TM1620_DATA�Ĵ���
#define AUTO_ADDR_MODE 0x00  // ��ַ����ģʽ
#define FIXED_ADDR_MODE 0x04 // �̶���ַģʽ

/* type definition --------------------------------------------------------------------------------------------------------- */
typedef enum
{
  TM1620_DISP_MODE = 0x0, // ��ʾģʽ����
  TM1620_DATA = 0x1,      // ������������
  TM1620_BRIGHT = 0x2,    // ��ʾ������������
  TM1620_ADDR = 0x3       // ��ַ��������
} TM1620_CMD;

class TM1620
{
public:
  enum
  {
    SEG_ROD = 21, // ��ʾ��-��
    SEG_NOT_DISPLAY = 23,
    SEG_COLON = 20, // ��ʾ��:��,���ڵڶ�λ��ʾ
  };

  void Init(uint8_t stb_pin, uint8_t clk_pin, uint8_t dio_pin);
  void STB_HIGH();
  void STB_LOW();
  void CLK_HIGH();
  void CLK_LOW();
  void DIO_HIGH();
  void DIO_LOW();
  void WriteByte(uint8_t byte);
  void Write(TM1620_CMD cmd, uint8_t data);
  uint8_t DispSeg(uint8_t seg_num, uint8_t number);
  uint8_t DispSegCode(uint8_t seg_num, uint8_t code);
  uint8_t GetCodeNum(uint8_t number);
  void TurnOffDispaly(); // TODO �޷��ر���ʾ

private:
  uint8_t stb_pin;
  uint8_t clk_pin;
  uint8_t dio_pin;
};
/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */

#endif // __TM1620_H_
