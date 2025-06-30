import QtQuick
import QtQuick.Window
import QtQuick.Controls
import Qt.labs.platform
import Skinny 1.0 as Qsk

//import "qrc:/qml"

Qsk.Window {
    id: window
    width: 800
    height: 600
    color: darkStyle.windowBackgroundColor
    title: qsTr("Hello, world!")
    visible: true
    autoLayoutChildren: false
    
    Component.onCompleted: {
        windowAgent.setup(window)
        setupWindowOptimizations()
        if (window.showWhenReady) {
            window.visible = true
        }
    }
    
    // Window optimization setup function
    // 窗口优化设置函数
    function setupWindowOptimizations() {
        // Enable dark mode for better system integration
        // 启用深色模式，获得更好的系统集成
        windowAgent.setWindowAttribute("dark-mode", true)
        
        // Enable Windows 11 Mica effect for smoother visual experience (if supported)
        // 启用Windows 11 Mica效果，获得更平滑的视觉体验（如果支持）
        if (Qt.platform.os === "windows") {
            // Try to enable Mica for Windows 11
            // 尝试为Windows 11启用Mica
            try {
                windowAgent.setWindowAttribute("mica", true)
                
                // If Mica is not available, fallback to acrylic
                // 如果Mica不可用，回退到亚克力效果
                if (!windowAgent.windowAttribute("mica")) {
                    windowAgent.setWindowAttribute("acrylic-material", true)
                }
            } catch (e) {
                console.log("Windows visual effects not supported:", e)
            }
        }
        
        console.log("Window optimization setup completed")
    }

    QtObject {
        id: darkStyle
        readonly property color windowBackgroundColor: "#534020"
    }

    Timer {
        interval: 100
        running: true
        repeat: true
        onTriggered: timeLabel.text = Qt.formatTime(new Date(), "hh:mm:ss")
    }
    
    Qsk.QuickWindowAgent {
        id: windowAgent
    }
    
    Rectangle {
        id: titleBar
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        //        height: 32
        implicitHeight: 32
        color: window.active ? "#0782b7" : "#56b30e"
        //color: "transparent"
        
        Component.onCompleted: {
            windowAgent.setTitleBar(titleBar)
        }
        
        Image {
            id: iconButton
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 10
            }
            width: 18
            height: 18
            mipmap: true
            source: "qrc:/qml/window-bar/example.png"
            fillMode: Image.PreserveAspectFit
            Component.onCompleted: windowAgent.setSystemButton(Qsk.QuickWindowAgent.WindowIcon, iconButton)
        }

        Text {
            id: cap_title
            anchors {
                verticalCenter: parent.verticalCenter
                left: iconButton.right
                leftMargin: 10
            }
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: window.title
            font.pixelSize: 14
            color: "#ECECEC"
            
            // Enable hardware acceleration for text rendering
            // 为文本渲染启用硬件加速
            renderType: Text.NativeRendering
        }
        
        Rectangle {
            id: buttonBox
            anchors {
                top: parent.top
//                left: cap_title.right
                right: parent.right
            }
            width: titleBar.width*0.12
            height: parent.height
            color: "transparent"
            
            Qsk.LinearBox {
                orientation: Qt.Horizontal
                dimension: 3
                panel: true // to have a themed background
                autoLayoutChildren: true
                spacing: 1
                
                Qsk.PushButton {
                    id: minButton
                    checkable: true
                    maximumSize{
                        width: buttonBox.width *0.33
                    }
                    iconStrutSize {
                        // no strutSize, so that the icon is adjustd
                        width: -1
                        height: -1
                    }

                    iconSource: "image://qrc/qml/window-bar/minimize.svg"
                    
                    // Smooth animation for button interactions
                    // 按钮交互平滑动画
                    Behavior on opacity {
                        NumberAnimation { duration: 150; easing.type: Easing.OutQuad }
                    }
                    
                    onClicked: window.showMinimized()
                    Component.onCompleted: windowAgent.setSystemButton(Qsk.QuickWindowAgent.Minimize, minButton)
                }

                Qsk.PushButton {
                    id: maxButton
                    height: parent.height
                    maximumSize{
                        width: buttonBox.width *0.33
                    }
                    iconSource: window.visibility === Window.Maximized ? "image://qrc/qml/window-bar/restore.svg" : "image://qrc/qml/window-bar/maximize.svg"
                    
                    // Smooth animation for button interactions
                    // 按钮交互平滑动画
                    Behavior on opacity {
                        NumberAnimation { duration: 150; easing.type: Easing.OutQuad }
                    }
                    
                    onClicked: {
                        if (window.visibility === Window.Maximized) {
                            window.showNormal();
                        } else {
                            window.showMaximized();
                        }
                    }
                    Component.onCompleted: windowAgent.setSystemButton(Qsk.QuickWindowAgent.Maximize, maxButton)
                }

                Qsk.PushButton {
                    id: closeButton
                    height: parent.height
                    iconSource: "image://qrc/qml/window-bar/close.svg"
                    maximumSize{
                        width: buttonBox.width *0.33
                    }
                    background: Rectangle {
                        color: {
                            if (!closeButton.enabled) {
                                return "gray";
                            }
                            if (closeButton.pressed) {
                                return "#e81123";
                            }
                            if (closeButton.hovered) {
                                return "#e81123";
                            }
                            return "transparent";
                        }
                        
                        // Smooth color transition
                        // 平滑颜色过渡
                        Behavior on color {
                            ColorAnimation { duration: 150; easing.type: Easing.OutQuad }
                        }
                    }
                    onClicked: window.close()
                    Component.onCompleted: windowAgent.setSystemButton(Qsk.QuickWindowAgent.Close, closeButton)
                }
            }
        }
    }

    Label {
        id: timeLabel
        anchors.centerIn: parent
        font {
            pointSize: 75
            bold: true
        }
        color: "#FEFEFE"
        
        // Enable optimal text rendering for large fonts
        // 为大字体启用最佳文本渲染
        renderType: Text.NativeRendering
        
        Component.onCompleted: {
            console.log("Curve rendering for text is available.");
            // Use NativeRendering for better performance with large text
            // 对大文本使用NativeRendering获得更好的性能
            timeLabel.renderType = Text.NativeRendering;
        }
    }
    
    Qsk.FocusIndicator {}
}
