#include "pagetableview.h"
#include "config.h"
#include "ui_pagetableview.h"

#include <QTableView>
#include <QCursor>
#include <QTimer>

class PageTableViewPriate
{
public:
    static const int WAIT_TIME = 3000;

    PageTableViewPriate(PageTableView* owner) :
        pageTableView(owner),
        recordsPerPage(Config::getInstance()->getTableRows()),
        currentPage(1),
        totalPages(0),
        totalRecords(0),
        bInitComplete(false)
    {
        // 最长未响应时间为 3s
        responseTimer.setInterval(WAIT_TIME);
    }

    PageTableView* pageTableView;
    int recordsPerPage;   // 每页的条目数
    int currentPage;      // 当前的页数
    int totalPages;       // 总共的页数
    int totalRecords;     // 总共的条目数
    QTimer responseTimer;  // 响应超时的定时器
    bool bInitComplete;
};


PageTableView::PageTableView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PageTableView),
    d(new PageTableViewPriate(this))
{
    initializeUi();
    handleEvents();
}

PageTableView::~PageTableView()
{
    delete ui;
    if (d)
        delete d;
}

void PageTableView::initializeUi()
{
    ui->setupUi(this);
    updateDescText();
    ui->prePageButton->setEnabled(false);
    ui->nextPageButton->setEnabled(false);
    ui->prePageButton->setProperty("class", "pageButton");
    ui->nextPageButton->setProperty("class", "pageButton");
    ui->firstPageButton->setProperty("class", "pageButton");
    ui->lastPageButton->setProperty("class", "pageButton");
}

void PageTableView::handleEvents()
{
    connect(&d->responseTimer, &QTimer::timeout, [this]() {
       // TODO 提示超时
       clearWaitState();
    });
//    connect(this, &PageTableView::signalPageJump, [this](int, int){
//        setButtonState();
//    });
}

void PageTableView::setButtonState()
{
    if (d->currentPage <= 1) {
        ui->prePageButton->setEnabled(false);
        ui->nextPageButton->setEnabled(true);
    } else if (d->currentPage >= d->totalPages) {
        ui->prePageButton->setEnabled(true);
        ui->nextPageButton->setEnabled(false);
    } else {
        ui->prePageButton->setEnabled(true);
        ui->nextPageButton->setEnabled(true);
    }
}


void PageTableView::setTotalRecordCount(int count)
{
    if (count >= 0)
        d->totalRecords = count;
}

void PageTableView::setCurentPage(int page)
{
    if (page > 0)
    {
        d->currentPage = page;
        ui->curPageBtn->setText(QString::number(page));
    }
}

void PageTableView::updateDescText()
{
    // 清除等待状态
    clearWaitState();
    int totalNum = d->totalRecords % Config::getInstance()->getTableRows() == 0 ?  \
                d->totalRecords / Config::getInstance()->getTableRows() : (d->totalRecords / Config::getInstance()->getTableRows()) + 1;
    ui->descLabel->
            setText(QString(QStringLiteral("共：%1 页"))
                    .arg(totalNum));
    if(d->totalPages != totalNum)
    {
        d->bInitComplete = false;
        ui->comboBox->clear();
        QStringList list;
        for(int i = 0;i<totalNum;i++)
        {
            list.append(QString::number(i+1));
        }
        ui->comboBox->addItems(list);
        d->bInitComplete = true;

    }
    d->totalPages = totalNum;
    ui->curPageBtn->setText(QString::number(d->currentPage));
    setButtonState();


}

QTableView* PageTableView::tableView()
{
    return ui->tableView;
}

void PageTableView::updateQuery()
{
    emit signalPageJump((d->currentPage-1) * Config::getInstance()->getTableRows(), Config::getInstance()->getTableRows());
}

void PageTableView::on_firstPageButton_clicked()
{
    if (cursor().shape() != Qt::ArrowCursor)
        return;

    waitState();
    d->currentPage = 1;
    emit signalPageJump(0, Config::getInstance()->getTableRows());
}

void PageTableView::on_prePageButton_clicked()
{
    if (cursor().shape() != Qt::ArrowCursor)
        return;
    if(d->totalPages<=0)
        return;
    if (d->currentPage > 1) {
        waitState();
        int page = d->currentPage - 1;
        int index = (page - 1) * Config::getInstance()->getTableRows();
        d->currentPage -= 1;
        emit signalPageJump(index, Config::getInstance()->getTableRows());

    }
}

void PageTableView::on_nextPageButton_clicked()
{
    if (cursor().shape() != Qt::ArrowCursor)
        return;
    if(d->totalPages<=0)
        return;
    if (d->currentPage < d->totalPages) {
        waitState();
        int page = d->currentPage - 1;
        int index = (page + 1) * Config::getInstance()->getTableRows();
        d->currentPage += 1;
        emit signalPageJump(index, Config::getInstance()->getTableRows());

    }
}

void PageTableView::on_lastPageButton_clicked()
{
    if (cursor().shape() != Qt::ArrowCursor)
        return;
    if(d->totalPages<=0)
        return;
    waitState();
    d->currentPage = d->totalPages;
    emit signalPageJump((d->totalPages - 1) * Config::getInstance()->getTableRows(), Config::getInstance()->getTableRows());
}

void PageTableView::waitState()
{
    setCursor(Qt::WaitCursor);
    d->responseTimer.start();
}

void PageTableView::clearWaitState()
{
    setCursor(Qt::ArrowCursor);
    d->responseTimer.stop();
    d->responseTimer.setInterval(PageTableViewPriate::WAIT_TIME);
}

void PageTableView::on_comboBox_currentIndexChanged(int index)
{
    if (cursor().shape() != Qt::ArrowCursor)
        return;
    if(!d->bInitComplete)
        return;
    waitState();
    int jumpPage = index+1;
    if (jumpPage > d->totalPages)
        jumpPage = d->totalPages;
    setCurentPage(jumpPage);
    emit signalPageJump((jumpPage-1) * Config::getInstance()->getTableRows(), Config::getInstance()->getTableRows());
}
