#include "RTPen.hpp"
const IID IID_IStylusPlugin = { 0xA81436D8, 0x4757, 0x4fd1, {0xA1, 0x85, 0x13, 0x3F, 0x97, 0xC6, 0xC5, 0x45} };

void RTPen::TestingFn()
{
    printf("RRRAAAAAHHHH :eagle:\n");
}

HRESULT STDMETHODCALLTYPE RTPen::QueryInterface(REFIID riid, void** ppvObject)
{
    if (riid == IID_IUnknown || riid == IID_IStylusPlugin) {
        *ppvObject = static_cast<IStylusPlugin*>(this);
        AddRef();
        return S_OK;
    }
    *ppvObject = nullptr;
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE RTPen::AddRef()
{
    return ++refCount;
}

ULONG STDMETHODCALLTYPE RTPen::Release()
{
    ULONG count = --refCount;
    if (count == 0) {
        delete this;
    }
    return count;
}

// IStylusPlugin methods

HRESULT STDMETHODCALLTYPE RTPen::RealTimeStylusEnabled(IRealTimeStylus* piRtsSrc, ULONG cTcidCount, const TABLET_CONTEXT_ID* pTcids)
{
    std::cout << "Stylus enabled.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::RealTimeStylusDisabled(IRealTimeStylus* piRtsSrc, ULONG cTcidCount, const TABLET_CONTEXT_ID* pTcids)
{
    std::cout << "Stylus disabled.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::StylusInRange(IRealTimeStylus* piRtsSrc, TABLET_CONTEXT_ID tcid, STYLUS_ID sid)
{
    std::cout << "Stylus in range.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::StylusOutOfRange(IRealTimeStylus* piRtsSrc, TABLET_CONTEXT_ID tcid, STYLUS_ID sid)
{
    std::cout << "Stylus out of range.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::StylusDown(IRealTimeStylus* piRtsSrc, const StylusInfo* pStylusInfo, ULONG cPropCountPerPkt, LONG* pPacket, LONG** ppInOutPkt)
{
    std::cout << "Stylus down.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::StylusUp(IRealTimeStylus* piRtsSrc, const StylusInfo* pStylusInfo, ULONG cPropCountPerPkt, LONG* pPacket, LONG** ppInOutPkt)
{
    std::cout << "Stylus up.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::StylusButtonDown(IRealTimeStylus* piRtsSrc, STYLUS_ID sid, const GUID* pGuidStylusButton, POINT* pStylusPos)
{
    std::cout << "Stylus button down.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::StylusButtonUp(IRealTimeStylus* piRtsSrc, STYLUS_ID sid, const GUID* pGuidStylusButton, POINT* pStylusPos)
{
    std::cout << "Stylus button up.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::InAirPackets(IRealTimeStylus* piRtsSrc, const StylusInfo* pStylusInfo, ULONG cPktCount, ULONG cPktBuffLength, LONG* pPackets, ULONG* pcInOutPkts, LONG** ppInOutPkts)
{
    std::cout << "In-air packets.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::Packets(IRealTimeStylus* piRtsSrc, const StylusInfo* pStylusInfo, ULONG cPktCount, ULONG cPktBuffLength, LONG* pPackets, ULONG* pcInOutPkts, LONG** ppInOutPkts)
{
    std::cout << "Packets received: ";
    for (ULONG i = 0; i < cPktCount; i += 2) {
        std::cout << "(" << pPackets[i] << ", " << pPackets[i + 1] << ") ";
    }
    std::cout << std::endl;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::CustomStylusDataAdded(IRealTimeStylus* piRtsSrc, const GUID* pGuidId, ULONG cbData, const BYTE* pbData)
{
    std::cout << "Custom stylus data added.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::SystemEvent(IRealTimeStylus* piRtsSrc, TABLET_CONTEXT_ID tcid, STYLUS_ID sid, SYSTEM_EVENT event, SYSTEM_EVENT_DATA eventdata)
{
    std::cout << "System event.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::TabletAdded(IRealTimeStylus* piRtsSrc, IInkTablet* piTablet)
{
    std::cout << "Tablet added.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::TabletRemoved(IRealTimeStylus* piRtsSrc, LONG iTabletIndex)
{
    std::cout << "Tablet removed.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::Error(IRealTimeStylus* piRtsSrc, IStylusPlugin* piPlugin, RealTimeStylusDataInterest dataInterest, HRESULT hrErrorCode, LONG_PTR* lptrKey) {
    std::cout << "Error encountered.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::UpdateMapping(IRealTimeStylus* piRtsSrc)
{
    std::cout << "Update mapping.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::DataInterest(RealTimeStylusDataInterest* pDataInterest) {
    //*pDataInterest = RealTimeStylusDataInterest_AllData;  // Adjust as needed
    return S_OK;
}