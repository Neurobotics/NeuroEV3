cmake_minimum_required(VERSION 3.5)
project(NeuroEV3 VERSION 1.3.0 LANGUAGES C CXX)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
cmake_policy(SET CMP0071 NEW)

find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Qml)
qt6_policy(SET QTP0001 NEW)

#add_compile_definitions(NEUROPLAYPRO)


set(PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/classes/NeuroPlayAppConnector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/classes/Common.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/classes/Settings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ev3/EV3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ev3/EV3_Command.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ev3/EV3_Motor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/BaseMotorsWidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/IconLabel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/MotorsCoeffWidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/MotorsWidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/MainWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/classes/Common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/classes/NeuroPlayAppConnector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/classes/Settings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ev3/EV3.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ev3/EV3_Command.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ev3/EV3_ByteCodes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ev3/EV3_Motor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/BaseMotorsWidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/IconLabel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/MainWindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/MotorsCoeffWidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/MotorsWidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc
)



add_compile_definitions(APP_VERSION=${CMAKE_PROJECT_VERSION})
add_compile_definitions(APP_VERSION_STR="${CMAKE_PROJECT_VERSION}")
add_compile_definitions(TARGET_NAME="${CMAKE_PROJECT_NAME}")
add_compile_definitions(IS_X64)

if(ANDROID)
#    include_directories(${CMAKE_PREFIX_PATH}/include/QtZlib)
#    add_compile_definitions(NT_OS_MOBILE)

    set(ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/platforms/android CACHE INTERNAL "")
    set(PROJECT_SOURCES
        ${PROJECT_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/platforms/android/AndroidManifest.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/../NeuroboticsToolkit/helpers/platforms/android/androidHelper.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../NeuroboticsToolkit/helpers/platforms/android/androidHelper.cpp
    )
    qt_add_executable(${CMAKE_PROJECT_NAME} WIN32 MACOSX_BUNDLE ${PROJECT_SOURCES})
    set(ANDROID_MIN_SDK_VERSION 33)
#    include(C:/Users/konys/AppData/Local/Android/Sdk/android_openssl/CMakeLists.txt)
else()
    if(WIN32)
        string(TIMESTAMP CURRENT_YEAR "%Y")
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_product_version.cmake)
        message("{CMAKE_PROJECT_NAME} " ${CMAKE_PROJECT_NAME})
        generate_product_version(
           VersionFilesOutputVariable
           NAME ${CMAKE_PROJECT_NAME}
           ICON ${CMAKE_CURRENT_SOURCE_DIR}/resources/neuroEV3.ico
           VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR}
           VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR}
           VERSION_PATCH ${CMAKE_PROJECT_VERSION_PATCH}
           VERSION_REVISION ${CMAKE_PROJECT_VERSION_TWEAK}
           COMPANY_NAME "Neurobotics LLC"
           COMPANY_COPYRIGHT "(C) Copyright ${CURRENT_YEAR} Neurobotics LLC"
        )
    endif()

    qt_add_executable(${CMAKE_PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${VersionFilesOutputVariable}
    )
endif()

set(QT_PACKAGES Core Core5Compat Bluetooth Widgets WebSockets Xml Network Svg)

foreach(X IN LISTS QT_PACKAGES)
    find_package(Qt6 REQUIRED COMPONENTS ${X})
    message("NEED PACKAGE " Qt::${X})
    set(QT_COMPONENTS_IMP ${QT_COMPONENTS_IMP} Qt::${X})
endforeach()


target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    PUBLIC
    ${QT_COMPONENTS_IMP}
)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/platforms/android
)
include(GNUInstallDirs)
install(TARGETS ${CMAKE_PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
