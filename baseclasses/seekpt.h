//------------------------------------------------------------------------------
// File: SeekPT.h
//
// Desc: DirectShow base classes.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __seekpt_h__
#define __seekpt_h__


class CSeekingPassThru : public ISeekingPassThru, public CUnknown
{
public:
    static CUnknown *CreateInstance(LPUNKNOWN pUnk, HRESULT *phr);
    CSeekingPassThru(LPCTSTR pName, LPUNKNOWN pUnk, HRESULT *phr);
    ~CSeekingPassThru();

    DECLARE_IUNKNOWN;
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, __deref_out void ** ppv);

    STDMETHODIMP Init(BOOL bSupportRendering, IPin *pPin);

private:
    CPosPassThru              *m_pPosPassThru;
};

#endif
