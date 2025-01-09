#include "RTPen.hpp"

RTPen* RTPen::Create(IRealTimeStylus* rtsp)
{
    if (!rtsp)
    {
        return NULL;
    }

    RTPen* pRTPen = new RTPen();
    if (!pRTPen)
    {
        return NULL;
    }

    HRESULT hr = CoCreateFreeThreadedMarshaler(pRTPen, &pRTPen->punkFTMarshaller);
    if (FAILED(hr))
    {
        std::cout << "CSyncEventHandlerRTS::Create: cannot create free-threaded marshaller\n";
        pRTPen->Release();
        return NULL;
    }

    hr = rtsp->AddStylusSyncPlugin(
        0,                      // insert plugin at position 0 in the sync plugin list
        pRTPen);  // plugin to be inserted - event handler CSyncEventHandlerRTS
    if (FAILED(hr))
    {
        std::cout << "CEventHandlerRTS::Create: failed to add CSyncEventHandlerRTS to the RealTimeStylus plugins\n";
        pRTPen->Release();
        return NULL;
    }
    
    GUID lWantedProps[] = 
    {
        GUID_PACKETPROPERTY_GUID_X,
        GUID_PACKETPROPERTY_GUID_Y,
        GUID_PACKETPROPERTY_GUID_NORMAL_PRESSURE,
        GUID_PACKETPROPERTY_GUID_WIDTH,
        GUID_PACKETPROPERTY_GUID_HEIGHT
    };

    rtsp->SetDesiredPacketDescription(5, lWantedProps);
    rtsp->put_Enabled(true);

    //// Detect what tablet context IDs will give us pressure data
    //{
    //    g_nContexts = 0;
    //    ULONG nTabletContexts = 0;
    //    TABLET_CONTEXT_ID* piTabletContexts;
    //    HRESULT res = rtsp->GetAllTabletContextIds(&nTabletContexts, &piTabletContexts);
    //    for (ULONG i = 0; i < nTabletContexts; i++)
    //    {
    //        IInkTablet* pInkTablet;
    //        if (SUCCEEDED(rtsp->GetTabletFromTabletContextId(piTabletContexts[i], &pInkTablet)))
    //        {
    //            // float ScaleX, ScaleY;
    //            ULONG nPacketProps;
    //            PACKET_PROPERTY* pPacketProps;
    //            if (i == 0) 
    //            {
    //                rtsp->GetPacketDescriptionData(piTabletContexts[i], &ScaleX, &ScaleY, &nPacketProps, &pPacketProps);
    //            }
    //            else 
    //            {
    //                float a;
    //                float b;
    //                rtsp->GetPacketDescriptionData(piTabletContexts[i], &a, &b, &nPacketProps, &pPacketProps);
    //            }

    //            for (ULONG j = 0; j < nPacketProps; j++)
    //            {
    //                if (pPacketProps[j].guid == GUID_PACKETPROPERTY_GUID_X) {
    //                    g_lXContexts[0].iTabletContext = piTabletContexts[i];
    //                    g_lXContexts[0].iX = j;
    //                }
    //                if (pPacketProps[j].guid == GUID_PACKETPROPERTY_GUID_Y) {
    //                    g_lXContexts[0].iY = j;
    //                }
    //                if (pPacketProps[j].guid == GUID_PACKETPROPERTY_GUID_WIDTH) {
    //                    g_lXContexts[0].iWidth = j;
    //                }
    //                if (pPacketProps[j].guid == GUID_PACKETPROPERTY_GUID_HEIGHT) {
    //                    g_lXContexts[0].iHeight = j;
    //                }

    //                if (pPacketProps[j].guid != GUID_PACKETPROPERTY_GUID_NORMAL_PRESSURE)
    //                    continue;

    //                g_lContexts[g_nContexts].iTabletContext = piTabletContexts[i];
    //                g_lContexts[g_nContexts].iPressure = j;
    //                g_lContexts[g_nContexts].PressureRcp = 1.0f / pPacketProps[j].PropertyMetrics.nLogicalMax;
    //                g_nContexts++;
    //                break;
    //            }
    //            CoTaskMemFree(pPacketProps);
    //        }
    //    }

    //    // If we can't get pressure information, no use in having the tablet context
    //    if (g_nContexts == 0)
    //    {
    //        ReleaseRTS();
    //        return false;
    //    }
    //}

    return pRTPen;
}

HRESULT RTPen::QueryInterface(REFIID riid, void** ppvObject)
{
    if ((riid == IID_IStylusSyncPlugin) || (riid == IID_IUnknown))
    {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }
    else if (riid == IID_IMarshal)
    {
        return punkFTMarshaller->QueryInterface(riid, ppvObject);
    }

    *ppvObject = NULL;
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
    //std::cout << "Stylus enabled.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::RealTimeStylusDisabled(IRealTimeStylus* piRtsSrc, ULONG cTcidCount, const TABLET_CONTEXT_ID* pTcids)
{
    //std::cout << "Stylus disabled.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::StylusInRange(IRealTimeStylus* piRtsSrc, TABLET_CONTEXT_ID tcid, STYLUS_ID sid)
{
    //std::cout << "Stylus in range.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::StylusOutOfRange(IRealTimeStylus* piRtsSrc, TABLET_CONTEXT_ID tcid, STYLUS_ID sid)
{
    //std::cout << "Stylus out of range.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::StylusDown(IRealTimeStylus* piRtsSrc, const StylusInfo* pStylusInfo, ULONG cPropCountPerPkt, LONG* pPacket, LONG** ppInOutPkt)
{
    //std::cout << "Stylus down.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::StylusUp(IRealTimeStylus* piRtsSrc, const StylusInfo* pStylusInfo, ULONG cPropCountPerPkt, LONG* pPacket, LONG** ppInOutPkt)
{
    //std::cout << "Stylus up.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::StylusButtonDown(IRealTimeStylus* piRtsSrc, STYLUS_ID sid, const GUID* pGuidStylusButton, POINT* pStylusPos)
{
    //std::cout << "Stylus button down.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::StylusButtonUp(IRealTimeStylus* piRtsSrc, STYLUS_ID sid, const GUID* pGuidStylusButton, POINT* pStylusPos)
{
    //std::cout << "Stylus button up.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::InAirPackets(IRealTimeStylus* piRtsSrc, const StylusInfo* pStylusInfo, ULONG cPktCount, ULONG cPktBuffLength, LONG* pPackets, ULONG* pcInOutPkts, LONG** ppInOutPkts)
{
    //std::cout << "In-air packets.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::Packets(IRealTimeStylus* piRtsSrc, const StylusInfo* pStylusInfo, ULONG cPktCount, ULONG cPktBuffLength, LONG* pPackets, ULONG* pcInOutPkts, LONG** ppInOutPkts)
{
    //std::cout << "Packets received: ";
    //for (ULONG i = 0; i < cPktCount; i += 2) {
    //    std::cout << "(" << pPackets[i] << ", " << pPackets[i + 1] << ") ";
    //}
    //std::cout << std::endl;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::CustomStylusDataAdded(IRealTimeStylus* piRtsSrc, const GUID* pGuidId, ULONG cbData, const BYTE* pbData)
{
    //std::cout << "Custom stylus data added.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::SystemEvent(IRealTimeStylus* piRtsSrc, TABLET_CONTEXT_ID tcid, STYLUS_ID sid, SYSTEM_EVENT event, SYSTEM_EVENT_DATA eventdata)
{
    //std::cout << "System event.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::TabletAdded(IRealTimeStylus* piRtsSrc, IInkTablet* piTablet)
{
    //std::cout << "Tablet added.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::TabletRemoved(IRealTimeStylus* piRtsSrc, LONG iTabletIndex)
{
    //std::cout << "Tablet removed.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::Error(IRealTimeStylus* piRtsSrc, IStylusPlugin* piPlugin, RealTimeStylusDataInterest dataInterest, HRESULT hrErrorCode, LONG_PTR* lptrKey) 
{
    //std::cout << "Error encountered.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::UpdateMapping(IRealTimeStylus* piRtsSrc)
{
    //std::cout << "Update mapping.\n";
    return S_OK;
}

HRESULT STDMETHODCALLTYPE RTPen::DataInterest(RealTimeStylusDataInterest* pDataInterest) 
{
    *pDataInterest = (RealTimeStylusDataInterest)(RTSDI_AllData);  // Adjust as needed
    return S_OK;
}
