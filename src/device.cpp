#include "device.h"

#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)

#include <Windows.h>
#include <uuids.h>
#include <strmif.h>
#include "fg4.h"

static IFG4InputConfig *inputConfig(int id)
{
	IFG4InputConfig *piConfig = NULL;
	HRESULT hr;
	ICreateDevEnum *piCreateDevEnum;

	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
	  __uuidof(piCreateDevEnum), reinterpret_cast<void**>(&piCreateDevEnum));
	if (SUCCEEDED(hr)) {
		int Xorder = id;
		IEnumMoniker *piEnumMoniker;

		hr = piCreateDevEnum->CreateClassEnumerator(AM_KSCATEGORY_CAPTURE,
		  &piEnumMoniker, 0);
		piCreateDevEnum->Release();
		if (SUCCEEDED(hr))
			hr = piEnumMoniker->Reset();
		if (SUCCEEDED(hr)) {
			ULONG cFetched;
			IMoniker *piMoniker;
			while ((hr = piEnumMoniker->Next(1, &piMoniker, &cFetched)) == S_OK) {
				IBaseFilter *piFilter;
				hr = piMoniker->BindToObject(NULL, NULL, __uuidof(piFilter),
				  reinterpret_cast<void**>(&piFilter));
				if (SUCCEEDED(hr)) {
					hr = piFilter->QueryInterface(__uuidof(piConfig),
					  reinterpret_cast< void**>(&piConfig));
					if (FAILED(hr))
						piFilter->Release();
					else {
						if (Xorder > 0) {
							Xorder--;
							piFilter->Release();
							piConfig->Release();
						} else {
							piFilter->Release();
							piMoniker->Release();
							piEnumMoniker->Release();

							return piConfig;
						}
					}
				}
				piMoniker->Release();
			}
		}
		piEnumMoniker->Release();
	}

	return 0;
}

static IFG4OutputConfig *outputConfig(int id)
{
	IFG4OutputConfig *piConfig = NULL;
	HRESULT hr;
	ICreateDevEnum *piCreateDevEnum;

	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
	  __uuidof(piCreateDevEnum), reinterpret_cast<void**>(&piCreateDevEnum));
	if (SUCCEEDED(hr)) {
		int Xorder = id;
		IEnumMoniker *piEnumMoniker;

		hr = piCreateDevEnum->CreateClassEnumerator(AM_KSCATEGORY_RENDER,
		  &piEnumMoniker, 0);
		piCreateDevEnum->Release();
		if (SUCCEEDED(hr))
			hr = piEnumMoniker->Reset();
		if (SUCCEEDED(hr)) {
			ULONG cFetched;
			IMoniker *piMoniker;
			while ((hr = piEnumMoniker->Next(1, &piMoniker, &cFetched)) == S_OK) {
				IBaseFilter *piFilter;
				hr = piMoniker->BindToObject(NULL, NULL, __uuidof(piFilter),
				  reinterpret_cast<void**>(&piFilter));
				if (SUCCEEDED(hr)) {
					hr = piFilter->QueryInterface(__uuidof(piConfig),
					  reinterpret_cast< void**>(&piConfig));
					if (FAILED(hr))
						piFilter->Release();
					else {
						if (Xorder > 0) {
							Xorder--;
							piFilter->Release();
							piConfig->Release();
						} else {
							piFilter->Release();
							piMoniker->Release();
							piEnumMoniker->Release();

							return piConfig;
						}
					}
				}
				piMoniker->Release();
			}
		}
		piEnumMoniker->Release();
	}

	return 0;
}

void *Device::config() const
{
	switch (_type) {
		case Input:
			return (void*)inputConfig(_id);
		case Output:
			return (void*)outputConfig(_id);
		default:
			return 0;
	}
}

#endif
