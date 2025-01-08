#ifndef RTPEN_H
#define RTPEN_H

#ifdef RTSC_EXPORTS
    #define RTS_API __declspec(dllexport)  // Export for DLL creation
#else
    #define RTS_API __declspec(dllimport)  // Import for using the DLL
#endif


#ifdef _WIN32

    #include <Windows.h>
    #include <iostream>
    #include <RTSCOM.h>  // Include RealTimeStylus headers
    #include <rtscom_i.c>
    #include <ole2.h>

    class RTS_API RTPen : public IStylusSyncPlugin {
    public:
        RTPen() : refCount(1), punkFTMarshaller(NULL) {}

        static RTPen* Create(IRealTimeStylus* rtsp);

        // IUnknown methods
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;

        ULONG STDMETHODCALLTYPE AddRef() override;

        ULONG STDMETHODCALLTYPE Release() override;

        // IStylusPlugin methods
        HRESULT STDMETHODCALLTYPE RealTimeStylusEnabled(
            IRealTimeStylus* piRtsSrc, 
            ULONG cTcidCount, 
            const TABLET_CONTEXT_ID* pTcids) override;

        HRESULT STDMETHODCALLTYPE RealTimeStylusDisabled(
            IRealTimeStylus* piRtsSrc, 
            ULONG cTcidCount, 
            const TABLET_CONTEXT_ID* pTcids) override;

        HRESULT STDMETHODCALLTYPE StylusInRange(
            IRealTimeStylus* piRtsSrc,
            TABLET_CONTEXT_ID tcid,
            STYLUS_ID sid) override;

        HRESULT STDMETHODCALLTYPE StylusOutOfRange(
            IRealTimeStylus* piRtsSrc,
            TABLET_CONTEXT_ID tcid,
            STYLUS_ID sid) override;

        HRESULT STDMETHODCALLTYPE StylusDown(
            IRealTimeStylus* piRtsSrc,
            const StylusInfo* pStylusInfo,
            ULONG cPropCountPerPkt,
            LONG* pPacket,
            LONG** ppInOutPkt) override;

        HRESULT STDMETHODCALLTYPE StylusUp(
            IRealTimeStylus* piRtsSrc,
            const StylusInfo* pStylusInfo,
            ULONG cPropCountPerPkt,
            LONG* pPacket,
            LONG** ppInOutPkt) override;

        HRESULT STDMETHODCALLTYPE StylusButtonDown(
            IRealTimeStylus* piRtsSrc,
            STYLUS_ID sid,
            const GUID* pGuidStylusButton,
            POINT* pStylusPos) override;

        HRESULT STDMETHODCALLTYPE StylusButtonUp(
            IRealTimeStylus* piRtsSrc,
            STYLUS_ID sid,
            const GUID* pGuidStylusButton,
            POINT* pStylusPos) override;

        HRESULT STDMETHODCALLTYPE InAirPackets(
            IRealTimeStylus* piRtsSrc,
            const StylusInfo* pStylusInfo,
            ULONG cPktCount,
            ULONG cPktBuffLength,
            LONG* pPackets,
            ULONG* pcInOutPkts,
            LONG** ppInOutPkts) override;

        HRESULT STDMETHODCALLTYPE Packets(
            IRealTimeStylus* piRtsSrc,
            const StylusInfo* pStylusInfo,
            ULONG cPktCount,
            ULONG cPktBuffLength,
            LONG* pPackets,
            ULONG* pcInOutPkts,
            LONG** ppInOutPkts) override;

        HRESULT STDMETHODCALLTYPE CustomStylusDataAdded(
            IRealTimeStylus* piRtsSrc,
            const GUID* pGuidId,
            ULONG cbData,
            const BYTE* pbData) override;

        HRESULT STDMETHODCALLTYPE SystemEvent(
            IRealTimeStylus* piRtsSrc,
            TABLET_CONTEXT_ID tcid,
            STYLUS_ID sid,
            SYSTEM_EVENT event,
            SYSTEM_EVENT_DATA eventdata) override;

        HRESULT STDMETHODCALLTYPE TabletAdded(
            IRealTimeStylus* piRtsSrc,
            IInkTablet* piTablet) override;

        HRESULT STDMETHODCALLTYPE TabletRemoved(
            IRealTimeStylus* piRtsSrc,
            LONG iTabletIndex) override;

        HRESULT STDMETHODCALLTYPE Error(
            IRealTimeStylus* piRtsSrc,
            IStylusPlugin* piPlugin,
            RealTimeStylusDataInterest dataInterest,
            HRESULT hrErrorCode,
            LONG_PTR* lptrKey) override;

        HRESULT STDMETHODCALLTYPE UpdateMapping(
            IRealTimeStylus* piRtsSrc) override;

        HRESULT STDMETHODCALLTYPE DataInterest(
            RealTimeStylusDataInterest* pDataInterest) override;

    private:
        ULONG refCount;
        IUnknown* punkFTMarshaller;
    };

#endif

#endif  

