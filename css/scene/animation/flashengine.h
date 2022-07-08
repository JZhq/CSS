/**********************************************************************
 * CopyRight:   Sinux Ltd
 * File Name:   flashengine.h
 * Description: 闪烁引擎
 * Author:      lx
 * Date:        2016.06.30
 * Version:     v1.0.0
 * History:     (none)
 **********************************************************************/
#ifndef FLASHENGINE_H
#define FLASHENGINE_H

#include <QObject>
#include <QTimer>

class BaseItem;

// 闪烁引擎
class FlashEngine : public QObject
{
    Q_OBJECT

public:
    explicit FlashEngine(QObject *parent = 0, BaseItem *pItem = 0);

    void startEngine();
    bool engineStatus();

signals:

public slots:
    void slotFlash();

private:
    bool m_bEngineStatus;       // 引擎的状态
    BaseItem *m_pItem;
};

#endif // FLASHENGINE_H
