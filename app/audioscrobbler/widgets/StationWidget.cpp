
#include <QMenu>
#include <QContextMenuEvent>

#include "../StationListModel.h"

#include "StationWidget.h"
#include "ui_StationWidget.h"

#include "../radio.h"

StationWidget::StationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StationWidget)
{
    ui->setupUi(this);

    m_model = new StationListModel( this );
    ui->treeView->setModel( m_model );

    connect( ui->treeView, SIGNAL(doubleClicked(QModelIndex)), SLOT(onDoubleClicked(QModelIndex)));
    connect( radio, SIGNAL(tuningIn(RadioStation)), SLOT(onTuningIn(RadioStation)) );
    connect( radio, SIGNAL(trackSpooled(Track)), SLOT(onTrackSpooled(Track)));

    setContextMenuPolicy( Qt::DefaultContextMenu );

    resizeEvent( 0 );
}


StationWidget::~StationWidget()
{
    delete ui;
}


QTreeView* StationWidget::treeView() const
{
    return ui->treeView;
}

void
StationWidget::clear()
{
    m_model->clear();
}


RadioStation
StationWidget::getStation()
{
    RadioStation station;

    int rowsSelected = ui->treeView->selectionModel()->selectedIndexes().count() / 2;

    if ( rowsSelected == 1 )
    {
        station = RadioStation( ui->treeView->model()->data( ui->treeView->selectionModel()->selectedIndexes().at(0), Qt::UserRole + 1 ).toString() );
    }
    else
    {
        QList<Artist> artists;
        QList<User> users;
        QList<Tag> tags;

        foreach( QModelIndex index, ui->treeView->selectionModel()->selectedIndexes() )
        {
            QString stationUrl = ui->treeView->model()->data( index, StationListModel::UrlRole ).toString();

            if ( stationUrl.startsWith("lastfm://artist/") )
            {
                int endPos = stationUrl.indexOf( "/" , 16 );
                if ( endPos == -1 )
                    endPos = stationUrl.length();

                artists << Artist( stationUrl.mid( 16, endPos - 16 ) );
            }
            else if ( stationUrl.startsWith("lastfm://user/") )
            {
                int endPos = stationUrl.indexOf( "/" , 14 );
                if ( endPos == -1 )
                    endPos = stationUrl.length();

                users << User( stationUrl.mid( 14, endPos - 14 ) );
            }
            else if ( "lastfm://globaltags/" )
            {
                int endPos = stationUrl.indexOf( "/" , 20 );
                if ( endPos == -1 )
                    endPos = stationUrl.length();

                QString tag = stationUrl.mid( 20, endPos - 20 );
                qDebug() << tag;
                tags << Tag( tag );
            }

        }

        if ( artists.count() )
            station = RadioStation::similar( artists );
        else if ( users.count() )
            station = RadioStation::library( users );
        else if ( tags.count() )
            station = RadioStation::tag( tags );
    }

    return station;
}


void
StationWidget::onPlay()
{
    radio->play( getStation() );

}


void
StationWidget::onPlayNext()
{
    radio->playNext( getStation() );
}


void
StationWidget::onTagFilter()
{
    RadioStation station = radio->station();

    QString stationUrl = ui->treeView->model()->data( ui->treeView->selectionModel()->selectedIndexes().at( 0 ), StationListModel::UrlRole ).toString();

    int endPos = stationUrl.indexOf( "/" , 20 );
    if ( endPos == -1 )
        endPos = stationUrl.length();

    QString tag = stationUrl.mid( 20, endPos - 20 );

    station.setTagFilter( tag );
    radio->playNext( station );
}


void
StationWidget::contextMenuEvent( QContextMenuEvent* event )
{
    QMenu* contextMenu = new QMenu( this );

    int numSelected = ui->treeView->selectionModel()->selectedIndexes().count() / 2;

    if ( numSelected > 0 )
    {
        QString title = ui->treeView->model()->data( ui->treeView->selectionModel()->selectedIndexes().at( 0 ), StationListModel::TitleRole ).toString();

        contextMenu->addAction( numSelected == 1 ? tr("Play %1").arg( title ) : tr("Play Multi Radio"), this, SLOT(onPlay()));

        if ( radio->state() == Radio::Playing )
            contextMenu->addAction( numSelected == 1 ? tr("Play %1 next").arg( title ) : tr("Play Multi Radio next"), this, SLOT(onPlayNext()));
    }

    contextMenu->addSeparator();

    // Tag filter?
    if ( numSelected == 1
         && ui->treeView->model()->data( ui->treeView->selectionModel()->selectedIndexes().at( 0 ), StationListModel::UrlRole ).toString().startsWith("lastfm://globaltags/")
         && !radio->station().url().startsWith("lastfm://globalTags/")
         && !radio->station().url().startsWith("lastfm://tag/"))
        contextMenu->addAction( "Tag filter", this, SLOT(onTagFilter()));

    if ( contextMenu->actions().count() )
        contextMenu->exec( event->globalPos() );
}


void
StationWidget::addStation( const RadioStation& station, const QString& description )
{
    m_model->addItem( station, description );

    QString stationUrl = station.url();

    if ( stationUrl.startsWith("lastfm://user/") )
    {
        int endPos = stationUrl.indexOf( "/" , 14 );
        if ( endPos == -1 )
            endPos = stationUrl.length();

        QMap<int, QVariant> map;
        map[StationListModel::NameRole] = stationUrl.mid( 14, endPos - 14 );
        m_model->setItemData( m_model->index( m_model->rowCount() - 1, 0), map );
    }
}


void
StationWidget::setTasteometerCompareScore( const QString& user, float score )
{
    for ( int i(0) ; i < m_model->rowCount() ; ++i )
    {
        QString userName = m_model->data( m_model->index( i, 0 ), StationListModel::NameRole ).toString();
        if ( user ==  userName )
        {
            QMap<int, QVariant> map;
            map[StationListModel::TasteometerScoreRole] = score;
            m_model->setItemData( m_model->index( i, 0 ), map );
            break;
        }
    }
}


void
StationWidget::recentStation( const RadioStation& station )
{
    // check if the widget is already in the recent list

    // Don't add if the station title is blank.
//    if ( station.title().isEmpty() )
//        return;

//    QTreeWidgetItem* item = 0;

//    QTreeWidgetItemIterator it( ui->treeWidget );
//    while (*it)
//    {
//        if ( station.title().compare( (*it)->data( 0, Qt::UserRole ).toString(), Qt::CaseInsensitive ) == 0 )
//        {
//            item = *it;
//            break;
//        }

//        ++it;
//    }

//    if ( item )
//    {
//        // Found so move it to the begninning
//        ui->treeWidget->insertTopLevelItem( 0, ui->treeWidget->takeTopLevelItem( ui->treeWidget->indexOfTopLevelItem( item ) ) );
//    }
//    else
//    {
//        // not found so add to the beginning and remove the last element
//        ui->treeWidget->insertTopLevelItem( 0, createItem( station, "" ) );

//        while ( ui->treeWidget->topLevelItemCount() > 20 )
//            ui->treeWidget->takeTopLevelItem( ui->treeWidget->topLevelItemCount() - 1 );
//    }

//    onTuningIn( station );
}


void
StationWidget::onTrackSpooled( const Track& /*track*/ )
{
    onTuningIn( radio->station() );
}


void
StationWidget::onTuningIn( const RadioStation& station )
{   
    // Disable items with the same station url
//    QTreeWidgetItemIterator it( ui->treeWidget );
//    while (*it)
//    {
//        if ( station.title().compare( (*it)->data( 0, Qt::UserRole ).toString(), Qt::CaseInsensitive ) == 0 )
//            (*it)->setFlags( (*it)->flags() & ~Qt::ItemIsEnabled );
//        else
//            (*it)->setFlags( (*it)->flags() | Qt::ItemIsEnabled );

//        ++it;
//    }
}


void
StationWidget::onDoubleClicked( const QModelIndex & index )
{
    if ( ui->treeView->model()->flags( index ) & Qt::ItemIsEnabled )
        radio->play( getStation() );
}


void
StationWidget::resizeEvent( QResizeEvent * event )
{
    ui->treeView->setColumnWidth( 0, (width() * 4) / 10 );
}
