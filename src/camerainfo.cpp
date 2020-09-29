#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include <QDir>
#include <QRegExp>
#include "camerainfo.h"

CameraInfo CameraInfo::cameraInfo(const QString &device)
{
	QByteArray ba = device.toLatin1();
	struct v4l2_capability vcap;
	int fd;

	if ((fd = open(ba.constData(), O_RDWR)) < 0)
		return CameraInfo();
	int err = ioctl(fd, VIDIOC_QUERYCAP, &vcap);
	close(fd);

	return err ? CameraInfo() : CameraInfo(device, (const char *)vcap.card);
}

QList<CameraInfo> CameraInfo::availableCameras()
{
	QList<CameraInfo> list;
	QRegExp re("video[0-9]+");
	QDir dir("/dev");

	QFileInfoList files(dir.entryInfoList(QDir::System));
	for (int i = 0; i < files.size(); i++) {
		if (re.exactMatch(files.at(i).baseName())) {
			CameraInfo info(cameraInfo(files.at(i).absoluteFilePath()));
			if (!info.isNull())
				list.append(info);
		}
	}

	return list;
}
