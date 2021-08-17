

#ifndef __EFS_TYPES_H__
#define __EFS_TYPES_H__

/*****************************************************************************/
#include "config.h"
/*****************************************************************************/

#if defined(HW_ENDPOINT_LINUX)
	typedef char eint8;
	typedef signed char esint8;
	typedef unsigned char euint8;
	typedef short eint16;
	typedef signed short esint16;
	typedef unsigned short euint16; 
	typedef long eint32; 
	typedef signed long esint32;
	typedef unsigned long euint32;
#elif defined(HW_ENDPOINT_LINUX64)
	typedef char eint8;
	typedef signed char esint8;
	typedef unsigned char euint8;
	typedef short eint16;
	typedef signed short esint16;
	typedef unsigned short euint16; 
	typedef int eint32; 
	typedef signed int esint32;
	typedef unsigned int euint32;
#elif defined (HW_ENDPOINT_ATMEGA128_SD)
	typedef char eint8;
	typedef signed char esint8;
	typedef unsigned char euint8;
	typedef short eint16;
	typedef signed short esint16;
	typedef unsigned short euint16; 
	typedef long eint32; 
	typedef signed long esint32;
	typedef unsigned long euint32; 
#elif defined(HW_ENDPOINT_DSP_TI6713_SD)
	typedef char eint8;
	typedef signed char esint8;
	typedef unsigned char euint8;
	typedef short eint16;
	typedef signed short esint16;
	typedef unsigned short euint16; 
	typedef int eint32; 
	typedef signed int esint32;
	typedef unsigned int euint32;
#elif defined(NIOS_2)
	typedef char eint8;
	typedef signed char esint8;
	typedef unsigned char euint8;
	typedef short eint16;
	typedef signed short esint16;
	typedef unsigned short euint16;
	typedef int eint32;
	typedef signed int esint32;
	typedef unsigned int euint32; 
#else
	typedef char eint8;
	typedef signed char esint8;
	typedef unsigned char euint8;
	typedef short eint16;
	typedef signed short esint16;
	typedef unsigned short euint16; 
	typedef long eint32; 
	typedef signed long esint32;
	typedef unsigned long euint32; 
#endif

#endif
