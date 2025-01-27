#ifndef DEVICECONFIGDIALOG_H
#define DEVICECONFIGDIALOG_H

#include <QDialog>
#include "device.h"
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
#include "fg4.h"
#endif

class QComboBox;
class QSpinBox;
class QLineEdit;

class DeviceConfigDialog : public QDialog
{
public:
	DeviceConfigDialog(Device *device, QWidget *parent = 0);

protected:
	enum class FWType {None, FPDL3, GMSL};
	enum class ModuleType {None, FPDL3, GMSLv1, GMSLv2, GMSLv3};
	enum SyncType {ActiveLow, ActiveHigh, NotAvailable};
	enum FPDL3Width {FPDL3Auto, FPDL3Single, FPDL3Dual};

#if defined(Q_OS_LINUX)
	bool getModuleType(ModuleType *type);
	bool getModuleVersion(unsigned *version);
	bool getFwType(FWType *type);
	bool getFwVersion(unsigned *version);
	bool getSerialNumber(QString *serialNumber);
#endif

	Device *_device;
};

class InputConfigDialog : public DeviceConfigDialog
{
public:
	InputConfigDialog(Device *device, QWidget *parent = 0);

public slots:
	void accept();

private:
	enum LinkStatus {Unlocked, Locked};
	enum ColorMapping {OLDI, SPWG};
	enum LineWidth {Single, Dual};
	enum FreqRange {Under50MHz, Over50MHz};
	enum GMSLMode {GMSL12Gb, GMSL6Gb, GMSL3Gb, GMSL1GB};
	enum GMSLFEC {GMSLFECDisabled, GMSLFECEnabled};

#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	bool getModuleType(ModuleType *type);
	bool getModuleVersion(unsigned *version);
	bool getFwType(ModuleType *type);
	bool getFwVersion(unsigned *version);
	bool getSerialNumber(QString *serialNumber);
	bool getDefaultColor(unsigned *color);
	bool setDefaultColor(unsigned color);
#endif

	bool getInputId(unsigned *id);
	bool getLinkStatus(LinkStatus *status);
	bool getVideoWidth(unsigned *width);
	bool getVideoHeight(unsigned *height);
	bool getVSyncStatus(SyncType *status);
	bool getHSyncStatus(SyncType *status);
	bool getColorMapping(ColorMapping *mapping);
	bool getLaneWidth(LineWidth *lineWidth);
	bool getVSyncGapLength(unsigned *length);
	bool getHSyncGapLength(unsigned *length);
	bool getPclkFreq(unsigned *freq);
	bool getHSyncWidth(unsigned *width);
	bool getVSyncWidth(unsigned *width);
	bool getHBackPorch(unsigned *porch);
	bool getHFrontPorch(unsigned *porch);
	bool getVBackPorch(unsigned *porch);
	bool getVFrontPorch(unsigned *porch);
	bool getFreqRange(FreqRange *range);
	bool getFPDL3InputWidth(FPDL3Width *width);
	bool getGMSLMode(GMSLMode *mode);
	bool getGMSLStreamId(unsigned *streamId);
	bool getGMSLFEC(GMSLFEC *fec);

	bool setColorMapping(ColorMapping mapping);
	bool setLaneWidth(LineWidth lineWidth);
	bool setVSyncGapLength(unsigned length);
	bool setHSyncGapLength(unsigned length);
	bool setFreqRange(FreqRange range);
	bool setFPDL3InputWidth(FPDL3Width width);
	bool setGMSLMode(GMSLMode mode);
	bool setGMSLStreamId(unsigned streamId);
	bool setGMSLFEC(GMSLFEC fec);

	QComboBox *_pixelFormat;
	QComboBox *_colorMapping;
	QComboBox *_oldiLineWidth;
	QSpinBox *_vsyncGapLength;
	QSpinBox *_hsyncGapLength;
	QComboBox *_freqRange;
	QComboBox *_fpdl3InputWidth;
	QComboBox *_gmslMode;
	QComboBox *_gmslStreamId;
	QComboBox *_gmslFec;
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	QLineEdit *_defaultColor;
#endif
};

class OutputConfigDialog : public DeviceConfigDialog
{
public:
	OutputConfigDialog(Device *device, QWidget *parent = 0);

	void setConfig(Device *dev);

public slots:
	void accept();

private slots:
	void copyConfig();

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
	bool getPclkFreq(unsigned *freq);
	bool getHsyncPolarity(SyncType *polarity);
	bool getVsyncPolarity(SyncType *polarity);
	bool getDePolarity(SyncType *polarity);
	bool getHSyncWidth(unsigned *width);
	bool getVSyncWidth(unsigned *width);
	bool getHBackPorch(unsigned *porch);
	bool getHFrontPorch(unsigned *porch);
	bool getVBackPorch(unsigned *porch);
	bool getVFrontPorch(unsigned *porch);
	bool getFPDL3OutputWidth(FPDL3Width *width);

	bool setDisplayWidth(unsigned width);
	bool setDisplayHeight(unsigned height);
	bool setFrameRate(unsigned frameRate);
	bool setVideoSource(unsigned source);
	bool setPclkFreq(unsigned freq);
	bool setHsyncPolarity(SyncType polarity);
	bool setVsyncPolarity(SyncType polarity);
	bool setDePolarity(SyncType polarity);
	bool setHSyncWidth(unsigned width);
	bool setVSyncWidth(unsigned width);
	bool setHBackPorch(unsigned porch);
	bool setHFrontPorch(unsigned porch);
	bool setVBackPorch(unsigned porch);
	bool setVFrontPorch(unsigned porch);
	bool setFPDL3OutputWidth(FPDL3Width width);

	QSpinBox *_displayWidth;
	QSpinBox *_displayHeight;
	QSpinBox *_frameRate;
	QComboBox *_videoSource;
	QSpinBox *_pclkFreq;
	QComboBox *_hsyncPolarity;
	QComboBox *_vsyncPolarity;
	QComboBox *_dePolarity;
	QSpinBox *_hsyncWidth;
	QSpinBox *_vsyncWidth;
	QSpinBox *_hbackPorch;
	QSpinBox *_hfrontPorch;
	QSpinBox *_vbackPorch;
	QSpinBox *_vfrontPorch;
	QComboBox *_fpdl3OutputWidth;
};

#endif // DEVICECONFIGDIALOG_H
