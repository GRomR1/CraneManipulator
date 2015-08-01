#-------------------------------------------------
#
# Project created by QtCreator 2014-12-02T11:44:52
#
#-------------------------------------------------

QT       += core gui bluetooth svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CraneManipulator
TEMPLATE = app


SOURCES += main.cpp \
    BluetoothClient.cpp \
    FindDevices.cpp \
    SavedOptionsInterface.cpp

HEADERS  += \
    ../Defines.h \
    BluetoothClient.h \
    FindDevices.h \
    SavedOptionsInterface.h \
    MyWake.h \
    MyWakePort.h

CONFIG += mobility
MOBILITY = 

FORMS += \
    BluetoothClient.ui \
    FindDevices.ui

RESOURCES += \
    resources.qrc

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES += \
    android/AndroidManifest.xml \
    README.md

