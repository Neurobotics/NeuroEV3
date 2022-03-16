QT += core gui network widgets websockets
CONFIG += c++14

SOURCES += \
    EV3.cpp \
    EV3_Command.cpp \
    EV3_Motor.cpp \
    MotorsCoeffWidget.cpp \
    MotorsWidget.cpp \
    Settings.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    EV3.h \
    EV3_Command.h \
    EV3_ByteCodes.h \
    EV3_Motor.h \
    MainWindow.h \
    MotorsCoeffWidget.h \
    MotorsWidget.h \
    Settings.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

win32: RC_ICONS += resources/neuroEV3.ico

win32: DEFINES += OS_DESKTOP
macx: DEFINES += OS_DESKTOP
unix:!android:!ios: DEFINES += OS_DESKTOP
