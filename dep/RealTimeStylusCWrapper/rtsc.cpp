#include "RTPen.hpp"

#include "rtsc.h"

static RTPen rtp;

EXTERN void TestingFn()
{
	rtp.TestingFn();
}

//struct RTSWrapper {
//    IRealTimeStylus* realTimeStylus;
//    RTPen* handler;
//};
//
//void* RTS_Init(HWND hwnd, StylusCallback callback) 
//{
//    CoInitialize(NULL);
//
//    RTSWrapper* wrapper = new RTSWrapper();
//    wrapper->handler = new RTPen();
//
//    // Create RTS instance
//    HRESULT hr = CoCreateInstance(CLSID_RealTimeStylus, NULL, CLSCTX_INPROC_SERVER, IID_IRealTimeStylus, (void**)&wrapper->realTimeStylus);
//    if (FAILED(hr)) 
//    {
//        delete wrapper;
//        return nullptr;
//    }
//
//    // Attach plugin
//
//    wrapper->realTimeStylus->AddStylusSyncPlugin(0, wrapper->handler);
//    wrapper->realTimeStylus->put_Enabled(TRUE);
//
//    return wrapper;
//}
//
//void RTS_Shutdown(void* rtsHandle) {
//    if (!rtsHandle) return;
//
//    RTSWrapper* wrapper = (RTSWrapper*)rtsHandle;
//
//    if (wrapper->realTimeStylus) {
//        wrapper->realTimeStylus->put_Enabled(FALSE);
//        wrapper->realTimeStylus->Release();
//    }
//
//    if (wrapper->handler) {
//        wrapper->handler->Release();
//    }
//
//    delete wrapper;
//    CoUninitialize();
//}
