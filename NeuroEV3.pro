QT       += core gui network widgets websockets

CONFIG += c++11

include(../NeuroboticsToolkit/NTCompilation.pri)
include(../NeuroboticsToolkit/NT.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    EV3_Motor.cpp \
    EV3.cpp \
    EV3_Command.cpp \
    MotorsCoeffWidget.cpp \
    MotorsWidget.cpp \
    Settings.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    EV3_ByteCodes.h \
    EV3_Motor.h \
    MainWindow.h \
    EV3.h \
    EV3_Command.h \
    MotorsCoeffWidget.h \
    MotorsWidget.h \
    Settings.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
