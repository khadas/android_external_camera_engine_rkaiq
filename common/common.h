
#ifndef  _Common_H_
#define  _Common_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdlib.h>
#include <cstdlib>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>

#define RGB_B_IDX 0
#define RGB_G_IDX 1
#define RGB_R_IDX 2

#define DIVMIN 0.00001

//---- ABS()
#define	    ABS_U16(a)			    		(uint16_t)( (a) > 0 ? (a) : (-(a)) )
#define	    ABS_U32(a)			    		(uint32_t)( (a) > 0 ? (a) : (-(a)) )
#define	    ABS_F32(a)			    		(f32_t)( (a) > 0 ? (a) : (-(a)) )

#define CLIP(a, min_v, max_v)			    (((a) < (min_v)) ? (min_v) : (((a) > (max_v)) ? (max_v) : (a)))

/*=======================================Definiations==============================================*/
//typedef unsigned char       uint8_t;
//typedef unsigned short   	uint16_t;
typedef unsigned int        uint32_t;
typedef int                 sint32_t;
//typedef int                 int32_t;
typedef char                sint8_t;
//typedef short               int16_t;
//typedef signed char         int8_t;
//typedef unsigned long long 	uint64_t;
typedef unsigned long long 	uint64t;


// all of cases
typedef enum result {
	RET_SUCCESS				= 0,   // this has to be 0, if clauses rely on it
	RET_FAILURE				= 1,   // process failure
	RET_INVALID_PARM	    = 2,   // invalid parameter
	RET_WRONG_CONFIG		= 3,   // feature not supported
	RET_BUSY				= 4,   // there's already something going on...
	RET_CANCELED			= 5,   // operation canceled
	RET_OUTOFMEM			= 6,   // out of memory
	RET_OUTOFRANGE			= 7,   // parameter/value out of range
	RET_NULL_POINTER		= 8,   // the/one/all parameter(s) is a(are) NULL pointer(s)
	RET_DIVISION_BY_ZERO	= 9,   // a divisor equals ZERO
	RET_NO_INPUTIMAGE		= 10   // no input image
} result_t;

#define MAX(a,b)							((a) >= (b) ? (a):(b))


#endif



