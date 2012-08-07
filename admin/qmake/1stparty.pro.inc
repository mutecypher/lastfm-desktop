CONFIG( unicorn ) {
    LIBS += -lunicorn
    QT += gui
    CONFIG += lastfm
}
CONFIG( listener ) {
    LIBS += -llistener
    CONFIG += lastfm
}
CONFIG( fingerprint ) {
    LIBS += -llastfm_fingerprint
    CONFIG += lastfm
}
CONFIG( logger ) {
    LIBS += -llogger
    CONFIG += lastfm
}
CONFIG( lastfm ) {
    QT += core network xml
    LIBS += -llastfm
    win32 {
        CONFIG += link_pkgconfig
        PKGCONFIG += lastfm

    }
    mac{
        MAC_SDK  = $$system(xcode-select --print-path)/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk
        if( !exists( $$MAC_SDK) ) {
            error("The selected Mac OSX SDK does not exist at $$MAC_SDK!")
        }
        macx:QMAKE_MAC_SDK = $$MAC_SDK

        LIBS += -weak_framework Carbon -weak_framework CoreFoundation -weak_framework Foundation
    }
}
