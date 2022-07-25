#ifndef QTHISTORICALRECORDTEXTEDIT_H
#define QTHISTORICALRECORDTEXTEDIT_H

#include <QTextEdit>
#include <QStringListModel>
#include <QFile>
#include <QLabel>

QT_BEGIN_NAMESPACE
class QCompleter;
QT_END_NAMESPACE

class QHistoricalRecordTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit QHistoricalRecordTextEdit(QWidget *parent = 0);
	~QHistoricalRecordTextEdit();

public:
    void setCompleter(QCompleter *completer);
    void setAllFuncName(const QList<QString> &funcName);

protected:
	void keyPressEvent(QKeyEvent *e);

    //void enterEvent(QEvent *event);
    //void leaveEvent(QEvent *event);
    //void mouseMoveEvent(QMouseEvent *event);
private Q_SLOTS:
	void onCompleterActivated(const QString &completion); // 响应选中QCompleter中的选项后，QCompleter发出的activated()信号

	void onEditComplete();
private:
	QString wordUnderCursor(); // 获取当前光标所在的单词

private:
	//保存历史记录到本地
	void writeKeywordsToLocal();

	//从本地读取历史记录
	void readKeywordsFromLocal();

	//添加关键词
	void addKeyword(const QString &kw);

    //添加函数名列表
    void addFuncName(const QList<QString> &func);

	//本地文件路径
	static QString localKeywordsFilePath();

private:
    QStringList m_keywords;           // 保存当前输入记录
	QList<QByteArray> m_keywordBase64; //输入记录的Base64
    QStringListModel m_keywordsModel;     // 字符串的列表模式
    QCompleter *m_completer;
    QList<QString> m_funcNameList;
    //QLabel *label;
    //所有函数名称
    QList<QString> m_allFunctionNameList;
};

#endif // QTHISTORICALRECORDTEXTEDIT_H
