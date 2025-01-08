#include "rtsc.h"
#include "RTPen.hpp"  // The header file for the RTPen class

IRealTimeStylus* realTimeStylus;
struct RTSWrapper 
{
    RTPen* handler;
};

RTSC_API RTS_HANDLE RTS_Init(HWND hwnd)
{
    HRESULT res = CoInitialize(NULL);
    if (FAILED(res)) 
    {
        return nullptr;
    }

    RTSWrapper* wrapper = new RTSWrapper();
    /*wrapper->handler = new RTPen();*/

    HRESULT hr = CoCreateInstance(CLSID_RealTimeStylus, NULL, CLSCTX_ALL, IID_PPV_ARGS(&(realTimeStylus)));
    if (FAILED(hr)) 
    {
        std::cout << "RTS_Init: failed to CoCreateInstance of RealTimeStylus\n";
        //delete wrapper->handler;
        delete wrapper;
        CoUninitialize();
        return nullptr;
    }

    realTimeStylus->put_HWND(reinterpret_cast<UINT_PTR>(hwnd));
    if (FAILED(hr))
    {
        std::cout << "RTS_Init: failed to set window handle\n";
        realTimeStylus->Release();
        return NULL;
    }

    wrapper->handler = RTPen::Create(realTimeStylus);
    if (!wrapper->handler)
    {
        std::cout << "RTS_Init: failed to create RTPen\n";
        realTimeStylus->Release();
        return NULL;
    }

    //hr = realTimeStylus->AddStylusSyncPlugin(0, wrapper->handler);
    //if (FAILED(hr)) 
    //{
    //    //syncPlugin->Release();
    //    realTimeStylus->Release();
    //    delete wrapper->handler;
    //    delete wrapper;
    //    CoUninitialize();
    //    return nullptr;
    //}

    hr = realTimeStylus->put_Enabled(TRUE);
    if (FAILED(hr)) 
    {
        realTimeStylus->Release();
        delete wrapper->handler;
        delete wrapper;
        CoUninitialize();
        return nullptr;
    }

    return wrapper;
}

RTSC_API void RTS_Shutdown(RTS_HANDLE rtsHandle)
{
    if (!rtsHandle) return;

    RTSWrapper* wrapper = (RTSWrapper*)rtsHandle;

    if (realTimeStylus) 
    {
        realTimeStylus->put_Enabled(FALSE);
        realTimeStylus->Release();
    }

    if (wrapper->handler) 
    {
        wrapper->handler->Release();
    }

    delete wrapper;
    CoUninitialize();
}

RTSC_API HRESULT RTS_RealTimeStylusEnabled(RTS_HANDLE rtsHandle)
{
    RTSWrapper* wrapper = (RTSWrapper*)rtsHandle;
    if (!wrapper || !wrapper->handler) 
    {
        return E_POINTER;
    }
    return wrapper->handler->RealTimeStylusEnabled(nullptr, 0, nullptr);
}

RTSC_API HRESULT RTS_RealTimeStylusDisabled(RTS_HANDLE rtsHandle)
{
    RTSWrapper* wrapper = (RTSWrapper*)rtsHandle;
    if (!wrapper || !wrapper->handler) 
    {
        return E_POINTER;
    }
    return wrapper->handler->RealTimeStylusDisabled(nullptr, 0, nullptr);
}

RTSC_API HRESULT RTS_StylusInRange(RTS_HANDLE rtsHandle, LONG tcid, LONG sid)
{
    RTSWrapper* wrapper = (RTSWrapper*)rtsHandle;
    if (!wrapper || !wrapper->handler) 
    {
        return E_POINTER;
    }
    return wrapper->handler->StylusInRange(nullptr, tcid, sid);
}

RTSC_API HRESULT RTS_StylusOutOfRange(RTS_HANDLE rtsHandle, LONG tcid, LONG sid)
{
    RTSWrapper* wrapper = (RTSWrapper*)rtsHandle;
    if (!wrapper || !wrapper->handler) 
    {
        return E_POINTER;
    }
    return wrapper->handler->StylusOutOfRange(nullptr, tcid, sid);
}

RTSC_API HRESULT RTS_StylusDown(RTS_HANDLE rtsHandle, LONG x, LONG y)
{
    RTSWrapper* wrapper = (RTSWrapper*)rtsHandle;
    if (!wrapper || !wrapper->handler) 
    {
        return E_POINTER;
    }

    StylusInfo stylusInfo = { 0 };
    LONG packet[] = { x, y };
    return wrapper->handler->StylusDown(nullptr, &stylusInfo, 2, packet, nullptr);
}

RTSC_API HRESULT RTS_StylusUp(RTS_HANDLE rtsHandle, LONG x, LONG y)
{
    RTSWrapper* wrapper = (RTSWrapper*)rtsHandle;
    if (!wrapper || !wrapper->handler) 
    {
        return E_POINTER;
    }

    StylusInfo stylusInfo = { 0 };
    LONG packet[] = { x, y };
    return wrapper->handler->StylusUp(nullptr, &stylusInfo, 2, packet, nullptr);
}

RTSC_API HRESULT RTS_TabletAdded(RTS_HANDLE rtsHandle, void* piTablet)
{
    RTSWrapper* wrapper = (RTSWrapper*)rtsHandle;
    if (!wrapper || !wrapper->handler)
    {
        return E_POINTER;
    }

    return wrapper->handler->TabletAdded(nullptr, (IInkTablet*)piTablet);
}

RTSC_API HRESULT RTS_Packets(RTS_HANDLE rtsHandle, LONG* packets, ULONG packetCount, ULONG bufferLength )
{
    if (!rtsHandle || !packets) 
    {
        return E_POINTER;
    }

    RTSWrapper* wrapper = (RTSWrapper*)rtsHandle;

    if (!wrapper->handler) 
    {
        return E_POINTER;
    }

    StylusInfo stylusInfo = { 0 };
    ULONG inOutPacketCount = packetCount;
    LONG* inOutPackets = nullptr;

    return wrapper->handler->Packets(
        nullptr,        // Assuming no RealTimeStylus source for the wrapper
        &stylusInfo,    // Placeholder StylusInfo
        packetCount,    // Count of packets
        bufferLength,   // Length of the packet buffer
        packets,        // Input packet data
        &inOutPacketCount,
        &inOutPackets   // Placeholder for output (can be null or modified)
    );
}

