/***********************************************************************
 * @file main.h
 * MAIN
 * @author :	ch
 * @brief  :
 * @version:	v1
 * @Copyright (C)  2024-10-23  .cdWFVCEL. all right reserved
 ***********************************************************************/

#ifndef __MAIN_H_
#define __MAIN_H_
/* include ----------------------------------------------------------------------------------------------------------------- */
// #include "typedef.h"
#include <stdint.h>
#include <string.h>
#include <Arduino.h>
/* macro definition -------------------------------------------------------------------------------------------------------- */

#define LED_PIN 13
#define RELAY_PIN 4
#define KEY_PIN 5

#define GSTR_SIZE 1024
#define TMPBUF_SIZE 1024
#define JSON_DOC_SIZE 1024

/* type definition --------------------------------------------------------------------------------------------------------- */

/* variable declaration ---------------------------------------------------------------------------------------------------- */
extern uint8_t g_tmpbuf[TMPBUF_SIZE];
extern String g_str;

/* function declaration ---------------------------------------------------------------------------------------------------- */

#endif // __MAIN_H_
