#include "SkinnyShapeProvider.h"
#include <QskQml.h>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQuick/QQuickWindow>
#include <QSurfaceFormat>

// Rendering performance optimization function
// 渲染性能优化函数
void setupRenderingOptimizations()
{
    // Enable persistent OpenGL context to avoid resource recreation during window operations
    // 启用持久化OpenGL上下文，避免窗口操作时的资源重建
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
    
    // Configure optimal surface format for smooth rendering
    // 配置最佳表面格式以实现流畅渲染
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    
    // Enable multi-sampling anti-aliasing for smoother graphics
    // 启用多重采样抗锯齿，获得更平滑的图形效果
    format.setSamples(4);
    
    // Enable vertical sync to prevent screen tearing during window dragging
    // 启用垂直同步，防止窗口拖拽时的屏幕撕裂
    format.setSwapInterval(1);
    
    // Use double buffering for smooth frame transitions
    // 使用双缓冲实现平滑的帧切换
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    /*
    // Set optimal color buffer depth
    // 设置最佳颜色缓冲深度
    format.setRedBufferSize(8);
    format.setGreenBufferSize(8);
    format.setBlueBufferSize(8);
    format.setAlphaBufferSize(8);
    
    // Enable depth buffer for proper 3D rendering if needed
    // 如需要，启用深度缓冲进行正确的3D渲染
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    */
    QSurfaceFormat::setDefaultFormat(format);
    
    // Set rendering thread optimization
    // 设置渲染线程优化
    qputenv("QSG_RENDER_LOOP", "basic"); // Use basic render loop for better compatibility
    // 使用基础渲染循环，获得更好的兼容性
}

int main( int argc, char* argv[] )
{
    // Setup rendering optimizations before QGuiApplication creation
    // 在QGuiApplication创建前设置渲染优化
//    setupRenderingOptimizations();
    
    // Configure High DPI scaling behavior for crisp window operations
    // 配置高DPI缩放行为，确保窗口操作清晰
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough );
    
    // Enable High DPI pixmaps for better visual quality during scaling
    // 启用高DPI像素图，在缩放时获得更好的视觉效果
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    
    // Enable software rendering fallback if needed
    // 如需要，启用软件渲染回退
    QGuiApplication::setAttribute(Qt::AA_UseSoftwareOpenGL, false);
    
    QskQml::registerTypes();
    Qsk::addGraphicProvider( "shapes", new SkinnyShapeProvider() );
    QGuiApplication app( argc, argv );

    QQmlApplicationEngine engine;
    //    QWK::registerTypes( &engine );
    engine.load( QUrl( "qrc:/qml/main.qml" ) );

    return app.exec();
}