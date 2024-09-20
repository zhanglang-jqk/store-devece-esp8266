/***********************************************************************
 * @file param_tab.hpp
 * PARAM_TAB
 * @author :	ch
 * @brief  :  �洢�û�����
 *            1. ���ݴ洢,���¸�ʽ�洢, 0x7f(1�ֽ�) + datalen(���ݳ���,1�ֽ�) + databuf(���ݻ�����) + checksum(1�ֽ�)
 *            2. �����ܳ��Ȳ�����32�ֽ�,��Ч���ݳ��Ȳ�����30�ֽ�,����ͷΪ0x7f,����βΪchecksum
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
    MAX_BYTE_OF_PARAM = 48,                         // �������ݵ���󳤶�
    EEPROM_SIZE = 1024,                             // eeprom�Ĵ�С
    MAX_PARAM_NUM = EEPROM_SIZE / MAX_BYTE_OF_PARAM // ����������
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
   * @brief �Ƿ��δд�������
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

// ����Ҫ���Ĺ�����
// 1. ��Ҫ���������,������
// 2. ��eeprom ����Ϊһ���Ŀռ�,��������ʽ��������
// 3. ���������,��У��ͽ�β