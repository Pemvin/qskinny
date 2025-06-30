/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_QUICK_WINDOW_AGENT_H
#define QSK_QUICK_WINDOW_AGENT_H

#include "QskGlobal.h"
#include <QWKQuick/quickwindowagent.h>
#include <QQmlEngine>

/**
 * @brief QSkinny封装的QuickWindowAgent，用于QML集成
 * QSkinny encapsulated QuickWindowAgent for QML integration
 */
class QSK_EXPORT QskQuickWindowAgent : public QWK::QuickWindowAgent, public QQmlParserStatus
{
    Q_OBJECT
//    QML_ELEMENT  
    Q_INTERFACES(QQmlParserStatus)
    QML_NAMED_ELEMENT(QskWindowAgent)// 自动注册为QML类型
        
public:
    explicit QskQuickWindowAgent(QObject* parent = nullptr);
    ~QskQuickWindowAgent() override;
    void classBegin() override;
    void componentComplete() override;
        
    // 静态注册函数
    static void registerQmlTypes();
    
};

#endif // QSK_QUICK_WINDOW_AGENT_H