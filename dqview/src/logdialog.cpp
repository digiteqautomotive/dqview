#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include "logdialog.h"


class Highlighter : public QSyntaxHighlighter
{
public:
	Highlighter(QTextDocument *parent = 0);

protected:
	void highlightBlock(const QString &text);

private:
	struct Rule
	{
		QRegularExpression pattern;
		QTextCharFormat format;
	};
	QVector<Rule> _rules;
};

Highlighter::Highlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
	Rule rule;
	QTextCharFormat format;

	rule.pattern = QRegularExpression("^\\[.+\\] Error:.*");
	format.setForeground(Qt::red);
	rule.format = format;
	_rules.append(rule);

	rule.pattern = QRegularExpression("^\\[.+\\] Warning:.*");
	format.setForeground(QColor("orange"));
	rule.format = format;
	_rules.append(rule);
}

void Highlighter::highlightBlock(const QString &text)
{
	for (int i = 0; i < _rules.size(); i++) {
		const Rule &rule = _rules.at(i);
		QRegularExpressionMatchIterator matchIterator
		  = rule.pattern.globalMatch(text);

		while (matchIterator.hasNext()) {
			QRegularExpressionMatch match = matchIterator.next();
			setFormat(match.capturedStart(), match.capturedLength(),
			  rule.format);
		}
	}
}


static QString logLevel(int level)
{
	switch (level) {
		case 0:
			return "Debug";
		case 2:
			return "Notice";
		case 3:
			return "Warning";
		case 4:
			return "Error";
		default:
			return QString();
	}
}

static QString format(Log &log)
{
	QString text;

	log.mutex.lock();
	for (int i = 0; i < log.list.size(); i++) {
		const LogEntry &e = log.list.at(i);
		text.append(e.time().toString("[h:m:s] ") + logLevel(e.level()) + ": "
		  + e.message().trimmed() + "\n");
	}
	log.mutex.unlock();

	return text;
}

LogDialog::LogDialog(Log &log, QWidget *parent)
 : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
	setModal(true);
	setWindowTitle(tr("Log File"));
	setMinimumWidth(640);
	setMinimumHeight(480);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);

	QPlainTextEdit *textEdit = new QPlainTextEdit(format(log));
	textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
	textEdit->setReadOnly(true);
	new Highlighter(textEdit->document());

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(textEdit);
	layout->addWidget(buttonBox);
	show();
}
