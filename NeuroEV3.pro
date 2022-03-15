QT       += core gui network widgets

CONFIG += c++11

include(../NeuroboticsToolkit/NTCompilation.pri)
include(../NeuroboticsToolkit/NT.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ev3.cpp \
    ev3command.cpp \
    ev3motor.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    MainWindow.h \
    bytecodes.h \
    ev3.h \
    ev3command.h \
    ev3motor.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
