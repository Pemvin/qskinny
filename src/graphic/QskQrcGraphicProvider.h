/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_QRC_GRAPHIC_PROVIDER_H
#define QSK_QRC_GRAPHIC_PROVIDER_H

#include "QskGlobal.h"
#include <QskGraphicProvider.h>

class QSK_EXPORT QskQrcGraphicProvider : public QskGraphicProvider
{
  public:
    QskQrcGraphicProvider( QObject* parent = nullptr );
    ~QskQrcGraphicProvider() override;

  protected:
    const QskGraphic* loadGraphic( const QString& id ) const override;

  private:
    static bool isSupportedImageFormat( const QString& suffix );
};

#endif