/*****************************************************************************/
                                        
/*****************************************************************************/
#ifndef __TIME_H_
#define __TIME_H_

/*****************************************************************************/
#include "stdint.h"
#include "types.h"
/*****************************************************************************/

#ifdef DATE_TIME_SUPPORT
	#define time_getYear(void) efsl_getYear()
	#define time_getMonth(void) efsl_getMonth()
	#define time_getDay(void) efsl_getDay()
	#define time_getHour(void) efsl_getHour()
	#define time_getMinute(void) efsl_getMinute()
	#define time_getSecond(void) efsl_getSecond()
	#define time_getDate(void) fs_makeDate()
	#define time_getTime(void) fs_makeTime()
#else
	#define time_getYear(void) 0x0;
	#define time_getMonth(void) 0x0;
	#define time_getDay(void) 0x0;
	#define time_getHour(void) 0x0;
	#define time_getMinute(void) 0x0;
	#define time_getSecond(void) 0x0;
	#define time_getDate(void) 0x0;
	#define time_getTime(void) 0x0;
#endif

#ifdef DATE_TIME_SUPPORT
euint16 efsl_getYear(void);
euint8  efsl_getMonth(void);
euint8  efsl_getDay(void);
euint8  efsl_getHour(void);
euint8  efsl_getMinute(void);
euint8  efsl_getSecond(void);
euint16 fs_makeDate(void);
euint16 fs_makeTime(void);
#endif

euint8 fs_hasTimeSupport(void);

#endif
