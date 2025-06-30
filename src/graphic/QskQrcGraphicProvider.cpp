/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskQrcGraphicProvider.h"
#include "QskGraphic.h"

#include <qimage.h>
#include <qfile.h>
#include <qdebug.h>
#include <qimagereader.h>
#include <qimagewriter.h>
#include <qpixmap.h>
#include <QPainter>
#include <QtSvg/QSvgRenderer>

QskQrcGraphicProvider::QskQrcGraphicProvider( QObject* parent )
    : QskGraphicProvider( parent )
{
}

QskQrcGraphicProvider::~QskQrcGraphicProvider()
{
}

const QskGraphic* QskQrcGraphicProvider::loadGraphic( const QString& id ) const
{
    // 构造资源路径
    QString resourcePath = id;
    if ( resourcePath.startsWith( ":/" ) )
        resourcePath = resourcePath.mid( 2 );
    else if ( resourcePath.startsWith( '/' ) || resourcePath.startsWith( ':' ) )
        resourcePath = resourcePath.mid( 1 );

    resourcePath.prepend( ":/" );

    // 检查文件是否存在
    if ( !QFile::exists( resourcePath ) )
    {
        qWarning() << "QskQrcGraphicProvider: Resource not found:" << resourcePath;
        return nullptr;
    }

    // 尝试作为图像文件加载
    QImageReader reader( resourcePath );
    if ( reader.canRead() )
    {
        QImage image = reader.read();
        if ( !image.isNull() )
        {
            QskGraphic* graphic = new QskGraphic();
            QPainter painter( graphic );
            painter.drawImage( 0, 0, image );
            painter.end();
            return graphic;
        }
    }

    // 尝试作为SVG文件加载
    qWarning() << "QskQrcGraphicProvider: Try load SVG resource as graphic:" << resourcePath;
    if ( resourcePath.endsWith( ".svg", Qt::CaseInsensitive ) )
    {
        QSvgRenderer renderer( resourcePath );
        if( renderer.isValid() )
        {
            QImage image( renderer.defaultSize(), QImage::Format_ARGB32 );
            image.fill( Qt::transparent );
            QPainter painter( &image );
            renderer.render( &painter );
            
            QskGraphic* graphic = new QskGraphic();
            QPainter graphicPainter( graphic );
            graphicPainter.drawImage( 0, 0, image );
            graphicPainter.end();
            return graphic;
        }
    }

    qWarning() << "QskQrcGraphicProvider: Cannot load resource as graphic:" << resourcePath;
    return nullptr;
}

bool QskQrcGraphicProvider::isSupportedImageFormat( const QString& suffix )
{
    const QStringList supportedFormats = {
        "png", "jpg", "jpeg", "bmp", "gif", "pbm", "pgm", "ppm", "xbm", "xpm"
    };
    
    return supportedFormats.contains( suffix.toLower() );
}