QT += core
QT -= gui

CONFIG += c++11

TARGET = gx_uuid
CONFIG += console
CONFIG -= app_bundle
macx {
    CONFIG += app_bundle
}

TEMPLATE = app

SOURCES += main.cpp
