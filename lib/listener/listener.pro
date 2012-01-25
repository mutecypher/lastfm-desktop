TARGET = listener
TEMPLATE = lib
QT = core xml network
CONFIG += unicorn logger

# basically not easy to support on other platforms, but feel free to fork
linux*:QT += dbus

include( $$ROOT_DIR/admin/include.qmake )

DEFINES += _LISTENER_DLLEXPORT LASTFM_COLLAPSE_NAMESPACE

win32:LIBS += Advapi32.lib User32.lib

SOURCES += \
	PlayerMediator.cpp \
	PlayerListener.cpp \
	PlayerConnection.cpp \
	PlayerCommandParser.cpp \
        legacy/LegacyPlayerListener.cpp \
	DBusListener.cpp

HEADERS += \
	State.h \
	PlayerMediator.h \
	PlayerListener.h \
	PlayerConnection.h \
	PlayerCommandParser.h \
	PlayerCommand.h \
	legacy/LegacyPlayerListener.h \
	DBusListener.h

mac:SOURCES += mac/ITunesListener.cpp \
                mac/SpotifyListener.cpp

mac:HEADERS += mac/ITunesListener.h \
                mac/SpotifyListener.h

win32 {
    SOURCES += win/SpotifyListener.cpp \
               ../../plugins/iTunes/ITunesTrack.cpp \
               ../../plugins/iTunes/ITunesComWrapper.cpp \
               $$ROOT_DIR/plugins/scrobsub/EncodingUtils.cpp \
               $$ROOT_DIR/lib/3rdparty/iTunesCOMAPI/iTunesCOMInterface_i.c

    HEADERS += win/SpotifyListener.h \
               ../../plugins/iTunes/ITunesTrack.h \
               ../../plugins/iTunes/ITunesComWrapper.h \
               ../../plugins/iTunes/ITunesEventInterface.h \
               ../../plugins/iTunes/ITunesExceptions.h \
               $$ROOT_DIR/plugins/scrobsub/EncodingUtils.h

    LIBS += -lcomsuppw

    DEFINES += _WIN32_DCOM
}
