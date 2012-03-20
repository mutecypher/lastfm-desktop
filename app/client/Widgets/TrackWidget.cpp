
#include <lib/unicorn/dialogs/ShareDialog.h>
#include <lib/unicorn/dialogs/TagDialog.h>
#include <lib/unicorn/DesktopServices.h>

#include "../Application.h"

#include "TrackWidget.h"
#include "ui_TrackWidget.h"

TrackWidget::TrackWidget( Track& track, QWidget *parent )
    :QWidget(parent),
    ui( new Ui::TrackWidget )
{
    ui->setupUi(this);

    setTrack( track );
}

TrackWidget::~TrackWidget()
{
    delete ui;
}

void
TrackWidget::setTrack( lastfm::Track& track )
{
    m_track = track;

    ui->trackTitle->setText( m_track.title() );
    ui->artist->setText( m_track.artist().name() );
    ui->timestamp->setText( prettyTime( m_track.timestamp() ) );

    //ui->albumArt
}

lastfm::Track
TrackWidget::track() const
{
    return m_track;
}

void
TrackWidget::onLoveClicked()
{

}

void
TrackWidget::onTagClicked()
{
    TagDialog* td = new TagDialog( m_track, window() );
    td->raise();
    td->show();
    td->activateWindow();
}

void
TrackWidget::onShareClicked()
{
    QMenu* shareMenu = new QMenu( this );

    shareMenu->addAction( tr( "Share on Last.fm" ), this, SLOT(onShareLastFm()) );
    shareMenu->addAction( tr( "Share on Twitter" ), this, SLOT(onShareTwitter()) );
    shareMenu->addAction( tr( "Share on Facebook" ), this, SLOT(onShareFacebook()) );

    shareMenu->exec( cursor().pos() );
}

void
TrackWidget::onBuyClicked()
{
    // show the buy links please!
    QString country = aApp->currentSession()->userInfo().country();
    qDebug() << country;

    if ( country.compare( "us", Qt::CaseInsensitive ) == 0 )
        country = "united states";
    else if ( country.compare( "de", Qt::CaseInsensitive ) == 0 )
        country = "germany";
    else
        country = "united kingdom";

    connect( m_track.getBuyLinks( country ), SIGNAL(finished()), SLOT(onGotBuyLinks()));

    m_buyCursor = cursor().pos();
}

QString
TrackWidget::price( const QString& price, const QString& currency ) const
{
    QString returnPrice;

    if ( currency.compare( "eur", Qt::CaseInsensitive ) == 0 )
        returnPrice = QString::fromUtf8( "€%1" ).arg( price );
    else if ( currency.compare( "usd", Qt::CaseInsensitive ) == 0 )
        returnPrice = QString::fromUtf8( "$%1" ).arg( price );
    else if ( currency.compare( "gbp", Qt::CaseInsensitive ) == 0 )
        returnPrice = QString::fromUtf8( "£%1" ).arg( price );
    else
        returnPrice = QString( "%1 %2" ).arg( price, currency );

    return returnPrice;
}

QString
TrackWidget::prettyTime( const QDateTime& timestamp ) const
{
    QString dateFormat( "d MMM h:mmap" );
    QDateTime now = QDateTime::currentDateTime();
    int secondsAgo = timestamp.secsTo( now );

    if ( secondsAgo < (60 * 60) )
    {
        // Less than an hour ago
        int minutesAgo = ( timestamp.secsTo( now ) / 60 );
        return (minutesAgo == 1 ? tr( "%1 minute ago" ) : tr( "%1 minutes ago" ) ).arg( QString::number( minutesAgo ) );
    }
    else if ( secondsAgo < (60 * 60 * 6) || now.date() == timestamp.date() )
    {
        // Less than 6 hours ago or on the same date
        int hoursAgo = ( timestamp.secsTo( now ) / (60 * 60) );
        return (hoursAgo == 1 ? tr( "%1 hour ago" ) : tr( "%1 hours ago" ) ).arg( QString::number( hoursAgo ) );
    }
    else
    {
        return timestamp.toString( dateFormat );
        // We don't need to set the timer because this date will never change
    }
}


void
TrackWidget::onGotBuyLinks()
{
    XmlQuery lfm;

    if ( lfm.parse( qobject_cast<QNetworkReply*>(sender())->readAll() ) )
    {
        bool thingsToBuy = false;

        QMenu* menu = new QMenu( this );

        menu->addAction( tr("Downloads") )->setEnabled( false );

        // USD EUR GBP

        foreach ( const XmlQuery& affiliation, lfm["affiliations"]["downloads"].children( "affiliation" ) )
        {
            bool isSearch = affiliation["isSearch"].text() == "1";

            QAction* buyAction = 0;

            if ( isSearch )
                buyAction = menu->addAction( tr("Search on %1").arg( affiliation["supplierName"].text() ) );
            else
                buyAction = menu->addAction( tr("Buy on %1 %2").arg( affiliation["supplierName"].text(), price( affiliation["price"]["amount"].text(), affiliation["price"]["currency"].text() ) ) );

            buyAction->setData( affiliation["buyLink"].text() );

            thingsToBuy = true;
        }

        menu->addSeparator();
        menu->addAction( tr("Physical") )->setEnabled( false );

        foreach ( const XmlQuery& affiliation, lfm["affiliations"]["physicals"].children( "affiliation" ) )
        {
            bool isSearch = affiliation["isSearch"].text() == "1";

            QAction* buyAction = 0;

            if ( isSearch )
                buyAction = menu->addAction( tr("Search on %1").arg( affiliation["supplierName"].text() ) );
            else
                buyAction = menu->addAction( tr("Buy on %1 %2").arg( affiliation["supplierName"].text(), price( affiliation["price"]["amount"].text(), affiliation["price"]["currency"].text() ) ) );

            buyAction->setData( affiliation["buyLink"].text() );

            thingsToBuy = true;
        }

        connect( menu, SIGNAL(triggered(QAction*)), SLOT(onBuyActionTriggered(QAction*)) );

        menu->exec( m_buyCursor );
    }
    else
    {
        // TODO: what happens when we fail?
        qDebug() << lfm.parseError().message() << lfm.parseError().enumValue();
    }
}

void
TrackWidget::onBuyActionTriggered( QAction* buyAction )
{
    unicorn::DesktopServices::openUrl( buyAction->data().toString() );
}

void
TrackWidget::onShareLastFm()
{
    ShareDialog* sd = new ShareDialog( m_track, window() );
    sd->raise();
    sd->show();
    sd->activateWindow();
}

void
TrackWidget::onShareTwitter()
{
    ShareDialog::shareTwitter( m_track );
}

void
TrackWidget::onShareFacebook()
{
    ShareDialog::shareFacebook( m_track );
}

