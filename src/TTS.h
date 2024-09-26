/***********************************************************************
 * @file TTS.h
 * TTS
 * @author :	ch
 * @brief  :
 * @version:	v1
 * @Copyright (C)  2024-07-28  .cdWFVCEL. all right reserved
 ***********************************************************************/

#ifndef __TTS_H_
#define __TTS_H_
/* include ----------------------------------------------------------------------------------------------------------------- */
// #include "typedef.h"
#include <Arduino.h>
// #include <SoftwareSerial.h>
#include <Bonezegei_SoftSerial.h>
#include "debug_log.h"
/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */
class TTS
{
public:
  typedef enum
  {
    BUSYSTATUS_BUSY = 0,
    BUSYSTATUS_IDLE,
    BUSYSTATUS_ERROR,

    PLAY_VOICE_OK,
    PALY_VOICE_BUSY,
    PALY_VOICE_ASK_ERR, // ???,????
  } BusyStatus_et;

  // TTS(SoftwareSerial &serial) : serial(serial)
  // {
  // }

  void Init(Bonezegei_SoftSerial *serial) { this->serial = serial; }

  BusyStatus_et SendPlayVoice_cmd(uint8_t *buf, uint16_t len)
  {
    const uint16_t WAIT_READ_CH_MS = 500;

    // TODO: 由于软件串口的问题，这里可能会读取到错误的数据,待优化
    // { // send query busy status cmd
    //   uint8_t sendbuf[4] = {0xfd, 0x00, 0x01, 0x21};
    //   serial->write(sendbuf, sizeof(sendbuf));
    //   tpbuf(sendbuf, sizeof(sendbuf));
    //   uint8_t ret_byte = serial->read(WAIT_READ_CH_MS);
    //   tpf("%x", ret_byte);
    //   // if (ret_byte == 0x4f)
    //   if (ret_byte == 0xfd) // 由于软件串口的问题，这里可能会读取到错误的数据,待优化
    //   {
    //     // tts is idle
    //     Serial.println("tts is idle");
    //   }
    //   else if (ret_byte == 0x4e)
    //   {
    //     // tts is busy
    //     Serial.println("tts is busy");
    //     return PALY_VOICE_BUSY;
    //   }
    //   else
    //   {
    //     // tts is error
    //     Serial.println("tts is error");
    //     return PALY_VOICE_ASK_ERR;
    //   }
    // }

    { // send voice cmd
      uint16_t sendbuf_len = 5 + len;
      uint8_t sendbuf[sendbuf_len];
      memset(sendbuf, 0, sizeof(sendbuf));
      uint16_t frame_len = 2 + len;
      sendbuf[0] = 0xfd;
      sendbuf[1] = frame_len >> 8;
      sendbuf[2] = frame_len & 0xff;
      sendbuf[3] = 0x01;
      sendbuf[4] = 0x01;
      memcpy(sendbuf + 5, buf, len);
      // FD 00 06 01 01 C4 E3 BA C3;
      // uint8_t sendbuf[] = {0xfd, 0x00, 0x06, 0x01, 0x01, 0xc4, 0xe3, 0xba, 0xc3};
      // uint8_t sendbuf_len = sizeof(sendbuf);
      serial->write(sendbuf, sendbuf_len);
      tpbuf(sendbuf, sendbuf_len);

      return PLAY_VOICE_OK;

      // uint8_t ret = serial->read(WAIT_READ_CH_MS); // TODO: 由于软件串口的问题，这里可能会读取到错误的数据,待优化
      // if (ret == 0x41)
      // {
      //   Serial.println("tts is ok");
      //   return PLAY_VOICE_OK;
      // }
      // else
      // {
      //   Serial.println("tts is error");
      //   return PALY_VOICE_ASK_ERR;
      // }
    }
  }

private:
  // HardwareSerial &serial;
  // SoftwareSerial *serial;
  Bonezegei_SoftSerial *serial;
};
/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */

#endif // __TTS_H_
