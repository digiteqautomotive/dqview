//------------------------------------------------------------------------------
// File: CProp.h
//
// Desc: DirectShow base classes.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __CPROP__
#define __CPROP__

// Base property page class. Filters typically expose custom properties by
// implementing special control interfaces, examples are IDirectDrawVideo
// and IQualProp on renderers. This allows property pages to be built that
// use the given interface. Applications such as the ActiveMovie OCX query
// filters for the property pages they support and expose them to the user
//
// This class provides all the framework for a property page. A property
// page is a COM object that supports IPropertyPage. We should be created
// with a resource ID for the dialog which we will load when required. We
// should also be given in the constructor a resource ID for a title string
// we will load from the DLLs STRINGTABLE. The property page titles must be
// stored in resource files so that they can be easily internationalised
//
// We have a number of virtual methods (not PURE) that may be overriden in
// derived classes to query for interfaces and so on. These functions have
// simple implementations here that just return NOERROR. Derived classes
// will almost definately have to override the message handler method called
// OnReceiveMessage. We have a static dialog procedure that calls the method
// so that derived classes don't have to fiddle around with the this pointer

class CBasePropertyPage : public IPropertyPage, public CUnknown
{
protected:

    LPPROPERTYPAGESITE m_pPageSite;       // Details for our property site
    HWND m_hwnd;                          // Window handle for the page
    HWND m_Dlg;                           // Actual dialog window handle
    BOOL m_bDirty;                        // Has anything been changed
    int m_TitleId;                        // Resource identifier for title
    int m_DialogId;                       // Dialog resource identifier

    static INT_PTR CALLBACK DialogProc(HWND hwnd,
                                       UINT uMsg,
                                       WPARAM wParam,
                                       LPARAM lParam);

private:
    BOOL m_bObjectSet ;                  // SetObject has been called or not.
public:

	CBasePropertyPage(LPCTSTR pName,      // Debug only name
					  LPUNKNOWN pUnk, // COM Delegator
                      int DialogId,               // Resource ID
                      int TitleId);               // To get tital

#ifdef UNICODE
	CBasePropertyPage(LPCSTR pName,
					  LPUNKNOWN pUnk,
                      int DialogId,  
                      int TitleId);
#endif
	virtual ~CBasePropertyPage() { }
    DECLARE_IUNKNOWN

    // Override these virtual methods

	virtual HRESULT OnConnect(IUnknown *) { return NOERROR; }
	virtual HRESULT OnDisconnect() { return NOERROR; }
	virtual HRESULT OnActivate() { return NOERROR; }
	virtual HRESULT OnDeactivate() { return NOERROR; }
	virtual HRESULT OnApplyChanges() { return NOERROR; }
    virtual INT_PTR OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

    // These implement an IPropertyPage interface

	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
    STDMETHODIMP_(ULONG) NonDelegatingRelease();
    STDMETHODIMP_(ULONG) NonDelegatingAddRef();
	STDMETHODIMP SetPageSite(LPPROPERTYPAGESITE pPageSite);
    STDMETHODIMP Activate(HWND hwndParent, LPCRECT prect,BOOL fModal);
    STDMETHODIMP Deactivate(void);
	STDMETHODIMP GetPageInfo(LPPROPPAGEINFO pPageInfo);
	STDMETHODIMP SetObjects(ULONG cObjects, LPUNKNOWN *ppUnk);
    STDMETHODIMP Show(UINT nCmdShow);
    STDMETHODIMP Move(LPCRECT prect);
    STDMETHODIMP IsPageDirty(void) { return m_bDirty ? S_OK : S_FALSE; }
    STDMETHODIMP Apply(void);
	STDMETHODIMP Help(LPCWSTR) { return E_NOTIMPL; }
	STDMETHODIMP TranslateAccelerator(LPMSG) { return E_NOTIMPL; }
};

#endif // __CPROP__
