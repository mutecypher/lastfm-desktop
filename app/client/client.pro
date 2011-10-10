TEMPLATE = app
TARGET = "Last.fm"
VERSION = 2.0.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
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
    Wizard/LoginPage.cpp \
    Wizard/BootstrapPage.cpp \
    Widgets/TitleBar.cpp \
    Widgets/TagFilterDialog.cpp \
    Widgets/StatusBar.cpp \
    Widgets/SideBar.cpp \
    Settings/SettingsWidget.cpp \
    Settings/ScrobbleSettingsWidget.cpp \
    Widgets/ScrobbleControls.cpp \
    Widgets/ProgressBar.cpp \
    Widgets/QuickStartWidget.cpp \
    Widgets/PointyArrow.cpp \
    Widgets/PlaybackControlsWidget.cpp \
    Widgets/PlayableItemWidget.cpp \
    Widgets/NowPlayingWidget.cpp \
    Settings/IpodSettingsWidget.cpp \
    Widgets/ActivityListWidget.cpp \
    Widgets/RecentTracksWidget.cpp \
    Settings/AccountSettingsWidget.cpp \
    StationSearch.cpp \
    SkipListener.cpp \
    Services/ScrobbleService/StopWatch.cpp \
    Services/ScrobbleService/ScrobbleService.cpp \
    Services/RadioService/RadioService.cpp \
    ScrobSocket.cpp \
    MediaDevices/MediaDevice.cpp \
    MediaDevices/IpodDevice.cpp \
    MediaDevices/DeviceScrobbler.cpp \
    MainWindow.cpp \
    main.cpp \
    Dialogs/ScrobbleSetupDialog.cpp \
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
    ImageTrack.cpp \
    Widgets/ScrobblesWidget.cpp \
    Widgets/RefreshButton.cpp \
    Widgets/BackButton.cpp \
    Widgets/WidgetTextObject.cpp \
    Wizard/FirstRunWizard.cpp \
    Wizard/AccessPage.cpp \
    Wizard/TourMetadataPage.cpp \
    Wizard/PluginsPage.cpp \
    Wizard/TourRadioPage.cpp \
    Wizard/TourFinishPage.cpp \
    Wizard/PluginsInstallPage.cpp \
    Wizard/BootstrapProgressPage.cpp \
    Wizard/TourScrobblesPage.cpp \
    Wizard/TourLocationPage.cpp \
    Settings/PreferencesDialog.cpp \
    Settings/AdvancedSettingsWidget.cpp \
    Settings/GeneralSettingsWidget.cpp

HEADERS += \
    Wizard/LoginPage.h \
    Wizard/FirstRunWizard.h \
    Wizard/BootstrapPage.h \
    Widgets/TitleBar.h \
    Widgets/TagFilterDialog.h \
    Widgets/StatusBar.h \
    Widgets/SideBar.h \
    Settings/SettingsWidget.h \
    Settings/ScrobbleSettingsWidget.h \
    Widgets/ScrobbleControls.h \
    Widgets/ProgressBar.h \
    Widgets/QuickStartWidget.h \
    Widgets/PointyArrow.h \
    Widgets/PlaybackControlsWidget.h \
    Widgets/PlayableItemWidget.h \
    Widgets/NowPlayingWidget.h \
    Settings/IpodSettingsWidget.h \
    Widgets/ActivityListWidget.h \
    Widgets/RecentTracksWidget.h \
    Settings/AccountSettingsWidget.h \
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
    MediaDevices/MediaDevice.h \
    MediaDevices/IpodDevice.h \
    MediaDevices/DeviceScrobbler.h \
    MainWindow.h \
    Dialogs/ScrobbleSetupDialog.h \
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
    ImageTrack.h \
    Widgets/ScrobblesWidget.h \
    Widgets/RefreshButton.h \
    Widgets/BackButton.h \
    Widgets/WidgetTextObject.h \
    Wizard/AccessPage.h \
    Wizard/TourMetadataPage.h \
    Wizard/PluginsPage.h \
    Wizard/TourRadioPage.h \
    Wizard/TourFinishPage.h \
    Wizard/PluginsInstallPage.h \
    Wizard/BootstrapProgressPage.h \
    Wizard/TourScrobblesPage.h \
    Wizard/TourLocationPage.h \
    Settings/PreferencesDialog.h \
    Settings/AdvancedSettingsWidget.h \
    Settings/GeneralSettingsWidget.h

mac:SOURCES += Services/ITunesPluginInstaller/ITunesPluginInstaller_mac.cpp

FORMS += \
    Widgets/TagFilterDialog.ui \
    Widgets/PlaybackControlsWidget.ui \
    Dialogs/ScrobbleSetupDialog.ui \
    Dialogs/DiagnosticsDialog.ui \
    Widgets/MetadataWidget.ui \
    Settings/PreferencesDialog.ui

RESOURCES += \
    qrc/audioscrobbler.qrc































































