#ifndef SCENE_GLOBAL_H
#define SCENE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GRAPHICSITEMS_LIBRARY)
#  define SCENE_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define SCENE_SHARED_EXPORT Q_DECL_IMPORT
#endif

#define SAFE_DELETE(p) if(p){delete p ; p = NULL;}

#endif // SCENE_GLOBAL_H
