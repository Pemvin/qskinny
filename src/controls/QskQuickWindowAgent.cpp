/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskQuickWindowAgent.h"
#include <QQmlEngine>

QskQuickWindowAgent::QskQuickWindowAgent(QObject* parent)
    : QWK::QuickWindowAgent(parent)
{
}

QskQuickWindowAgent::~QskQuickWindowAgent() = default;


void QskQuickWindowAgent::classBegin()
{
    auto p = parent();
    Q_ASSERT_X(p, "QskQuickWindowAgent", "parent() return nullptr!");
    if (p) {
        if (p->objectName() == QLatin1StringView("QskWindow")) {
            setup(qobject_cast<QQuickWindow *>(p));
        }
    }
}

void QskQuickWindowAgent::componentComplete()
{

}

// 静态注册函数
void QskQuickWindowAgent::registerQmlTypes()
{
    // 已由 QML_ELEMENT 宏自动注册
    // 保留此函数用于向后兼容
    qmlRegisterType<QskQuickWindowAgent>("QSkinny", 1, 0, "QskWindowAgent");
}