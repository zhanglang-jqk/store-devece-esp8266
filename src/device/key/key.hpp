/***********************************************************************
 * @file key.hpp
 * KEY
 * @author :	ch
 * @brief  :
 * @version:	v1
 * @Copyright (C)  2024-07-22  .cdWFVCEL. all right reserved
 ***********************************************************************/

#ifndef __KEY_H_
#define __KEY_H_
/* include ----------------------------------------------------------------------------------------------------------------- */
#include "include.h"
/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */

class Key
{
public:
  /* typedef -------------------- */
  typedef enum
  {
    CLICK,
    DOUBLE_CLICK,
    LONG_CLICK,
  } KeyEvent_et;

  enum
  {
    DEFAULT_LONG_TIME_MS = 2000,       // more than 5000ms mean is long press
    DEFAULT_SHORT_PRESS_TIME_MS = 500, // less than 1000ms mean is short press
    // DEFAULT_DOUBLE_INTERVAL_TIME_MS = 200
  };

  typedef enum
  {
    KEY_LOW = 0, //
    KEY_HIGH = 1
  } KeyPressState_et;

  typedef void (*pfun_t)(KeyEvent_et);

  typedef struct
  {
    uint8_t stat;
    uint8_t press_cnt; // then the Key press one times and it auto-increment 1
    uint32_t last_ms;
    const uint32_t IDENTIFY_MS = 500;
    uint32_t press_ms_st[32];

    uint32_t long_cnt;
    uint8_t long_stat;
  } FSM_t;
  /* typedef -------------------- */

  /* variable -------------------- */
  uint32_t long_click_time_ms;
  /* variable -------------------- */

  /* function -------------------- */
  void Init(uint8_t pin, uint8_t mode, pfun_t pfun, KeyPressState_et key_press_stat, uint32_t long_click_time_ms = DEFAULT_LONG_TIME_MS);
  void pool();
  /* function -------------------- */

private:
  static void KeyIdentify();
  static pfun_t app_cb; // it is pointer of user application function
  static KeyPressState_et key_press_stat;
  static uint8_t pin;
  static FSM_t fsm;
};

/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */

#endif // __KEY_H_
