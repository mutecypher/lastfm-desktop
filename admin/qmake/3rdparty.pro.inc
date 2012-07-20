mac {
    LIBS += -L/usr/local/lib
    INCLUDEPATH += /usr/local/include
}

CONFIG( taglib ) {
    CONFIG += link_pkgconfig
    PKGCONFIG += taglib
    win32:debug:LIBS += tagd.lib    # tagd.lib will precede PKGCONFIG added libs
}

CONFIG( mad ) {
    mac{
                LIBS += /usr/local/lib/libmad.a
	}else{
        CONFIG += link_pkgconfig
        PKGCONFIG += mad
    } 
}

CONFIG( faad ) {
    mac {
                LIBS += /usr/local/lib/libfaad.a
                LIBS += /usr/local/lib/libmp4ff.a
                DEFINES += HAVE_STDINT_H
        }else{
        CONFIG += link_pkgconfig
        PKGCONFIG += faad2
        PKGCONFIG += mp4ff
    }
}

CONFIG( vorbis ) {
    mac{
                LIBS += /usr/local/lib/libvorbis.a
                LIBS += /usr/local/lib/libvorbisfile.a
                LIBS += /usr/local/lib/libogg.a
        }else{
        CONFIG += link_pkgconfig
        PKGCONFIG += vorbis
    }
}

CONFIG( flac ) {
    mac{
                LIBS += /usr/local/lib/libFLAC.a
        }else{
        CONFIG += link_pkgconfig
        PKGCONFIG += flac
    }
}

CONFIG( boost ) {
    mac:CONFIG(app_bundle){
        LIBS += /usr/local/lib/libboost_thread-mt.a
    }
    else:unix {
        LIBS += -lboost_thread-mt
    }
}

CONFIG( break ) {
    win32{
        CONFIG += link_pkgconfig
        PKGCONFIG += breakpad
        LIBS += wininet.lib
    }
    else:mac{
        LIBS += -framework Breakpad
    }
}

CONFIG( fftw3f ) {
    mac{
                LIBS += /usr/local/lib/libfftw3f.a
	}else{
        CONFIG += link_pkgconfig
        PKGCONFIG += fftw3f
    }
}

CONFIG( growl ) {
    mac {
        LIBS += -framework Growl
    }
}

CONFIG( phonon ) {
    mac:exists( /usr/local/Cellar/kde-phonon/4.6.0/lib/libphonon.dylib ) {
        # Prefer to use the specific phonon we build ourselves than the old one ditributed with Qt
        LIBS += /usr/local/Cellar/kde-phonon/4.6.0/lib/libphonon.dylib
    }
    else:win32 {
        CONFIG += link_pkgconfig
        PKGCONFIG += phonon
    }
	else {
		QT += phonon
	}
}

CONFIG( samplerate ) {
    win32{
		LIBS += -L$$COMMON_LIBS_DIR/libsamplerate -lsamplerate
	}else:mac:CONFIG( app_bundle ){
                LIBS += libsamplerate.a -L/usr/local/lib
	}else{
        CONFIG += link_pkgconfig
        PKGCONFIG += samplerate
    }
}

CONFIG( sparkle ) {
    win32{
        CONFIG += link_pkgconfig
        PKGCONFIG += sparkle
    }
    else:mac{
        LIBS += -framework Sparkle
    }
}

CONFIG( sqlite3 ) {
    win32 {
        CONFIG += link_pkgconfig
        PKGCONFIG += sqlite3
    }
    else:LIBS += -lsqlite3
}

CONFIG( yajl ) {
	win32 {
		CONFIG += link_pkgconfig
		debug:PKGCONFIG += yajl-debug
		release:PKGCONFIG += yajl-release
	}
	else:CONFIG(app_bundle) {
                LIBS += /usr/local/lib/libyajl_s.a
	}
	else:LIBS += -lyajl
}

