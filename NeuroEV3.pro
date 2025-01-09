QT += core gui network widgets websockets bluetooth serialport svg
CONFIG += c++14

VER_MAJ = 1
VER_MIN = 4
VER_PAT = 2

VERSION = $$VER_MAJ"."$$VER_MIN"."$$VER_PAT
DEFINES += APP_VERSION=$$VERSION
DEFINES += APP_VERSION_STR=\"\\\"$${VERSION}\\\"\" \

SOURCES += \
    com/ComDevice.cpp \
    com/ComProfile.cpp \
    \
    com/ui/ComCommandSelector.cpp \
    com/ui/ComDeviceBiosignalControl.cpp \
    com/ui/ComDeviceManualControl.cpp \
    com/ui/ComDeviceProportionalControl.cpp \
    com/ui/ComDeviceStatusWidget.cpp \
    com/ui/ComProfileWidget.cpp \
    com/ui/ComDeviceMultiplayerControl.cpp \
    \
    classes/Common.cpp \
    classes/Settings.cpp \
    classes/NeuroPlayAppConnector.cpp \
    \
    ev3/EV3.cpp \
    ev3/EV3_Command.cpp \
    ev3/EV3_Motor.cpp \
    \
    ev3/ui/EV3BiosignalStateControl.cpp \
    ev3/ui/EV3ManualControl.cpp \
    ev3/ui/EV3MultiplayerControl.cpp \
    ev3/ui/EV3ProportionalControl.cpp \
    ev3/ui/BaseMotorsWidget.cpp \
    ev3/ui/MotorsCoeffWidget.cpp \
    ev3/ui/MotorsWidget.cpp \
    \
    ui/BiosignalStateCircle.cpp \
    ui/DeviceBiosignalStateControl.cpp \
    ui/DeviceMultiplayerControl.cpp \
    ui/DeviceProportionalControl.cpp \
    ui/GroupBox.cpp \
    ui/IconLabel.cpp \
    ui/ImpovedSlider.cpp \
    ui/LabelWithIcon.cpp \
    ui/MainWindow.cpp \
    ui/UICommon.cpp \
    \
    main.cpp \
    ui/nscrollviewer.cpp \
    ui/nstackpanel.cpp

HEADERS += \
    com/ComDevice.h \
    com/ComProfile.h \
    \
    com/ui/ComCommandSelector.h \
    com/ui/ComDeviceBiosignalControl.h \
    com/ui/ComDeviceManualControl.h \
    com/ui/ComDeviceProportionalControl.h \
    com/ui/ComDeviceStatusWidget.h \
    com/ui/ComProfileWidget.h \
    com/ui/ComDeviceMultiplayerControl.h \
    \
    classes/Common.h \
    classes/Settings.h \
    classes/NeuroPlayAppConnector.h \
    \
    ev3/EV3.h \
    ev3/EV3_Command.h \
    ev3/EV3_Motor.h \
    \
    ev3/ui/EV3BiosignalStateControl.h \
    ev3/ui/EV3ManualControl.h \
    ev3/ui/EV3MultiplayerControl.h \
    ev3/ui/EV3ProportionalControl.h \
    ev3/ui/BaseMotorsWidget.h \
    ev3/ui/MotorsCoeffWidget.h \
    ev3/ui/MotorsWidget.h \
    \
    ui/BiosignalStateCircle.h \
    ui/DeviceBiosignalStateControl.h \
    ui/DeviceMultiplayerControl.h \
    ui/DeviceProportionalControl.h \
    ui/GroupBox.h \
    ui/IconLabel.h \
    ui/ImpovedSlider.h \
    ui/LabelWithIcon.h \
    ui/MainWindow.h \
    ui/UICommon.h \
    ui/nscrollviewer.h \
    ui/nstackpanel.h

CONFIG += force_debug_info

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += resources.qrc

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
