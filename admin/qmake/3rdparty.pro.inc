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
    win32{
		LIBS += -L$$COMMON_LIBS_DIR/libmad -lmad
	}else:CONFIG( app_bundle ){
                LIBS += /usr/local/lib/libmad.a
	}else{
        CONFIG += link_pkgconfig
        PKGCONFIG += mad
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
    win32{
		LIBS += -L$$COMMON_LIBS_DIR/libfftw -lfftw3f
	}else:CONFIG( app_bundle ){
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
        LIBS += -L$$COMMON_LIBS_DIR/WinSparkle -lWinSparkle
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

