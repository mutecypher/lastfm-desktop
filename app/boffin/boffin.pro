CONFIG += unicorn boost yajl
QT += opengl sql phonon
VERSION = 1.0.0
DEFINES += LASTFM_COLLAPSE_NAMESPACE

include( $$ROOT_DIR/admin/include.qmake )

mac{
    release{
        QMAKE_INFO_PLIST = Info.plist.in
        system( $$ROOT_DIR/admin/dist/mac/Makefile.dmg.pl $$LIBS > Makefile.dmg )
        QMAKE_EXTRA_INCLUDES += Makefile.dmg
        ICON = ../client/mac/client.icns
        CONFIG += app_bundle
    }
}

SOURCES += \
	XspfReader.cpp \
	XspfDialog.cpp \
	TrackSource.cpp \
	TagDelegate.cpp \
	TagCloudView.cpp \
	TagBrowserWidget.cpp \
	Shuffler.cpp \
	ScrobSocket.cpp \
	ScanProgressWidget.cpp \
	PlaylistModel.cpp \
	Playlist.cpp \
	PlaydarTagCloudModel.cpp \
	PlaydarHostsModel.cpp \
	playdar/TrackResolveRequest.cpp \
	playdar/PlaydarStatRequest.cpp \
	playdar/PlaydarRosterRequest.cpp \
	playdar/PlaydarConnection.cpp \
	playdar/PlaydarCometRequest.cpp \
	playdar/PlaydarAuthRequest.cpp \
	playdar/jsonGetMember.cpp \
	playdar/CometRequest.cpp \
	playdar/BoffinTagRequest.cpp \
	playdar/BoffinRqlRequest.cpp \
	playdar/BoffinPlayableItem.cpp \
	PickDirsDialog.cpp \
	MediaPipeline.cpp \
	MainWindow.cpp \
	main.cpp \
	LocalCollectionScanner.cpp \
	layouts/SideBySideLayout.cpp \
	json_spirit/json_spirit_writer.cpp \
	json_spirit/json_spirit_value.cpp \
	json_spirit/json_spirit_reader.cpp \
	HistoryWidget.cpp \
	comet/CometParser.cpp \
	App.cpp
    
HEADERS += \
	XspfReader.h \
	XspfDialog.h \
	WordleDialog.h \
	TrackSource.h \
	TagDelegate.h \
	TagCloudView.h \
	TagBrowserWidget.h \
	Shuffler.h \
	ScrobSocket.h \
	ScanProgressWidget.h \
	sample/SampleFromDistribution.h \
	PlaylistWidget.h \
	PlaylistModel.h \
	Playlist.h \
	PlaydarTagCloudModel.h \
	PlaydarHostsModel.h \
	playdar/TrackResolveRequest.h \
	playdar/PlaydarStatRequest.h \
	playdar/PlaydarRosterRequest.h \
	playdar/PlaydarConnection.h \
	playdar/PlaydarCometRequest.h \
	playdar/PlaydarAuthRequest.h \
	playdar/PlaydarApi.h \
	playdar/jsonGetMember.h \
	playdar/CometRequest.h \
	playdar/BoffinTagRequest.h \
	playdar/BoffinRqlRequest.h \
	playdar/BoffinPlayableItem.h \
	PickDirsDialog.h \
	MediaPipeline.h \
	MainWindow.h \
	LocalCollectionScanner.h \
	layouts/SideBySideLayout.h \
	json_spirit/json_spirit_writer.h \
	json_spirit/json_spirit_value.h \
	json_spirit/json_spirit_utils.h \
	json_spirit/json_spirit_reader.h \
	json_spirit/json_spirit.h \
	HistoryWidget.h \
	comet/CometParser.h \
	App.h
    
RESOURCES += \
	qrc/boffin.qrc
