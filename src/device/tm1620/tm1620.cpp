/***********************************************************************
 * @file 	:	tm1620.c
 * @author	:	CH
 * @brief	:
 * @Copyright (C)  2024-07-30  .cdWFVCEL. all right reserved
 ***********************************************************************/

/* include ----------------------------------------------------------------------------------------------------------------- */
// #include "include.h"
#include "Arduino.h"
#include "tm1620.h"
/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */

/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */

/* variable definition ----------------------------------------------------------------------------------------------------- */
// #if 0
uint8_t font_code[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, // 0-4
    0x6D, 0x7D, 0x07, 0x7F, 0x6F, // 5-9
    0x77, 0x7C, 0x39, 0x5E, 0x79, // A-E
    0x71, 0x73, 0x3E, 0x76, 0x38, // F,P,U,H,L
    0x80,                         // .
    0x40,                         // -
    0xFF,                         // test led
    0x00,                         // don't display
};
/* function implementation ------------------------------------------------------------------------------------------------- */
void TM1620::WriteByte(uint8_t byte)
{
  for (int i = 0; i < 8; i++)
  {
    CLK_LOW();
    if (byte & 0x1)
    {
      DIO_HIGH();
    }
    else
    {
      DIO_LOW();
    }
    byte = byte >> 1;
    delayMicroseconds(10);
    CLK_HIGH();
    // delay_ms(1);
    delayMicroseconds(10);
  }
};

void TM1620::Write(TM1620_CMD cmd, uint8_t data)
{
  switch (cmd)
  {
  case TM1620_DISP_MODE:
    STB_LOW();
    // 显示模式6位8段,0b0000_0010
    WriteByte(data & 0x03);
    STB_HIGH();
    // delay_ms(1);
    break;
  case TM1620_DATA:
    STB_LOW();
    // 数据读写模式0b0100_0000
    WriteByte(0x40 | (data & 0x7));
    STB_HIGH();
    // delay_ms(1);
    break;
  case TM1620_BRIGHT:
    STB_LOW();
    // 亮度设置0b1000_1xxx
    WriteByte(0x88 | (0x7 & data)); // TODO
                                    //		TM1620_Write_8bit(00000000x80);
    STB_HIGH();
    // delay_ms(1);
    break;
  case TM1620_ADDR: // 显示地址
    STB_LOW();
    WriteByte(0xC0 | (0xF & data));
    // delay_ms(1);
    break;
  }
}

uint8_t TM1620::GetCodeNum(uint8_t number)
{
  uint8_t _number = font_code[number];
  return _number;
}

uint8_t TM1620::DispSeg(uint8_t seg_num, uint8_t number)
{
  uint8_t _number = font_code[number];

  Write(TM1620_DISP_MODE, 0x02);       // 显示模式6位8段
  Write(TM1620_DATA, FIXED_ADDR_MODE); // 固定地址模式

  Write(TM1620_ADDR, 0x00 + seg_num * 2); // 起始地址0xC0
  WriteByte(_number);

  STB_HIGH();
  Write(TM1620_BRIGHT, MAX_TUBE_BRIGHTNESS); // 亮度设置

  return 0;
}

void TM1620::Init(uint8_t stb_pin, uint8_t clk_pin, uint8_t dio_pin)
{
  this->stb_pin = stb_pin, this->clk_pin = clk_pin, this->dio_pin = dio_pin;
  pinMode(stb_pin, OUTPUT), pinMode(clk_pin, OUTPUT), pinMode(dio_pin, OUTPUT);
  STB_HIGH(), CLK_HIGH(), DIO_HIGH();
  for (int i = 0; i < 6; i++)
    DispSeg(i, 0x20);

  // DispSegNone();
  DispSeg(0, TM1620::SEG_ROD), DispSeg(1, TM1620::SEG_ROD), DispSeg(2, TM1620::SEG_ROD), DispSeg(3, TM1620::SEG_ROD);
}

void TM1620::STB_HIGH()
{
  digitalWrite(stb_pin, HIGH);
};

void TM1620::STB_LOW()
{
  digitalWrite(stb_pin, LOW);
};

void TM1620::CLK_HIGH()
{
  digitalWrite(clk_pin, HIGH);
};

void TM1620::CLK_LOW()
{
  digitalWrite(clk_pin, LOW);
};

void TM1620::DIO_HIGH()
{
  digitalWrite(dio_pin, HIGH);
};

void TM1620::DIO_LOW()
{
  digitalWrite(dio_pin, LOW);
};

void TM1620::TurnOffDispaly()
{
  STB_LOW();
  WriteByte(0b10000000); // 发送关闭显示的命令
  STB_HIGH();
}

uint8_t TM1620::DispSegCode(uint8_t seg_num, uint8_t code)
{
  Write(TM1620_DISP_MODE, 0x02);       // 显示模式6位8段
  Write(TM1620_DATA, FIXED_ADDR_MODE); // 固定地址模式

  Write(TM1620_ADDR, 0x00 + seg_num * 2); // 起始地址0xC0
  WriteByte(code);

  STB_HIGH();
  Write(TM1620_BRIGHT, MAX_TUBE_BRIGHTNESS); // 亮度设置

  return 0;
}
// void TM1620::DispSegNone()
// {
//   for (int i = 0; i < 4; i++)
//     DispSeg(i, SEG_NOT_DISPLAY);
// }
// #endif
// tm1620.c