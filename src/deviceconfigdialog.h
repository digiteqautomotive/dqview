#ifndef DEVICECONFIGDIALOG_H
#define DEVICECONFIGDIALOG_H

#include <QDialog>

class QComboBox;
class QSpinBox;

class DeviceConfigDialog : public QDialog
{
public:
	DeviceConfigDialog(const QString &device, QWidget *parent = 0);

public slots:
	void accept();

private:
	enum ModuleType {None, FPDL3, GMSL};
	enum LinkStatus {Unlocked, Locked};
	enum SyncStatus {ActiveLow, ActiveHigh, NotAvailable};
	enum ColorMapping {OLDI, SPWG};
	enum LineWidth {Single, Dual};
	enum FPDL3InputWidth {FPDL3Auto, FPDL3Single, FPDL3Dual};
	enum GMSLMode {GMSL12Gb, GMSL6Gb, GMSL3Gb, GMSL1GB};
	enum GMSLFEC {GMSLFECDisabled, GMSLFECEnabled};

#if defined(Q_OS_LINUX)
	bool readSysfsInt(const QString &path, unsigned *val);
	bool readSysfsString(const QString &path, QString *val);
	bool writeSysfsInt(const QString &path, unsigned val);
#endif

	bool getModuleType(ModuleType *type);
	bool getModuleVersion(unsigned *version);
	bool getFwType(ModuleType *type);
	bool getFwVersion(unsigned *version);
	bool getSerialNumber(QString *serialNumber);
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

	QString _device;

	QComboBox *_colorMapping;
	QComboBox *_oldiLineWidth;
	QSpinBox *_vsyncGapLength;
	QSpinBox *_hsyncGapLength;
	QComboBox *_fpdl3InputWidth;
	QComboBox *_gmslMode;
	QComboBox *_gmslStreamId;
	QComboBox *_gmslFec;
};

#endif // DEVICECONFIGDIALOG_H
