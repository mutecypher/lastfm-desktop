TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = lib/unicorn \
          lib/listener \
          app/audioscrobbler \
          app/radio \
          app/twiddly
          #app/boffin

linux*:SUBDIRS -= app/twiddly
mac:SUBDIRS += app/audioscrobbler/PrefPane

CONFIG( tests ) {
    SUBDIRS += \
        lib/lastfm/core/tests/test_libcore.pro \
        lib/lastfm/types/tests/test_libtypes.pro \
        lib/lastfm/scrobble/tests/test_libscrobble.pro \
        lib/listener/tests/test_liblistener.pro \
        app/client/tests/test_client.pro
}

