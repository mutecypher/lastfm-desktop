
#include <QAction>
#include "Actions.h"
#include "Radio.h"

Actions::Actions()
    :QObject()
{
    {
        m_loveAction = new QAction( tr( "Love" ), this );
        m_loveAction->setCheckable( true );
        QIcon loveIcon;
        loveIcon.addFile( ":/love-isloved.png", QSize(), QIcon::Normal, QIcon::On );
        loveIcon.addFile( ":/love-rest.png", QSize(), QIcon::Normal, QIcon::Off );
        m_loveAction->setIcon( loveIcon );

        m_loveAction->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_L ) );
    }
    {
        m_banAction = new QAction( tr( "Ban" ), this );
        QIcon banIcon;
        banIcon.addFile( ":/ban-rest.png" );
        m_banAction->setIcon( banIcon );

        m_banAction->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_B ) );
    }
    {
        m_playAction = new QAction( tr( "Play" ), this );
        m_playAction->setCheckable( true );
        QIcon playIcon;
        playIcon.addFile( ":/pause-rest.png", QSize(), QIcon::Normal, QIcon::On );
        playIcon.addFile( ":/play-rest.png", QSize(), QIcon::Normal, QIcon::Off );
        m_playAction->setIcon( playIcon );

        m_playAction->setShortcut( QKeySequence(Qt::Key_Space) );
    }
    {
        m_skipAction = new QAction( tr( "Skip" ), this );
        QIcon skipIcon;
        skipIcon.addFile( ":/skip-rest.png" );
        m_skipAction->setIcon( skipIcon );

        m_skipAction->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_Right ) );
    }
}

void
Actions::doConnect( QObject* object )
{
    connect( m_loveAction, SIGNAL(triggered(bool)), object, SLOT(onLoveClicked(bool)) );
    connect( m_loveAction, SIGNAL(changed()), object, SLOT(onActionsChanged()) );

    connect( m_banAction, SIGNAL(triggered(bool)), object, SLOT(onBanClicked()) );
    connect( m_banAction, SIGNAL(changed()), object, SLOT(onActionsChanged()) );

    connect( m_playAction, SIGNAL(triggered(bool)), object, SLOT(onPlayClicked(bool)) );
    connect( m_playAction, SIGNAL(changed()), object, SLOT(onActionsChanged()) );

    connect( m_skipAction, SIGNAL(changed()), object, SLOT(onActionsChanged()) );
    connect( m_skipAction, SIGNAL(triggered(bool)), object, SLOT(onSkipClicked()) );
}
