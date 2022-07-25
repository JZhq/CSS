#ifndef QTHISTORICALRECORDLINEEDIT_H
#define QTHISTORICALRECORDLINEEDIT_H

#include <QLineEdit>
#include <QStringListModel>
#include <QFile>

class QHistoricalRecordLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit QHistoricalRecordLineEdit(QWidget *parent = 0);


private:
	/**
	* @brief 设置正常
	*/
	void setNormal();
	/**
	* @brief 设置搜索
	*/
	void setSearching();

private slots:
    /**
     * @brief 编辑完成, 记录历史输入
     */
    void onEditComplete();

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
    QStringListModel m_keywordsModel;     // 字符串的列表模式
    //所有函数名称
    QList<QString> m_allFunctionNameList;
};

#endif // QTHISTORICALRECORDLINEEDIT_H
