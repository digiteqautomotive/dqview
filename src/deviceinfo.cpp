#include <QtGlobal>

#if defined(Q_OS_LINUX)

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include <QDir>
#include <QRegularExpression>
#include "deviceinfo.h"

DeviceInfo *DeviceInfo::deviceInfo(Device::Type type, const QString &device,
  int *id)
{
	QFileInfo fi(device);
	QDir sysfsDir("/sys/class/video4linux/");
	QDir deviceDir(sysfsDir.filePath(fi.fileName()));
	// Use some mgb4-unique properties for detection
	bool mgb4In = deviceDir.exists("color_mapping")
	  && deviceDir.exists("oldi_lane_width");
	bool mgb4Out = deviceDir.exists("display_height")
	  && deviceDir.exists("frame_rate");

	QByteArray ba = device.toLatin1();
	struct v4l2_capability vcap;
	int fd;

	if ((fd = open(ba.constData(), O_RDWR)) < 0)
		return (mgb4In || mgb4Out)
		  ? new DeviceInfo(mgb4In ? Device::Input : Device::Output,
			*id++, device, "MGB4 PCIe Card")
		  : 0;

	int err = ioctl(fd, VIDIOC_QUERYCAP, &vcap);
	close(fd);

	if (err)
		return 0;
	else if (type == Device::Input && vcap.device_caps & V4L2_CAP_VIDEO_CAPTURE)
		return new DeviceInfo(Device::Input, mgb4In ? *id++ : -1, device,
		  (const char *)vcap.card);
	else if (type == Device::Output && vcap.device_caps & V4L2_CAP_VIDEO_OUTPUT)
		return new DeviceInfo(Device::Output, mgb4Out ? *id++ : -1, device,
		  (const char *)vcap.card);
	else
		return 0;
}

QList<DeviceInfo*> DeviceInfo::devices(Device::Type type)
{
	static const QRegularExpression re("video[0-9]+");
	QList<DeviceInfo*> list;
	QDir dir("/dev");
	int id = 0;

	QFileInfoList files(dir.entryInfoList(QDir::System));
	for (int i = 0; i < files.size(); i++) {
		QRegularExpressionMatch match(re.match(files.at(i).baseName()));
		if (match.hasMatch()) {
			DeviceInfo *info(deviceInfo(type, files.at(i).absoluteFilePath(), &id));
			if (info)
				list.append(info);
		}
	}

	return list;
}

QList<DeviceInfo*> DeviceInfo::inputDevices()
{
	return devices(Device::Input);
}

QList<DeviceInfo*> DeviceInfo::outputDevices()
{
	return devices(Device::Output);
}

#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)


#include <dshow.h>
#include <wrl/client.h>
#include <QDebug>
#include "deviceinfo.h"
#include "fg4.h"

static char *FromWide(const wchar_t *wide)
{
	size_t len;
	len = WideCharToMultiByte(CP_UTF8, 0, wide, -1, NULL, 0, NULL, NULL);

	char *out = (char*)malloc(len);
	if (out)
		WideCharToMultiByte(CP_UTF8, 0, wide, -1, out, len, NULL, NULL);
	return out;
}

static QString SN(long val)
{
	char buf[16];

	sprintf(buf, "%03u-%03u-%03u-%03u", (unsigned)val >> 24,
	  ((unsigned)val >> 16) & 0xFF, ((unsigned)val >> 8) & 0xFF,
	  (unsigned)val & 0xFF);

	return QString(buf);
}

static QString deviceName(const QString &devname, int id)
{
	return id ? devname + QString(" #%1").arg(id) : devname;
}

QList<DeviceInfo *> DeviceInfo::inputDevices()
{
	Microsoft::WRL::ComPtr<IMoniker> p_moniker;
	ULONG i_fetched;
	HRESULT hr;
	QList<DeviceInfo*> list;
	QMap<int, long> snMap;

	/* Create the system device enumerator */
	Microsoft::WRL::ComPtr<ICreateDevEnum> p_dev_enum;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
	  IID_ICreateDevEnum, (void**)p_dev_enum.GetAddressOf());
	if (FAILED(hr)) {
		qWarning("failed to create the device enumerator (0x%lX)", hr);
		return list;
	}

	/* Create an enumerator for the video capture devices */
	Microsoft::WRL::ComPtr<IEnumMoniker> p_class_enum;
	hr = p_dev_enum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
	  p_class_enum.GetAddressOf(), 0);
	if (FAILED(hr)) {
		qWarning("failed to create the class enumerator (0x%lX)", hr);
		return list;
	}

	/* If there are no enumerators for the requested type, then
	 * CreateClassEnumerator will succeed, but p_class_enum will be NULL */
	if (p_class_enum == NULL) {
		qWarning("no video capture device was detected");
		return list;
	}

	/* Enumerate the devices */
	/* Note that if the Next() call succeeds but there are no monikers,
	 * it will return S_FALSE (which is not a failure). Therefore, we check
	 * that the return code is S_OK instead of using SUCCEEDED() macro. */
	while (p_class_enum->Next(1, p_moniker.ReleaseAndGetAddressOf(),
	  &i_fetched) == S_OK) {
		Microsoft::WRL::ComPtr<IPropertyBag> p_bag;
		hr = p_moniker->BindToStorage(0, 0, IID_IPropertyBag,
		  (void**)p_bag.GetAddressOf());
		if (SUCCEEDED(hr)) {
			VARIANT var;
			var.vt = VT_BSTR;

			hr = p_bag->Read(L"FriendlyName", &var, NULL);
			if (SUCCEEDED(hr)) {
				char *p_buf = FromWide(var.bstrVal);
				QString devname(p_buf);
				free(p_buf);
				bool mgb4 = false;

				/* Check whether the device is a MGB4 device */
				IFG4InputConfig *piConfig = NULL;
				IBaseFilter* piFilter;
				hr = p_moniker->BindToObject(NULL, NULL, __uuidof(piFilter),
				  reinterpret_cast<void**>(&piFilter));
				if (SUCCEEDED(hr)) {
					hr = piFilter->QueryInterface(__uuidof(piConfig),
					  reinterpret_cast<void**>(&piConfig));
					if (SUCCEEDED(hr)) {
						long sn;
						piConfig->GetCardSerial(&sn);
						snMap.insert(list.size(), sn);
						mgb4 = true;
					}
					piFilter->Release();
				}

				int id = 0;
				for (int i = 0; i < list.size(); i++)
					if (list.at(i)->_device.name().startsWith(devname))
						id++;

				list.append(new DeviceInfo(Device::Input, mgb4 ? id : -1,
				  deviceName(devname, id)));

			}
		}
	}

	if (snMap.size() > 2) {
		for (int i = 0; i < list.size(); i++) {
			QMap<int, long>::const_iterator it = snMap.constFind(i);
			if (it != snMap.constEnd())
				list[i]->setDescription(SN(*it));
		}
	}

	return list;
}

QList<DeviceInfo *> DeviceInfo::outputDevices()
{
	Microsoft::WRL::ComPtr<IMoniker> p_moniker;
	ULONG i_fetched;
	HRESULT hr;
	QList<DeviceInfo*> list;
	QMap<int, long> snMap;

	/* Create the system device enumerator */
	Microsoft::WRL::ComPtr<ICreateDevEnum> p_dev_enum;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
	  IID_ICreateDevEnum, (void**)p_dev_enum.GetAddressOf());
	if (FAILED(hr)) {
		qWarning("failed to create the device enumerator (0x%lX)", hr);
		return list;
	}

	/* Create an enumerator for the video capture devices */
	Microsoft::WRL::ComPtr<IEnumMoniker> p_class_enum;
	hr = p_dev_enum->CreateClassEnumerator(AM_KSCATEGORY_RENDER,
	  p_class_enum.GetAddressOf(), 0);
	if (FAILED(hr)) {
		qWarning("failed to create the class enumerator (0x%lX)", hr);
		return list;
	}

	/* If there are no enumerators for the requested type, then
	 * CreateClassEnumerator will succeed, but p_class_enum will be NULL */
	if (p_class_enum == NULL) {
		qWarning("no video output device was detected");
		return list;
	}

	/* Enumerate the devices */
	/* Note that if the Next() call succeeds but there are no monikers,
	 * it will return S_FALSE (which is not a failure). Therefore, we check
	 * that the return code is S_OK instead of using SUCCEEDED() macro. */
	while (p_class_enum->Next(1, p_moniker.ReleaseAndGetAddressOf(),
	  &i_fetched) == S_OK) {
		Microsoft::WRL::ComPtr<IPropertyBag> p_bag;
		hr = p_moniker->BindToStorage(0, 0, IID_IPropertyBag,
		  (void**)p_bag.GetAddressOf());
		if (SUCCEEDED(hr)) {
			VARIANT var;
			var.vt = VT_BSTR;
			hr = p_bag->Read(L"FriendlyName", &var, NULL);
			if (SUCCEEDED(hr)) {
				char *p_buf = FromWide(var.bstrVal);
				QString devname(p_buf);
				free(p_buf);

				/* Check whether the device is a MGB4 device */
				IFG4OutputConfig *piConfig = NULL;
				IBaseFilter* piFilter;
				hr = p_moniker->BindToObject(NULL, NULL, __uuidof(piFilter),
				  reinterpret_cast<void**>(&piFilter));
				if (SUCCEEDED(hr)) {
					hr = piFilter->QueryInterface(__uuidof(piConfig),
					  reinterpret_cast<void**>(&piConfig));
					/* There is nothing we can do with output devices other
					   than mgb4, so skip such devices. */
					if (!SUCCEEDED(hr))
						continue;
					long sn;
					piConfig->GetCardSerial(&sn);
					snMap.insert(list.size(), sn);
					piFilter->Release();
				}

				int id = 0;
				for (int i = 0; i < list.size(); i++)
					if (list.at(i)->_device.name().startsWith(devname))
						id++;

				list.append(new DeviceInfo(Device::Output, id,
				  deviceName(devname, id)));
			}
		}
	}

	if (snMap.size() > 2) {
		for (int i = 0; i < list.size(); i++) {
			QMap<int, long>::const_iterator it = snMap.constFind(i);
			if (it != snMap.constEnd())
				list[i]->setDescription(SN(*it));
		}
	}

	return list;
}
#else
#error "unsupported platform"
#endif
