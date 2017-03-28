#ifndef __JQQMLIMAGE_JQQMLIMAGEMANAGE_INC__
#define __JQQMLIMAGE_JQQMLIMAGEMANAGE_INC__

// .h include
#include "jqqmlimagemanage.h"

inline void JQQmlImageManage::setQmlApplicationEngine(const QPointer< QQmlApplicationEngine > &qmlApplicationEngine)
{
    qmlApplicationEngine_ = qmlApplicationEngine;
}

inline QPointer< QQmlApplicationEngine > JQQmlImageManage::qmlApplicationEngine()
{
    return qmlApplicationEngine_;
}

#endif//__JQQMLIMAGE_JQQMLIMAGEMANAGE_INC__
