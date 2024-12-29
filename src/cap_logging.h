#ifndef CAP_LOGGING_H
#define CAP_LOGGING_H

#include <stdio.h>

// TODO: EITHER FINISH THIS THING
// OR GET ONE FROM ONLINE (seems to be the better option atm)

static FILE* logFile = NULL;

typedef enum
{
	CAP_LOG_TRCE,
	CAP_LOG_DEBG,
	CAP_LOG_INFO,
	CAP_LOG_WARN,
	CAP_LOG_EROR,
	CAP_LOG_FATL
} CAP_LOG_LEVEL;

int cap_log_setup_file(const char* fileLocation)
{
	logFile = fopen(fileLocation, "w");
	if (!logFile)
	{
		return 1;
	}
	return 0;
}

void cap_log_close_file()
{

}

void cap_log_print(CAP_LOG_LEVEL l, const char* fmt, ...)
{
	switch (l)
	{
	case CAP_LOG_TRCE:
	{
		break;
	}
	case CAP_LOG_DEBG:
	{
		break;
	}
	case CAP_LOG_INFO:
	{
		break;
	}
	case CAP_LOG_WARN:
	{
		break;
	}
	case CAP_LOG_EROR:
	{
		break;
	}
	case CAP_LOG_FATL:
	{
		break;
	}
	}
}

// SET UP DEBUG MACROS
#ifdef _DEBUG
	#define print_trc(...) printf("[TRCE]"); printf(__VA_ARGS__);
	#define print_dgb(...) printf("[DEBG]"); printf(__VA_ARGS__);
	#define print_inf(...) printf("[INFO]"); printf(__VA_ARGS__);
	#define print_wng(...) printf("[WARN]"); printf(__VA_ARGS__);
	#define print_err(...) printf("[EROR]"); printf(__VA_ARGS__);
	#define print_ftl(...) printf("[FATL]"); printf(__VA_ARGS__);
#else
// STRIP DEBUG MACROS IF NOT DEBUG
	#define print_trc(x)
	#define print_dgb(x)
	#define print_inf(x)
	#define print_wng(x)
	#define print_err(x)
	#define print_ftl(x)
#endif

#endif
