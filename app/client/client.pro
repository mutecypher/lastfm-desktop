TEMPLATE = app
TARGET = "Last.fm"
VERSION = 2.0.0
QT = core gui xml network sql phonon
CONFIG += lastfm unicorn listener
win32:LIBS += user32.lib
DEFINES += LASTFM_COLLAPSE_NAMESPACE

macx:LIBS += -framework Cocoa
win32:release {
	LIBS += -lAdvAPI32
}

include( $$ROOT_DIR/admin/include.qmake )
CONFIG -= silent
DEFINES += LASTFM_COLLAPSE_NAMESPACE
SOURCES -= LegacyTuner.cpp
HEADERS -= LegacyTuner.h

macx:ICON = ./audioscrobbler.icns
macx:LIBS += -lz
win32:LIBS += shell32.lib User32.lib

RC_FILE = audioscrobbler.rc
linux* {
        CONFIG += link_pkgconfig
        PKGCONFIG += libgpod-1.0
}

SUBDIRS=PrefPane

SOURCES += \
    Wizard/WelcomePage.cpp \
    Wizard/SystemTrayPage.cpp \
    Wizard/PluginPage.cpp \
    Wizard/LoginPage.cpp \
    Wizard/BootstrapPage.cpp \
    Wizard/BootstrapInProgressPage.cpp \
    Wizard/AuthInProgressPage.cpp \
    Widgets/TitleBar.cpp \
    Widgets/TagFilterDialog.cpp \
    Widgets/StatusBar.cpp \
    Widgets/SideBar.cpp \
    Widgets/SettingsWidget.cpp \
    Widgets/ScrobbleSettingsWidget.cpp \
    Widgets/ScrobbleControls.cpp \
    Widgets/ProgressBar.cpp \
    Widgets/QuickStartWidget.cpp \
    Widgets/PointyArrow.cpp \
    Widgets/PlaybackControlsWidget.cpp \
    Widgets/PlayableItemWidget.cpp \
    Widgets/NowPlayingWidget.cpp \
    Widgets/KeyboardSettingsWidget.cpp \
    Widgets/IpodSettingsWidget.cpp \
    Widgets/IPodScrobbleInfoWidget.cpp \
    Widgets/ActivityListWidget.cpp \
    Widgets/RecentTracksWidget.cpp \
    Widgets/AccountSettingsWidget.cpp \
    StationSearch.cpp \
    SkipListener.cpp \
    Services/ScrobbleService/StopWatch.cpp \
    Services/ScrobbleService/ScrobbleService.cpp \
    Services/RadioService/RadioService.cpp \
    ScrobSocket.cpp \
    ScrobblesModel.cpp \
    MediaDevices/MediaDevice.cpp \
    MediaDevices/IpodDevice.cpp \
    MediaDevices/DeviceScrobbler.cpp \
    MainWindow.cpp \
    main.cpp \
    Dialogs/SettingsDialog.cpp \
    Dialogs/ScrobbleSetupDialog.cpp \
    Dialogs/ScrobbleConfirmationDialog.cpp \
    Dialogs/DiagnosticsDialog.cpp \
    Bootstrapper/PluginBootstrapper.cpp \
    Bootstrapper/ITunesDevice/itunesdevice.cpp \
    Bootstrapper/iTunesBootstrapper.cpp \
    Bootstrapper/AbstractFileBootstrapper.cpp \
    Bootstrapper/AbstractBootstrapper.cpp \
    AudioscrobblerSettings.cpp \
    Application.cpp \
    ActivityListModel.cpp \
    Widgets/RadioWidget.cpp \
    Services/RadioService/RadioConnection.cpp \
    Widgets/NothingPlayingWidget.cpp \
    Widgets/NowPlayingStackedWidget.cpp \
    Widgets/ProfileWidget.cpp \
    Widgets/FriendListWidget.cpp \
    Widgets/FriendWidget.cpp \
    Widgets/BioWidget.cpp \
    Widgets/MetadataWidget.cpp \
    Widgets/TagWidget.cpp \
    Widgets/CheckableDelegate.cpp \
    Widgets/TrackDelegate.cpp \
    ImageTrack.cpp

HEADERS += \
    Wizard/WelcomePage.h \
    Wizard/SystemTrayPage.h \
    Wizard/PluginPage.h \
    Wizard/LoginPage.h \
    Wizard/IntroPage.h \
    Wizard/FirstRunWizard.h \
    Wizard/BootstrapPage.h \
    Wizard/BootstrapInProgressPage.h \
    Wizard/AuthInProgressPage.h \
    Widgets/TitleBar.h \
    Widgets/TagFilterDialog.h \
    Widgets/StatusBar.h \
    Widgets/SideBar.h \
    Widgets/SettingsWidget.h \
    Widgets/ScrobbleSettingsWidget.h \
    Widgets/ScrobbleControls.h \
    Widgets/ProgressBar.h \
    Widgets/QuickStartWidget.h \
    Widgets/PointyArrow.h \
    Widgets/PlaybackControlsWidget.h \
    Widgets/PlayableItemWidget.h \
    Widgets/NowPlayingWidget.h \
    Widgets/KeyboardSettingsWidget.h \
    Widgets/IpodSettingsWidget.h \
    Widgets/IPodScrobbleInfoWidget.h \
    Widgets/ActivityListWidget.h \
    Widgets/RecentTracksWidget.h \
    Widgets/AccountSettingsWidget.h \
    StationSearch.h \
    SkipListener.h \
    Services/ScrobbleService.h \
    Services/ScrobbleService/StopWatch.h \
    Services/ScrobbleService/ScrobbleService.h \
    Services/RadioService.h \
    Services/RadioService/RadioService.h \
    Services/ITunesPluginInstaller.h \
    Services/ITunesPluginInstaller/ITunesPluginInstaller.h \
    ScrobSocket.h \
    ScrobblesModel.h \
    MediaDevices/MediaDevice.h \
    MediaDevices/IpodDevice.h \
    MediaDevices/DeviceScrobbler.h \
    MainWindow.h \
    Dialogs/SettingsDialog.h \
    Dialogs/ScrobbleSetupDialog.h \
    Dialogs/ScrobbleConfirmationDialog.h \
    Dialogs/DiagnosticsDialog.h \
    Bootstrapper/PluginBootstrapper.h \
    Bootstrapper/ITunesDevice/MediaDeviceInterface.h \
    Bootstrapper/ITunesDevice/ITunesParser.h \
    Bootstrapper/ITunesDevice/itunesdevice.h \
    Bootstrapper/iTunesBootstrapper.h \
    Bootstrapper/AbstractFileBootstrapper.h \
    Bootstrapper/AbstractBootstrapper.h \
    AudioscrobblerSettings.h \
    Application.h \
    ActivityListModel.h \
    Widgets/RadioWidget.h \
    Services/RadioService/RadioConnection.h \
    Widgets/NothingPlayingWidget.h \
    Widgets/NowPlayingStackedWidget.h \
    Widgets/ProfileWidget.h \
    Widgets/FriendListWidget.h \
    Widgets/FriendWidget.h \
    Widgets/BioWidget.h \
    Widgets/MetadataWidget.h \
    Widgets/TagWidget.h \
    Widgets/CheckableDelegate.h \
    Widgets/TrackDelegate.h \
    ImageTrack.h

mac:SOURCES += Services/ITunesPluginInstaller/ITunesPluginInstaller_mac.cpp

FORMS += \
    Widgets/TagFilterDialog.ui \
    Widgets/PlaybackControlsWidget.ui \
    Dialogs/ScrobbleSetupDialog.ui \
    Dialogs/DiagnosticsDialog.ui \
    Widgets/MetadataWidget.ui

RESOURCES += \
    qrc/audioscrobbler.qrc
