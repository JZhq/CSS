#include "managescene.h"
#include "graphicsscene.h"
#include "graphicsview.h"

#include <QDebug>

class ManageScenePrivate
{
public:
    ManageScenePrivate():
        m_scene(Q_NULLPTR),
        m_view(Q_NULLPTR)
    {

    }

    GraphicsScene *m_scene;
    GraphicsView *m_view;
};

ManageScene::ManageScene(QObject *parent) : QObject(parent),d(new ManageScenePrivate)
{
    m_object = parent;
}

ManageScene::~ManageScene()
{
    delete d;
}

GraphicsScene *ManageScene::createScene()
{
    if(d->m_scene == Q_NULLPTR)
    {
        d->m_scene = new GraphicsScene(m_object);
    }
    return d->m_scene;
}

GraphicsView *ManageScene::createView()
{
    if(d->m_view == Q_NULLPTR)
    {
        d->m_view = new GraphicsView();
    }

    return d->m_view;
}
