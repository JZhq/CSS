#include "qthistoricalrecordlineedit.h"
#include <QStandardPaths>
#include <QPushButton>
#include <QCompleter>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QTextStream>

static const char* kHistoryFileName = "lineedit_history.txt";
static const int kLimitHistoriesSize = 100;

QHistoricalRecordLineEdit::QHistoricalRecordLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
	readKeywordsFromLocal();
    // 设置文本边际
    setTextMargins(0,0,this->height(),0);
    // QLineEdit模糊匹配
	QCompleter * completer = new QCompleter(this);
	m_keywordsModel.setStringList(m_keywords);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setModel(&m_keywordsModel);
    this->setCompleter(completer);
    connect(this, SIGNAL(editingFinished()), this, SLOT(onEditComplete()));
}

void QHistoricalRecordLineEdit::setNormal()
{
    setText("");
    setTextMargins(-5,0,this->height(),0);    // Solve the cursor problem
}

void QHistoricalRecordLineEdit::setSearching()
{
    setTextMargins(0,0,this->height(),0);
}

void QHistoricalRecordLineEdit::writeKeywordsToLocal()
{
	QFile file(localKeywordsFilePath());
    if (!file.open(QIODevice::WriteOnly | QFile::Text))
	{
		qWarning() << QStringLiteral("不能写入历史数据文件:%1, %2!").arg(localKeywordsFilePath()).arg(file.errorString());
		return;
	}

	QTextStream textStream(&file);
	for (int i = 0; i < m_keywords.size(); i++)
	{
		textStream << m_keywords[i].trimmed().toUtf8() << endl;
	}

    file.flush();
    file.close();
}

void QHistoricalRecordLineEdit::readKeywordsFromLocal()
{
	QFile file(localKeywordsFilePath());
	if (!file.open(QIODevice::ReadOnly | QFile::Text))
	{
		qWarning() << QStringLiteral("不能读取历史数据文件:%1, %2!").arg(localKeywordsFilePath()).arg(file.errorString());
		return;
	}

	m_keywords.clear();
	while (!file.atEnd())
	{
		m_keywords << QString::fromLocal8Bit(file.readLine()).trimmed();
	}

    file.close();
}

void QHistoricalRecordLineEdit::addKeyword(const QString &kw)
{
	if (kw.isEmpty()) return;
	if (m_keywords.contains(kw.trimmed())) return;
	m_keywords << kw.trimmed();
	m_keywordsModel.setStringList(m_keywords);
    if (m_keywords.size() > kLimitHistoriesSize) m_keywords.removeAt(0);
}

void QHistoricalRecordLineEdit::addFuncName(const QList<QString> &func)
{
    m_allFunctionNameList = func;
}

QString QHistoricalRecordLineEdit::localKeywordsFilePath()
{
	const QString root_dir_string = QStandardPaths::standardLocations(QStandardPaths::DataLocation)[0] + "/app";
	QDir root_dir(root_dir_string);
	if (!root_dir.exists())
	{
		root_dir.mkpath(root_dir_string);
	}

	return root_dir_string + "/" + kHistoryFileName;
}

void QHistoricalRecordLineEdit::onEditComplete()
{
	addKeyword(this->text());
	writeKeywordsToLocal();
}



