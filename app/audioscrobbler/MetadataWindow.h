#ifndef METADATA_WINDOW_H_
#define METADATA_WINDOW_H_

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
    void onIPodDetected();
    void onProcessingScrobbles();
    void onScrobblesFound();

signals:
    void closeClicked();

private:
    QLabel* m_inetStatus;
    QLabel* m_iPodStatus;
};

#endif //METADATA_WINDOW_H_

