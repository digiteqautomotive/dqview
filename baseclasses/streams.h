//------------------------------------------------------------------------------
// File: Streams.h
//
// Desc: DirectShow base classes - defines overall streams architecture.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __STREAMS__
#define __STREAMS__

#ifdef	_MSC_VER
// disable some level-4 warnings, use #pragma warning(enable:###) to re-enable
#pragma warning(disable:4100) // warning C4100: unreferenced formal parameter
#pragma warning(disable:4201) // warning C4201: nonstandard extension used : nameless struct/union
#pragma warning(disable:4511) // warning C4511: copy constructor could not be generated
#pragma warning(disable:4512) // warning C4512: assignment operator could not be generated
#pragma warning(disable:4514) // warning C4514: "unreferenced inline function has been removed"
#endif	// MSC_VER

// Because of differences between Visual C++ and older Microsoft SDKs,
// you may have defined _DEBUG without defining DEBUG.  This logic
// ensures that both will be set if Visual C++ sets _DEBUG.
#ifdef _DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif


#include <windows.h>
#include <windowsx.h>
#include <olectl.h>
#include <ddraw.h>
#include <mmsystem.h>

#ifdef __GNUC__
 #ifndef min
  #define min(_a,_b)	((_a<_b)?(_a):(_b))
 #endif
 #ifndef max
  #define max(_a,_b)	((_a>_b)?(_a):(_b))
 #endif
// These macros define some standard bitmap format sizes

 #define SIZE_EGA_PALETTE (iEGA_COLORS * sizeof(RGBQUAD))
 #define SIZE_PALETTE (iPALETTE_COLORS * sizeof(RGBQUAD))
 #define SIZE_MASKS (iMASK_COLORS * sizeof(DWORD))
 #define SIZE_PREHEADER (FIELD_OFFSET(VIDEOINFOHEADER,bmiHeader))
 #define SIZE_VIDEOHEADER (sizeof(BITMAPINFOHEADER) + SIZE_PREHEADER)

 #define WIDTHBYTES(bits) ((DWORD)(((bits)+31) & (~31)) / 8)
 #define DIBWIDTHBYTES(bi) (DWORD)WIDTHBYTES((DWORD)(bi).biWidth * (DWORD)(bi).biBitCount)
 #ifndef _DIBSIZE
  #define _DIBSIZE(bi) (DIBWIDTHBYTES(bi) * (DWORD)(bi).biHeight)
 #endif
 #ifndef DIBSIZE
  #define DIBSIZE(bi) ((bi).biHeight < 0 ? (-1)*(_DIBSIZE(bi)) : _DIBSIZE(bi))
 #endif

 #ifdef _INTSAFE_H_INCLUDED_
 inline HRESULT SAFE_DIBWIDTHBYTES(const BITMAPINFOHEADER *pbi, DWORD *pcbWidth)
 {
    DWORD dw;
    HRESULT hr;
    if (pbi->biWidth < 0 || pbi->biBitCount <= 0) {
        return E_INVALIDARG;
    }
    //  Calculate width in bits
    hr = DWordMult((DWORD)pbi->biWidth, (DWORD)pbi->biBitCount, &dw);
    if (FAILED(hr)) {
        return hr;
    }
    //  Round up to bytes
    dw = (dw & 7) ? dw / 8 + 1: dw / 8;

    //  Round up to a multiple of 4 bytes
    if (dw & 3) {
        dw += 4 - (dw & 3);
    }

    *pcbWidth = dw;
    return S_OK;
 }

 inline HRESULT SAFE_DIBSIZE(const BITMAPINFOHEADER *pbi, DWORD *pcbSize)
 {
    DWORD dw;
    DWORD dwWidthBytes;
    HRESULT hr;
	if (pbi->biHeight == (LONG)0x80000000) {
        return E_INVALIDARG;
    }
    hr = SAFE_DIBWIDTHBYTES(pbi, &dwWidthBytes);
    if (FAILED(hr)) {
        return hr;
    }
    dw = abs(pbi->biHeight);
    hr = DWordMult(dw, dwWidthBytes, &dw);
    if (FAILED(hr)) {
        return hr;
    }
    *pcbSize = dw;
    return S_OK;
 }
 #endif

	// Other (hopefully) useful bits and bobs
 #define PALETTISED(pbmi) ((pbmi)->bmiHeader.biBitCount <= iPALETTE)
 #define PALETTE_ENTRIES(pbmi) ((DWORD) 1 << (pbmi)->bmiHeader.biBitCount)

	// Returns the address of the BITMAPINFOHEADER from the VIDEOINFOHEADER
 #define HEADER(pVideoInfo) (&(((VIDEOINFOHEADER *) (pVideoInfo))->bmiHeader))
 #define iPALETTE_COLORS 256     // Maximum colours in palette
 #define iEGA_COLORS 16          // Number colours in EGA palette
 #define iMASK_COLORS 3          // Maximum three components
 #define iTRUECOLOR 16           // Minimum true colour device
 #define iRED 0                  // Index position for RED mask
 #define iGREEN 1                // Index position for GREEN mask
 #define iBLUE 2                 // Index position for BLUE mask
 #define iPALETTE 8              // Maximum colour depth using a palette
 #define iMAXBITS 8              // Maximum bits per colour component

// make sure the pbmi is initialized before using these macros
 #ifndef TRUECOLOR
  #define TRUECOLOR(pbmi)  ((TRUECOLORINFO *)(((LPBYTE)&((pbmi)->bmiHeader)) \
					+ (pbmi)->bmiHeader.biSize))
 #endif
 #ifndef COLORS
  #define COLORS(pbmi)	((RGBQUAD *)(((LPBYTE)&((pbmi)->bmiHeader)) 	\
					+ (pbmi)->bmiHeader.biSize))
 #endif
 #ifndef BITMASKS
  #define BITMASKS(pbmi)	((DWORD *)(((LPBYTE)&((pbmi)->bmiHeader)) 	\
 					+ (pbmi)->bmiHeader.biSize))
 #endif
#endif


#ifndef NUMELMS
#if _WIN32_WINNT < 0x0600
   #define NUMELMS(aa) (sizeof(aa)/sizeof((aa)[0]))
#else
   #define NUMELMS(aa) ARRAYSIZE(aa)
#endif   
#endif

///////////////////////////////////////////////////////////////////////////
// The following definitions come from the Platform SDK and are required if
// the applicaiton is being compiled with the headers from Visual C++ 6.0.
/////////////////////////////////////////////////// ////////////////////////
#ifndef InterlockedExchangePointer
	#define InterlockedExchangePointer(Target, Value) \
   (PVOID)InterlockedExchange((PLONG)(Target), (LONG)(Value))
#endif

#ifndef _WAVEFORMATEXTENSIBLE_
#define _WAVEFORMATEXTENSIBLE_
typedef struct {
    WAVEFORMATEX    Format;
    union {
        WORD wValidBitsPerSample;       /* bits of precision  */
        WORD wSamplesPerBlock;          /* valid if wBitsPerSample==0 */
        WORD wReserved;                 /* If neither applies, set to zero. */
    } Samples;
    DWORD           dwChannelMask;      /* which channels are */
                                        /* present in stream  */
    GUID            SubFormat;
} WAVEFORMATEXTENSIBLE, *PWAVEFORMATEXTENSIBLE;
#endif // !_WAVEFORMATEXTENSIBLE_

#if !defined(WAVE_FORMAT_EXTENSIBLE)
#define  WAVE_FORMAT_EXTENSIBLE                 0xFFFE
#endif // !defined(WAVE_FORMAT_EXTENSIBLE)

#ifndef GetWindowLongPtr
  #define GetWindowLongPtrA   GetWindowLongA
  #define GetWindowLongPtrW   GetWindowLongW
  #ifdef UNICODE
    #define GetWindowLongPtr  GetWindowLongPtrW
  #else
    #define GetWindowLongPtr  GetWindowLongPtrA
  #endif // !UNICODE
#endif // !GetWindowLongPtr

#ifndef SetWindowLongPtr
  #define SetWindowLongPtrA   SetWindowLongA
  #define SetWindowLongPtrW   SetWindowLongW
  #ifdef UNICODE
    #define SetWindowLongPtr  SetWindowLongPtrW
  #else
    #define SetWindowLongPtr  SetWindowLongPtrA
  #endif // !UNICODE
#endif // !SetWindowLongPtr

#ifndef GWLP_WNDPROC
  #define GWLP_WNDPROC        (-4)
#endif
#ifndef GWLP_HINSTANCE
  #define GWLP_HINSTANCE      (-6)
#endif
#ifndef GWLP_HWNDPARENT
  #define GWLP_HWNDPARENT     (-8)
#endif
#ifndef GWLP_USERDATA
  #define GWLP_USERDATA       (-21)
#endif
#ifndef GWLP_ID
  #define GWLP_ID             (-12)
#endif
#ifndef DWLP_MSGRESULT
  #define DWLP_MSGRESULT  0
#endif
#ifndef DWLP_DLGPROC 
  #define DWLP_DLGPROC    DWLP_MSGRESULT + sizeof(LRESULT)
#endif
#ifndef DWLP_USER
  #define DWLP_USER       DWLP_DLGPROC + sizeof(DLGPROC)
#endif

// _GetWindowLongPtr
// Templated version of GetWindowLongPtr, to suppress spurious compiler warning.
template <class T>
T _GetWindowLongPtr(HWND hwnd, int nIndex)
{
    return (T)GetWindowLongPtr(hwnd, nIndex);
}

// _SetWindowLongPtr
// Templated version of SetWindowLongPtr, to suppress spurious compiler warning.
template <class T>
LONG_PTR _SetWindowLongPtr(HWND hwnd, int nIndex, T p)
{
    return SetWindowLongPtr(hwnd, nIndex, (LONG_PTR)p);
}

///////////////////////////////////////////////////////////////////////////
// End Platform SDK definitions
///////////////////////////////////////////////////////////////////////////


#include <strmif.h>     // Generated IDL header file for streams interfaces
#include <intsafe.h>    // required by amvideo.h

#include "reftime.h"    // Helper class for REFERENCE_TIME management
#include "wxdebug.h"    // Debug support for logging and ASSERTs
#include <amvideo.h>    // ActiveMovie video interfaces and definitions
//include amaudio.h explicitly if you need it.  it requires the DX SDK.
//#include <amaudio.h>    // ActiveMovie audio interfaces and definitions
#include "wxutil.h"	// General helper classes for threads etc
#include "combase.h"	// Base COM classes to support IUnknown
#include "dllsetup.h"	// Filter registration support functions
#include "measure.h"	// Performance measurement
#include <comlite.h>	// Light weight com function prototypes

#include "cache.h"	// Simple cache container class
#include "wxlist.h"	// Non MFC generic list class
#include "msgthrd.h"	// CMsgThread
#include "mtype.h"	// Helper class for managing media types
#include "fourcc.h"	// conversions between FOURCCs and GUIDs
#include <control.h>	// generated from control.odl
#include "ctlutil.h"	// control interface utility classes
#include <evcode.h>	// event code definitions
#include "amfilter.h"	// Main streams architecture class hierachy
#include "transfrm.h"	// Generic transform filter
#include "transip.h"    // Generic transform-in-place filter
#include <uuids.h>      // declaration of type GUIDs and well-known clsids
#include "source.h"	// Generic source filter
#include "outputq.h"    // Output pin queueing
#include <errors.h>     // HRESULT status and error definitions
#include "renbase.h"    // Base class for writing ActiveX renderers
#include "winutil.h"    // Helps with filters that manage windows
#include "winctrl.h"	// Implements the IVideoWindow interface
#include "videoctl.h"	// Specifically video related classes
#include "refclock.h"	// Base clock class
#include "sysclock.h"	// System clock
#include "pstream.h"	// IPersistStream helper class
#include "vtrans.h"	// Video Transform Filter base class
#include "amextra.h"
#include "cprop.h"	// Base property page class
#include "strmctl.h"    // IAMStreamControl support
#include <edevdefs.h>   // External device control interface defines
#include <audevcod.h>   // audio filter device error event codes



#else
    #ifdef DEBUG
    #pragma message("STREAMS.H included TWICE")
    #endif
#endif // __STREAMS__

