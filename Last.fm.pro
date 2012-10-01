TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = lib/logger \
          lib/unicorn \
          lib/listener \
          app/client \
          app/twiddly 
          #app/boffin

unix:!mac:SUBDIRS -= app/twiddly

CONFIG( tests ) {
    SUBDIRS += \
        lib/lastfm/core/tests/test_libcore.pro \
        lib/lastfm/types/tests/test_libtypes.pro \
        lib/lastfm/scrobble/tests/test_libscrobble.pro \
        lib/listener/tests/test_liblistener.pro
}

TRANSLATIONS = i18n/lastfm_de.ts \
                i18n/lastfm_en.ts \
                i18n/lastfm_es.ts \
                i18n/lastfm_fr.ts \
                i18n/lastfm_it.ts \
                i18n/lastfm_ja.ts \
                i18n/lastfm_pl.ts \
                i18n/lastfm_pt.ts \
                i18n/lastfm_ru.ts \
                i18n/lastfm_sv.ts \
                i18n/lastfm_tr.ts \
                i18n/lastfm_zh_CN.ts

