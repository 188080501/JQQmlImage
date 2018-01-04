/*
    This file is part of JQLibrary

    Copyright: Jason

    Contact email: 188080501@qq.com

    GNU Lesser General Public License Usage
    Alternatively, this file may be used under the terms of the GNU Lesser
    General Public License version 2.1 or version 3 as published by the Free
    Software Foundation and appearing in the file LICENSE.LGPLv21 and
    LICENSE.LGPLv3 included in the packaging of this file. Please review the
    following information to ensure the GNU Lesser General Public License
    requirements will be met: https://www.gnu.org/licenses/lgpl.html and
    http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
*/

#ifndef JQQMLIMAGE_JQQMLIMAGEMANAGE_INC_
#define JQQMLIMAGE_JQQMLIMAGEMANAGE_INC_

// .h include
#include "jqqmlimagemanage.h"

inline QPointer< JQQmlImageManage > JQQmlImageManage::jqQmlImageManage()
{
    return jqQmlImageManage_;
}

inline bool JQQmlImageManage::enableCacheFeature()
{
    return enableCacheFeature_;
}

inline void JQQmlImageManage::setEnableCacheFeature(const bool &newValue)
{
    enableCacheFeature_ = newValue;
}

inline bool JQQmlImageManage::cachePathIsWritable()
{
    return cachePathIsWritable_;
}

inline qreal JQQmlImageManage::devicePixelRatio()
{
    return devicePixelRatio_;
}

inline void JQQmlImageManage::setDevicePixelRatio(const qreal &newValue)
{
    devicePixelRatio_ = newValue;
}

inline QStringList JQQmlImageManage::extraSelectors()
{
    return extraSelectors_;
}

inline void JQQmlImageManage::setExtraSelectors(const QStringList &newValue)
{
    extraSelectors_ = newValue;
}

inline QColor JQQmlImageManage::uint32ToColor(const quint32 &color)
{
    return QColor(
                *( reinterpret_cast< const quint8 * >( &color ) + 0 ),
                *( reinterpret_cast< const quint8 * >( &color ) + 1 ),
                *( reinterpret_cast< const quint8 * >( &color ) + 2 ),
                *( reinterpret_cast< const quint8 * >( &color ) + 3 )
                );
}

inline quint32 JQQmlImageManage::colorToUint32(const QColor &color)
{
    quint32 result;

    *( reinterpret_cast< quint8 * >( &result ) + 0 ) = static_cast< quint8 >( color.blue() );
    *( reinterpret_cast< quint8 * >( &result ) + 1 ) = static_cast< quint8 >( color.green() );
    *( reinterpret_cast< quint8 * >( &result ) + 2 ) = static_cast< quint8 >( color.red() );
    *( reinterpret_cast< quint8 * >( &result ) + 3 ) = static_cast< quint8 >( color.alpha() );

    return result;
}

inline qint32 JQQmlImageManage::getBitIndexFromColorIndex(const QImage &image, const qint32 &colorIndex, const bool &byteIsOrdered)
{
    const auto &&pixelByte = ( image.format() == QImage::Format_ARGB32 ) ? ( 4 ) : ( 3 );

    if ( byteIsOrdered )
    {
        return colorIndex * pixelByte;
    }
    else
    {
        return colorIndex * pixelByte + ( ( colorIndex / image.width() ) * ( image.width() % 4 ) );
    }
}

#endif//JQQMLIMAGE_JQQMLIMAGEMANAGE_INC_
