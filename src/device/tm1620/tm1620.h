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

#define MAX_TUBE_BRIGHTNESS 0x02 // 固定亮度设置
// TM1620_DATA寄存器
#define AUTO_ADDR_MODE 0x00  // 地址自增模式
#define FIXED_ADDR_MODE 0x04 // 固定地址模式

/* type definition --------------------------------------------------------------------------------------------------------- */
typedef enum
{
  TM1620_DISP_MODE = 0x0, // 显示模式设置
  TM1620_DATA = 0x1,      // 数据命令设置
  TM1620_BRIGHT = 0x2,    // 显示控制命令设置
  TM1620_ADDR = 0x3       // 地址命令设置
} TM1620_CMD;

class TM1620
{
public:
  enum
  {
    SEG_ROD = 21, // 显示“-”
    SEG_NOT_DISPLAY = 23,
    SEG_COLON = 20, // 显示“:”,仅在第二位显示
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
  void TurnOffDispaly(); // TODO 无法关闭显示

private:
  uint8_t stb_pin;
  uint8_t clk_pin;
  uint8_t dio_pin;
};
/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */

#endif // __TM1620_H_
