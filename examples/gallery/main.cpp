/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "progressbar/ProgressBarPage.h"
#include "inputs/InputPage.h"
#include "button/ButtonPage.h"
#include "selector/SelectorPage.h"
#include "dialog/DialogPage.h"
#include "listbox/ListBoxPage.h"

#include <QskSetup.h>
#include <QskSkinManager.h>
#include <QskSkin.h>
#include <SkinnyNamespace.h>
#include <SkinnyShapeProvider.h>
#include <SkinnyShortcut.h>

#include "QskLinearBox.h"
#include <QskMainView.h>
#include <QskFocusIndicator.h>
#include <QskObjectCounter.h>
#include <QskDrawer.h>
#include <QskTabView.h>
#include <QskTextLabel.h>
#include <QskSwitchButton.h>
#include <QskPushButton.h>
#include <QskPageIndicator.h>
#include <QskScrollArea.h>
#include <QskMenu.h>
#include <QskWindow.h>
#include <QskDialog.h>
#include <QskSkinManager.h>
#include <QskSkin.h>
#include <QskSkinTransition.h>
#include <QskAnimationHint.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskGraphicProvider.h>
#include <QskGraphicIO.h>
#include <QskGraphic.h>

#include <QGuiApplication>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QtMessageHandler>
#include <QDebug>
#include <functional>
#include <vector>
#include <exception>

// Custom message handler to write logs to a file
// 自定义消息处理函数，用于将日志写入文件
void logToAFileMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QFile file("gallery_debug_log.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        fprintf(stderr, "Could not open gallery_debug_log.txt for writing.\n");
        return;
    }

    QTextStream out(&file);
    QString log = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");

    switch (type) {
    case QtDebugMsg:
        log += "[Debug]   ";
        break;
    case QtInfoMsg:
        log += "[Info]    ";
        break;
    case QtWarningMsg:
        log += "[Warning] ";
        break;
    case QtCriticalMsg:
        log += "[Critical]";
        break;
    case QtFatalMsg:
        log += "[Fatal]   ";
        break;
    }

    if (context.file) {
        log += QString("%1 (%2:%3, %4)").arg(msg, context.file, QString::number(context.line), context.function);
    } else {
        log += msg;
    }
    
    out << log << "\n";
    out.flush();
    file.close();

    // Also print to console to see it in real time
    // 实时打印到控制台
    fprintf(stderr, "%s\n", qPrintable(log));
    fflush(stderr);

    if (type == QtFatalMsg)
        abort();
}

namespace
{
    // Page loading control mechanism for performance testing
    // 页面加载控制机制，用于性能测试
    struct PageConfig {
        QString name;                                    // Tab display name / Tab显示名称
        std::function<QQuickItem*()> pageCreator;       // Page creation function / 页面创建函数
        bool enabled;                                   // Whether to load this page / 是否加载此页面
        QString description;                            // Page description for debugging / 页面描述（用于调试）
    };
    
    // Configure which pages to load - modify 'enabled' field for testing
    // 配置要加载的页面 - 修改'enabled'字段进行测试
    static std::vector<PageConfig> g_pageConfigs = {
        // Start with only 1 page enabled to test basic functionality
        // 首先只启用1个页面来测试基本功能
        {"Buttons",    []() -> QQuickItem* { return new ButtonPage(); },    true  ,  "Button controls and actions"},
        {"Inputs",     []() -> QQuickItem* { return new InputPage(); },      true , "Text inputs and form controls"},  
        {"Indicators", []() -> QQuickItem* { return new ProgressBarPage(); }, true, "Progress bars and indicators"},
        {"Selectors",  []() -> QQuickItem* { return new SelectorPage(); },  true  , "Selection controls and menus"},
        {"Dialogs",    []() -> QQuickItem* { return new DialogPage(); },     true , "Modal dialogs and popups"},
        {"ListBox",    []() -> QQuickItem* { return new ListBoxPage(); },    true , "List boxes and data views"}
    };

    class GraphicProvider : public QskGraphicProvider
    {
      protected:
        const QskGraphic* loadGraphic( const QString& id ) const override
        {
            const QString path = QStringLiteral( ":gallery/icons/qvg/" )
                + id + QStringLiteral( ".qvg" );

            const auto graphic = QskGraphicIO::read( path );
            return graphic.isNull() ? nullptr : new QskGraphic( graphic );
        }
    };

    class Drawer : public QskDrawer
    {
      public:
        Drawer( QQuickItem* parent = nullptr )
            : QskDrawer( parent )
        {
            auto box = new QskLinearBox( Qt::Vertical, this );

            box->setSection( QskAspect::Header );
            box->setPanel( true );
            box->setPaddingHint( QskBox::Panel, 20 );

            new QskPushButton( "One", box );
            new QskPushButton( "Two", box );
            new QskPushButton( "Three", box );

            box->addStretch( 1 );

            auto btn = new QskPushButton( "Close", box );
            connect( btn, &QskPushButton::clicked, this, &QskDrawer::close );
        }
    };

    class TabView : public QskTabView
    {
      public:
        TabView( QQuickItem* parent = nullptr )
            : QskTabView( parent )
        {
            setAutoFitTabs( true );
        }

        void setPagesEnabled( bool on )
        {
            for ( int i = 0; i < count(); i++ )
                pageAt( i )->setEnabled( on );
        }

        void addPage( const QString& tabText, QQuickItem* page )
        {
            auto scrollArea = new QskScrollArea();
            scrollArea->setMargins( 5 );

#if 1
            /*
                We need a mode, where the focus policy gets adjusted
                when a scroll bar becomes visible. TODO ...
             */
            scrollArea->setFocusPolicy( Qt::NoFocus );
#endif

            // hiding the viewport
            scrollArea->setGradientHint( QskScrollView::Viewport, QskGradient() );
            scrollArea->setBoxShapeHint( QskScrollView::Viewport, 0 );
            scrollArea->setBoxBorderMetricsHint( QskScrollView::Viewport, 0 );

            scrollArea->setItemResizable( true );
            scrollArea->setScrolledItem( page );

            addTab( tabText, scrollArea );
        }

        // Add page lazily: create the heavy page only when user activates the tab
        // 延迟添加页面：仅在用户激活该Tab时才创建重量级页面
        void addPageLazy( const QString& tabText, const std::function<QQuickItem*()>& pageFactory )
        {
            auto scrollArea = new QskScrollArea();
            scrollArea->setMargins( 5 );
            scrollArea->setFocusPolicy( Qt::NoFocus );
            scrollArea->setGradientHint( QskScrollView::Viewport, QskGradient() );
            scrollArea->setBoxShapeHint( QskScrollView::Viewport, 0 );
            scrollArea->setBoxBorderMetricsHint( QskScrollView::Viewport, 0 );
            scrollArea->setItemResizable( true );

            // Lightweight placeholder to avoid immediate heavy loading
            // 轻量占位，避免立即加载重量级页面
            auto placeholder = new QskLinearBox( Qt::Vertical );
            placeholder->setLayoutAlignmentHint( Qt::AlignCenter );
            scrollArea->setScrolledItem( placeholder );

            addTab( tabText, scrollArea );

            // Record lazy entry
            // 记录延迟加载项
            LazyEntry entry;
            entry.index = count() - 1;
            entry.factory = pageFactory;
            entry.scrollArea = scrollArea;
            entry.loaded = false;
            m_lazyEntries.push_back( entry );

            // Connect once to handle lazy loading on tab activation
            // 仅连接一次信号，用于在Tab激活时执行延迟加载
            if ( !m_lazyHandlerConnected )
            {
                m_lazyHandlerConnected = true;
                connect( this, &QskTabView::currentIndexChanged, this,
                    [this]( int index )
                    {
                        for ( auto& e : m_lazyEntries )
                        {
                            if ( e.index == index && !e.loaded )
                            {
                                qDebug() << "Lazy-loading page at index" << index << "...";
                                QQuickItem* page = nullptr;
                                try {
                                    page = e.factory ? e.factory() : nullptr;
                                } catch ( const std::exception& ex ) {
                                    qWarning() << "Exception in pageFactory:" << ex.what();
                                } catch ( ... ) {
                                    qWarning() << "Unknown exception in pageFactory";
                                }

                                if ( page )
                                {
                                    e.scrollArea->setScrolledItem( page );
                                    e.loaded = true;
                                    qDebug() << "Lazy-loaded page at index" << index << "done.";
                                }
                                else
                                {
                                    qWarning() << "Failed to lazy-load page at index" << index;
                                }

                                break;
                            }
                        }
                    }
                );
            }
        }

      private:
        struct LazyEntry
        {
            int index;
            std::function<QQuickItem*()> factory;
            QskScrollArea* scrollArea;
            bool loaded;
        };

        std::vector< LazyEntry > m_lazyEntries;
        bool m_lazyHandlerConnected = false;
    };

    class MenuButton : public QskPushButton
    {
      public:
        MenuButton( const QString& text, QQuickItem* parent = nullptr )
            : QskPushButton( text, parent )
        {
            connect( this, &QskPushButton::pressed, this, &MenuButton::openMenu );
        }

      private:
        void openMenu()
        {
            auto menu = new QskMenu( window()->contentItem() );

            populateMenu( menu );

            menu->setOrigin( geometry().bottomLeft() );
            menu->open();
        }

        virtual void populateMenu( QskMenu* ) = 0;
    };

    class SkinButton final : public MenuButton
    {
      public:
        SkinButton( const QString& text, QQuickItem* parent = nullptr )
            : MenuButton( text, parent )
        {
        }

      private:
        void populateMenu( QskMenu* menu ) override
        {
            const auto names = qskSkinManager->skinNames();

            for ( const auto& name : names )
                menu->addOption( QUrl(), name );

            if ( const auto index = names.indexOf( qskSkinManager->skinName() ) )
                menu->setCurrentIndex( index );

            connect( menu, &QskMenu::triggered, this, &SkinButton::changeSkin );
        }

        void changeSkin( int index )
        {
            const auto names = qskSkinManager->skinNames();

            if ( ( index >= 0 ) && ( index < names.size() )
                 && ( index != names.indexOf( qskSkinManager->skinName() ) ) )
            {
                qskSkinManager->setSkin( names[index] );
            }
        }
    };

    class FileButton final : public MenuButton
    {
      public:
        FileButton( const QString& text, QQuickItem* parent = nullptr )
            : MenuButton( text, parent )
        {
        }

      private:
        void populateMenu( QskMenu* menu ) override
        {
            menu->addOption( "image://shapes/Rectangle/White", "Print" );
            menu->addOption( "image://shapes/Diamond/Yellow", "Save As" );
            menu->addOption( "image://shapes/Ellipse/Red", "Setup" );
            menu->addSeparator();
            menu->addOption( "image://shapes/Hexagon/PapayaWhip", "Quit" );

            // see https://github.com/uwerat/qskinny/issues/192
            connect( menu, &QskMenu::triggered,
                []( int index ) { if ( index == 4 ) qApp->quit(); } );
        }
    };

    class Header : public QskLinearBox
    {
        Q_OBJECT

      public:
        Header( int tabCount, QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, parent )
        {
            setPaddingHint( QskBox::Panel, 5 );

            initSizePolicy( QskSizePolicy::Ignored, QskSizePolicy::Fixed );
            setPanel( true );

            new FileButton( "File", this );
            new SkinButton( "Skin", this );

            addStretch( 10 );
            m_pageIndicator = new QskPageIndicator( tabCount, this );
            m_pageIndicator->setCurrentIndex( 0 );
            addStretch( 10 );

            {
                new QskTextLabel( "Enabled", this );

                auto button = new QskSwitchButton( this );
                button->setChecked( true );

                connect( button, &QskSwitchButton::toggled,
                    this, &Header::enabledToggled );
            }

            {
                auto burger = new QskPushButton( "≡", this );
                burger->setEmphasis( QskPushButton::LowEmphasis );

                connect( burger, &QskPushButton::clicked,
                    this, &Header::drawerRequested );
            }
        }

      public Q_SLOTS:
        void setCurrentTab( int index )
        {
            m_pageIndicator->setCurrentIndex( index );
        }

      Q_SIGNALS:
        void enabledToggled( bool );
        void drawerRequested();

      private:
        QskPageIndicator* m_pageIndicator;
    };

    class MainView : public QskMainView
    {
      public:
        MainView( QQuickItem* parent = nullptr )
            : QskMainView( parent )
        {
            auto tabView = new TabView( this );
            
            // Load only enabled pages for performance testing
            // 只加载启用的页面进行性能测试
            int enabledPageCount = 0;
            qDebug() << "=== Page Loading Configuration ===";
            
            for (const auto& config : g_pageConfigs) {
                if (config.enabled) {
                    qDebug() << "Loading page:" << config.name << "-" << config.description;
                    
                    // For the heavy Indicators page, load lazily to avoid background cost
                    // 对于较重的“Indicators”页面，采用延迟加载，避免未显示时的后台开销
                    if (config.name == "Indicators") {
                        tabView->addPageLazy(config.name, config.pageCreator);
                        enabledPageCount++;
                        continue;
                    }

                    // Create page with error handling
                    // 创建页面并进行错误处理
                    try {
                        QQuickItem* page = config.pageCreator();
                        if (page) {
                            tabView->addPage(config.name, page);
                            enabledPageCount++;
                        } else {
                            qWarning() << "Failed to create page:" << config.name;
                        }
                    } catch (const std::exception& e) {
                        qWarning() << "Exception creating page" << config.name << ":" << e.what();
                    } catch (...) {
                        qWarning() << "Unknown exception creating page:" << config.name;
                    }
                } else {
                    qDebug() << "Skipping page:" << config.name << "-" << config.description;
                }
            }
            
            qDebug() << "Total pages loaded:" << enabledPageCount << "out of" << g_pageConfigs.size();
            qDebug() << "=== End Page Loading Configuration ===";

            // Ensure we have at least one page loaded
            // 确保至少加载了一个页面
            if (enabledPageCount == 0) {
                qWarning() << "No pages loaded! Adding emergency ButtonPage to prevent crash.";
                tabView->addPage("Emergency", new ButtonPage());
                enabledPageCount = 1;
            }

            auto header = new Header( enabledPageCount, this );

            connect( header, &Header::enabledToggled,
                tabView, &TabView::setPagesEnabled );

            connect( tabView, &TabView::currentIndexChanged,
                header, &Header::setCurrentTab );

            auto drawer = new Drawer( tabView );
            drawer->setEdge( Qt::RightEdge );

            connect( header, &Header::drawerRequested,
                drawer, &QskPopup::toggle );

            setHeader( header );
            setBody( tabView );
        }
    };
}

int main( int argc, char* argv[] )
{
//    qInstallMessageHandler(logToAFileMessageHandler);
    qDebug() << "Application starting...";

//    qputenv("QSG_RENDERER_DEBUG", "render");

#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    Qsk::addGraphicProvider( QString(), new GraphicProvider() );
    Qsk::addGraphicProvider( "shapes", new SkinnyShapeProvider() );


    QGuiApplication app( argc, argv );
    if ( true ) // environment variable, TODO ...
    {
        // dialogs in faked windows -> QskSubWindow
        QskDialog::instance()->setPolicy( QskDialog::EmbeddedBox );
    }
    // Setting a skin early avoids a lazy initialization, that might lead
    // to a pending update request, that is processed, when showing the window
    // for the first time. This can be avoided by an explicit initialization
    // before the window is created.

    qDebug() << "Setting skin...";
    auto skinManager = QskSkinManager::instance();

    QStringList availableSkins = skinManager->skinNames();
    if ( !availableSkins.isEmpty() )
    {
        skinManager->setSkin( availableSkins.first() );
    }
    else
    {
        qWarning() << "No skins found!";
        // 在这里可以决定是退出还是继续
        return -1;
    }
    qDebug() << "Skin set.";

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    auto mainView = new MainView();

    QskWindow window;
    window.addItem( mainView );
    window.addItem( new QskFocusIndicator() );

    window.resize( 800, 600 );

    qDebug() << "Showing window...";
    window.show();
    qDebug() << "Window shown.";

    const int ret = app.exec();
    qDebug() << "Application finished with exit code" << ret;
    return ret;
}

#include "main.moc"
