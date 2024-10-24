/***********************************************************************
 * @file version.h
 * VERSION
 * @author :	ch
 * @brief  :
 * @version:	v1
 * @Copyright (C)  2024-10-23  .cdWFVCEL. all right reserved
 ***********************************************************************/

#ifndef __VERSION_H_
#define __VERSION_H_
/* include ----------------------------------------------------------------------------------------------------------------- */
// #include "typedef.h"
#include <string.h>
#include <stdio.h>
/* macro definition -------------------------------------------------------------------------------------------------------- */

inline const char *GetFullVersion();
inline const char *GetBuildDateTime();
#define BUILD_DATETIME GetBuildDateTime() // ����һ����������ȡ����ʱ��
#define SOFT_VER 2
#define HARDWARE_VER 1
#define PROTOCOL_VER 1
#define PARAM_VER 2 // @brief �����汾��,�������ṹ�巢���仯ʱ,��Ҫ���´˰汾��
#define PROJECT_VER GetFullVersion()
/* type definition --------------------------------------------------------------------------------------------------------- */

/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */

// Function to get the full version string
inline const char *GetFullVersion()
{
  static char fullVersion[32]; // Adjust size if needed
  snprintf(fullVersion, sizeof(fullVersion), "%d.%d.%d.%s", SOFT_VER, HARDWARE_VER, PROTOCOL_VER, BUILD_DATETIME);
  return fullVersion;
}

inline const char *GetBuildDateTime()
{
  static char buildDateTime[13]; // 12 characters for date/time + 1 for null terminator
  // ��ȡ�������ں�ʱ��
  const char *date = __DATE__;
  const char *time = __TIME__;
  // �����·�
  const char *monthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  int month = 0;
  for (int i = 0; i < 12; i++)
  {
    if (strncmp(date, monthNames[i], 3) == 0)
    {
      month = i + 1;
      break;
    }
  }
  // �������ں����
  int day, year;
  sscanf(date + 4, "%d %d", &day, &year);
  // ����ʱ��
  int hour, minute, second;
  sscanf(time, "%d:%d:%d", &hour, &minute, &second);
  // ��ʽ��Ϊ "YYMMDDHHMMSS"
  snprintf(buildDateTime, sizeof(buildDateTime), "%02d%02d%02d%02d%02d%02d", year % 100, month, day, hour, minute, second);
  return buildDateTime;
}


#endif // __VERSION_H_
