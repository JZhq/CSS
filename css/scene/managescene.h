/**
*@copyright     sinux
*@projectName   FaultDiagnosis
*@brief         管理场景
*@author        luox
*@date          2018-05-24
**/
#ifndef MANAGESCENE_H
#define MANAGESCENE_H

#include <QObject>
#include "common/scene_global.h"

class ManageScenePrivate;
class GraphicsView;
class GraphicsScene;

class SCENE_SHARED_EXPORT ManageScene : public QObject
{
    Q_OBJECT
public:
    explicit ManageScene(QObject *parent = nullptr);
    ~ManageScene();

    GraphicsScene* createScene();
    GraphicsView* createView();

private:
    ManageScenePrivate *d;
    QObject *m_object;
};

#endif // MANAGESCENE_H
