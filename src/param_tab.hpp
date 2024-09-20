/***********************************************************************
 * @file param_tab.hpp
 * PARAM_TAB
 * @author :	ch
 * @brief  :  存储用户数据
 *            1. 数据存储,以下格式存储, 0x7f(1字节) + datalen(数据长度,1字节) + databuf(数据缓冲区) + checksum(1字节)
 *            2. 数据总长度不超过32字节,有效数据长度不超过30字节,数据头为0x7f,数据尾为checksum
 * @version:	v1
 * @Copyright (C)  2024-07-21  .cdWFVCEL. all right reserved
 ***********************************************************************/

#ifndef __PARAM_TAB_H_
#define __PARAM_TAB_H_
/* include ----------------------------------------------------------------------------------------------------------------- */
#include <stdint.h>
/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */

class ParamTable
{
public:
  enum
  {
    MAX_BYTE_OF_PARAM = 48,                         // 保存数据的最大长度
    EEPROM_SIZE = 1024,                             // eeprom的大小
    MAX_PARAM_NUM = EEPROM_SIZE / MAX_BYTE_OF_PARAM // 最大参数个数
  };

  void Init(uint8_t valid_params_num);
  void WriteParam(uint8_t id, uint8_t *wbuf, uint8_t wlen);
  /**
   * @brief read param according to id
   *
   * @param id
   * @param rbuf read buffer that save read data
   * @param rlen read buffer length
   * @return int8_t positive value is read data length, negative value is error
   */
  int8_t ReadParam(uint8_t id, uint8_t *rbuf, uint8_t rlen);
  /**
   * @brief 是否从未写入过数据
   *
   * @param id
   * @return true
   * @return false
   */
  bool IsEmpty(uint8_t id);
  uint8_t Checksum(uint8_t *buf, uint8_t len);

private:
};

/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */

#endif // __PARAM_TAB_H_

// 我需要做的功能是
// 1. 需要保存的数据,以数据
// 2. 将eeprom 划分为一定的空间,索引的形式保存数据
// 3. 保存的数据,以校验和结尾