TARGET = iPodScrobbler
LIBS += -lunicorn -llastfm
QT = core xml sql

macx:CONFIG( app_bundle ) {
    DESTDIR = "$$DESTDIR/The Scrobbler.app/Contents/MacOS"
    QMAKE_POST_LINK += $$ROOT_DIR/admin/dist/mac/bundleFrameworks.sh \"$$DESTDIR/$$TARGET\"
}

CONFIG += lastfm
CONFIG -= app_bundle

include( $$ROOT_DIR/admin/include.qmake )

DEFINES += LASTFM_COLLAPSE_NAMESPACE
SOURCES = main.cpp \
          TwiddlyApplication.cpp \
          PlayCountsDatabase.cpp \
          IPod.cpp \
          $$ROOT_DIR/common/c++/Logger.cpp

HEADERS = TwiddlyApplication.h \
          PlayCountsDatabase.h \
          IPod.h \
          $$ROOT_DIR/common/c++/Logger.h

mac {
    SOURCES += ITunesLibrary_mac.cpp
    HEADERS += ITunesLibrary_mac.h
}

win32 {
    # Would prefer to refer to ITunesTrack.cpp and ITunesComWrapper.cpp in-situ
    # in the ../../plugins/iTunes/ directory, but that triggers bugs in nmake
    # causing it to compile the wrong main.cpp and IPod.cpp!
    # So here we are copying them and their dependencies.
    # Oh, and for some reason, cygwin mutilates their permissions.
    
    system( cp -f ../../plugins/iTunes/ITunesTrack.cpp . )
    system( cp -f ../../plugins/iTunes/ITunesTrack.h . )
    system( cp -f ../../plugins/iTunes/ITunesExceptions.h . )
    system( cp -f ../../plugins/iTunes/ITunesComWrapper.cpp . )
    system( cp -f ../../plugins/iTunes/ITunesComWrapper.h . )
    system( cp -f ../../plugins/iTunes/ITunesEventInterface.h . )
    system( chmod a+r ITunesTrack.cpp )
    system( chmod a+r ITunesTrack.h )
    system( chmod a+r ITunesExceptions.h )
    system( chmod a+r ITunesComWrapper.cpp )
    system( chmod a+r ITunesComWrapper.h )
    system( chmod a+r ITunesEventInterface.h )

    SOURCES += ITunesLibrary_win.cpp \
			   ITunesTrack.cpp \
               ITunesComWrapper.cpp \
               $$ROOT_DIR/plugins/scrobsub/EncodingUtils.cpp \
               $$ROOT_DIR/lib/3rdparty/iTunesCOMAPI/iTunesCOMInterface_i.c

    HEADERS += ITunesTrack.h \
               ITunesComWrapper.h \
               $$ROOT_DIR/plugins/scrobsub/EncodingUtils.h \

    LIBS += -lcomsuppw

    DEFINES += _WIN32_DCOM
    RC_FILE = Twiddly.rc
}
