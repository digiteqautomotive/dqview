#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QComboBox>
#include <QLabel>
#include <QFormLayout>
#include <QGroupBox>
#include <QSpinBox>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include "deviceconfigdialog.h"

#if defined(Q_OS_LINUX)

bool DeviceConfigDialog::readSysfsInt(const QString &path, unsigned *val)
{
	QFileInfo fi(_device);
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

bool DeviceConfigDialog::writeSysfsInt(const QString &path, unsigned val)
{
	QFileInfo fi(_device);
	QDir sysfsDir("/sys/class/video4linux/");
	QDir deviceDir(sysfsDir.filePath(fi.fileName()));
	QString sysfsPath = deviceDir.filePath(path);

	QFile f(sysfsPath);
	if (!f.open(QIODevice::WriteOnly)) {
		qWarning("%s: %s", qPrintable(sysfsPath), qPrintable(f.errorString()));
		return false;
	}

	QByteArray ba(QByteArray::number(val));
	return (f.write(ba) == ba.size());
}

bool DeviceConfigDialog::readSysfsString(const QString &path, QString *val)
{
	QFileInfo fi(_device);
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
	return readSysfsInt("device/module_type", (unsigned*)type);
}

bool DeviceConfigDialog::getModuleVersion(unsigned *version)
{
	return readSysfsInt("device/module_version", version);
}

bool DeviceConfigDialog::getFwType(ModuleType *type)
{
	return readSysfsInt("device/fw_type", (unsigned*)type);
}

bool DeviceConfigDialog::getFwVersion(unsigned *version)
{
	return readSysfsInt("device/fw_version", version);
}

bool DeviceConfigDialog::getSerialNumber(QString *serialNumber)
{
	return readSysfsString("device/serial_number", serialNumber);
}


bool InputConfigDialog::getInputId(unsigned *id)
{
	return readSysfsInt("input_id", id);
}

bool InputConfigDialog::getLinkStatus(LinkStatus *status)
{
	return readSysfsInt("link_status", (unsigned*)status);
}

bool InputConfigDialog::getVSyncStatus(SyncStatus *status)
{
	return readSysfsInt("vsync_status", (unsigned*)status);
}

bool InputConfigDialog::getHSyncStatus(SyncStatus *status)
{
	return readSysfsInt("hsync_status", (unsigned*)status);
}

bool InputConfigDialog::getColorMapping(ColorMapping *mapping)
{
	return readSysfsInt("color_mapping", (unsigned*)mapping);
}

bool InputConfigDialog::setColorMapping(ColorMapping mapping)
{
	return writeSysfsInt("color_mapping", mapping);
}

bool InputConfigDialog::getLaneWidth(LineWidth *lineWidth)
{
	return readSysfsInt("oldi_lane_width", (unsigned*)lineWidth);
}

bool InputConfigDialog::setLaneWidth(LineWidth lineWidth)
{
	return writeSysfsInt("oldi_lane_width", lineWidth);
}

bool InputConfigDialog::getVSyncGapLength(unsigned *length)
{
	return readSysfsInt("vsync_gap_length", length);
}

bool InputConfigDialog::setVSyncGapLength(unsigned length)
{
	return writeSysfsInt("vsync_gap_length", length);
}

bool InputConfigDialog::getHSyncGapLength(unsigned *length)
{
	return readSysfsInt("hsync_gap_length", length);
}

bool InputConfigDialog::setHSyncGapLength(unsigned length)
{
	return writeSysfsInt("hsync_gap_length", length);
}

bool InputConfigDialog::getFPDL3InputWidth(FPDL3InputWidth *width)
{
	return readSysfsInt("fpdl3_input_width", (unsigned*)width);
}

bool InputConfigDialog::setFPDL3InputWidth(FPDL3InputWidth width)
{
	return writeSysfsInt("fpdl3_input_width", width);
}

bool InputConfigDialog::getGMSLMode(GMSLMode *mode)
{
	return readSysfsInt("gmsl_mode", (unsigned*)mode);
}

bool InputConfigDialog::setGMSLMode(GMSLMode mode)
{
	return writeSysfsInt("gmsl_mode", mode);
}

bool InputConfigDialog::getGMSLStreamId(unsigned *streamId)
{
	return readSysfsInt("gmsl_stream_id", streamId);
}

bool InputConfigDialog::setGMSLStreamId(unsigned streamId)
{
	return writeSysfsInt("gmsl_stream_id", streamId);
}

bool InputConfigDialog::getGMSLFEC(GMSLFEC *fec)
{
	return readSysfsInt("gmsl_fec", (unsigned*)fec);
}

bool InputConfigDialog::setGMSLFEC(GMSLFEC fec)
{
	return writeSysfsInt("gmsl_fec", fec);
}


bool OutputConfigDialog::getOutputId(unsigned *id)
{
	return readSysfsInt("output_id", id);
}

bool OutputConfigDialog::getDisplayWidth(unsigned *width)
{
	return readSysfsInt("display_width", width);
}

bool OutputConfigDialog::setDisplayWidth(unsigned int width)
{
	return writeSysfsInt("display_width", width);
}

bool OutputConfigDialog::getDisplayHeight(unsigned *height)
{
	return readSysfsInt("display_height", height);
}

bool OutputConfigDialog::setDisplayHeight(unsigned int height)
{
	return writeSysfsInt("display_height", height);
}

bool OutputConfigDialog::getFrameRate(unsigned *frameRate)
{
	return readSysfsInt("frame_rate", frameRate);
}

bool OutputConfigDialog::setFrameRate(unsigned frameRate)
{
	return writeSysfsInt("frame_rate", frameRate);
}

bool OutputConfigDialog::getVideoSource(unsigned *source)
{
	return readSysfsInt("video_source", source);
}

bool OutputConfigDialog::setVideoSource(unsigned source)
{
	return writeSysfsInt("video_source", source);
}

#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)

#include <Windows.h>
#include <uuids.h>
#include <strmif.h>

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


bool InputConfigDialog::getModuleType(ModuleType *type)
{
	long val;

	if (!_config || FAILED(_config->GetModuleId(&val)))
		return false;
	*type = (ModuleType)((unsigned)val >> 4);

	return true;
}

bool InputConfigDialog::getModuleVersion(unsigned *version)
{
	long val;

	if (!_config || FAILED(_config->GetModuleId(&val)))
		return false;
	*version = ((unsigned)val & 0x0F);

	return true;
}

bool InputConfigDialog::getFwType(ModuleType *type)
{
	long val;

	if (!_config || FAILED(_config->GetFpgaFwId(&val)))
		return false;
	*type = (ModuleType)(val >> 24);

	return true;
}

bool InputConfigDialog::getFwVersion(unsigned *version)
{
	long val;

	if (!_config || FAILED(_config->GetFpgaFwId(&val)))
		return false;
	*version = (unsigned)(val & 0xFFFF);

	return true;
}

bool InputConfigDialog::getSerialNumber(QString *serialNumber)
{
	long val;
	char buf[16];

	if (!_config || FAILED(_config->GetCardSerial(&val)))
		return false;
	sprintf(buf, "%03u-%03u-%03u-%03u", (unsigned)val >> 24,
	  ((unsigned)val >> 16) & 0xFF, ((unsigned)val >> 8) & 0xFF,
	  (unsigned)val & 0xFF);
	*serialNumber = QString(buf);

	return true;
}

bool InputConfigDialog::getInputId(unsigned *id)
{
	return (_config && SUCCEEDED(_config->GetChannel((long int*)id)));
}

bool InputConfigDialog::getLinkStatus(LinkStatus *status)
{
	return (_config && SUCCEEDED(_config->GetLinkLckStatus((long int*)status)));
}

bool InputConfigDialog::getVSyncStatus(SyncStatus *status)
{
	return (_config && SUCCEEDED(_config->GetVsStatus((long int*)status)));
}

bool InputConfigDialog::getHSyncStatus(SyncStatus *status)
{
	return (_config && SUCCEEDED(_config->GetHsStatus((long int*)status)));
}

bool InputConfigDialog::getColorMapping(ColorMapping *mapping)
{
	return (_config && SUCCEEDED(_config->GetColorMapping((long int*)mapping)));
}

bool InputConfigDialog::setColorMapping(ColorMapping mapping)
{
	return (_config && SUCCEEDED(_config->SetColorMapping(mapping)));
}

bool InputConfigDialog::getLaneWidth(LineWidth *lineWidth)
{
	return (_config && SUCCEEDED(_config->GetOldiLink((long int*)lineWidth)));
}

bool InputConfigDialog::setLaneWidth(LineWidth lineWidth)
{
	return (_config && SUCCEEDED(_config->SetOldiLink(lineWidth)));
}

bool InputConfigDialog::getVSyncGapLength(unsigned *length)
{
	return (_config && SUCCEEDED(_config->GetDeGap2VS((long int*)length)));
}

bool InputConfigDialog::setVSyncGapLength(unsigned length)
{
	return (_config && SUCCEEDED(_config->SetDeGap2VS(length)));
}

bool InputConfigDialog::getHSyncGapLength(unsigned *length)
{
	return (_config && SUCCEEDED(_config->GetDeGap2HS((long int*)length)));
}

bool InputConfigDialog::setHSyncGapLength(unsigned length)
{
	return (_config && SUCCEEDED(_config->SetDeGap2HS(length)));
}

bool InputConfigDialog::getFPDL3InputWidth(FPDL3InputWidth *width)
{
	return (_config && SUCCEEDED(_config->GetFpdl3InputWidth((long int*)width)));
}

bool InputConfigDialog::setFPDL3InputWidth(FPDL3InputWidth width)
{
	return (_config && SUCCEEDED(_config->SetFpdl3InputWidth(width)));
}

bool InputConfigDialog::getGMSLMode(GMSLMode *mode)
{
	return (_config && SUCCEEDED(_config->GetGmslRate((long int*)mode)));
}

bool InputConfigDialog::setGMSLMode(GMSLMode mode)
{
	return (_config && SUCCEEDED(_config->SetGmslRate(mode)));
}

bool InputConfigDialog::getGMSLStreamId(unsigned *streamId)
{
	return (_config && SUCCEEDED(_config->GetStreamId((long int*)streamId)));
}

bool InputConfigDialog::setGMSLStreamId(unsigned streamId)
{
	return (_config && SUCCEEDED(_config->SetStreamId(streamId)));
}

bool InputConfigDialog::getGMSLFEC(GMSLFEC *fec)
{
	return (_config && SUCCEEDED(_config->GetGmslFEC((long int*)fec)));
}

bool InputConfigDialog::setGMSLFEC(GMSLFEC fec)
{
	return (_config && SUCCEEDED(_config->SetGmslFEC(fec)));
}


bool OutputConfigDialog::getModuleType(ModuleType *type)
{
	long val;

	if (!_config || FAILED(_config->GetModuleId(&val)))
		return false;
	*type = (ModuleType)((unsigned)val >> 4);

	return true;
}

bool OutputConfigDialog::getModuleVersion(unsigned *version)
{
	long val;

	if (!_config || FAILED(_config->GetModuleId(&val)))
		return false;
	*version = ((unsigned)val & 0x0F);

	return true;
}

bool OutputConfigDialog::getFwType(ModuleType *type)
{
	long val;

	if (!_config || FAILED(_config->GetFpgaFwId(&val)))
		return false;
	*type = (ModuleType)(val >> 24);

	return true;
}

bool OutputConfigDialog::getFwVersion(unsigned *version)
{
	long val;

	if (!_config || FAILED(_config->GetFpgaFwId(&val)))
		return false;
	*version = (unsigned)(val & 0xFFFF);

	return true;
}

bool OutputConfigDialog::getSerialNumber(QString *serialNumber)
{
	long val;
	char buf[16];

	if (!_config || FAILED(_config->GetCardSerial(&val)))
		return false;
	sprintf(buf, "%03u-%03u-%03u-%03u", (unsigned)val >> 24,
	  ((unsigned)val >> 16) & 0xFF, ((unsigned)val >> 8) & 0xFF,
	  (unsigned)val & 0xFF);
	*serialNumber = QString(buf);

	return true;
}

bool OutputConfigDialog::getOutputId(unsigned *id)
{
	return (_config && SUCCEEDED(_config->GetChannel((long int*)id)));
}

bool OutputConfigDialog::getDisplayWidth(unsigned *width)
{
	long val;

	if (!_config || FAILED(_config->GetResolution(&val)))
		return false;
	*width = val >> 16;

	return true;
}

bool OutputConfigDialog::setDisplayWidth(unsigned int width)
{
	unsigned height;
	if (!getDisplayHeight(&height))
		return false;

	return (SUCCEEDED(_config->SetResolution(width << 16 | height)));
}

bool OutputConfigDialog::getDisplayHeight(unsigned *height)
{
	long val;

	if (!_config || FAILED(_config->GetResolution(&val)))
		return false;
	*height = val & 0xFFFF;

	return true;
}

bool OutputConfigDialog::setDisplayHeight(unsigned int height)
{
	unsigned width;
	if (!getDisplayWidth(&width))
		return false;

	return (SUCCEEDED(_config->SetResolution(width << 16 | height)));
}

bool OutputConfigDialog::getFrameRate(unsigned *frameRate)
{
	return (_config && SUCCEEDED(_config->GetFramerate((long int*)frameRate)));
}

bool OutputConfigDialog::setFrameRate(unsigned frameRate)
{
	return (_config && SUCCEEDED(_config->SetFramerate(frameRate)));
}

bool OutputConfigDialog::getVideoSource(unsigned *source)
{
	return (_config && SUCCEEDED(_config->GetSignalSource((long int*)source)));
}

bool OutputConfigDialog::setVideoSource(unsigned source)
{
	return (_config && SUCCEEDED(_config->SetSignalSource(source)));
}

#else
#error "unsupported platform"
#endif


DeviceConfigDialog::DeviceConfigDialog(const Device &device, QWidget *parent)
  : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
#if defined(Q_OS_LINUX)
	_device = device.name();
#endif

	setModal(true);
	setWindowTitle(tr("%1 Configuration").arg(device.name()));
}


InputConfigDialog::InputConfigDialog(const Device &device, QWidget *parent)
  : DeviceConfigDialog(device, parent), _fpdl3InputWidth(0), _gmslMode(0),
  _gmslStreamId(0), _gmslFec(0)
{
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	_config = inputConfig(device.id());
#endif

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
	  | QDialogButtonBox::Cancel);

	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	ModuleType moduleType = None;
	QLabel *moduleTypeLabel = new QLabel();
	if (!getModuleType(&moduleType))
		moduleTypeLabel->setText(tr("N/A"));
	else if (moduleType == GMSL)
		moduleTypeLabel->setText("GMSL");
	else if (moduleType == FPDL3)
		moduleTypeLabel->setText("FPDL3");

	unsigned moduleVersion;
	QLabel *moduleVersionLabel = new QLabel();
	if (getModuleVersion(&moduleVersion))
		moduleVersionLabel->setText(QString::number(moduleVersion));
	else
		moduleVersionLabel->setText(tr("N/A"));

	ModuleType fwType = None;
	QLabel *fwTypeLabel = new QLabel();
	if (!getFwType(&fwType))
		fwTypeLabel->setText(tr("N/A"));
	else if (fwType == GMSL)
		fwTypeLabel->setText("GMSL");
	else if (fwType == FPDL3)
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

	SyncStatus vsyncStatus = NotAvailable;
	QLabel *vsyncStatusLabel = new QLabel();
	if (!getVSyncStatus(&vsyncStatus)|| vsyncStatus == NotAvailable)
		vsyncStatusLabel->setText(tr("N/A"));
	else if (vsyncStatus == ActiveLow)
		vsyncStatusLabel->setText(tr("Active Low"));
	else if (vsyncStatus == ActiveHigh)
		vsyncStatusLabel->setText(tr("Active High"));

	SyncStatus hsyncStatus = NotAvailable;
	QLabel *hsyncStatusLabel = new QLabel();
	if (!getHSyncStatus(&hsyncStatus) || hsyncStatus == NotAvailable)
		hsyncStatusLabel->setText(tr("N/A"));
	else if (hsyncStatus == ActiveLow)
		hsyncStatusLabel->setText(tr("Active Low"));
	else if (hsyncStatus == ActiveHigh)
		hsyncStatusLabel->setText(tr("Active High"));

	QGroupBox *inputStatus = new QGroupBox(tr("Video Input"));
	QFormLayout *inputStatusLayout = new QFormLayout();
	inputStatusLayout->addRow(tr("Input ID:"), inputIdLabel);
	inputStatusLayout->addRow(tr("Link Status:"), linkStatusLabel);
	inputStatusLayout->addRow(tr("VSync Status:"), vsyncStatusLabel);
	inputStatusLayout->addRow(tr("HSync Status:"), hsyncStatusLabel);
	inputStatus->setLayout(inputStatusLayout);

	QVBoxLayout *statusLayout = new QVBoxLayout();
	statusLayout->addWidget(deviceStatus);
	statusLayout->addWidget(inputStatus);

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

	QGroupBox *commonConfig = new QGroupBox(tr("Common"));
	QFormLayout *commonConfigLayout = new QFormLayout();
	commonConfigLayout->addRow(tr("Color Mapping:"), _colorMapping);
	commonConfigLayout->addRow(tr("OLDI Line Width:"), _oldiLineWidth);
	commonConfigLayout->addRow(tr("VSYNC Gap Length:"), _vsyncGapLength);
	commonConfigLayout->addRow(tr("HSYNC Gap Length:"), _hsyncGapLength);
	commonConfig->setLayout(commonConfigLayout);


	QVBoxLayout *configLayout = new QVBoxLayout();
	configLayout->addWidget(commonConfig);

	if (fwType == FPDL3) {
		_fpdl3InputWidth = new QComboBox();
		_fpdl3InputWidth->addItem(tr("Automatic"), QVariant(FPDL3Auto));
		_fpdl3InputWidth->addItem(tr("Single"), QVariant(FPDL3Single));
		_fpdl3InputWidth->addItem(tr("Dual"), QVariant(FPDL3Dual));
		FPDL3InputWidth inputWidth;
		if (getFPDL3InputWidth(&inputWidth))
			_fpdl3InputWidth->setCurrentIndex(_fpdl3InputWidth->findData(
			  (int)inputWidth));

		QGroupBox *fpdl3Config = new QGroupBox(tr("FPDL3"));
		QFormLayout *fpdl3ConfigLayout = new QFormLayout();
		fpdl3ConfigLayout->addRow(tr("FPDL3 Input Width:"), _fpdl3InputWidth);
		fpdl3Config->setLayout(fpdl3ConfigLayout);

		configLayout->addWidget(fpdl3Config);
	} else if (fwType == GMSL) {
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

InputConfigDialog::~InputConfigDialog()
{
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	if (_config)
		_config->Release();
#endif
}

void InputConfigDialog::accept()
{
	bool ret = true;

	ret &= setColorMapping((ColorMapping)_colorMapping->currentData().toUInt());
	ret &= setLaneWidth((LineWidth)_oldiLineWidth->currentData().toUInt());
	if (_vsyncGapLength->isEnabled())
		ret &= setVSyncGapLength(_vsyncGapLength->value());
	if (_hsyncGapLength->isEnabled())
		ret &= setHSyncGapLength(_hsyncGapLength->value());
	if (_fpdl3InputWidth)
		ret &= setFPDL3InputWidth(
		  (FPDL3InputWidth)_fpdl3InputWidth->currentData().toUInt());
	if (_gmslMode)
		ret &= setGMSLMode((GMSLMode)_gmslMode->currentData().toUInt());
	if (_gmslStreamId)
		ret &= setGMSLStreamId(_gmslStreamId->currentData().toUInt());
	if (_gmslFec)
		ret &= setGMSLFEC((GMSLFEC)_gmslFec->currentData().toUInt());

	if (!ret)
		QMessageBox::critical(this, tr("Error"),
		  tr("Error changing device configuration"));

	QDialog::accept();
}


OutputConfigDialog::OutputConfigDialog(const Device &device, QWidget *parent)
  : DeviceConfigDialog(device, parent)
{
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	_config = outputConfig(device.id());
#endif

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
	  | QDialogButtonBox::Cancel);

	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	ModuleType moduleType = None;
	QLabel *moduleTypeLabel = new QLabel();
	if (!getModuleType(&moduleType))
		moduleTypeLabel->setText(tr("N/A"));
	else if (moduleType == GMSL)
		moduleTypeLabel->setText("GMSL");
	else if (moduleType == FPDL3)
		moduleTypeLabel->setText("FPDL3");

	unsigned moduleVersion;
	QLabel *moduleVersionLabel = new QLabel();
	if (getModuleVersion(&moduleVersion))
		moduleVersionLabel->setText(QString::number(moduleVersion));
	else
		moduleVersionLabel->setText(tr("N/A"));

	ModuleType fwType = None;
	QLabel *fwTypeLabel = new QLabel();
	if (!getFwType(&fwType))
		fwTypeLabel->setText(tr("N/A"));
	else if (fwType == GMSL)
		fwTypeLabel->setText("GMSL");
	else if (fwType == FPDL3)
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

	QGroupBox *commonConfig = new QGroupBox(tr("Common"));
	QFormLayout *commonConfigLayout = new QFormLayout();
	commonConfigLayout->addRow(tr("Display Width:"), _displayWidth);
	commonConfigLayout->addRow(tr("Display Height:"), _displayHeight);
	commonConfigLayout->addRow(tr("Frame Rate:"), _frameRate);
	commonConfigLayout->addRow(tr("Video Source:"), _videoSource);
	commonConfig->setLayout(commonConfigLayout);

	QVBoxLayout *configLayout = new QVBoxLayout();
	configLayout->addWidget(commonConfig);

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

OutputConfigDialog::~OutputConfigDialog()
{
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	if (_config)
		_config->Release();
#endif
}

#include <QDebug>
void OutputConfigDialog::accept()
{
	bool ret = true;

	ret &= setDisplayWidth(_displayWidth->value());
	ret &= setDisplayHeight(_displayHeight->value());
	ret &= setFrameRate(_frameRate->value());
	ret &= setVideoSource(_videoSource->currentData().toUInt());

	if (!ret)
		QMessageBox::critical(this, tr("Error"),
		  tr("Error changing device configuration"));

	QDialog::accept();
}
