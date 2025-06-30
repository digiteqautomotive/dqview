#include <QPushButton>
#include <QToolButton>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QApplication>
#include <QFontMetrics>
#include "dirselectwidget.h"


DirSelectWidget::DirSelectWidget(QWidget *parent) : QWidget(parent)
{
	QFontMetrics fm(QApplication::font());
	_edit = new QLineEdit();
	_edit->setMinimumWidth(fm.boundingRect(QDir::homePath()).width());
#ifdef Q_OS_WIN32
	_button = new QPushButton("...");
	_button->setMaximumWidth(_button->sizeHint().width() / 2);
	connect(_button, &QPushButton::clicked, this, &DirSelectWidget::browse);
#else // Q_OS_WIN32
	_button = new QToolButton();
	_button->setText("...");
	connect(_button, &QToolButton::clicked, this, &DirSelectWidget::browse);
#endif // Q_OS_WIN32

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setContentsMargins(QMargins());
	layout->addWidget(_edit);
	layout->addWidget(_button);
	setLayout(layout);

	QSizePolicy p(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	setSizePolicy(p);
}

void DirSelectWidget::browse()
{
	QFileInfo fi(_edit->text());
	QString fileName = QFileDialog::getExistingDirectory(this,
	  tr("Select Directory"), fi.dir().absolutePath());

	if (!fileName.isEmpty())
		_edit->setText(fileName);
}
