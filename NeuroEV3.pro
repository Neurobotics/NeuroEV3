QT += core gui network widgets websockets bluetooth serialport
CONFIG += c++14

VER_MAJ = 1
VER_MIN = 3
VER_PAT = 1

VERSION = $$VER_MAJ"."$$VER_MIN"."$$VER_PAT
DEFINES += APP_VERSION=$$VERSION
DEFINES += APP_VERSION_STR=\"\\\"$${VERSION}\\\"\" \

SOURCES += \
    com/ComDevice.cpp \
    com/ComDeviceControl.cpp \
    com/ComDeviceMentalControl.cpp \
    com/ComDeviceProportionalControl.cpp \
    com/ComDeviceStatusWidget.cpp \
    com/ComProfile.cpp \
    com/ComProfileWidget.cpp \
    classes/NeuroPlayAppConnector.cpp \
    ev3/ui/EV3ProportionalControl.cpp \
    main.cpp \
    classes/Common.cpp \
    classes/Settings.cpp \
    ev3/EV3.cpp \
    ev3/EV3_Command.cpp \
    ev3/EV3_Motor.cpp \
    ui/BaseMotorsWidget.cpp \
    ui/DeviceProportionalControl.cpp \
    ui/IconLabel.cpp \
    ui/MotorsCoeffWidget.cpp \
    ui/MotorsWidget.cpp \
    ui/MainWindow.cpp \
    ui/UICommon.cpp

HEADERS += \
    com/ComDevice.h \
    com/ComDeviceControl.h \
    com/ComDeviceMentalControl.h \
    com/ComDeviceProportionalControl.h \
    com/ComDeviceStatusWidget.h \
    com/ComProfile.h \
    com/ComProfileWidget.h \
    classes/Common.h \
    classes/NeuroPlayAppConnector.h \
    classes/Settings.h \
    ev3/EV3.h \
    ev3/EV3_Command.h \
    ev3/EV3_ByteCodes.h \
    ev3/EV3_Motor.h \
    ev3/ui/EV3ProportionalControl.h \
    ui/BaseMotorsWidget.h \
    ui/DeviceProportionalControl.h \
    ui/IconLabel.h \
    ui/MainWindow.h \
    ui/MotorsCoeffWidget.h \
    ui/MotorsWidget.h \
    ui/UICommon.h


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

    QMAKE_FULL_VERSION = $$VERSION
    QMAKE_SHORT_VERSION = $$VERSION
    QMAKE_TARGET_BUNDLE_PREFIX = neurobotics

    QMAKE_INFO_PLIST = $$PWD/platforms/mac/Info.plist
    QMAKE_ASSET_CATALOGS += $$PWD/platforms/mac/Images.xcassets
}
unix:!android:!ios: DEFINES += OS_DESKTOP

TRANSLATIONS = resources/translations/NeuroEV3_ru-RU.ts resources/translations/NeuroEV3_en-US.ts

DISTFILES += \
    ChangeLog.txt
