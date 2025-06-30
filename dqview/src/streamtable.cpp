#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QToolButton>
#include <QHBoxLayout>
#include <QMessageBox>
#include "streamdialog.h"
#include "streamtable.h"


StreamTable::StreamTable(QWidget *parent) : QWidget(parent)
{
	_table = new QTableWidget(0, 3);
	QHeaderView *hh = _table->horizontalHeader();
	hh->setStretchLastSection(true);
	QStringList labels = {tr("Address"), tr("Port"), tr("Format")};
	_table->setHorizontalHeaderLabels(labels);
	_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	_table->setMinimumWidth(400);
	connect(_table, &QTableWidget::itemSelectionChanged, this,
	  &StreamTable::enableRemove);

#ifdef Q_OS_WIN32
	_addButton = new QPushButton("+");
	_addButton->setMaximumWidth(_addButton->sizeHint().width() / 2);
	connect(_addButton, &QPushButton::clicked, this, &StreamTable::add);
	_removeButton = new QPushButton("-");
	_removeButton->setMaximumWidth(_removeButton->sizeHint().width() / 2);
	connect(_removeButton, &QPushButton::clicked, this, &StreamTable::remove);
#else // Q_OS_WIN32
	_addButton = new QToolButton();
	_addButton->setText("+");
	_addButton->setMinimumWidth(_addButton->sizeHint().height());
	connect(_addButton, &QToolButton::clicked, this, &StreamTable::add);
	_removeButton = new QToolButton();
	_removeButton->setText("-");
	_removeButton->setMinimumWidth(_removeButton->sizeHint().height());
	connect(_removeButton, &QToolButton::clicked, this, &StreamTable::remove);
#endif // Q_OS_WIN32

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->setSpacing(0);
	buttonLayout->addStretch();
	buttonLayout->addWidget(_addButton);
	buttonLayout->addWidget(_removeButton);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setSpacing(0);
	layout->addWidget(_table);
	layout->addLayout(buttonLayout);
}

void StreamTable::load(const QList<StreamInfo> &streams)
{
	for (int i = 0; i < streams.size(); i++) {
		const StreamInfo &stream = streams.at(i);
		_table->insertRow(i);
		_table->setItem(i, 0, new QTableWidgetItem(stream.address()));
		_table->setItem(i, 1, new QTableWidgetItem(QString::number(stream.port())));
		_table->setItem(i, 2, new QTableWidgetItem(stream.type()));

		for (int j = 0; j < 3; j++)
			_table->item(i, j)->setFlags(_table->item(i, j)->flags()
			  ^ Qt::ItemIsEditable);
	}
}

void StreamTable::store(QList<StreamInfo> &streams) const
{
	for (int i = 0; i < _table->rowCount(); i++) {
		StreamInfo stream(_table->item(i, 0)->text(),
		  _table->item(i, 1)->text().toInt(), _table->item(i, 2)->text());
		streams.append(stream);
	}
}

void StreamTable::enableRemove()
{
	_removeButton->setEnabled(!_table->selectedItems().isEmpty());
}

bool StreamTable::findStream(const StreamInfo stream) const
{
	for (int i = 0; i < _table->rowCount(); i++) {
		StreamInfo ts(_table->item(i, 0)->text(),
		  _table->item(i, 1)->text().toInt(), _table->item(i, 2)->text());
		if (ts == stream)
			return true;
	}

	return false;
}

void StreamTable::add()
{
	StreamInfo stream = StreamDialog::getStream(this, tr("Add Remote Device"));
	if (stream.isNull())
		return;

	if (findStream(stream))
		QMessageBox::critical(this, tr("Error"), tr("Duplicit device entry"));
	else {
		int i = _table->rowCount();
		_table->insertRow(i);
		_table->setItem(i, 0, new QTableWidgetItem(stream.address()));
		_table->setItem(i, 1, new QTableWidgetItem(QString::number(stream.port())));
		_table->setItem(i, 2, new QTableWidgetItem(stream.type()));

		for (int j = 0; j < 3; j++)
			_table->item(i, j)->setFlags(_table->item(i, j)->flags()
			  ^ Qt::ItemIsEditable);
	}
}

void StreamTable::remove()
{
	QSet<int> rows;
	QList<QTableWidgetItem *> items = _table->selectedItems();
	for (int i = 0; i < items.count(); i++)
		rows.insert(items.at(i)->row());

	for (QSet<int>::iterator i = rows.begin(); i != rows.end(); ++i)
		_table->removeRow(*i);
}
