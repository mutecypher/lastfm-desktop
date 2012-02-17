TARGET = logger
TEMPLATE = lib
QT -= gui
CONFIG += dll

include( ../../admin/include.qmake )

DEFINES += _LOGGER_DLLEXPORT LASTFM_COLLAPSE_NAMESPACE

SOURCES += $$ROOT_DIR/common/c++/Logger.cpp

HEADERS += $$ROOT_DIR/common/c++/Logger.h



