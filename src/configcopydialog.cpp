#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include "deviceinfo.h"
#include "deviceconfigdialog.h"
#include "configcopydialog.h"

ConfigCopyDialog::ConfigCopyDialog(OutputConfigDialog *config, QWidget *parent)
  : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
	_config(config)
{
	setModal(true);
	setWindowTitle(tr("Select Source"));

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
	  | QDialogButtonBox::Cancel);

	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	QList<DeviceInfo> list(DeviceInfo::inputDevices());
	_copySrc = new QComboBox();
	for (int i = 0; i < list.size(); i++) {
		if (list.at(i).device().isValid())
			_copySrc->addItem(list.at(i).name(), QVariant::fromValue(
			  list.at(i).device()));
	}

	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("Input Device:"), _copySrc);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addLayout(formLayout);
	layout->addWidget(buttonBox);
	show();
}

void ConfigCopyDialog::accept()
{
	Device dev(_copySrc->currentData().value<Device>());
	_config->setConfig(dev);

	QDialog::accept();
}
