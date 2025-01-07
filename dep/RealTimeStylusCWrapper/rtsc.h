#ifndef RTSC_H
#define RTSC_H

#if defined __cplusplus
	#define EXTERN extern "C"
#else
	#include <stdarg.h>
	#include <stdbool.h>
	#define EXTERN extern
#endif


EXTERN void TestingFn();

#endif
