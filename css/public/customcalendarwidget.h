#ifndef CUSTOMCALENDARWIDGET_H
#define CUSTOMCALENDARWIDGET_H

#include <QCalendarWidget>
#include "public_global.h"

class QTimeEdit;
class QPushButton;
class QLabel;
class GLOBAL_EXPORT CustomCalendarWidget : public QCalendarWidget
{
    Q_OBJECT
public:
    CustomCalendarWidget(QWidget *parent = Q_NULLPTR);

private:
    void initControl();
    void initTopWidget();
    void initBottomWidget();
    void setDataLabelTimeText(int year, int month);

signals:
    void signalSetCalendarTime(const QDate& data);

private slots:
    void onbtnClicked();

protected:
    void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const;

private:
    QPushButton* m_leftYearBtn;
    QPushButton* m_leftMonthBtn;

    QPushButton* m_rightYearBtn;
    QPushButton* m_rightMonthBtn;

    QPushButton* m_ensureBtn;
    QPushButton* m_toDayBtn;

    QLabel* m_dataLabel;
    QTimeEdit* m_timeEdit;
};

#endif // CUSTOMCALENDARWIDGET_H
