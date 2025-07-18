//------------------------------------------------------------------------------
// File: Transfrm.h
//
// Desc: DirectShow base classes - defines classes from which simple 
//       transform codecs may be derived.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


// It assumes the codec has one input and one output stream, and has no
// interest in memory management, interface negotiation or anything else.
//
// derive your class from this, and supply Transform and the media type/format
// negotiation functions. Implement that class, compile and link and
// you're done.


#ifndef __TRANSFRM__
#define __TRANSFRM__

// ======================================================================
// This is the com object that represents a simple transform filter. It
// supports IBaseFilter, IMediaFilter and two pins through nested interfaces
// ======================================================================

class CTransformFilter;

// ==================================================
// Implements the input pin
// ==================================================

class CTransformInputPin : public CBaseInputPin
{
    friend class CTransformFilter;

protected:
    CTransformFilter *m_pTransformFilter;


public:

    CTransformInputPin(
		LPCTSTR pObjectName,
		CTransformFilter *pTransformFilter,
		HRESULT * phr,
		LPCWSTR pName);
#ifdef UNICODE
    CTransformInputPin(
		LPCSTR pObjectName,
		CTransformFilter *pTransformFilter,
		HRESULT * phr,
		LPCWSTR pName);
#endif

	STDMETHODIMP QueryId(LPWSTR * Id)
    {
        return AMGetWideString(L"In", Id);
    }

    // Grab and release extra interfaces if required

    HRESULT CheckConnect(IPin *pPin);
    HRESULT BreakConnect();
    HRESULT CompleteConnect(IPin *pReceivePin);

    // check that we can support this output type
    HRESULT CheckMediaType(const CMediaType* mtIn);

    // set the connection media type
    HRESULT SetMediaType(const CMediaType* mt);

    // --- IMemInputPin -----

    // here's the next block of data from the stream.
    // AddRef it yourself if you need to hold it beyond the end
    // of this call.
    STDMETHODIMP Receive(IMediaSample * pSample);

    // provide EndOfStream that passes straight downstream
    // (there is no queued data)
    STDMETHODIMP EndOfStream(void);

    // passes it to CTransformFilter::BeginFlush
    STDMETHODIMP BeginFlush(void);

    // passes it to CTransformFilter::EndFlush
    STDMETHODIMP EndFlush(void);

    STDMETHODIMP NewSegment(
                        REFERENCE_TIME tStart,
                        REFERENCE_TIME tStop,
                        double dRate);

    // Check if it's OK to process samples
    virtual HRESULT CheckStreaming();

    // Media type
public:
    CMediaType& CurrentMediaType() { return m_mt; };

};

// ==================================================
// Implements the output pin
// ==================================================

class CTransformOutputPin : public CBaseOutputPin
{
    friend class CTransformFilter;

protected:
    CTransformFilter *m_pTransformFilter;

public:

    // implement IMediaPosition by passing upstream
    IUnknown * m_pPosition;

    CTransformOutputPin(
		LPCTSTR pObjectName,
		CTransformFilter *pTransformFilter,
		HRESULT * phr,
		LPCWSTR pName);
#ifdef UNICODE
    CTransformOutputPin(
		LPCSTR pObjectName,
		CTransformFilter *pTransformFilter,
		HRESULT * phr,
		LPCWSTR pName);
#endif
    ~CTransformOutputPin();

    // override to expose IMediaPosition
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

    // --- CBaseOutputPin ------------

	STDMETHODIMP QueryId(LPWSTR * Id)
    {
        return AMGetWideString(L"Out", Id);
    }

    // Grab and release extra interfaces if required

    HRESULT CheckConnect(IPin *pPin);
    HRESULT BreakConnect();
    HRESULT CompleteConnect(IPin *pReceivePin);

    // check that we can support this output type
    HRESULT CheckMediaType(const CMediaType* mtOut);

    // set the connection media type
    HRESULT SetMediaType(const CMediaType *pmt);

    // called from CBaseOutputPin during connection to ask for
    // the count and size of buffers we need.
    HRESULT DecideBufferSize(
                IMemAllocator * pAlloc,
				ALLOCATOR_PROPERTIES *pProp);

    // returns the preferred formats for a pin
	HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);

    // inherited from IQualityControl via CBasePin
    STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);

    // Media type
public:
    CMediaType& CurrentMediaType() { return m_mt; };
};


class CTransformFilter : public CBaseFilter
{

public:

    // map getpin/getpincount for base enum of pins to owner
    // override this to return more specialised pin objects

    virtual int GetPinCount();
    virtual CBasePin * GetPin(int n);
	STDMETHODIMP FindPin(LPCWSTR Id, IPin **ppPin);

    // override state changes to allow derived transform filter
    // to control streaming start/stop
    STDMETHODIMP Stop();
    STDMETHODIMP Pause();

public:

	CTransformFilter(LPCTSTR , LPUNKNOWN, REFCLSID clsid);
#ifdef UNICODE
	CTransformFilter(LPCSTR , LPUNKNOWN, REFCLSID clsid);
#endif
    ~CTransformFilter();

    // =================================================================
    // ----- override these bits ---------------------------------------
    // =================================================================

    // These must be supplied in a derived class

    virtual HRESULT Transform(IMediaSample * pIn, IMediaSample *pOut);

    // check if you can support mtIn
    virtual HRESULT CheckInputType(const CMediaType* mtIn) PURE;

    // check if you can support the transform from this input to this output
    virtual HRESULT CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut) PURE;

    // this goes in the factory template table to create new instances
    // static CCOMObject * CreateInstance(__inout_opt LPUNKNOWN, HRESULT *);

    // call the SetProperties function with appropriate arguments
    virtual HRESULT DecideBufferSize(
                        IMemAllocator * pAllocator,
						ALLOCATOR_PROPERTIES *pprop) PURE;

    // override to suggest OUTPUT pin media types
	virtual HRESULT GetMediaType(int iPosition, CMediaType *pMediaType) PURE;



    // =================================================================
    // ----- Optional Override Methods           -----------------------
    // =================================================================

    // you can also override these if you want to know about streaming
    virtual HRESULT StartStreaming();
    virtual HRESULT StopStreaming();

    // override if you can do anything constructive with quality notifications
    virtual HRESULT AlterQuality(Quality q);

    // override this to know when the media type is actually set
    virtual HRESULT SetMediaType(PIN_DIRECTION direction,const CMediaType *pmt);

    // chance to grab extra interfaces on connection
    virtual HRESULT CheckConnect(PIN_DIRECTION dir,IPin *pPin);
    virtual HRESULT BreakConnect(PIN_DIRECTION dir);
    virtual HRESULT CompleteConnect(PIN_DIRECTION direction,IPin *pReceivePin);

    // chance to customize the transform process
    virtual HRESULT Receive(IMediaSample *pSample);

    // Standard setup for output sample
	HRESULT InitializeOutputSample(IMediaSample *pSample, IMediaSample **ppOutSample);

    // if you override Receive, you may need to override these three too
    virtual HRESULT EndOfStream(void);
    virtual HRESULT BeginFlush(void);
    virtual HRESULT EndFlush(void);
    virtual HRESULT NewSegment(
                        REFERENCE_TIME tStart,
                        REFERENCE_TIME tStop,
                        double dRate);

#ifdef PERF
    // Override to register performance measurement with a less generic string
    // You should do this to avoid confusion with other filters
    virtual void RegisterPerfId()
         {m_idTransform = MSR_REGISTER(TEXT("Transform"));}
#endif // PERF


// implementation details

protected:

#ifdef PERF
    int m_idTransform;                 // performance measuring id
#endif
    BOOL m_bEOSDelivered;              // have we sent EndOfStream
    BOOL m_bSampleSkipped;             // Did we just skip a frame
    BOOL m_bQualityChanged;            // Have we degraded?

    // critical section protecting filter state.

    CCritSec m_csFilter;

    // critical section stopping state changes (ie Stop) while we're
    // processing a sample.
    //
    // This critical section is held when processing
    // events that occur on the receive thread - Receive() and EndOfStream().
    //
    // If you want to hold both m_csReceive and m_csFilter then grab
    // m_csFilter FIRST - like CTransformFilter::Stop() does.

    CCritSec m_csReceive;

    // these hold our input and output pins

    friend class CTransformInputPin;
    friend class CTransformOutputPin;
    CTransformInputPin *m_pInput;
    CTransformOutputPin *m_pOutput;
};

#endif /* __TRANSFRM__ */


