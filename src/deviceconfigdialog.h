#ifndef DEVICECONFIGDIALOG_H
#define DEVICECONFIGDIALOG_H

#include <QDialog>
#include "device.h"
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
#include "fg4.h"
#endif

class QComboBox;
class QSpinBox;

class DeviceConfigDialog : public QDialog
{
public:
	DeviceConfigDialog(const Device &device, QWidget *parent = 0);

protected:
	enum ModuleType {None, FPDL3, GMSL};

#if defined(Q_OS_LINUX)
	bool getModuleType(ModuleType *type);
	bool getModuleVersion(unsigned *version);
	bool getFwType(ModuleType *type);
	bool getFwVersion(unsigned *version);
	bool getSerialNumber(QString *serialNumber);

	bool readSysfsInt(const QString &path, unsigned *val);
	bool readSysfsString(const QString &path, QString *val);
	bool writeSysfsInt(const QString &path, unsigned val);

	QString _device;
#endif
};

class InputConfigDialog : public DeviceConfigDialog
{
public:
	InputConfigDialog(const Device &device, QWidget *parent = 0);
	~InputConfigDialog();

public slots:
	void accept();

private:
	enum LinkStatus {Unlocked, Locked};
	enum SyncStatus {ActiveLow, ActiveHigh, NotAvailable};
	enum ColorMapping {OLDI, SPWG};
	enum LineWidth {Single, Dual};
	enum FPDL3InputWidth {FPDL3Auto, FPDL3Single, FPDL3Dual};
	enum GMSLMode {GMSL12Gb, GMSL6Gb, GMSL3Gb, GMSL1GB};
	enum GMSLFEC {GMSLFECDisabled, GMSLFECEnabled};

#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	bool getModuleType(ModuleType *type);
	bool getModuleVersion(unsigned *version);
	bool getFwType(ModuleType *type);
	bool getFwVersion(unsigned *version);
	bool getSerialNumber(QString *serialNumber);
#endif

	bool getInputId(unsigned *id);
	bool getLinkStatus(LinkStatus *status);
	bool getVSyncStatus(SyncStatus *status);
	bool getHSyncStatus(SyncStatus *status);
	bool getColorMapping(ColorMapping *mapping);
	bool getLaneWidth(LineWidth *lineWidth);
	bool getVSyncGapLength(unsigned *length);
	bool getHSyncGapLength(unsigned *length);
	bool getFPDL3InputWidth(FPDL3InputWidth *width);
	bool getGMSLMode(GMSLMode *mode);
	bool getGMSLStreamId(unsigned *streamId);
	bool getGMSLFEC(GMSLFEC *fec);

	bool setColorMapping(ColorMapping mapping);
	bool setLaneWidth(LineWidth lineWidth);
	bool setVSyncGapLength(unsigned length);
	bool setHSyncGapLength(unsigned length);
	bool setFPDL3InputWidth(FPDL3InputWidth width);
	bool setGMSLMode(GMSLMode mode);
	bool setGMSLStreamId(unsigned streamId);
	bool setGMSLFEC(GMSLFEC fec);

	QComboBox *_colorMapping;
	QComboBox *_oldiLineWidth;
	QSpinBox *_vsyncGapLength;
	QSpinBox *_hsyncGapLength;
	QComboBox *_fpdl3InputWidth;
	QComboBox *_gmslMode;
	QComboBox *_gmslStreamId;
	QComboBox *_gmslFec;

#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	IFG4InputConfig *_config;
#endif
};

class OutputConfigDialog : public DeviceConfigDialog
{
public:
	OutputConfigDialog(const Device &device, QWidget *parent = 0);
	~OutputConfigDialog();

public slots:
	void accept();

private:
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	bool getModuleType(ModuleType *type);
	bool getModuleVersion(unsigned *version);
	bool getFwType(ModuleType *type);
	bool getFwVersion(unsigned *version);
	bool getSerialNumber(QString *serialNumber);
#endif

	bool getOutputId(unsigned *id);
	bool getDisplayWidth(unsigned *width);
	bool getDisplayHeight(unsigned *height);
	bool getFrameRate(unsigned *frameRate);
	bool getVideoSource(unsigned *source);

	bool setDisplayWidth(unsigned width);
	bool setDisplayHeight(unsigned height);
	bool setFrameRate(unsigned frameRate);
	bool setVideoSource(unsigned source);

	QSpinBox *_displayWidth;
	QSpinBox *_displayHeight;
	QSpinBox *_frameRate;
	QComboBox *_videoSource;

#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	IFG4OutputConfig *_config;
#endif
};

#endif // DEVICECONFIGDIALOG_H
