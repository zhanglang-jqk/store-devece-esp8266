/***********************************************************************
 * @file digit_out.h
 * DIGIT_OUT
 * @author :	ch
 * @brief  :  1. 该文件是一个抽象类，用于定义数字输出设备的接口
 * @version:	v2
 * @note   :
 *          1. fix toggle issue
 * @Copyright (C)  2024-07-18  .cdWFVCEL. all right reserved
 ***********************************************************************/

#ifndef __DIGIT_OUT_H_
#define __DIGIT_OUT_H_
/* include ----------------------------------------------------------------------------------------------------------------- */
#include "include.h"
#include "Arduino.h"
/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */

class DOut
{
public:
  typedef enum
  {
    ON_POLARITY_LOW = 0,
    ON_POLARITY_HIGH,
  } OnPolarity_et;

  virtual void Init(uint8_t pin, OnPolarity_et on_polarity)
  {
    pinMode(pin, OUTPUT);
    this->on_polarity = on_polarity;
    this->pin = pin;
    this->state = !on_polarity; // 初始化为关闭状态
    // Off();  // 确保初始状态为关闭
  }

  inline virtual bool On()
  {
    tpf("relay on");
    digitalWrite(pin, on_polarity);
    state = on_polarity;
    return state;
  }

  inline virtual bool Off()
  {
    tpf("relay off");
    digitalWrite(pin, !on_polarity);
    state = !on_polarity;
    return state;
  }

  inline virtual void Set(bool state) { state ? On() : Off(); }

  inline virtual bool Toggle()
  {
    tpf("state: %d", state);
    // state == on_polarity ? Off() : On();
    if (state == on_polarity)
      Off();
    else
      On();
    return state;
  }
  /**
   * @brief 获取继电器状态
   *
   * @return true 继电器打开
   * @return false 继电器关闭
   */
  inline virtual bool GetState()
  {
    if (state == on_polarity)
      return true;
    else
      return false;
  }

private:
  uint8_t pin;
  OnPolarity_et on_polarity;
  bool state; // 改为 bool 类型
};

/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */

#endif // __DIGIT_OUT_H_f
