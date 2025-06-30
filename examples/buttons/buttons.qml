import Skinny 1.0 as Qsk
import QtQuick 2.5
import "qrc:/qml"

Qsk.Window {
    id: window
    visible: true

    width: 900
    height: 600
    autoLayoutChildren: false

    Component.onCompleted: {
        // very much standard: we should find a better way

        var hint = sizeConstraint();
        setMinimumWidth(hint.width);
        setMinimumHeight(hint.height);
    }

    Qsk.Shortcut {
        sequence: "Ctrl+X"
        onActivated: console.log("Ctrl+X")
    }
    Rectangle {
        id: buttonBox
        anchors {
            top: parent.top
            // left: cap_title.right
            right: parent.right
        }
        width: parent.width*0.3
        height: parent.height
        color: "#d6edc4"

        Qsk.LinearBox {
            orientation: Qt.Horizontal
            dimension: 3
            panel: true // to have a themed background

            // padding: 10 // only possible with Qt <= 6.1
            padding {
                left: 10
                top: 10
                right: 10
                bottom: 10
            }
            spacing: 10

            TestButton {
                text: "1 Push Me "+width
                focus: true
                maximumSize{
                    width: buttonBox.width *0.3
                }
            }

            TestButton {
                text: "2 Disabled"
                enabled: false
                maximumSize{
                    width: buttonBox.width *0.3
                }
            }

            TestButton {
                text: "3 The quick brown fox jumps over the lazy dog !"
                clip: true
                maximumSize{
                    width: buttonBox.width *0.3
                }

                textOptions {
                    elideMode: Qt.ElideNone
                    wrapMode: Qsk.TextOptions.WordWrap
                }
            }

            TestButton {
                text: "4 The <b><font color='red'>quick</font></b> brown fox jumps over the lazy dog !"
                textOptions {
                    format: Qsk.TextOptions.AutoText
                    elideMode: Qt.ElideRight
                }
                maximumSize{
                    width: buttonBox.width *0.3
                }

                onClicked: {
                    textOptions.format = textOptions.format === Qsk.TextOptions.AutoText ? Qsk.TextOptions.PlainText : Qsk.TextOptions.AutoText;
                }
            }

            TestButton {
                text: "5 Autorepeater"
                maximumSize{
                    width: buttonBox.width *0.3
                }

                autoRepeat: true
                autoRepeatDelay: 500
                autoRepeatInterval: 200
            }

            TestButton {
                text: "6 Check Me"
                iconSource: "image://shapes/Ring/Khaki"
                maximumSize{
                    width: buttonBox.width *0.3
                }

                checkable: true
            }

            TestButton {
                text: "7 Check Me "+ buttonBox.width
                maximumSize{
                    width: buttonBox.width *0.3
                }

                section: Qsk.Aspect.Header
                checkable: true
            }

            TestButton {
                text: "8 Push Me"
                maximumSize{
                    width: buttonBox.width *0.3
                }

                section: Qsk.Aspect.Header
                /*            sizePolicy
            {
                // avoid the effect of long texts
                horizontal: Qsk.SizePolicy.MinimumExpanding
                vertical: Qsk.SizePolicy.Ignored
            }

            minimumSize
            {
                width: 10
                height: 10
            }
*/
            }

            TestButton {
                text: "9 Disabled"
                maximumSize{
                    width: buttonBox.width *0.3
                }

                section: Qsk.Aspect.Header
                enabled: false
            }

            TestButton {
                iconSource: "image://qrc/qml/window-bar/close.svg"//"image://shapes/Diamond/SandyBrown"

                iconStrutSize {
                    // no strutSize, so that the icon is adjustd
                    width: -1
                    height: -1
                }
                maximumSize{
                    width: buttonBox.width *0.3
                }

                shape {
                    sizeMode: Qt.RelativeSize
                    scalingMode: Qsk.BoxShapeMetrics.SymmetricByMaximum
                    radius: 100
                }
            }

            TestButton {
                text: "11 Push Me"
                maximumSize{
                    width: buttonBox.width *0.3
                }

                shape {
                    sizeMode: Qt.AbsoluteSize
                    radius: 0.0
                }
            }
        }
    }
    Qsk.FocusIndicator {}
}
