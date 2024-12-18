#ifndef WINDOWS_RTPEN_ACCESSOR_H
#define WINDOWS_RTPEN_ACCESSOR_H

// adapted mostly from 
// https://github.com/glynskyi/stylus_api/blob/5de9533ef487a811ad04c02fdeff76ecc95d10a0/windows/stylus_api_plugin.cpp#L314
// https://github.com/microsoft/Windows-classic-samples/blob/main/Samples/Win7Samples/Touch/MTScratchpadRTStylus/cpp/MTScratchpadRTStylus.cpp

#ifdef _WIN32
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <assert.h>
#define ASSERT assert

// COM header files
#include <ole2.h>
#include <rtscom.h>     // RTS interface and GUID declarations
#include <rtscom_i.c>   // RTS GUID definitions


///////////////////////////////////////////////////////////////////////////////
// Real Time Stylus sync event handler

// Synchronous plugin, notitification receiver that changes pen color.
class CRTSEventHandler : public IStylusSyncPlugin
{
    CRTSEventHandler();
    virtual ~CRTSEventHandler();

public:
    // Factory method
    static IStylusSyncPlugin* Create(IRealTimeStylus* pRealTimeStylus);

    // IStylusSyncPlugin methods

    // Handled IStylusSyncPlugin methods, they require nontrivial implementation
    STDMETHOD(StylusDown)(IRealTimeStylus* piSrcRtp, const StylusInfo* pStylusInfo, ULONG cPropCountPerPkt, LONG* pPacket, LONG** ppInOutPkt);
    STDMETHOD(StylusUp)(IRealTimeStylus* piSrcRtp, const StylusInfo* pStylusInfo, ULONG cPropCountPerPkt, LONG* pPacket, LONG** ppInOutPkt);
    STDMETHOD(Packets)(IRealTimeStylus* piSrcRtp, const StylusInfo* pStylusInfo, ULONG cPktCount, ULONG cPktBuffLength, LONG* pPackets, ULONG* pcInOutPkts, LONG** ppInOutPkts);
    STDMETHOD(DataInterest)(RealTimeStylusDataInterest* pEventInterest);

    // IStylusSyncPlugin methods with trivial inline implementation, they all return S_OK
    STDMETHOD(RealTimeStylusEnabled)(IRealTimeStylus*, ULONG, const TABLET_CONTEXT_ID*) { return S_OK; }
    STDMETHOD(RealTimeStylusDisabled)(IRealTimeStylus*, ULONG, const TABLET_CONTEXT_ID*) { return S_OK; }
    STDMETHOD(StylusInRange)(IRealTimeStylus*, TABLET_CONTEXT_ID, STYLUS_ID) { return S_OK; }
    STDMETHOD(StylusOutOfRange)(IRealTimeStylus*, TABLET_CONTEXT_ID, STYLUS_ID) { return S_OK; }
    STDMETHOD(InAirPackets)(IRealTimeStylus*, const StylusInfo*, ULONG, ULONG, LONG*, ULONG*, LONG**) { return S_OK; }
    STDMETHOD(StylusButtonUp)(IRealTimeStylus*, STYLUS_ID, const GUID*, POINT*) { return S_OK; }
    STDMETHOD(StylusButtonDown)(IRealTimeStylus*, STYLUS_ID, const GUID*, POINT*) { return S_OK; }
    STDMETHOD(SystemEvent)(IRealTimeStylus*, TABLET_CONTEXT_ID, STYLUS_ID, SYSTEM_EVENT, SYSTEM_EVENT_DATA) { return S_OK; }
    STDMETHOD(TabletAdded)(IRealTimeStylus*, IInkTablet*) { return S_OK; }
    STDMETHOD(TabletRemoved)(IRealTimeStylus*, LONG) { return S_OK; }
    STDMETHOD(CustomStylusDataAdded)(IRealTimeStylus*, const GUID*, ULONG, const BYTE*) { return S_OK; }
    STDMETHOD(Error)(IRealTimeStylus*, IStylusPlugin*, RealTimeStylusDataInterest, HRESULT, LONG_PTR*) { return S_OK; }
    STDMETHOD(UpdateMapping)(IRealTimeStylus*) { return S_OK; }

    // IUnknown methods
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID* ppvObj);

private:
    LONG m_cRefCount;                   // COM object reference count
    IUnknown* m_punkFTMarshaller;       // free-threaded marshaller
    int m_nContacts;                    // number of fingers currently in the contact with the touch digitizer
};

IRealTimeStylus* g_pRealTimeStylus = NULL;              // RTS object
bool EnableRealTimeStylus(IRealTimeStylus* pRealTimeStylus, IDynamicRenderer* pDynamicRenderer);



#endif

#endif  

