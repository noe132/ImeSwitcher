######################################################################
# Automatically generated by qmake (3.1) Tue Dec 12 17:47:15 2023
######################################################################

TEMPLATE = app
TARGET = ImeSwitcher
INCLUDEPATH += .
LIBS += -framework ApplicationServices
LIBS += -framework Carbon
CONFIG += c++11
# ICON = icons\icon.icns
QT       += core gui widgets
QMAKE_INFO_PLIST = ./Info.plist

# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_UP_TO=0x060000 # disables all APIs deprecated in Qt 6.0.0 and earlier

RESOURCES = app_resources.qrc

# Input
HEADERS += KeyPressHandler.h
SOURCES += main.cpp KeyPressHandler.cpp

CONFIG(debug, debug|release) {
    DESTDIR = build/debug
} else {
    DESTDIR = build/release
}