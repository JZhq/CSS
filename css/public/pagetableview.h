#ifndef PAGETABLEVIEW_H
#define PAGETABLEVIEW_H

#include "public_global.h"
#include <QWidget>

namespace Ui {
class PageTableView;
}

class QAbstractItemModel;
class QTableView;
class PageTableViewPriate;
class GLOBAL_EXPORT PageTableView : public QWidget
{
    Q_OBJECT

public:
    explicit PageTableView(QWidget *parent = 0);
    ~PageTableView();

    //!
    //! \brief 设置表中的总记录数
    //! \param count
    //!
    void setTotalRecordCount(int count);

    //!
    //! \brief 设置当前页数
    //! \param page
    //!
    void setCurentPage(int page);

    //!
    //! \brief 刷新表状态描述的文字
    //!
    void updateDescText();


    //!
    //! \brief 返回本控件中的表视图
    //! \return
    //!
    QTableView* tableView();

    void updateQuery();

signals:
    //!
    //! \brief 页面跳转信号
    //! \param index 起始索引
    //! \param offset 偏移量
    //!
    void signalPageJump(int index, int offset);

private:
    //!
    //! \brief 初始化界面
    //!
    void initializeUi();

    //!
    //! \brief 信号槽事件处理
    //!
    void handleEvents();

    //!
    //! \brief 页面跳转，未收到结果时的状态
    //!
    void waitState();

    //!
    //! \brief 页面完成跳转或响应超时的状态
    //!
    void clearWaitState();

    //!
    //! \brief 控制按钮是否启用
    //!
    void setButtonState();

public slots:
    //!
    //! \brief 首页
    //!
    void on_firstPageButton_clicked();

    //!
    //! \brief 上一页
    //!
    void on_prePageButton_clicked();

    //!
    //! \brief 下一页
    //!
    void on_nextPageButton_clicked();

    //!
    //! \brief 尾页
    //!
    void on_lastPageButton_clicked();

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::PageTableView *ui;
    PageTableViewPriate* d;
};

#endif // PAGETABLEVIEW_H
