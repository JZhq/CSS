#ifndef CALENDARTIMEWIDGET_H
#define CALENDARTIMEWIDGET_H

#include <QWidget>
#include <QDateTime>

#include "public_global.h"

namespace Ui {
class CalendarTimeWidget;
}

class GLOBAL_EXPORT CalendarTimeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarTimeWidget(QWidget *parent = 0);
    ~CalendarTimeWidget();
    void setDateTime(const QDateTime &dateTime);
    QDateTime getDateTime() const;
    void setDateTimeRange(const QDateTime &min, const QDateTime &max);

private slots:
    void on_calendarWidget_clicked(const QDate &date);

private:
    Ui::CalendarTimeWidget *ui;
    QDateTime m_min;
    QDateTime m_max;
};

#endif // CALENDARTIMEWIDGET_H
