QT += core gui network widgets websockets bluetooth
CONFIG += c++14

VER_MAJ = 1
VER_MIN = 3
VER_PAT = 0

VERSION = $$VER_MAJ"."$$VER_MIN"."$$VER_PAT
DEFINES += APP_VERSION=$$VERSION
DEFINES += APP_VERSION_STR=\"\\\"$${VERSION}\\\"\" \

SOURCES += \
    classes/NeuroPlayAppConnector.cpp \
    main.cpp \
    classes/Common.cpp \
    classes/Settings.cpp \
    ev3/EV3.cpp \
    ev3/EV3_Command.cpp \
    ev3/EV3_Motor.cpp \
    ui/BaseMotorsWidget.cpp \
    ui/IconLabel.cpp \
    ui/MotorsCoeffWidget.cpp \
    ui/MotorsWidget.cpp \
    ui/MainWindow.cpp

HEADERS += \
    classes/Common.h \
    classes/NeuroPlayAppConnector.h \
    classes/Settings.h \
    ev3/EV3.h \
    ev3/EV3_Command.h \
    ev3/EV3_ByteCodes.h \
    ev3/EV3_Motor.h \
    ui/BaseMotorsWidget.h \
    ui/IconLabel.h \
    ui/MainWindow.h \
    ui/MotorsCoeffWidget.h \
    ui/MotorsWidget.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

win32: RC_ICONS += resources/neuroEV3.ico

win32: DEFINES += OS_DESKTOP
macx: {
    DEFINES += OS_DESKTOP
    QMAKE_INFO_PLIST = $$PWD/platforms/mac/Info.plist
    QMAKE_ASSET_CATALOGS += $$PWD/platforms/mac/Images.xcassets
}
unix:!android:!ios: DEFINES += OS_DESKTOP
