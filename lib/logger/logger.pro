#-------------------------------------------------
#
# Project created by QtCreator 2012-01-20T11:05:56
#
#-------------------------------------------------

QT -= gui
TEMPLATE = lib
CONFIG += dll
include( $$ROOT_DIR/admin/include.qmake )
DEFINES += _LOGGER_DLLEXPORT LASTFM_COLLAPSE_NAMESPACE

TARGET = logger


SOURCES += $$ROOT_DIR/common/c++/Logger.cpp

HEADERS += $$ROOT_DIR/common/c++/Logger.h



