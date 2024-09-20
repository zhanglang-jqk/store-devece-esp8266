/***********************************************************************
 * @file 	:	key.cpp
 * @author	:	CH
 * @brief	:
 * @Copyright (C)  2024-07-23  .cdWFVCEL. all right reserved
 ***********************************************************************/

/* include ----------------------------------------------------------------------------------------------------------------- */
#include "include.h"
#include "key.hpp"

/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */

/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */

/* variable definition ----------------------------------------------------------------------------------------------------- */
uint8_t Key::pin = 0;                                                       // 初始化为 0
Key::KeyPressState_et Key::key_press_stat = Key::KeyPressState_et::KEY_LOW; // 初始化为 0
Key::pfun_t Key::app_cb = nullptr;                                          // 初始化为 nullptr
Key::FSM_t Key::fsm;
/* function implementation ------------------------------------------------------------------------------------------------- */
void Key::Init(uint8_t pin, uint8_t mode, pfun_t pfun, KeyPressState_et key_press_stat, uint32_t long_click_time_ms)
{
  pinMode(pin, mode);
  attachInterrupt(digitalPinToInterrupt(pin), Key::KeyIdentify, FALLING);
  Key::app_cb = pfun;
  Key::pin = pin;
}

void ICACHE_RAM_ATTR Key::KeyIdentify()
{
  // /* identify action type -------------------- */
  // static struct
  // {
  //   uint8_t stat;
  //   uint32_t key_press_st, key_release_st; // timestamp of key press, timestamp of key long release
  //   KeyEvent_et key_result;
  // } ctrl_fsm;

  // ctrl_fsm.key_result = KeyEvent_et::CLICK;

  // if (ctrl_fsm.stat == 0)
  // {
  //   if (digitalRead(Key::pin) == Key::key_press_stat)
  //   {
  //     ctrl_fsm.key_press_st = millis();
  //     ctrl_fsm.stat = 1;
  //   }
  // }
  // if (ctrl_fsm.stat == 1) // 判断长按还是短按
  // {
  //   volatile uint8_t aa = digitalRead(pin);
  //   // while (digitalRead(pin) == (uint8_t)Key::key_press_stat)
  //   // {
  //   //   delay(1);
  //   ESP.wdtFeed();
  //   // }
  //   ctrl_fsm.key_release_st = millis();

  //   if (ctrl_fsm.key_release_st - ctrl_fsm.key_press_st >= Key::DEFAULT_LONG_TIME_MS)
  //   {
  //     ctrl_fsm.key_result = KeyEvent_et::LONG_CLICK;
  //   }
  // }

  // app_cb(ctrl_fsm.key_result);
  // fsm.press_cnt = 1;
  // fsm.press_us_st[fsm.press_cnt++] = micros64();
  // fsm.press_us_st[fsm.press_cnt++] = micros();
  // tpf("timestamp:%d,press_cnt:%d", fsm.press_ms_st, fsm.press_cnt);

  // delay(100);
  // if (digitalRead(Key::pin) == (uint8_t)Key::key_press_stat)
  // {
  fsm.press_ms_st[fsm.press_cnt++] = millis();
  if (fsm.press_cnt >= 32)
  {
    fsm.press_cnt = 0;
  }
  // }

  // logf("%d \n", millis());
}

void Key::pool()
{
  if (fsm.stat == 0)
  {
    if (fsm.press_cnt)
    {
      fsm.last_ms = millis();
      fsm.stat = 1;
    }
  }
  if (fsm.stat == 1)
  {
    if (millis() - fsm.last_ms >= fsm.IDENTIFY_MS) // 按键被按下100ms后,识别按键的模式
    {
      for (int i = 0; i < fsm.press_cnt; i++)
      {
        tpf("index:%d, timestamp:%d", i, fsm.press_ms_st[i]);
      }

      if ((fsm.press_cnt >= 1) && (digitalRead(pin) != Key::key_press_stat))
      {
        fsm.stat = 2;
      }
      // else if (fsm.press_cnt > 1)
      // {
      //   fsm.stat = 3;
      // }

      else if ((fsm.press_cnt >= 1) && (digitalRead(pin) == Key::key_press_stat)) // 100ms后,按键还在按下状态,进行长按识别
      {
        fsm.stat = 4;
      }
    }
    // }
  }
  if (fsm.stat == 2)
  {
    app_cb(CLICK);
    // tpf("key short press");
    fsm.stat = 5;
  }
  // if (fsm.stat == 3) // 双击识别
  // {
  //   app_cb(DOUBLE_CLICK);
  //   fsm.stat = 5;
  // }
  if (fsm.stat == 4) // 长按识别
  {
    // wait for key release
    if (digitalRead(pin) == Key::key_press_stat)
    {
      logf(".");
    }
    else
    {
      ln();
      app_cb(LONG_CLICK);
      // tpf("key long press");
      fsm.stat = 5;
    }
  }

  if (fsm.stat == 5) // clean about fsm flags
  {
    // memset(&fsm, 0, sizeof(fsm));
    fsm.stat = 0;
    fsm.press_cnt = 0;
    fsm.last_ms = 0;
    for (int i = 0; i < 32; i++)
    {
      fsm.press_ms_st[i] = 0;
    }
  }
}

// key.cpp