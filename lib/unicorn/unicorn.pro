TARGET = unicorn
TEMPLATE = lib
CONFIG += dll lastfm sparkle
QT = core gui xml network
include( $$ROOT_DIR/admin/include.qmake )
DEFINES += _UNICORN_DLLEXPORT LASTFM_COLLAPSE_NAMESPACE

# UniqueApplication
win32:LIBS += user32.lib shell32.lib ole32.lib

macx:LIBS += -framework Cocoa

SOURCES += \
        $$ROOT_DIR/common/c++/Logger.cpp \
        dialogs/ShareDialog.cpp \
        widgets/AvatarWidget.cpp \
        layouts/FlowLayout.cpp \
        widgets/UserMenu.cpp \
        widgets/SlidingStackedWidget.cpp \
	widgets/UserToolButton.cpp \
	widgets/UserManagerWidget.cpp \
	widgets/UnicornTabWidget.cpp \
	widgets/TrackWidget.cpp \
	widgets/TagListWidget.cpp \
	widgets/TagBuckets.cpp \
	widgets/StatusLight.cpp \
	widgets/SearchBox.cpp \
	widgets/MessageBar.cpp \
	widgets/LfmListViewWidget.cpp \
	widgets/Label.cpp \
	widgets/ItemSelectorWidget.cpp \
	widgets/ImageButton.cpp \
	widgets/HttpImageWidget.cpp \
	widgets/GhostWidget.cpp \
	widgets/FriendsPicker.cpp \
	widgets/DataListWidget.cpp \
	widgets/BannerWidget.cpp \
	widgets/ActionButton.cpp \
	Updater/PluginList.cpp \
	Updater/IPluginInfo.cpp \
	UpdateInfoFetcher.cpp \
	UnicornSettings.cpp \
	UnicornSession.cpp \
	UnicornMainWindow.cpp \
	UnicornCoreApplication.cpp \
	UnicornApplication.cpp \
	TrackImageFetcher.cpp \
        ScrobblesModel.cpp \
	qtwin.cpp \
	qtsingleapplication/qtsinglecoreapplication.cpp \
	qtsingleapplication/qtsingleapplication.cpp \
	qtsingleapplication/qtlockedfile_unix.cpp \
	qtsingleapplication/qtlockedfile.cpp \
	qtsingleapplication/qtlocalpeer.cpp \
	QMessageBoxBuilder.cpp \
        LoginProcess.cpp \
	layouts/SlideOverLayout.cpp \
	layouts/SideBySideLayout.cpp \
	layouts/AnimatedListLayout.cpp \
	dialogs/UserManagerDialog.cpp \
	dialogs/UpdateDialog.cpp \
	dialogs/TagDialog.cpp \
	dialogs/LoginDialog.cpp \
	dialogs/LoginContinueDialog.cpp \
	dialogs/AboutDialog.cpp \
        dialogs/ScrobbleConfirmationDialog.cpp \
	AnimatedStatusBar.cpp \
    Updater/Updater.mm

HEADERS += \
	widgets/UserToolButton.h \
	widgets/UserMenu.h \
	widgets/UserManagerWidget.h \
	widgets/UserComboSelector.h \
	widgets/UnicornTabWidget.h \
	widgets/TrackWidget.h \
	widgets/TagListWidget.h \
	widgets/TagBuckets.h \
	widgets/StatusLight.h \
	widgets/SpinnerLabel.h \
	widgets/Seed.h \
	widgets/SearchBox.h \
	widgets/PlayableMimeData.h \
	widgets/MessageBar.h \
	widgets/LfmListViewWidget.h \
	widgets/Label.h \
	widgets/ItemSelectorWidget.h \
	widgets/ImageButton.h \
	widgets/HttpImageWidget.h \
	widgets/GhostWidget.h \
	widgets/FriendsPicker.h \
	widgets/DataListWidget.h \
	widgets/DataBox.h \
	widgets/BannerWidget.h \
	widgets/ActionButton.h \
	Updater/PluginList.h \
	Updater/KillProcess.h \
	Updater/IPluginInfo.h \
	Updater/gen_PluginList.h \
	UpdateInfoFetcher.h \
	UnicornSettings.h \
	UnicornSession.h \
	UnicornMainWindow.h \
	UnicornCoreApplication.h \
	UnicornApplication.h \
	TrackImageFetcher.h \
	StylableWidget.h \
	SignalBlocker.h \
        ScrobblesModel.h \
	qtwin.h \
	qtsingleapplication/qtsinglecoreapplication.h \
	qtsingleapplication/qtsingleapplication.h \
	qtsingleapplication/qtlockedfile.h \
	qtsingleapplication/qtlocalpeer.h \
	QMessageBoxBuilder.h \
	PlayBus.h \
	LoginProcess.h \
	layouts/SlideOverLayout.h \
	layouts/SideBySideLayout.h \
	layouts/AnimatedListLayout.h \
	dialogs/WelcomeDialog.h \
	dialogs/UserManagerDialog.h \
	dialogs/UpdateDialog.h \
	dialogs/UnicornDialog.h \
	dialogs/TagDialog.h \
	dialogs/LoginDialog.h \
	dialogs/LoginContinueDialog.h \
	dialogs/AboutDialog.h \
        dialogs/ScrobbleConfirmationDialog.h \
	AnimatedStatusBar.h \
	AnimatedPushButton.h \
    dialogs/ShareDialog.h \
    widgets/AvatarWidget.h \
    layouts/FlowLayout.h \
    widgets/SlidingStackedWidget.h \
    Updater/Updater.h
	
win32:SOURCES += qtsingleapplication/qtlockedfile_win.cpp
	

mac:SOURCES += mac/AppleScript.cpp \
               UnicornApplication_mac.mm

mac:HEADERS += mac/AppleScript.h

FORMS += \
	dialogs/ShareDialog.ui \
	dialogs/LoginDialog.ui

RESOURCES += \
	qrc/unicorn.qrc









