#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include "stream.h"
#include "streaminfo.h"
#include "streamdialog.h"


StreamDialog::StreamDialog(QWidget *parent, const QString &caption)
  : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
	setModal(true);
	setWindowTitle(caption);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
	  | QDialogButtonBox::Cancel);

	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	_address = new QLineEdit();
	_address->setText("192.168.1.200");
	_port = new QSpinBox();
	_port->setMaximum(65535);
	_port->setValue(50000);
	_format = new QComboBox();
	_format->addItems(Stream::types());

	QWidget *page = new QWidget(this);
	QFormLayout *pagelayout = new QFormLayout(page);
	pagelayout->addRow(tr("Address:"), _address);
	pagelayout->addRow(tr("Port:"), _port);
	pagelayout->addRow(tr("Format:"), _format);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(page);
	layout->addWidget(buttonBox);
	show();
}

StreamInfo StreamDialog::stream()
{
	return StreamInfo(_address->text(), _port->value(), _format->currentText());
}

StreamInfo StreamDialog::getStream(QWidget *parent, const QString &caption)
{
	StreamDialog dialog(parent, caption);

	if (dialog.exec() == QDialog::Accepted)
		return dialog.stream();

	return StreamInfo();
}
