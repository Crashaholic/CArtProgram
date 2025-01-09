//#ifndef RTSC_H
//#define RTSC_H
//#ifdef _WIN32
//#include <windows.h> 
//#include <stdarg.h>  
//#include <stdbool.h> 
//#include <stddef.h>  
//
//#if defined __cplusplus
//	#define EXTERN extern "C"
//#else
//	#include <stdarg.h>
//	#include <stdbool.h>
//	#define EXTERN extern
//#endif
//
//typedef struct StylusPacket 
//{
//	LONG x;
//	LONG y;
//} StylusPacket;
//
//EXTERN void* RTS_Init(HWND hwnd);
//EXTERN void RTS_Shutdown(void* rtsHandle);
//
//EXTERN HRESULT RTS_Error();
//
//#endif
//#endif

#ifndef RTSC_H
#define RTSC_H

// Define RTSC_EXPORTS during DLL creation
#ifdef RTSC_EXPORTS
	#define RTSC_API __declspec(dllexport)  // Export the symbols when building the DLL
#else
	#define RTSC_API __declspec(dllimport)  // Import the symbols when using the DLL
#endif

#if defined(__cplusplus)
extern "C" {
#endif

	#include <Windows.h>

	typedef struct StylusPacket {
		LONG x;
		LONG y;
	} StylusPacket;

	RTSC_API typedef void (*StylusCallback)(StylusPacket* packets, size_t count);

	// Handle to the RealTimeStylus system
	typedef void* RTS_HANDLE;

	// Initialize the RealTimeStylus system and attach the handler
	RTSC_API RTS_HANDLE RTS_Init(HWND hwnd, StylusCallback callback);

	// Shutdown the RealTimeStylus system
	RTSC_API void RTS_Shutdown(RTS_HANDLE rtsHandle);

	// Methods corresponding to IStylusPlugin methods
	RTSC_API HRESULT RTS_RealTimeStylusEnabled(RTS_HANDLE rtsHandle);
	RTSC_API HRESULT RTS_RealTimeStylusDisabled(RTS_HANDLE rtsHandle);
	RTSC_API HRESULT RTS_StylusInRange(RTS_HANDLE rtsHandle, LONG tcid, LONG sid);
	RTSC_API HRESULT RTS_StylusOutOfRange(RTS_HANDLE rtsHandle, LONG tcid, LONG sid);
	RTSC_API HRESULT RTS_StylusDown(RTS_HANDLE rtsHandle, LONG x, LONG y);
	RTSC_API HRESULT RTS_StylusUp(RTS_HANDLE rtsHandle, LONG x, LONG y);
	RTSC_API HRESULT RTS_TabletAdded(RTS_HANDLE rtsHandle, void* piTablet);
	RTSC_API HRESULT RTS_Packets(RTS_HANDLE rtsHandle, LONG* packets, ULONG packetCount, ULONG bufferLength);

#if defined(__cplusplus)
}
#endif

#endif // RTSC_H

