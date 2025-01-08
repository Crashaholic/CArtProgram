#ifndef CAP_PENINPUT_H
#define CAP_PENINPUT_H
// if on windows, use window specific tablet pen lib
#ifdef _WIN32
	#include <rtsc.h>
	typedef RTS_HANDLE CAP_PenHandle;

	int Cap_PenInit_Win(HWND handle);
#elif __linux__
	// TODO: LINUX SPECIFIC INPUT
	// if and when i want to feel pain :/
#endif

void Cap_PenShutdown();
#endif
