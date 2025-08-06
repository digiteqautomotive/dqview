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

static IBaseFilter *outputFilter(int id)
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
							piConfig->Release();
							piMoniker->Release();
							piEnumMoniker->Release();

							return piFilter;
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

Device::Device(Type type, int id, const QString &name)
  : _type(type), _id(id), _name(name), _config(0), _filter(0)
{
	if (type == Unknown || _id < 0)
		_format = PixelFormat::UnknownFormat;
	else
		_format = (type == Output) ? PixelFormat::YUV : PixelFormat::RGB;
}

Device::~Device()
{
	if (_config) {
		if (_type == Input)
			((IFG4InputConfig*)_config)->Release();
		else if (_type == Output)
			((IFG4OutputConfig*)_config)->Release();
	}

	if (_filter)
		_filter->Release();
}

void *Device::config()
{
	if (_id < 0)
		return 0;

	switch (_type) {
		case Input:
			if (!_config)
				_config = (void*)inputConfig(_id);
			return _config;
		case Output:
			if (!_config)
				_config = (void*)outputConfig(_id);
			return _config;
		default:
			return 0;
	}
}

IBaseFilter *Device::filter()
{
	switch (_type) {
		case Output:
			if (!_filter)
				_filter = outputFilter(_id);
			return _filter;
		default:
			return 0;
	}
}

#else

Device::Device(Type type, int id, const QString &name)
  : _type(type), _id(id), _name(name)
{

	if (type == Unknown || _id < 0)
		_format = PixelFormat::UnknownFormat;
	else
		_format = (type == Output) ? PixelFormat::YUV : PixelFormat::RGB;
}

#endif
