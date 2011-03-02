
#include <QShortcut>

#include "WindowMacro.h"

#include "WindowMini.h"
#include "ui_WindowMini.h"

#include <lastfm/XmlQuery>

#include "Radio.h"
#include "Actions.h"

WindowMini::WindowMini( Actions& actions ) :
        unicorn::MainWindow(),
    ui(new Ui::WindowMini),
    m_actions( &actions )
{
    SETUP()

    ui->context->hide();

    new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_M ), this, SLOT(onSwitch()));
}

WindowMini::~WindowMini()
{
    delete ui;
}

void
WindowMini::onPlayClicked( bool checked )
{
    PLAY_CLICKED()
}


void
WindowMini::onLoveClicked( bool loved )
{
    LOVE_CLICKED()
}

void
WindowMini::onLoveTriggered()
{
    LOVE_TRIGGERED()
}

void
WindowMini::onBanClicked()
{
    BAN_CLICKED()
}

void
WindowMini::onBanFinished()
{
    BAN_FINISHED()
}


void
WindowMini::onRadioTick( qint64 tick )
{
    RADIO_TICK()
}


void
WindowMini::onTuningIn( const RadioStation& station )
{
    ON_TUNING_IN()
}


void
WindowMini::onTrackSpooled( const Track& track )
{
    TRACK_SPOOLED()
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
    ui->love->setChecked( m_actions->m_loveAction->isChecked() );
    ui->ban->setChecked( m_actions->m_banAction->isChecked() );
    ui->play->setChecked( m_actions->m_playAction->isChecked() );
    ui->skip->setChecked( m_actions->m_skipAction->isChecked() );
}
