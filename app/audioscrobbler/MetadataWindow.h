/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "lib/unicorn/UnicornMainWindow.h"
#include "lib/unicorn/StylableWidget.h"
#include <lastfm/Track>

class QAbstractButton;
class QTabBar;
class QLabel;
class ScrobbleStatus;
class ScrobbleControls;
class FirstRunWizard;
class MessageBar;
class ProfileWidget;
class SlideOverLayout;
class UserToolButton;

class MetadataWindow : public unicorn::MainWindow
{
    Q_OBJECT

    struct{ 
        FirstRunWizard* firstrun;
        ProfileWidget* profile;
        QWidget* nowScrobbling;
    } stack;

    struct{
        ScrobbleStatus* now_playing_source;
        UserToolButton* userButton;

        QTabBar* tabBar;

        ScrobbleControls* sc;
        MessageBar* message_bar;

        struct {
            QAbstractButton* profile;
            QAbstractButton* nowScrobbling;
        } nav;
    } ui;

    SlideOverLayout* layout;

public:
    MetadataWindow();
    const Track& currentTrack() const{ return m_currentTrack; }
    ScrobbleControls* scrobbleControls() const{ return ui.sc; }

    QObject* nowScrobbling() const { return stack.nowScrobbling;}

    void addWinThumbBarButton( QAction* );

public slots:
    void onTrackStarted(const Track&, const Track&);
    void onStopped();
    void onPaused();
    void onResumed();

private slots:
    void toggleProfile( bool );
    void onTabChanged( int );
    void showNowScrobbling();
    
private:
    Track m_currentTrack;
    QList<QAction*> m_buttons;

    void setCurrentWidget( QWidget* );
    void addWinThumbBarButtons( QList<QAction*>& );

    enum {
        TAB_PROFILE,
        TAB_INFO
    };

};

class TitleBar : public StylableWidget {
Q_OBJECT
public:
    TitleBar(const QString&);

private slots:
    void onConnectionUp();
    void onConnectionDown();
signals:
    void closeClicked();

private:
    QLabel* m_inetStatus;
};
