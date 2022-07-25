#include "qthistoricalrecordtextedit.h"
#include <QStandardPaths>
#include <QPushButton>
#include <QCompleter>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QTextStream>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QCompleter>
#include <QHBoxLayout>
static const char* kHistoryFileName = "textedit_history.txt";
static const int kLimitHistoriesSize = 100;

QHistoricalRecordTextEdit::QHistoricalRecordTextEdit(QWidget *parent)
    : QTextEdit(parent)
    , m_completer(0)
      //label(new QLabel)
{
    readKeywordsFromLocal();
    // QLineEdit模糊匹配
    m_completer = new QCompleter(this);
    m_keywordsModel.setStringList(m_keywords);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setModel(&m_keywordsModel);
    this->setCompleter(m_completer);
    //setMouseTracking(true);
    //label->setVisible(false);
    //label->setWindowFlags(Qt::ToolTip);
    //connect(this, SIGNAL(textChanged()), this, SLOT(onEditComplete()));
#if 0
    QPushButton *button = new QPushButton(this);
    QHBoxLayout *layout = new QHBoxLayout();
    button->setCursor(Qt::ArrowCursor);
    layout->addStretch();
    layout->addWidget(button);
    layout->setContentsMargins(0,0,0,0);
    this->setLayout(layout);
#endif
}

QHistoricalRecordTextEdit::~QHistoricalRecordTextEdit()
{
	onEditComplete();
}

void QHistoricalRecordTextEdit::setCompleter(QCompleter *completer)
{
	m_completer = completer;
	if (m_completer) {
		m_completer->setWidget(this);
		connect(m_completer, SIGNAL(activated(QString)),
			this, SLOT(onCompleterActivated(QString)));
    }
}

void QHistoricalRecordTextEdit::setAllFuncName(const QList<QString> &funcName)
{
    foreach (QString singleFuncName, funcName)
    {
          singleFuncName += QString("{}");
          m_allFunctionNameList.append(singleFuncName);
    }

    //设置高阶函数名自动查找 by zyg
    readKeywordsFromLocal();
    m_keywordsModel.setStringList(m_keywords);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setModel(&m_keywordsModel);
    this->setCompleter(m_completer);
}

void QHistoricalRecordTextEdit::keyPressEvent(QKeyEvent *e)
{
	if (m_completer) {
		if (m_completer->popup()->isVisible()) {
			switch (e->key()) {
			case Qt::Key_Escape:
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Tab:
				e->ignore();
				return;
			default:
				break;
			}
		}
		QTextEdit::keyPressEvent(e);
        QString completerPrefix = this->wordUnderCursor();
		m_completer->setCompletionPrefix(completerPrefix); // 通过设置QCompleter的前缀，来让Completer寻找关键词
		m_completer->complete();
    }
}
#if 0
void QHistoricalRecordTextEdit::enterEvent(QEvent *event)
{
    label->setVisible(true);
}

void QHistoricalRecordTextEdit::leaveEvent(QEvent *event)
{
    label->setVisible(false);
}

void QHistoricalRecordTextEdit::mouseMoveEvent(QMouseEvent *event)
{
    if(label && label->isVisible())
    {
        label->move(mapToGlobal(event->pos()+QPoint(5, 5)));
    }
}
#endif
void QHistoricalRecordTextEdit::onCompleterActivated(const QString &completion)
{
	QString completionPrefix = wordUnderCursor(),
		shouldInertText = completion;
	QTextCursor cursor = this->textCursor();
	if (!completion.contains(completionPrefix)) {// delete the previously typed.
		cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, completionPrefix.size());
		cursor.clearSelection();
	}
	else { // 补全相应的字符
		shouldInertText = shouldInertText.replace(
			shouldInertText.indexOf(completionPrefix), completionPrefix.size(), "");
	}
	cursor.insertText(shouldInertText);
}

void QHistoricalRecordTextEdit::onEditComplete()
{
	addKeyword(this->toPlainText());
	writeKeywordsToLocal();
}

QString QHistoricalRecordTextEdit::wordUnderCursor()
{
	QTextCursor cursor = this->textCursor();
	while (cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor)) {
		if (cursor.selectedText().contains(" ")) {
			break;
		}
	}
	return cursor.selectedText().remove(" ");
}

void QHistoricalRecordTextEdit::writeKeywordsToLocal()
{
	QFile file(localKeywordsFilePath());
    if (!file.open(QIODevice::WriteOnly | QFile::Text))
	{
		qWarning() << QStringLiteral("不能写入历史数据文件:%1, %2!").arg(localKeywordsFilePath()).arg(file.errorString());
		return;
	}

	QTextStream textStream(&file);
	const QStringList keywords = m_keywords.toSet().toList();
	for (int i = 0; i < keywords.size(); i++)
	{
		textStream << keywords[i].toLocal8Bit().toBase64() << endl;
	}
    file.flush();
    file.close();
}

void QHistoricalRecordTextEdit::readKeywordsFromLocal()
{
	QFile file(localKeywordsFilePath());
	if (!file.open(QIODevice::ReadOnly | QFile::Text))
	{
		qWarning() << QStringLiteral("不能读取历史数据文件:%1, %2!").arg(localKeywordsFilePath()).arg(file.errorString());
		return;
	}

	m_keywords.clear();

    //首先写入函数信息 by zyg
    m_keywords.append(m_allFunctionNameList);

	while (!file.atEnd())
	{
        QByteArray temp = file.readLine();
        if(m_allFunctionNameList.contains(QString::fromLocal8Bit(QByteArray::fromBase64(temp))))
        {
            continue;
        }

        m_keywordBase64 << temp;
		m_keywords << QString::fromLocal8Bit(QByteArray::fromBase64(temp));
	}
}

void QHistoricalRecordTextEdit::addKeyword(const QString &kw)
{
	const QString temp = kw.trimmed();
	if (temp.isEmpty()) return;
	const QByteArray temp_base64 = temp.toLocal8Bit().toBase64();
	if (m_keywordBase64.contains(temp_base64)) return;
	m_keywords << temp;
	m_keywordBase64 << temp_base64;
	m_keywordsModel.setStringList(m_keywords);
	m_completer->setModel(&m_keywordsModel);
    if (m_keywords.size() > kLimitHistoriesSize) m_keywords.removeAt(0);
}

void QHistoricalRecordTextEdit::addFuncName(const QList<QString> &func)
{
    m_allFunctionNameList = func;
}

QString QHistoricalRecordTextEdit::localKeywordsFilePath()
{
	const QString root_dir_string = QStandardPaths::standardLocations(QStandardPaths::DataLocation)[0] + "/app";
	QDir root_dir(root_dir_string);
	if (!root_dir.exists())
	{
		root_dir.mkpath(root_dir_string);
	}

	return root_dir_string + "/" + kHistoryFileName;
}

