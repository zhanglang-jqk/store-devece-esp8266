/***********************************************************************
 * @file digit_out.h
 * DIGIT_OUT
 * @author :	ch
 * @brief  :  1. 该文件是一个抽象类，用于定义数字输出设备的接口
 * @version:	v1
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

  enum
  {
    ON_STATUS = 1,
    OFF_STATUS = 0,
  };

  virtual void Init(uint8_t pin, OnPolarity_et on_polarity)
  {
    pinMode(pin, OUTPUT);
    this->on_polarity = on_polarity;
    this->pin = pin;
  }

  inline virtual bool On()
  {
    tpf("relay on");
    this->on_polarity == ON_POLARITY_HIGH ? digitalWrite(pin, HIGH) : digitalWrite(pin, LOW);
    state = ON_STATUS;
    return state;
  }

  inline virtual bool Off()
  {
    tpf("relay off");
    this->on_polarity == ON_POLARITY_HIGH ? digitalWrite(pin, LOW) : digitalWrite(pin, HIGH);
    state = OFF_STATUS;
    return state;
  }

  inline virtual void Set(bool state) { state == ON_STATUS ? On() : Off(); }

  inline virtual bool Toggle()
  {
    state == ON_STATUS ? Off() : On();
    return state;
  }

  inline virtual bool GetPinState() { return state; }

private:
  uint8_t pin;
  OnPolarity_et on_polarity;
  uint8_t state;
};

/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */

#endif // __DIGIT_OUT_H_f
