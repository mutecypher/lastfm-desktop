
#include <QProcess>
#include <QShortcut>
#include <QStatusBar>
#include <QToolBar>

#include "WindowMacro.h"

#include "WindowMini.h"
#include "ui_WindowMini.h"

#include "widgets/TagFilterDialog.h"

#include <lastfm/XmlQuery>

#include "Radio.h"
#include "Actions.h"

WindowMini::WindowMini( Actions& actions ) :
        unicorn::MainWindow(),
    ui(new Ui::WindowMini),
    m_actions( &actions )
{
    SETUP();

    menuWidget()->hide();

    setFixedSize( 500, minimumHeight() );
    ui->context->hide();
}

WindowMini::~WindowMini()
{
    delete ui;
}


void
WindowMini::onSpace()
{
    SPACE();
}

void
WindowMini::onPlayClicked( bool checked )
{
    PLAY_CLICKED();
}

void
WindowMini::onPlayTriggered( bool checked )
{
    PLAY_TRIGGERED();
}

void
WindowMini::onSkipClicked()
{
    SKIP_CLICKED();
}

void
WindowMini::onLoveClicked( bool loved )
{
    LOVE_CLICKED();
}

void
WindowMini::onLoveTriggered( bool loved )
{
    LOVE_TRIGGERED();
}

void
WindowMini::onBanClicked()
{
    BAN_CLICKED();
}

void
WindowMini::onBanFinished()
{
    BAN_FINISHED();
}


void
WindowMini::onRadioTick( qint64 tick )
{
    RADIO_TICK();
}


void
WindowMini::onTuningIn( const RadioStation& station )
{
    ON_TUNING_IN();
}

void
WindowMini::onStopped()
{
    ON_STOPPED();
}

void
WindowMini::onTrackSpooled( const Track& track )
{
    TRACK_SPOOLED();
}

void
WindowMini::onFilterClicked()
{
    ON_FILTER_CLICKED();
}


void
WindowMini::onEditClicked()
{
    ON_EDIT_CLICKED();
}

void
WindowMini::onError(int error, const QVariant& errorText)
{
    ON_ERROR();
}

void
WindowMini::onInfoClicked()
{
#ifdef Q_OS_WIN32
    AllowSetForegroundWindow(ASFW_ANY);
#endif

#ifndef Q_OS_MAC
    QString path = qApp->applicationDirPath() + "/Last.fm Scrobbler";
#ifdef Q_OS_WIN
    path += ".exe";
    path.prepend("\"");
    path.append("\"");
#endif
    QProcess::startDetached( path );
#else
    FSRef appRef;
    LSFindApplicationForInfo( kLSUnknownCreator, CFSTR( "fm.last.audioscrobbler" ), NULL, &appRef, NULL );
    OSStatus status = LSOpenFSRef( &appRef, NULL );
#endif
}

void
WindowMini::onSwitch()
{
    emit aboutToHide();
    hide();
}

void
WindowMini::addWinThumbBarButtons( QList<QAction*>& thumbButtonActions )
{
    thumbButtonActions.append( m_actions->m_loveAction );
    thumbButtonActions.append( m_actions->m_banAction );
    thumbButtonActions.append( m_actions->m_playAction );
    thumbButtonActions.append( m_actions->m_skipAction );
}

void
WindowMini::onActionsChanged()
{
    ON_ACTIONS_CHANGED()
}

void
WindowMini::onGotEvents()
{
    GOT_EVENTS();
}
