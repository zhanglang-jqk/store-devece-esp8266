/***********************************************************************
 * @file debug.h
 * DEBUG
 * @author :	ch
 * @brief  :
 * @version:	v1.0
 * @Copyright (C)  2024-02-02  .cdWFVCEL. all right reserved
 ***********************************************************************/

#ifndef ___DEBUG_H_
#define ___DEBUG_H_
/* include -------------------------------------------------------------------------------------------------*/
#include <Arduino.h>

/* macro definition -------------------------------------------------------------------------------------------------*/

#define DEBUG_PRINT 1 // 1: enable debug, 0: disable debug

#define logf(...) Serial.printf(__VA_ARGS__) // 0个,1个,多个参数

#define tp() Serial.printf("%s:%d: \n", __FILE__, __LINE__)
#define tpf(msg, ...) Serial.printf("%s:%d: " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__) // todo 待实现,合并到tp()中
#define tpt(obj) Serial.printf("%s:%d: ", __FILE__, __LINE__), Serial.print(obj), Serial.println()
#define tpbuf(buf, len)               \
  do                                  \
  {                                   \
    logf("%s:%d: ", __FILE__, __LINE__); \
    for (int i = 0; i < len; i++)     \
    {                                 \
      Serial.printf("%02x ", buf[i]); \
    }                                 \
    Serial.println();                 \
  } while (0)

#define ln() Serial.println()

/* type definition -------------------------------------------------------------------------------------------------*/

/* variable declaration -------------------------------------------------------------------------------------------------*/

/* function declaration -------------------------------------------------------------------------------------------------*/

#endif // __DEBUG_H_
