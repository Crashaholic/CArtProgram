#include "RTPen.h"

// CRTSEventHandler constructor.
CRTSEventHandler::CRTSEventHandler()
    : m_cRefCount(1),
    m_punkFTMarshaller(NULL),
    m_nContacts(0)
{
}

// CRTSEventHandler destructor.
CRTSEventHandler::~CRTSEventHandler()
{
    if (m_punkFTMarshaller != NULL)
    {
        m_punkFTMarshaller->Release();
    }
}

IStylusSyncPlugin* CRTSEventHandler::Create(IRealTimeStylus* pRealTimeStylus)
{
    // Check input argument
    if (pRealTimeStylus == NULL)
    {
        ASSERT(pRealTimeStylus != NULL && L"CRTSEventHandler::Create: invalid argument RealTimeStylus");
        return NULL;
    }

    // Instantiate CRTSEventHandler object
    CRTSEventHandler* pSyncEventHandlerRTS = new CRTSEventHandler();
    if (pSyncEventHandlerRTS == NULL)
    {
        ASSERT(pSyncEventHandlerRTS != NULL && L"CRTSEventHandler::Create: cannot create instance of CRTSEventHandler");
        return NULL;
    }

    // Create free-threaded marshaller for this object and aggregate it.
    HRESULT hr = CoCreateFreeThreadedMarshaler(pSyncEventHandlerRTS, &pSyncEventHandlerRTS->m_punkFTMarshaller);
    if (FAILED(hr))
    {
        ASSERT(SUCCEEDED(hr) && L"CRTSEventHandler::Create: cannot create free-threaded marshaller");
        pSyncEventHandlerRTS->Release();
        return NULL;
    }

    // Add CRTSEventHandler object to the list of synchronous plugins in the RTS object.
    hr = pRealTimeStylus->AddStylusSyncPlugin(
        0,                      // insert plugin at position 0 in the sync plugin list
        pSyncEventHandlerRTS);  // plugin to be inserted - event handler CRTSEventHandler
    if (FAILED(hr))
    {
        ASSERT(SUCCEEDED(hr) && L"CEventHandlerRTS::Create: failed to add CRTSEventHandler to the RealTimeStylus plugins");
        pSyncEventHandlerRTS->Release();
        return NULL;
    }

    return pSyncEventHandlerRTS;
}

// Pen-down notification.
// Sets the color for the newly started stroke and increments finger-down counter.
// in:
//      piRtsSrc            RTS object that has sent this event
//      pStylusInfo         StylusInfo struct (context ID, cursor ID, etc)
//      cPropCountPerPkt    number of properties per packet
//      pPacket             packet data (layout depends on packet description set)
// in/out:
//      ppInOutPkt          modified packet data (same layout as pPackets)
// returns:
//      HRESULT error code
HRESULT CRTSEventHandler::StylusDown(IRealTimeStylus* piRtsSrc, const StylusInfo* pStylusInfo,
    ULONG cPropCountPerPkt,
    LONG* pPacket ,
    LONG** ppInOutPkt )
{
    // Get DrawingAttributes of DynamicRenderer
    IInkDrawingAttributes* pDrawingAttributesDynamicRenderer;
    //HRESULT hr = g_pDynamicRenderer->get_DrawingAttributes(&pDrawingAttributesDynamicRenderer);
    //if (FAILED(hr))
    //{
    //    ASSERT(SUCCEEDED(hr) && L"CRTSEventHandler::StylusDown: failed to get RTS's drawing attributes");
    //    return hr;
    //}

    // Set new stroke color to the DrawingAttributes of the DynamicRenderer
    // If there are no fingers down, this is a primary contact
    //hr = pDrawingAttributesDynamicRenderer->put_Color(GetTouchColor(m_nContacts == 0));
    if (FAILED(hr))
    {
        ASSERT(SUCCEEDED(hr) && L"CRTSEventHandler::StylusDown: failed to set color");
        pDrawingAttributesDynamicRenderer->Release();
        return hr;
    }

    pDrawingAttributesDynamicRenderer->Release();

    ++m_nContacts;  // Increment finger-down counter

    return S_OK;
}

// Pen-up notification.
// Decrements finger-down counter.
// in:
//      piRtsSrc            RTS object that has sent this event
//      pStylusInfo         StylusInfo struct (context ID, cursor ID, etc)
//      cPropCountPerPkt    number of properties per packet
//      pPacket             packet data (layout depends on packet description set)
// in/out:
//      ppInOutPkt          modified packet data (same layout as pPackets)
// returns:
//      HRESULT error code
HRESULT CRTSEventHandler::StylusUp(
    IRealTimeStylus* /* piRtsSrc */,
    const StylusInfo* /* pStylusInfo */,
    ULONG /* cPropCountPerPkt */,
    LONG* /* pPacket */,
    LONG** /* ppInOutPkt */)
{
    --m_nContacts;  // Decrement finger-down counter

    return S_OK;
}

// Pen-move notification.
// In this case, does nothing, but likely to be used in a more complex application.
// RTS framework does stroke collection and rendering for us.
// in:
//      piRtsRtp            RTS object that has sent this event
//      pStylusInfo         StylusInfo struct (context ID, cursor ID, etc)
//      cPktCount           number of packets
//      cPktBuffLength      pPacket buffer size, in elements, equal to number of packets times number of properties per packet
//      pPackets            packet data (layout depends on packet description set)
// in/out:
//      pcInOutPkts         modified number of packets
//      ppInOutPkts         modified packet data (same layout as pPackets)
// returns:
//      HRESULT error code
HRESULT CRTSEventHandler::Packets(
    IRealTimeStylus* piSrcRtp,
    const StylusInfo* pStylusInfo,
    ULONG cPktCount,
    ULONG cPktBuffLength,
    LONG* pPackets,
    ULONG* pcInOutPkts,
    LONG** ppInOutPkts)
{

    //g_Trace += 1;
    //uint32 iCtx = c_nMaxContexts;
    pStylusInfo->tcid;

    return S_OK;
}

// Defines which handlers are called by the framework. We set the flags for pen-down, pen-up and pen-move.
// in/out:
//      pDataInterest       flags that enable/disable notification handlers
// returns:
//      HRESULT error code
HRESULT CRTSEventHandler::DataInterest(RealTimeStylusDataInterest* pDataInterest)
{
    *pDataInterest = (RealTimeStylusDataInterest)(RTSDI_AllData);

    return S_OK;
}

// Increments reference count of the COM object.
// returns:
//      reference count
ULONG CRTSEventHandler::AddRef()
{
    return InterlockedIncrement(&m_cRefCount);
}

// Decrements reference count of the COM object, and deletes it
// if there are no more references left.
// returns:
//      reference count
ULONG CRTSEventHandler::Release()
{
    ULONG cNewRefCount = InterlockedDecrement(&m_cRefCount);
    if (cNewRefCount == 0)
    {
        delete this;
    }
    return cNewRefCount;
}

// Returns a pointer to any interface supported by this object.
// If IID_IMarshal interface is requested, delegate the call to the aggregated
// free-threaded marshaller.
// If a valid pointer is returned, COM object reference count is increased.
// returns:
//      pointer to the interface requested, or NULL if the interface is not supported by this object
HRESULT CRTSEventHandler::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
    if ((riid == IID_IStylusSyncPlugin) || (riid == IID_IUnknown))
    {
        *ppvObj = this;
        AddRef();
        return S_OK;
    }
    else if ((riid == IID_IMarshal) && (m_punkFTMarshaller != NULL))
    {
        return m_punkFTMarshaller->QueryInterface(riid, ppvObj);
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

IRealTimeStylus* CreateRealTimeStylus(HWND hWnd)
{
    // Check input argument
    if (hWnd == NULL)
    {
        ASSERT(hWnd && L"CreateRealTimeStylus: invalid argument hWnd");
        return NULL;
    }

    // Create RTS object
    IRealTimeStylus* pRealTimeStylus = NULL;
    HRESULT hr = CoCreateInstance(CLSID_RealTimeStylus, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pRealTimeStylus));
    if (FAILED(hr))
    {
        ASSERT(SUCCEEDED(hr) && L"CreateRealTimeStylus: failed to CoCreateInstance of RealTimeStylus");
        return NULL;
    }

    // Attach RTS object to a window
    hr = pRealTimeStylus->put_HWND((HANDLE_PTR)hWnd);
    if (FAILED(hr))
    {
        ASSERT(SUCCEEDED(hr) && L"CreateRealTimeStylus: failed to set window handle");
        pRealTimeStylus->Release();
        return NULL;
    }

    // Register RTS object for receiving multi-touch input.
    IRealTimeStylus3* pRealTimeStylus3 = NULL;
    hr = pRealTimeStylus->QueryInterface(&pRealTimeStylus3);
    if (FAILED(hr))
    {
        ASSERT(SUCCEEDED(hr) && L"CreateRealTimeStylus: cannot access IRealTimeStylus3");
        pRealTimeStylus->Release();
        return NULL;
    }
    hr = pRealTimeStylus3->put_MultiTouchEnabled(TRUE);
    if (FAILED(hr))
    {
        ASSERT(SUCCEEDED(hr) && L"CreateRealTimeStylus: failed to enable multi-touch");
        pRealTimeStylus->Release();
        pRealTimeStylus3->Release();
        return NULL;
    }
    pRealTimeStylus3->Release();

    return pRealTimeStylus;
}

bool EnableRealTimeStylus(IRealTimeStylus* pRealTimeStylus, IDynamicRenderer* pDynamicRenderer)
{
    // Check input arguments
    if (pRealTimeStylus == NULL)
    {
        ASSERT(pRealTimeStylus && L"EnableRealTimeStylus: invalid argument RealTimeStylus");
        return NULL;
    }
    if (pDynamicRenderer == NULL)
    {
        ASSERT(pDynamicRenderer && L"EnableRealTimeStylus: invalid argument DynamicRenderer");
        return false;
    }

    // Enable RTS object
    HRESULT hr = pRealTimeStylus->put_Enabled(TRUE);
    if (FAILED(hr))
    {
        ASSERT(SUCCEEDED(hr) && L"EnableRealTimeStylus: failed to enable RealTimeStylus");
        return false;
    }

    // Enable DynamicRenderer
    hr = pDynamicRenderer->put_Enabled(TRUE);
    if (FAILED(hr))
    {
        ASSERT(SUCCEEDED(hr) && L"EnableRealTimeStylus: failed to enable DynamicRenderer");
        return false;
    }

    // Enable DynamicRenderer's auto-redraw feature
    hr = pDynamicRenderer->put_DataCacheEnabled(TRUE);
    if (FAILED(hr))
    {
        ASSERT(SUCCEEDED(hr) && L"EnableRealTimeStylus: failed to enable DynamicRenderer data cache");
        return false;
    }

    return true;
}
