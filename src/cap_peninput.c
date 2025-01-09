#include "cap_peninput.h"

#include <stdio.h>

#ifdef _WIN32
	static CAP_PenHandle Cap__PenHandle;

	void StylusEventHandler(StylusPacket* packets, size_t count) 
	{
		for (size_t i = 0; i < count; ++i) 
		{
			printf("Stylus Packet: X = %ld, Y = %ld\n", packets[i].x, packets[i].y);
		}
	}

	int Cap_PenInit_Win(HWND handle)
	{
		CAP_PenHandle temp = RTS_Init(handle, StylusEventHandler);
		if (!temp)
		{
			fprintf(stderr, "Failed to initialize RealTimeStylus\n");
			return -1;
		}
		else // assume that the initialization was a success!
		{
			Cap__PenHandle = temp;
		}
		return 0;
	}
#elif __linux__
	// linux stuff
#endif

void Cap_PenShutdown()
{
	#ifdef _WIN32
		if (Cap__PenHandle)
		{
			RTS_Shutdown(Cap__PenHandle);
		}
	#elif __linux__

	#endif
}
