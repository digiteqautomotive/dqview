#include <QtGlobal>

#if defined(Q_OS_LINUX)

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include <QDir>
#include <QRegExp>
#include "camerainfo.h"

CameraInfo CameraInfo::cameraInfo(const QString &device, int *id)
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
		return mgb4In || mgb4Out
		  ? CameraInfo(Device(mgb4In ? Device::Input : Device::Output,
			*id++, device), "MGB4 PCIe Card")
		  : CameraInfo();

	int err = ioctl(fd, VIDIOC_QUERYCAP, &vcap);
	close(fd);

	return err
	  ? CameraInfo()
	  : CameraInfo(Device(mgb4In ? Device::Input : Device::Output,
		mgb4In || mgb4Out ? *id++ : -1, device), (const char *)vcap.card);
}

QList<CameraInfo> CameraInfo::availableCameras()
{
	QList<CameraInfo> list;
	QRegExp re("video[0-9]+");
	QDir dir("/dev");
	int id = 0;

	QFileInfoList files(dir.entryInfoList(QDir::System));
	for (int i = 0; i < files.size(); i++) {
		if (re.exactMatch(files.at(i).baseName())) {
			CameraInfo info(cameraInfo(files.at(i).absoluteFilePath(), &id));
			if (!info.isNull())
				list.append(info);
		}
	}

	return list;
}


#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)


#include <dshow.h>
#include <wrl/client.h>
#include <QDebug>
#include "camerainfo.h"
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

QList<CameraInfo> CameraInfo::availableCameras()
{
	Microsoft::WRL::ComPtr<IMoniker> p_moniker;
	ULONG i_fetched;
	HRESULT hr;
	QList<CameraInfo> list;

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
				IFG4KsproxySampleConfig *piConfig = NULL;
				IBaseFilter* piFilter;
				hr = p_moniker->BindToObject(NULL, NULL, __uuidof(piFilter),
				  reinterpret_cast<void**>(&piFilter));
				if (SUCCEEDED(hr)) {
					hr = piFilter->QueryInterface(__uuidof(piConfig),
					  reinterpret_cast<void**>(&piConfig));
					if (SUCCEEDED(hr))
						mgb4 = true;
					piFilter->Release();
				}

				CameraInfo ci(devname, mgb4 ? 0 : -1);
				int i = 0;
				while (list.contains(ci)) {
					QString name(devname + QString(" #%1").arg(++i));
					ci = CameraInfo(name, mgb4 ? i : -1);
				}

				list.append(ci);
			}
		}
	}

	return list;
}

#else
#error "unsupported platform"
#endif
