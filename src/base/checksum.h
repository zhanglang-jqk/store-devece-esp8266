/***********************************************************************
 * @file checksum.h
 * CHECKSUM
 * @author :	ch
 * @brief  :
 * @version:	v1
 * @Copyright (C)  2024-10-22  .cdWFVCEL. all right reserved
 ***********************************************************************/

#ifndef __CHECKSUM_H_
#define __CHECKSUM_H_
/* include ----------------------------------------------------------------------------------------------------------------- */
// #include "typedef.h"
/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */

/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */
/**
 * @brief Calculate CRC16 checksum
 * @param buf Pointer to the data buffer
 * @param len Length of the data buffer
 * @return Calculated CRC16 checksum
 */
uint16_t crc16(const uint8_t *buf, size_t len);

#endif // __CHECKSUM_H_
