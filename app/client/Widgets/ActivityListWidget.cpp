#include <QApplication>
#include <QHeaderView>

#include <lastfm/Track.h>

#include "lib/unicorn/dialogs/ShareDialog.h"
#include "lib/unicorn/dialogs/TagDialog.h"
#include "lib/unicorn/DesktopServices.h"

#include "../Services/ScrobbleService.h"
#include "../ActivityListModel.h"
#include "../Application.h"

#include "ActivityListWidget.h"
#include "CheckableDelegate.h"
#include "TrackDelegate.h"
#include "MetadataWidget.h"

ActivityListWidget::ActivityListWidget( QWidget* parent )
    :QTreeView( parent )
{
    setModel( m_model = new ActivityListModel );
    QStyledItemDelegate* delegate = new CheckableDelegate(this);
    setItemDelegate( delegate );
    setItemDelegateForColumn( 0, new TrackDelegate(this));
    header()->setStretchLastSection( false );
    setAttribute( Qt::WA_MacShowFocusRect, false );
    viewport()->setAttribute( Qt::WA_Hover, true );
    viewport()->setMouseTracking( true );
    setSelectionMode( NoSelection );
    header()->setResizeMode( 0, QHeaderView::Stretch );

    for ( int i = 1; i < m_model->columnCount(); i++ )
        header()->setResizeMode( i, QHeaderView::ResizeToContents );

    header()->setMinimumSectionSize( 0 );
    setHeaderHidden( true );
    setRootIsDecorated( false );
    setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );

    connect( this, SIGNAL( clicked(QModelIndex) ), SLOT(onItemClicked(QModelIndex)));
    connect( m_model, SIGNAL(refreshing(bool)), SIGNAL(refreshing(bool)) );

    setAttribute( Qt::WA_MacNoClickThrough );
}

void 
ActivityListWidget::onItemClicked( const QModelIndex& index ) 
{
    if( index.column() == 1 && index.data( ActivityListModel::HoverStateRole ).toBool() )
    {
        // do nothing here, loving is done in the ActivityListModel
    }
    else if( index.column() == 2 && index.data( ActivityListModel::HoverStateRole ).toBool() )
    {
        TagDialog* td = new TagDialog( index.data(ActivityListModel::TrackRole).value<Track>(), window() );
        td->raise(); 
        td->show(); 
        td->activateWindow();
    }
    else if( index.column() == 3 && index.data( ActivityListModel::HoverStateRole ).toBool() )
    {
        m_shareIndex = index;

        QMenu* shareMenu = new QMenu( this );

        shareMenu->addAction( tr( "Share on Last.fm" ), this, SLOT(onShareLastFm()) );
        shareMenu->addAction( tr( "Share on Twitter" ), this, SLOT(onShareTwitter()) );
        shareMenu->addAction( tr( "Share on Facebook" ), this, SLOT(onShareFacebook()) );

        shareMenu->exec( cursor().pos() );

    }
    else if( index.column() == 4 && index.data( ActivityListModel::HoverStateRole ).toBool() )
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

        connect( index.data(ActivityListModel::TrackRole).value<lastfm::Track>().getBuyLinks( country ), SIGNAL(finished()), SLOT(onGotBuyLinks()));

        m_buyCursor = cursor().pos();
    }
    else
    {
        emit trackClicked( index.data(ActivityListModel::TrackRole).value<Track>());
    }
}

QString
ActivityListWidget::price( const QString& price, const QString& currency ) const
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

void
ActivityListWidget::onGotBuyLinks()
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
ActivityListWidget::onBuyActionTriggered( QAction* buyAction )
{
    unicorn::DesktopServices::openUrl( buyAction->data().toString() );
}

void
ActivityListWidget::onShareLastFm()
{
    ShareDialog* sd = new ShareDialog( m_shareIndex.data(ActivityListModel::TrackRole).value<Track>(), window() );
    sd->raise();
    sd->show();
    sd->activateWindow();
}

void
ActivityListWidget::onShareTwitter()
{
    ShareDialog::shareTwitter( m_shareIndex.data(ActivityListModel::TrackRole).value<Track>() );
}

void
ActivityListWidget::onShareFacebook()
{
    ShareDialog::shareFacebook( m_shareIndex.data(ActivityListModel::TrackRole).value<Track>() );
}


void
ActivityListWidget::refresh()
{
    m_model->refresh();
}

