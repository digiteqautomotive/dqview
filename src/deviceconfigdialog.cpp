#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QComboBox>
#include <QLabel>
#include <QFormLayout>
#include <QGroupBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QPushButton>
#include <QtMath>
#include "configcopydialog.h"
#include "deviceconfigdialog.h"

#if defined(Q_OS_LINUX)

static bool readSysfsInt(const QString &dev, const QString &path, unsigned *val)
{
	QFileInfo fi(dev);
	QDir sysfsDir("/sys/class/video4linux/");
	QDir deviceDir(sysfsDir.filePath(fi.fileName()));
	QString sysfsPath = deviceDir.filePath(path);

	QFile f(sysfsPath);
	if (!f.open(QIODevice::ReadOnly)) {
		qWarning("%s: %s", qPrintable(sysfsPath), qPrintable(f.errorString()));
		return false;
	}
	bool ok;
	QByteArray ba(f.readLine());
	*val = ba.toUInt(&ok);

	return ok;
}

static bool writeSysfsInt(const QString &dev, const QString &path, unsigned val)
{
	QFileInfo fi(dev);
	QDir sysfsDir("/sys/class/video4linux/");
	QDir deviceDir(sysfsDir.filePath(fi.fileName()));
	QString sysfsPath = deviceDir.filePath(path);

	QFile f(sysfsPath);
	if (!f.open(QIODevice::WriteOnly)) {
		qWarning("%s: %s", qPrintable(sysfsPath), qPrintable(f.errorString()));
		return false;
	}

	QByteArray ba(QByteArray::number(val));
	if (f.write(ba) != ba.size())
		return false;

	return f.flush();
}

static bool readSysfsString(const QString &dev, const QString &path, QString *val)
{
	QFileInfo fi(dev);
	QDir sysfsDir("/sys/class/video4linux/");
	QDir deviceDir(sysfsDir.filePath(fi.fileName()));
	QString sysfsPath = deviceDir.filePath(path);

	QFile f(sysfsPath);
	if (!f.open(QIODevice::ReadOnly)) {
		qWarning("%s: %s", qPrintable(sysfsPath), qPrintable(f.errorString()));
		return false;
	}

	QByteArray ba(f.readLine());
	*val = QString(ba).trimmed();

	return !ba.isEmpty();
}

bool DeviceConfigDialog::getModuleType(ModuleType *type)
{
	return readSysfsInt(_device->name(), "device/module_type", (unsigned*)type);
}

bool DeviceConfigDialog::getModuleVersion(unsigned *version)
{
	return readSysfsInt(_device->name(), "device/module_version", version);
}

bool DeviceConfigDialog::getFwType(FWType *type)
{
	return readSysfsInt(_device->name(), "device/fw_type", (unsigned*)type);
}

bool DeviceConfigDialog::getFwVersion(unsigned *version)
{
	return readSysfsInt(_device->name(), "device/fw_version", version);
}

bool DeviceConfigDialog::getSerialNumber(QString *serialNumber)
{
	return readSysfsString(_device->name(), "device/serial_number", serialNumber);
}


bool InputConfigDialog::getInputId(unsigned *id)
{
	return readSysfsInt(_device->name(), "input_id", id);
}

bool InputConfigDialog::getLinkStatus(LinkStatus *status)
{
	return readSysfsInt(_device->name(), "link_status", (unsigned*)status);
}

bool InputConfigDialog::getVideoWidth(unsigned *width)
{
	return readSysfsInt(_device->name(), "video_width", width);
}

bool InputConfigDialog::getVideoHeight(unsigned *height)
{
	return readSysfsInt(_device->name(), "video_height", height);
}

bool InputConfigDialog::getVSyncStatus(SyncType *status)
{
	return readSysfsInt(_device->name(), "vsync_status", (unsigned*)status);
}

bool InputConfigDialog::getHSyncStatus(SyncType *status)
{
	return readSysfsInt(_device->name(), "hsync_status", (unsigned*)status);
}

bool InputConfigDialog::getColorMapping(ColorMapping *mapping)
{
	return readSysfsInt(_device->name(), "color_mapping", (unsigned*)mapping);
}

bool InputConfigDialog::setColorMapping(ColorMapping mapping)
{
	return writeSysfsInt(_device->name(), "color_mapping", mapping);
}

bool InputConfigDialog::getLaneWidth(LineWidth *lineWidth)
{
	return readSysfsInt(_device->name(), "oldi_lane_width", (unsigned*)lineWidth);
}

bool InputConfigDialog::setLaneWidth(LineWidth lineWidth)
{
	return writeSysfsInt(_device->name(), "oldi_lane_width", lineWidth);
}

bool InputConfigDialog::getVSyncGapLength(unsigned *length)
{
	return readSysfsInt(_device->name(), "vsync_gap_length", length);
}

bool InputConfigDialog::setVSyncGapLength(unsigned length)
{
	return writeSysfsInt(_device->name(), "vsync_gap_length", length);
}

bool InputConfigDialog::getHSyncGapLength(unsigned *length)
{
	return readSysfsInt(_device->name(), "hsync_gap_length", length);
}

bool InputConfigDialog::setHSyncGapLength(unsigned length)
{
	return writeSysfsInt(_device->name(), "hsync_gap_length", length);
}

bool InputConfigDialog::getPclkFreq(unsigned *freq)
{
	return readSysfsInt(_device->name(), "pclk_frequency", freq);
}

bool InputConfigDialog::getHSyncWidth(unsigned *width)
{
	return readSysfsInt(_device->name(), "hsync_width", width);
}

bool InputConfigDialog::getVSyncWidth(unsigned *width)
{
	return readSysfsInt(_device->name(), "vsync_width", width);
}

bool InputConfigDialog::getHBackPorch(unsigned *porch)
{
	return readSysfsInt(_device->name(), "hback_porch", porch);
}

bool InputConfigDialog::getHFrontPorch(unsigned *porch)
{
	return readSysfsInt(_device->name(), "hfront_porch", porch);
}

bool InputConfigDialog::getVBackPorch(unsigned *porch)
{
	return readSysfsInt(_device->name(), "vback_porch", porch);
}

bool InputConfigDialog::getVFrontPorch(unsigned *porch)
{
	return readSysfsInt(_device->name(), "vfront_porch", porch);
}

bool InputConfigDialog::getFreqRange(FreqRange *range)
{
	return readSysfsInt(_device->name(), "frequency_range", (unsigned*)range);
}

bool InputConfigDialog::setFreqRange(FreqRange range)
{
	return writeSysfsInt(_device->name(), "frequency_range", range);
}

bool InputConfigDialog::getFPDL3InputWidth(FPDL3Width *width)
{
	return readSysfsInt(_device->name(), "fpdl3_input_width", (unsigned*)width);
}

bool InputConfigDialog::setFPDL3InputWidth(FPDL3Width width)
{
	return writeSysfsInt(_device->name(), "fpdl3_input_width", width);
}

bool InputConfigDialog::getGMSLMode(GMSLMode *mode)
{
	return readSysfsInt(_device->name(), "gmsl_mode", (unsigned*)mode);
}

bool InputConfigDialog::setGMSLMode(GMSLMode mode)
{
	return writeSysfsInt(_device->name(), "gmsl_mode", mode);
}

bool InputConfigDialog::getGMSLStreamId(unsigned *streamId)
{
	return readSysfsInt(_device->name(), "gmsl_stream_id", streamId);
}

bool InputConfigDialog::setGMSLStreamId(unsigned streamId)
{
	return writeSysfsInt(_device->name(), "gmsl_stream_id", streamId);
}

bool InputConfigDialog::getGMSLFEC(GMSLFEC *fec)
{
	return readSysfsInt(_device->name(), "gmsl_fec", (unsigned*)fec);
}

bool InputConfigDialog::setGMSLFEC(GMSLFEC fec)
{
	return writeSysfsInt(_device->name(), "gmsl_fec", fec);
}


bool OutputConfigDialog::getOutputId(unsigned *id)
{
	return readSysfsInt(_device->name(), "output_id", id);
}

bool OutputConfigDialog::getDisplayWidth(unsigned *width)
{
	return readSysfsInt(_device->name(), "display_width", width);
}

bool OutputConfigDialog::setDisplayWidth(unsigned int width)
{
	return writeSysfsInt(_device->name(), "display_width", width);
}

bool OutputConfigDialog::getDisplayHeight(unsigned *height)
{
	return readSysfsInt(_device->name(), "display_height", height);
}

bool OutputConfigDialog::setDisplayHeight(unsigned int height)
{
	return writeSysfsInt(_device->name(), "display_height", height);
}

bool OutputConfigDialog::getFrameRate(unsigned *frameRate)
{
	return readSysfsInt(_device->name(), "frame_rate", frameRate);
}

bool OutputConfigDialog::setFrameRate(unsigned frameRate)
{
	return writeSysfsInt(_device->name(), "frame_rate", frameRate);
}

bool OutputConfigDialog::getVideoSource(unsigned *source)
{
	return readSysfsInt(_device->name(), "video_source", source);
}

bool OutputConfigDialog::setVideoSource(unsigned source)
{
	return writeSysfsInt(_device->name(), "video_source", source);
}

bool OutputConfigDialog::getPclkFreq(unsigned *freq)
{
	return readSysfsInt(_device->name(), "pclk_frequency", freq);
}

bool OutputConfigDialog::setPclkFreq(unsigned freq)
{
	return writeSysfsInt(_device->name(), "pclk_frequency", freq);
}

bool OutputConfigDialog::getHsyncPolarity(SyncType *polarity)
{
	return readSysfsInt(_device->name(), "hsync_polarity", (unsigned*)polarity);
}

bool OutputConfigDialog::setHsyncPolarity(SyncType polarity)
{
	return writeSysfsInt(_device->name(), "hsync_polarity", polarity);
}

bool OutputConfigDialog::getVsyncPolarity(SyncType *polarity)
{
	return readSysfsInt(_device->name(), "vsync_polarity", (unsigned*)polarity);
}

bool OutputConfigDialog::setVsyncPolarity(SyncType polarity)
{
	return writeSysfsInt(_device->name(), "vsync_polarity", polarity);
}

bool OutputConfigDialog::getDePolarity(SyncType *polarity)
{
	return readSysfsInt(_device->name(), "de_polarity", (unsigned*)polarity);
}

bool OutputConfigDialog::setDePolarity(SyncType polarity)
{
	return writeSysfsInt(_device->name(), "de_polarity", polarity);
}

bool OutputConfigDialog::getHSyncWidth(unsigned *width)
{
	return readSysfsInt(_device->name(), "hsync_width", width);
}

bool OutputConfigDialog::setHSyncWidth(unsigned width)
{
	return writeSysfsInt(_device->name(), "hsync_width", width);
}

bool OutputConfigDialog::getVSyncWidth(unsigned *width)
{
	return readSysfsInt(_device->name(), "vsync_width", width);
}

bool OutputConfigDialog::setVSyncWidth(unsigned width)
{
	return writeSysfsInt(_device->name(), "vsync_width", width);
}

bool OutputConfigDialog::getHBackPorch(unsigned *porch)
{
	return readSysfsInt(_device->name(), "hback_porch", porch);
}

bool OutputConfigDialog::setHBackPorch(unsigned porch)
{
	return writeSysfsInt(_device->name(), "hback_porch", porch);
}

bool OutputConfigDialog::getHFrontPorch(unsigned *porch)
{
	return readSysfsInt(_device->name(), "hfront_porch", porch);
}

bool OutputConfigDialog::setHFrontPorch(unsigned porch)
{
	return writeSysfsInt(_device->name(), "hfront_porch", porch);
}

bool OutputConfigDialog::getVBackPorch(unsigned *porch)
{
	return readSysfsInt(_device->name(), "vback_porch", porch);
}

bool OutputConfigDialog::setVBackPorch(unsigned porch)
{
	return writeSysfsInt(_device->name(), "vback_porch", porch);
}

bool OutputConfigDialog::getVFrontPorch(unsigned *porch)
{
	return readSysfsInt(_device->name(), "vfront_porch", porch);
}

bool OutputConfigDialog::setVFrontPorch(unsigned porch)
{
	return writeSysfsInt(_device->name(), "vfront_porch", porch);
}

bool OutputConfigDialog::getFPDL3OutputWidth(FPDL3Width *width)
{
	return readSysfsInt(_device->name(), "fpdl3_output_width", (unsigned*)width);
}

bool OutputConfigDialog::setFPDL3OutputWidth(FPDL3Width width)
{
	return writeSysfsInt(_device->name(), "fpdl3_output_width", width);
}

#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)

bool InputConfigDialog::getModuleType(ModuleType *type)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	long val;

	if (!config || FAILED(config->GetModuleId(&val)))
		return false;
	*type = (ModuleType)((unsigned)val >> 4);

	return true;
}

bool InputConfigDialog::getModuleVersion(unsigned *version)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	long val;

	if (!config || FAILED(config->GetModuleId(&val)))
		return false;
	*version = ((unsigned)val & 0x0F);

	return true;
}

bool InputConfigDialog::getFwType(FWType *type)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	long val;

	if (!config || FAILED(config->GetFpgaFwId(&val)))
		return false;
	*type = (FWType)(val >> 24);

	return true;
}

bool InputConfigDialog::getFwVersion(unsigned *version)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	long val;

	if (!config || FAILED(config->GetFpgaFwId(&val)))
		return false;
	*version = (unsigned)(val & 0xFFFF);

	return true;
}

bool InputConfigDialog::getSerialNumber(QString *serialNumber)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	long val;
	char buf[16];

	if (!config || FAILED(config->GetCardSerial(&val)))
		return false;
	sprintf(buf, "%03u-%03u-%03u-%03u", (unsigned)val >> 24,
	  ((unsigned)val >> 16) & 0xFF, ((unsigned)val >> 8) & 0xFF,
	  (unsigned)val & 0xFF);
	*serialNumber = QString(buf);

	return true;
}

bool InputConfigDialog::getInputId(unsigned *id)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetChannel((long*)id)));
}

bool InputConfigDialog::getLinkStatus(LinkStatus *status)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetLinkLckStatus((long*)status)));
}

bool InputConfigDialog::getVideoWidth(unsigned *width)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	long resolution;

	if (!config || FAILED(config->GetDetectedResolution(&resolution)))
		return false;
	else {
		*width = resolution >> 16;
		return true;
	}
}

bool InputConfigDialog::getVideoHeight(unsigned *height)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	long resolution;

	if (!config || FAILED(config->GetDetectedResolution(&resolution)))
		return false;
	else {
		*height = resolution & 0xFFFF;
		return true;
	}
}

bool InputConfigDialog::getVSyncStatus(SyncType *status)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetVsStatus((long*)status)));
}

bool InputConfigDialog::getHSyncStatus(SyncType *status)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetHsStatus((long*)status)));
}

bool InputConfigDialog::getColorMapping(ColorMapping *mapping)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetColorMapping((long*)mapping)));
}

bool InputConfigDialog::setColorMapping(ColorMapping mapping)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetColorMapping(mapping)));
}

bool InputConfigDialog::getLaneWidth(LineWidth *lineWidth)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetOldiLink((long*)lineWidth)));
}

bool InputConfigDialog::setLaneWidth(LineWidth lineWidth)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetOldiLink(lineWidth)));
}

bool InputConfigDialog::getVSyncGapLength(unsigned *length)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetDeGap2VS((long*)length)));
}

bool InputConfigDialog::setVSyncGapLength(unsigned length)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetDeGap2VS(length)));
}

bool InputConfigDialog::getHSyncGapLength(unsigned *length)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetDeGap2HS((long*)length)));
}

bool InputConfigDialog::setHSyncGapLength(unsigned length)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetDeGap2HS(length)));
}

bool InputConfigDialog::getPclkFreq(unsigned *freq)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetPclkFrequency((long*)freq)));
}

bool InputConfigDialog::getHSyncWidth(unsigned *width)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetWidthHSYNC((long*)width)));
}

bool InputConfigDialog::getVSyncWidth(unsigned *width)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetWidthVSYNC((long*)width)));
}

bool InputConfigDialog::getHBackPorch(unsigned *porch)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetHBackPorch((long*)porch)));
}

bool InputConfigDialog::getHFrontPorch(unsigned *porch)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetHFrontPorch((long*)porch)));
}

bool InputConfigDialog::getVBackPorch(unsigned *porch)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetVBackPorch((long*)porch)));
}

bool InputConfigDialog::getVFrontPorch(unsigned *porch)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetVFrontPorch((long*)porch)));
}

bool InputConfigDialog::getFreqRange(FreqRange *range)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetInputFrequencyRange((long*)range)));
}

bool InputConfigDialog::setFreqRange(FreqRange range)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetInputFrequencyRange(range)));
}

bool InputConfigDialog::getFPDL3InputWidth(FPDL3Width *width)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetFpdl3InputWidth((long*)width)));
}

bool InputConfigDialog::setFPDL3InputWidth(FPDL3Width width)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetFpdl3InputWidth(width)));
}

bool InputConfigDialog::getGMSLMode(GMSLMode *mode)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetGmslRate((long*)mode)));
}

bool InputConfigDialog::setGMSLMode(GMSLMode mode)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetGmslRate(mode)));
}

bool InputConfigDialog::getGMSLStreamId(unsigned *streamId)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetStreamId((long*)streamId)));
}

bool InputConfigDialog::setGMSLStreamId(unsigned streamId)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetStreamId(streamId)));
}

bool InputConfigDialog::getGMSLFEC(GMSLFEC *fec)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetGmslFEC((long*)fec)));
}

bool InputConfigDialog::setGMSLFEC(GMSLFEC fec)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetGmslFEC(fec)));
}

bool InputConfigDialog::getDefaultColor(unsigned *color)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetDefaultColor((long*)color)));
}

bool InputConfigDialog::setDefaultColor(unsigned color)
{
	IFG4InputConfig *config = (IFG4InputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetDefaultColor(color)));
}


bool OutputConfigDialog::getModuleType(ModuleType *type)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	long val;

	if (!config || FAILED(config->GetModuleId(&val)))
		return false;
	*type = (ModuleType)((unsigned)val >> 4);

	return true;
}

bool OutputConfigDialog::getModuleVersion(unsigned *version)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	long val;

	if (!config || FAILED(config->GetModuleId(&val)))
		return false;
	*version = ((unsigned)val & 0x0F);

	return true;
}

bool OutputConfigDialog::getFwType(FWType *type)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	long val;

	if (!config || FAILED(config->GetFpgaFwId(&val)))
		return false;
	*type = (FWType)(val >> 24);

	return true;
}

bool OutputConfigDialog::getFwVersion(unsigned *version)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	long val;

	if (!config || FAILED(config->GetFpgaFwId(&val)))
		return false;
	*version = (unsigned)(val & 0xFFFF);

	return true;
}

bool OutputConfigDialog::getSerialNumber(QString *serialNumber)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	long val;
	char buf[16];

	if (!config || FAILED(config->GetCardSerial(&val)))
		return false;
	sprintf(buf, "%03u-%03u-%03u-%03u", (unsigned)val >> 24,
	  ((unsigned)val >> 16) & 0xFF, ((unsigned)val >> 8) & 0xFF,
	  (unsigned)val & 0xFF);
	*serialNumber = QString(buf);

	return true;
}

bool OutputConfigDialog::getOutputId(unsigned *id)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetChannel((long*)id)));
}

bool OutputConfigDialog::getDisplayWidth(unsigned *width)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	long val;

	if (!config || FAILED(config->GetResolution(&val)))
		return false;
	*width = val >> 16;

	return true;
}

bool OutputConfigDialog::setDisplayWidth(unsigned int width)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	unsigned height;

	if (!getDisplayHeight(&height))
		return false;

	return (config && SUCCEEDED(config->SetResolution(width << 16 | height)));
}

bool OutputConfigDialog::getDisplayHeight(unsigned *height)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	long val;

	if (!config || FAILED(config->GetResolution(&val)))
		return false;
	*height = val & 0xFFFF;

	return true;
}

bool OutputConfigDialog::setDisplayHeight(unsigned int height)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	unsigned width;

	if (!getDisplayWidth(&width))
		return false;

	return (config && SUCCEEDED(config->SetResolution(width << 16 | height)));
}

bool OutputConfigDialog::getFrameRate(unsigned *frameRate)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetFramerate((long*)frameRate)));
}

bool OutputConfigDialog::setFrameRate(unsigned frameRate)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetFramerate(frameRate)));
}

bool OutputConfigDialog::getVideoSource(unsigned *source)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetSignalSource((long*)source)));
}

bool OutputConfigDialog::setVideoSource(unsigned source)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetSignalSource(source)));
}

bool OutputConfigDialog::getPclkFreq(unsigned *freq)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetPclkFreq((long*)freq)));
}

bool OutputConfigDialog::setPclkFreq(unsigned freq)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetPclkFreq(freq)));
}

bool OutputConfigDialog::getHsyncPolarity(SyncType *polarity)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetPolarityHSYNC((long*)polarity)));
}

bool OutputConfigDialog::setHsyncPolarity(SyncType polarity)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetPolarityHSYNC(polarity)));
}

bool OutputConfigDialog::getVsyncPolarity(SyncType *polarity)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetPolarityVSYNC((long*)polarity)));
}

bool OutputConfigDialog::setVsyncPolarity(SyncType polarity)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetPolarityVSYNC(polarity)));
}

bool OutputConfigDialog::getDePolarity(SyncType *polarity)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetPolarityDE((long*)polarity)));
}

bool OutputConfigDialog::setDePolarity(SyncType polarity)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetPolarityDE(polarity)));
}

bool OutputConfigDialog::getHSyncWidth(unsigned *width)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetWidthHSYNC((long*)width)));
}

bool OutputConfigDialog::setHSyncWidth(unsigned width)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetWidthHSYNC(width)));
}

bool OutputConfigDialog::getVSyncWidth(unsigned *width)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetWidthVSYNC((long*)width)));
}

bool OutputConfigDialog::setVSyncWidth(unsigned width)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetWidthVSYNC(width)));
}

bool OutputConfigDialog::getHBackPorch(unsigned *porch)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetHBackPorch((long*)porch)));
}

bool OutputConfigDialog::setHBackPorch(unsigned porch)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetHBackPorch(porch)));
}

bool OutputConfigDialog::getHFrontPorch(unsigned *porch)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetHFrontPorch((long*)porch)));
}

bool OutputConfigDialog::setHFrontPorch(unsigned porch)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetHFrontPorch(porch)));
}

bool OutputConfigDialog::getVBackPorch(unsigned *porch)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetVBackPorch((long*)porch)));
}

bool OutputConfigDialog::setVBackPorch(unsigned porch)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetVBackPorch(porch)));
}

bool OutputConfigDialog::getVFrontPorch(unsigned *porch)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetVFrontPorch((long*)porch)));
}

bool OutputConfigDialog::setVFrontPorch(unsigned porch)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetVFrontPorch(porch)));
}

bool OutputConfigDialog::getFPDL3OutputWidth(FPDL3Width *width)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->GetFpdl3OutputWidth((long int*)width)));
}

bool OutputConfigDialog::setFPDL3OutputWidth(FPDL3Width width)
{
	IFG4OutputConfig *config = (IFG4OutputConfig*)_device->config();
	return (config && SUCCEEDED(config->SetFpdl3OutputWidth(width)));
}

#else
#error "unsupported platform"
#endif


DeviceConfigDialog::DeviceConfigDialog(Device *device, QWidget *parent)
  : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
  _device(device)
{
	setModal(true);
	setWindowTitle(tr("%1 Configuration").arg(device->name()));
}


InputConfigDialog::InputConfigDialog(Device *device, QWidget *parent)
  : DeviceConfigDialog(device, parent), _fpdl3InputWidth(0), _gmslMode(0),
  _gmslStreamId(0), _gmslFec(0)
{
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
	  | QDialogButtonBox::Cancel);

	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	ModuleType moduleType = ModuleType::None;
	QLabel *moduleTypeLabel = new QLabel();
	if (!getModuleType(&moduleType))
		moduleTypeLabel->setText(tr("N/A"));
	else if (moduleType == ModuleType::GMSLv1)
		moduleTypeLabel->setText("GMSL v1");
	else if (moduleType == ModuleType::GMSLv2)
		moduleTypeLabel->setText("GMSL v2");
	else if (moduleType == ModuleType::GMSLv3)
		moduleTypeLabel->setText("GMSL v3");
	else if (moduleType == ModuleType::FPDL3)
		moduleTypeLabel->setText("FPDL3");

	unsigned moduleVersion;
	QLabel *moduleVersionLabel = new QLabel();
	if (getModuleVersion(&moduleVersion))
		moduleVersionLabel->setText(QString::number(moduleVersion));
	else
		moduleVersionLabel->setText(tr("N/A"));

	FWType fwType = FWType::None;
	QLabel *fwTypeLabel = new QLabel();
	if (!getFwType(&fwType))
		fwTypeLabel->setText(tr("N/A"));
	else if (fwType == FWType::GMSL)
		fwTypeLabel->setText("GMSL");
	else if (fwType == FWType::FPDL3)
		fwTypeLabel->setText("FPDL3");

	unsigned fwVersion;
	QLabel *fwVersionLabel = new QLabel();
	if (getFwVersion(&fwVersion))
		fwVersionLabel->setText(QString::number(fwVersion));
	else
		fwVersionLabel->setText(tr("N/A"));

	QString serialNumber;
	QLabel *serialNumberLabel = new QLabel();
	if (getSerialNumber(&serialNumber))
		serialNumberLabel->setText(serialNumber);
	else
		serialNumberLabel->setText(tr("N/A"));

	QGroupBox *deviceStatus = new QGroupBox(tr("PCI Card"));
	QFormLayout *deviceStatusLayout = new QFormLayout();
	deviceStatusLayout->addRow(tr("Module Type:"), moduleTypeLabel);
	deviceStatusLayout->addRow(tr("Module Version:"), moduleVersionLabel);
	deviceStatusLayout->addRow(tr("Firmware Type:"), fwTypeLabel);
	deviceStatusLayout->addRow(tr("Firmware Version:"), fwVersionLabel);
	deviceStatusLayout->addRow(tr("Serial Number:"), serialNumberLabel);
	deviceStatus->setLayout(deviceStatusLayout);


	unsigned inputId;
	QLabel *inputIdLabel = new QLabel();
	if (!getInputId(&inputId))
		inputIdLabel->setText("N/A");
	else
		inputIdLabel->setText(QString::number(inputId));

	LinkStatus linkStatus;
	QLabel *linkStatusLabel = new QLabel();
	if (!getLinkStatus(&linkStatus))
		linkStatusLabel->setText(tr("N/A"));
	else if (linkStatus == Locked)
		linkStatusLabel->setText(tr("Locked"));
	else if (linkStatus == Unlocked)
		linkStatusLabel->setText(tr("Unlocked"));

	unsigned videoWidth, videoHeight;
	QLabel *videoWidthLabel = new QLabel();
	QLabel *videoHeightLabel = new QLabel();
	if (!getVideoWidth(&videoWidth))
		videoWidthLabel->setText(tr("N/A"));
	else
		videoWidthLabel->setText(QString::number(videoWidth));
	if (!getVideoHeight(&videoHeight))
		videoHeightLabel->setText(tr("N/A"));
	else
		videoHeightLabel->setText(QString::number(videoHeight));

	SyncType vsyncStatus = NotAvailable;
	QLabel *vsyncStatusLabel = new QLabel();
	if (!getVSyncStatus(&vsyncStatus)|| vsyncStatus == NotAvailable)
		vsyncStatusLabel->setText(tr("N/A"));
	else if (vsyncStatus == ActiveLow)
		vsyncStatusLabel->setText(tr("Active Low"));
	else if (vsyncStatus == ActiveHigh)
		vsyncStatusLabel->setText(tr("Active High"));

	SyncType hsyncStatus = NotAvailable;
	QLabel *hsyncStatusLabel = new QLabel();
	if (!getHSyncStatus(&hsyncStatus) || hsyncStatus == NotAvailable)
		hsyncStatusLabel->setText(tr("N/A"));
	else if (hsyncStatus == ActiveLow)
		hsyncStatusLabel->setText(tr("Active Low"));
	else if (hsyncStatus == ActiveHigh)
		hsyncStatusLabel->setText(tr("Active High"));

	unsigned freq;
	QLabel *pclkFreqLabel = new QLabel();
	if (!getPclkFreq(&freq))
		pclkFreqLabel->setText(tr("N/A"));
	else
		pclkFreqLabel->setText(QString::number(freq));

	unsigned width;
	QLabel *hsyncWidthLabel = new QLabel();
	if (!getHSyncWidth(&width))
		hsyncWidthLabel->setText(tr("N/A"));
	else
		hsyncWidthLabel->setText(QString::number(width));
	QLabel *vsyncWidthLabel = new QLabel();
	if (!getVSyncWidth(&width))
		vsyncWidthLabel->setText(tr("N/A"));
	else
		vsyncWidthLabel->setText(QString::number(width));

	unsigned porch;
	QLabel *hbackPorchLabel = new QLabel();
	if (!getHBackPorch(&porch))
		hbackPorchLabel->setText(tr("N/A"));
	else
		hbackPorchLabel->setText(QString::number(porch));
	QLabel *hfrontPorchLabel = new QLabel();
	if (!getHFrontPorch(&porch))
		hfrontPorchLabel->setText(tr("N/A"));
	else
		hfrontPorchLabel->setText(QString::number(porch));
	QLabel *vbackPorchLabel = new QLabel();
	if (!getVBackPorch(&porch))
		vbackPorchLabel->setText(tr("N/A"));
	else
		vbackPorchLabel->setText(QString::number(porch));
	QLabel *vfrontPorchLabel = new QLabel();
	if (!getVFrontPorch(&porch))
		vfrontPorchLabel->setText(tr("N/A"));
	else
		vfrontPorchLabel->setText(QString::number(porch));

	QGroupBox *inputStatus = new QGroupBox(tr("Video Input"));
	QHBoxLayout *inputStatusLayout = new QHBoxLayout();
	QFormLayout *coreInputStatusLayout = new QFormLayout();
	coreInputStatusLayout->addRow(tr("Input ID:"), inputIdLabel);
	coreInputStatusLayout->addRow(tr("Link Status:"), linkStatusLabel);
	coreInputStatusLayout->addRow(tr("Video Width:"), videoWidthLabel);
	coreInputStatusLayout->addRow(tr("Video Height:"), videoHeightLabel);
	coreInputStatusLayout->addRow(tr("VSync Status:"), vsyncStatusLabel);
	coreInputStatusLayout->addRow(tr("HSync Status:"), hsyncStatusLabel);
	coreInputStatusLayout->addRow(tr("PCLK Frequency:"), pclkFreqLabel);
	QFormLayout *advancedInputStatusLayout = new QFormLayout();
	advancedInputStatusLayout->addRow(tr("HSync Width:"), hsyncWidthLabel);
	advancedInputStatusLayout->addRow(tr("VSync Width:"), vsyncWidthLabel);
	advancedInputStatusLayout->addRow(tr("HBack Porch:"), hbackPorchLabel);
	advancedInputStatusLayout->addRow(tr("HFront Porch:"), hfrontPorchLabel);
	advancedInputStatusLayout->addRow(tr("VBack Porch:"), vbackPorchLabel);
	advancedInputStatusLayout->addRow(tr("VFront Porch:"), vfrontPorchLabel);

	inputStatusLayout->addLayout(coreInputStatusLayout);
	inputStatusLayout->addSpacing(20);
	inputStatusLayout->addLayout(advancedInputStatusLayout);
	inputStatus->setLayout(inputStatusLayout);

	QVBoxLayout *statusLayout = new QVBoxLayout();
	statusLayout->addWidget(deviceStatus);
	statusLayout->addWidget(inputStatus);

	_pixelFormat = new QComboBox();
	_pixelFormat->addItem(tr("Default"), QVariant(UnknownFormat));
	_pixelFormat->addItem(tr("RGB"), QVariant(RGB));
	_pixelFormat->addItem(tr("YUV"), QVariant(YUV));
	_pixelFormat->setCurrentIndex(_pixelFormat->findData((int)_device->format()));

	_colorMapping = new QComboBox();
	_colorMapping->addItem(tr("OLDI/JEIDA"), QVariant(OLDI));
	_colorMapping->addItem(tr("SPWG/VESA"), QVariant(SPWG));
	ColorMapping colorMapping;
	if (getColorMapping(&colorMapping))
		_colorMapping->setCurrentIndex(_colorMapping->findData((int)colorMapping));

	_oldiLineWidth = new QComboBox();
	_oldiLineWidth->addItem(tr("Single"), QVariant(Single));
	_oldiLineWidth->addItem(tr("Dual"), QVariant(Dual));
	LineWidth lineWidth;
	if (getLaneWidth(&lineWidth))
		_oldiLineWidth->setCurrentIndex(_oldiLineWidth->findData((int)lineWidth));

	unsigned len;
	_vsyncGapLength = new QSpinBox();
	_vsyncGapLength->setMaximum(0xFFFF);
	if (getVSyncGapLength(&len))
		_vsyncGapLength->setValue(len);
	if (vsyncStatus != NotAvailable)
		_vsyncGapLength->setEnabled(false);
	_hsyncGapLength = new QSpinBox();
	_hsyncGapLength->setMaximum(0xFFFF);
	if (getHSyncGapLength(&len))
		_hsyncGapLength->setValue(len);
	if (hsyncStatus != NotAvailable)
		_hsyncGapLength->setEnabled(false);

	_freqRange = new QComboBox();
	_freqRange->addItem(tr("Under 50MHz"), QVariant(Under50MHz));
	_freqRange->addItem(tr("Over 50MHz"), QVariant(Over50MHz));
	FreqRange range;
	if (getFreqRange(&range))
		_freqRange->setCurrentIndex(_freqRange->findData((int)range));

#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	unsigned color;
	_defaultColor = new QLineEdit();
	_defaultColor->setInputMask("\\#hhhhhhhH");
	_defaultColor->setToolTip(
	  tr("If the MSB is non-zero, you get a periodically changing color "
	     "instead of a static RGB color as represented by the lower three bytes."));
	if (getDefaultColor(&color))
		_defaultColor->setText(QString::number(color, 16));
#endif

	QGroupBox *commonConfig = new QGroupBox(tr("Common"));
	QFormLayout *commonConfigLayout = new QFormLayout();
	commonConfigLayout->addRow(tr("Pixel Format:"), _pixelFormat);
	commonConfigLayout->addRow(tr("Color Mapping:"), _colorMapping);
	commonConfigLayout->addRow(tr("OLDI Line Width:"), _oldiLineWidth);
	commonConfigLayout->addRow(tr("VSYNC Gap Length:"), _vsyncGapLength);
	commonConfigLayout->addRow(tr("HSYNC Gap Length:"), _hsyncGapLength);
	commonConfigLayout->addRow(tr("Frequency Range:"), _freqRange);
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	commonConfigLayout->addRow(tr("Default Color:"), _defaultColor);
#endif
	commonConfig->setLayout(commonConfigLayout);


	QVBoxLayout *configLayout = new QVBoxLayout();
	configLayout->addWidget(commonConfig);

	if (fwType == FWType::FPDL3) {
		_fpdl3InputWidth = new QComboBox();
		_fpdl3InputWidth->addItem(tr("Automatic"), QVariant(FPDL3Auto));
		_fpdl3InputWidth->addItem(tr("Single"), QVariant(FPDL3Single));
		_fpdl3InputWidth->addItem(tr("Dual"), QVariant(FPDL3Dual));
		FPDL3Width inputWidth;
		if (getFPDL3InputWidth(&inputWidth))
			_fpdl3InputWidth->setCurrentIndex(_fpdl3InputWidth->findData(
			  (int)inputWidth));

		QGroupBox *fpdl3Config = new QGroupBox(tr("FPDL3"));
		QFormLayout *fpdl3ConfigLayout = new QFormLayout();
		fpdl3ConfigLayout->addRow(tr("FPDL3 Input Width:"), _fpdl3InputWidth);
		fpdl3Config->setLayout(fpdl3ConfigLayout);

		configLayout->addWidget(fpdl3Config);
	} else if (fwType == FWType::GMSL) {
		_gmslMode = new QComboBox();
		_gmslMode->addItem(tr("12Gb/s"), QVariant(GMSL12Gb));
		_gmslMode->addItem(tr("6Gb/s"), QVariant(GMSL6Gb));
		_gmslMode->addItem(tr("3Gb/s"), QVariant(GMSL3Gb));
		_gmslMode->addItem(tr("1.5Gb/s"), QVariant(GMSL1GB));
		GMSLMode mode;
		if (getGMSLMode(&mode))
			_gmslMode->setCurrentIndex(_gmslMode->findData((int)mode));

		_gmslStreamId = new QComboBox();
		_gmslStreamId->addItem("0", QVariant(0));
		_gmslStreamId->addItem("1", QVariant(1));
		_gmslStreamId->addItem("2", QVariant(2));
		_gmslStreamId->addItem("3", QVariant(3));
		unsigned streamId;
		if (getGMSLStreamId(&streamId))
			_gmslStreamId->setCurrentIndex(_gmslStreamId->findData(streamId));

		_gmslFec = new QComboBox();
		_gmslFec->addItem(tr("Disabled"), QVariant(0));
		_gmslFec->addItem(tr("Enabled"), QVariant(1));
		GMSLFEC fec;
		if (getGMSLFEC(&fec))
			_gmslFec->setCurrentIndex(_gmslFec->findData(fec));

		QGroupBox *gmslConfig = new QGroupBox(tr("GMSL"));
		QFormLayout *gmslConfigLayout = new QFormLayout();
		gmslConfigLayout->addRow(tr("GMSL Mode:"), _gmslMode);
		gmslConfigLayout->addRow(tr("GMSL Stream Id:"), _gmslStreamId);
		gmslConfigLayout->addRow(tr("GMSL FEC:"), _gmslFec);
		gmslConfig->setLayout(gmslConfigLayout);

		configLayout->addWidget(gmslConfig);
	}


	QWidget *statusPage = new QWidget();
	statusPage->setLayout(statusLayout);
	QWidget *configPage = new QWidget();
	configPage->setLayout(configLayout);

	QTabWidget *tabWidget = new QTabWidget();
	tabWidget->addTab(statusPage, tr("Status"));
	tabWidget->addTab(configPage, tr("Configuration"));

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(tabWidget);
	layout->addWidget(buttonBox);
	show();
}

void InputConfigDialog::accept()
{
	bool ret = true;

	_device->setFormat((PixelFormat)_pixelFormat->currentData().toUInt());

	ret &= setColorMapping((ColorMapping)_colorMapping->currentData().toUInt());
	ret &= setLaneWidth((LineWidth)_oldiLineWidth->currentData().toUInt());
	if (_vsyncGapLength->isEnabled())
		ret &= setVSyncGapLength(_vsyncGapLength->value());
	if (_hsyncGapLength->isEnabled())
		ret &= setHSyncGapLength(_hsyncGapLength->value());
	ret &= setFreqRange((FreqRange)_freqRange->currentData().toUInt());
	if (_fpdl3InputWidth)
		ret &= setFPDL3InputWidth(
		  (FPDL3Width)_fpdl3InputWidth->currentData().toUInt());
	if (_gmslMode)
		ret &= setGMSLMode((GMSLMode)_gmslMode->currentData().toUInt());
	if (_gmslStreamId)
		ret &= setGMSLStreamId(_gmslStreamId->currentData().toUInt());
	if (_gmslFec)
		ret &= setGMSLFEC((GMSLFEC)_gmslFec->currentData().toUInt());
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	uint color = _defaultColor->text().remove(0, 1).toUInt(0, 16);
	ret &= setDefaultColor(color);
#endif

	if (!ret)
		QMessageBox::critical(this, tr("Error"),
		  tr("Error changing device configuration"));

	QDialog::accept();
}


OutputConfigDialog::OutputConfigDialog(Device *device, QWidget *parent)
  : DeviceConfigDialog(device, parent), _fpdl3OutputWidth(0)
{
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
	  | QDialogButtonBox::Cancel);

	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	ModuleType moduleType = ModuleType::None;
	QLabel *moduleTypeLabel = new QLabel();
	if (!getModuleType(&moduleType))
		moduleTypeLabel->setText(tr("N/A"));
	else if (moduleType == ModuleType::GMSLv1)
		moduleTypeLabel->setText("GMSL v1");
	else if (moduleType == ModuleType::GMSLv2)
		moduleTypeLabel->setText("GMSL v2");
	else if (moduleType == ModuleType::GMSLv3)
		moduleTypeLabel->setText("GMSL v3");
	else if (moduleType == ModuleType::FPDL3)
		moduleTypeLabel->setText("FPDL3");

	unsigned moduleVersion;
	QLabel *moduleVersionLabel = new QLabel();
	if (getModuleVersion(&moduleVersion))
		moduleVersionLabel->setText(QString::number(moduleVersion));
	else
		moduleVersionLabel->setText(tr("N/A"));

	FWType fwType = FWType::None;
	QLabel *fwTypeLabel = new QLabel();
	if (!getFwType(&fwType))
		fwTypeLabel->setText(tr("N/A"));
	else if (fwType == FWType::GMSL)
		fwTypeLabel->setText("GMSL");
	else if (fwType == FWType::FPDL3)
		fwTypeLabel->setText("FPDL3");

	unsigned fwVersion;
	QLabel *fwVersionLabel = new QLabel();
	if (getFwVersion(&fwVersion))
		fwVersionLabel->setText(QString::number(fwVersion));
	else
		fwVersionLabel->setText(tr("N/A"));

	QString serialNumber;
	QLabel *serialNumberLabel = new QLabel();
	if (getSerialNumber(&serialNumber))
		serialNumberLabel->setText(serialNumber);
	else
		serialNumberLabel->setText(tr("N/A"));

	QGroupBox *deviceStatus = new QGroupBox(tr("PCI Card"));
	QFormLayout *deviceStatusLayout = new QFormLayout();
	deviceStatusLayout->addRow(tr("Module Type:"), moduleTypeLabel);
	deviceStatusLayout->addRow(tr("Module Version:"), moduleVersionLabel);
	deviceStatusLayout->addRow(tr("Firmware Type:"), fwTypeLabel);
	deviceStatusLayout->addRow(tr("Firmware Version:"), fwVersionLabel);
	deviceStatusLayout->addRow(tr("Serial Number:"), serialNumberLabel);
	deviceStatus->setLayout(deviceStatusLayout);


	unsigned outputId;
	QLabel *outputIdLabel = new QLabel();
	if (!getOutputId(&outputId))
		outputIdLabel->setText("N/A");
	else
		outputIdLabel->setText(QString::number(outputId));

	QGroupBox *outputStatus = new QGroupBox(tr("Video Output"));
	QFormLayout *outputStatusLayout = new QFormLayout();
	outputStatusLayout->addRow(tr("Output ID:"), outputIdLabel);
	outputStatus->setLayout(outputStatusLayout);

	QVBoxLayout *statusLayout = new QVBoxLayout();
	statusLayout->addWidget(deviceStatus);
	statusLayout->addWidget(outputStatus);

	unsigned val;
	_displayWidth = new QSpinBox();
	_displayWidth->setMaximum(8192);
	if (getDisplayWidth(&val))
		_displayWidth->setValue(val);
	_displayHeight = new QSpinBox();
	_displayHeight->setMaximum(8192);
	if (getDisplayHeight(&val))
		_displayHeight->setValue(val);
	_frameRate = new QSpinBox();
	_frameRate->setMaximum(200);
	if (getFrameRate(&val))
		_frameRate->setValue(val);

	_videoSource = new QComboBox();
	_videoSource->addItem(tr("Input 0"), QVariant(0));
	_videoSource->addItem(tr("Input 1"), QVariant(1));
	_videoSource->addItem(tr("PC Output 0"), QVariant(2));
	_videoSource->addItem(tr("PC Output 1"), QVariant(3));
	if (getVideoSource(&val))
		_videoSource->setCurrentIndex(_videoSource->findData((int)val));

	_pclkFreq = new QSpinBox();
	_pclkFreq->setMinimum(25000);
	_pclkFreq->setMaximum(190000);
	if (getPclkFreq(&val))
		_pclkFreq->setValue(val);
	SyncType polarity;
	_hsyncPolarity = new QComboBox();
	_hsyncPolarity->addItem(tr("Active low"), QVariant(ActiveLow));
	_hsyncPolarity->addItem(tr("Active high"), QVariant(ActiveHigh));
	if (getHsyncPolarity(&polarity))
		_hsyncPolarity->setCurrentIndex(_hsyncPolarity->findData((int)polarity));
	_vsyncPolarity = new QComboBox();
	_vsyncPolarity->addItem(tr("Active low"), QVariant(ActiveLow));
	_vsyncPolarity->addItem(tr("Active high"), QVariant(ActiveHigh));
	if (getVsyncPolarity(&polarity))
		_vsyncPolarity->setCurrentIndex(_vsyncPolarity->findData((int)polarity));
	_dePolarity = new QComboBox() ;
	_dePolarity->addItem(tr("Active low"), QVariant(ActiveLow));
	_dePolarity->addItem(tr("Active high"), QVariant(ActiveHigh));
	if (getDePolarity(&polarity))
		_dePolarity->setCurrentIndex(_dePolarity->findData((int)polarity));
	_hsyncWidth = new QSpinBox();
	_hsyncWidth->setMaximum(8192);
	if (getHSyncWidth(&val))
		_hsyncWidth->setValue(val);
	_vsyncWidth = new QSpinBox();
	_vsyncWidth->setMaximum(8192);
	if (getVSyncWidth(&val))
		_vsyncWidth->setValue(val);
	_hbackPorch = new QSpinBox();
	_hbackPorch->setMaximum(255);
	if (getHBackPorch(&val))
		_hbackPorch->setValue(val);
	_hfrontPorch = new QSpinBox();
	_hfrontPorch->setMaximum(255);
	if (getHFrontPorch(&val))
		_hfrontPorch->setValue(val);
	_vbackPorch = new QSpinBox();
	_vbackPorch->setMaximum(255);
	if (getVBackPorch(&val))
		_vbackPorch->setValue(val);
	_vfrontPorch = new QSpinBox();
	_vfrontPorch->setMaximum(255);
	if (getVFrontPorch(&val))
		_vfrontPorch->setValue(val);

	QGroupBox *commonConfig = new QGroupBox(tr("Common"));
	QHBoxLayout *commonConfigLayout = new QHBoxLayout();
	QFormLayout *commonConfigLayout1 = new QFormLayout();
	commonConfigLayout1->addRow(tr("Display Width:"), _displayWidth);
	commonConfigLayout1->addRow(tr("Display Height:"), _displayHeight);
	commonConfigLayout1->addRow(tr("Frame Rate:"), _frameRate);
	commonConfigLayout1->addRow(tr("Video Source:"), _videoSource);
	commonConfigLayout1->addRow(tr("PCLK Frequency:"), _pclkFreq);
	QFormLayout *commonConfigLayout2 = new QFormLayout();
	commonConfigLayout2->addRow(tr("HSYNC Polarity:"), _hsyncPolarity);
	commonConfigLayout2->addRow(tr("VSYNC Polarity:"), _vsyncPolarity);
	commonConfigLayout2->addRow(tr("DE Polarity:"), _dePolarity);
	commonConfigLayout2->addRow(tr("HSYNC Width:"), _hsyncWidth);
	commonConfigLayout2->addRow(tr("VSYNC Width:"), _vsyncWidth);
	commonConfigLayout2->addRow(tr("HBack Porch:"), _hbackPorch);
	commonConfigLayout2->addRow(tr("HFront Porch:"), _hfrontPorch);
	commonConfigLayout2->addRow(tr("VBack Porch:"), _vbackPorch);
	commonConfigLayout2->addRow(tr("VFront Porch:"), _vfrontPorch);

	commonConfigLayout->addLayout(commonConfigLayout1);
	commonConfigLayout->addSpacing(20);
	commonConfigLayout->addLayout(commonConfigLayout2);
	commonConfig->setLayout(commonConfigLayout);

	QVBoxLayout *configLayout = new QVBoxLayout();
	configLayout->addWidget(commonConfig);

	if (fwType == FWType::FPDL3) {
		_fpdl3OutputWidth = new QComboBox();
		_fpdl3OutputWidth->addItem(tr("Automatic"), QVariant(FPDL3Auto));
		_fpdl3OutputWidth->addItem(tr("Single"), QVariant(FPDL3Single));
		_fpdl3OutputWidth->addItem(tr("Dual"), QVariant(FPDL3Dual));
		FPDL3Width outputWidth;
		if (getFPDL3OutputWidth(&outputWidth))
			_fpdl3OutputWidth->setCurrentIndex(_fpdl3OutputWidth->findData(
			  (int)outputWidth));

		QGroupBox *fpdl3Config = new QGroupBox(tr("FPDL3"));
		QFormLayout *fpdl3ConfigLayout = new QFormLayout();
		fpdl3ConfigLayout->addRow(tr("FPDL3 Output Width:"), _fpdl3OutputWidth);
		fpdl3Config->setLayout(fpdl3ConfigLayout);

		configLayout->addWidget(fpdl3Config);
	}

	QPushButton *copyBtn = new QPushButton(tr("Copy From Input..."));
	connect(copyBtn, &QPushButton::clicked, this,
	  &OutputConfigDialog::copyConfig);
	QHBoxLayout *copyConfigLayout = new QHBoxLayout();
	copyConfigLayout->addStretch();
	copyConfigLayout->addWidget(copyBtn);

	configLayout->addLayout(copyConfigLayout);


	QWidget *statusPage = new QWidget();
	statusPage->setLayout(statusLayout);
	QWidget *configPage = new QWidget();
	configPage->setLayout(configLayout);

	QTabWidget *tabWidget = new QTabWidget();
	tabWidget->addTab(statusPage, tr("Status"));
	tabWidget->addTab(configPage, tr("Configuration"));

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(tabWidget);
	layout->addWidget(buttonBox);
	show();
}

void OutputConfigDialog::accept()
{
	bool ret = true;

	ret &= setDisplayWidth(_displayWidth->value());
	ret &= setDisplayHeight(_displayHeight->value());
	ret &= setFrameRate(_frameRate->value());
	ret &= setVideoSource(_videoSource->currentData().toUInt());
	ret &= setPclkFreq(_pclkFreq->value());
	ret &= setHsyncPolarity((SyncType)_hsyncPolarity->currentData().toUInt());
	ret &= setVsyncPolarity((SyncType)_vsyncPolarity->currentData().toUInt());
	ret &= setDePolarity((SyncType)_dePolarity->currentData().toUInt());
	ret &= setHSyncWidth(_hsyncWidth->value());
	ret &= setVSyncWidth(_vsyncWidth->value());
	ret &= setHBackPorch(_hbackPorch->value());
	ret &= setHFrontPorch(_hfrontPorch->value());
	ret &= setVBackPorch(_vbackPorch->value());
	ret &= setVFrontPorch(_vfrontPorch->value());

	if (_fpdl3OutputWidth)
		ret &= setFPDL3OutputWidth((FPDL3Width)_fpdl3OutputWidth->currentData()
		  .toUInt());

	if (!ret)
		QMessageBox::critical(this, tr("Error"),
		  tr("Error changing device configuration"));

	QDialog::accept();
}

void OutputConfigDialog::copyConfig()
{
	ConfigCopyDialog dialog(this, this);
	dialog.exec();
}

void OutputConfigDialog::setConfig(Device *dev)
{
	unsigned width, height, pclkFreq, hsync, vsync, hback, hfront, vback,
	  vfront, hpol, vpol;
	bool ok = true;

#if defined(Q_OS_LINUX)
	ok &= readSysfsInt(dev->name(), "video_width", &width);
	ok &= readSysfsInt(dev->name(), "video_height", &height);
	ok &= readSysfsInt(dev->name(), "pclk_frequency", &pclkFreq);
	ok &= readSysfsInt(dev->name(), "hsync_width", &hsync);
	ok &= readSysfsInt(dev->name(), "vsync_width", &vsync);
	ok &= readSysfsInt(dev->name(), "hback_porch", &hback);
	ok &= readSysfsInt(dev->name(), "hfront_porch", &hfront);
	ok &= readSysfsInt(dev->name(), "vback_porch", &vback);
	ok &= readSysfsInt(dev->name(), "vfront_porch", &vfront);
	ok &= readSysfsInt(dev->name(), "hsync_status", &hpol);
	ok &= readSysfsInt(dev->name(), "vsync_status", &vpol);
#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	IFG4InputConfig *src = (IFG4InputConfig*)dev->config();
	if (src) {
		long resolution;

		ok &= SUCCEEDED(src->GetDetectedResolution(&resolution));
		ok &= SUCCEEDED(src->GetPclkFrequency((long*)&pclkFreq));
		ok &= SUCCEEDED(src->GetWidthHSYNC((long*)&hsync));
		ok &= SUCCEEDED(src->GetWidthVSYNC((long*)&vsync));
		ok &= SUCCEEDED(src->GetHBackPorch((long*)&hback));
		ok &= SUCCEEDED(src->GetHFrontPorch((long*)&hfront));
		ok &= SUCCEEDED(src->GetVBackPorch((long*)&vback));
		ok &= SUCCEEDED(src->GetVFrontPorch((long*)&vfront));
		ok &= SUCCEEDED(src->GetHsStatus((long*)&hpol));
		ok &= SUCCEEDED(src->GetVsStatus((long*)&vpol));

		width = resolution >> 16;
		height = resolution & 0xFFFF;
	} else
		ok = false;
#else
#error "unsupported platform"
#endif

	if (!ok) {
		QMessageBox::critical(this, tr("Error"),
		  tr("Error reading input device configuration"));
	} else {
		unsigned frameSize = (width + hfront + hsync + hback)
		  * (height + vfront + vsync + vback);

		_displayWidth->setValue(width);
		_displayHeight->setValue(height);
		_pclkFreq->setValue(pclkFreq);
		_hsyncWidth->setValue(hsync);
		_vsyncWidth->setValue(vsync);
		_hbackPorch->setValue(hback);
		_hfrontPorch->setValue(hfront);
		_vbackPorch->setValue(vback);
		_vfrontPorch->setValue(vfront);
		if (hpol != NotAvailable)
			_hsyncPolarity->setCurrentIndex((SyncType)hpol);
		if (vpol != NotAvailable)
			_vsyncPolarity->setCurrentIndex((SyncType)vpol);
		_frameRate->setValue(qFloor((pclkFreq * 1000) / (double)frameSize));
	}
}
