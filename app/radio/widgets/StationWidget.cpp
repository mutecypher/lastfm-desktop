#include "StationWidget.h"
#include "ui_StationWidget.h"

#include "../radio.h"

StationWidget::StationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StationWidget)
{
    ui->setupUi(this);

    connect( ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));
    connect( radio, SIGNAL(tuningIn(RadioStation)), SLOT(onTuningIn(RadioStation)) );
    connect( radio, SIGNAL(trackSpooled(Track)), SLOT(onTrackSpooled(Track)));

    resizeEvent( 0 );
}


StationWidget::~StationWidget()
{
    delete ui;
}


QTreeWidgetItem*
StationWidget::createItem( const RadioStation& station, const QString& description )
{
    QIcon icon;
    icon.addFile(";/station-start-rest.png", QSize(17, 17), QIcon::Normal, QIcon::On);
    icon.addFile(":/station-start-onpress.png", QSize(17, 17), QIcon::Normal, QIcon::Off);

    QStringList columns;
    columns << station.title();
    columns << description;
    QTreeWidgetItem* item = new QTreeWidgetItem( columns );
    item->setData( 0, Qt::UserRole, station.title());
    item->setData( 0, Qt::UserRole + 1, station.url());
    item->setIcon( 0, icon );

    return item;
}


void
StationWidget::addStation( const RadioStation& station, const QString& description )
{
    ui->treeWidget->addTopLevelItem( createItem( station, description ) );
}


void
StationWidget::recentStation( const RadioStation& station )
{
    // check if the widget is already in the recent list

    // Don't add if the station title is blank.
    if ( station.title().isEmpty() )
        return;

    QTreeWidgetItem* item = 0;

    QTreeWidgetItemIterator it( ui->treeWidget );
    while (*it)
    {
        if ( (*it)->data( 0, Qt::UserRole ) == station.title() )
        {
            item = *it;
            break;
        }

        ++it;
    }

    QList<QTreeWidgetItem*> items = ui->treeWidget->findItems( station.title(), Qt::MatchExactly, 0 );

    if ( item )
    {
        // Found so move it to the begninning
        ui->treeWidget->insertTopLevelItem( 0, ui->treeWidget->takeTopLevelItem( ui->treeWidget->indexOfTopLevelItem( item ) ) );
    }
    else
    {
        // not found so add to the beginning and remove the last element
        ui->treeWidget->insertTopLevelItem( 0, createItem( station, "" ) );

        while ( ui->treeWidget->topLevelItemCount() > 20 )
            ui->treeWidget->takeTopLevelItem( ui->treeWidget->topLevelItemCount() - 1 );
    }

    onTuningIn( station );
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
    QTreeWidgetItemIterator it( ui->treeWidget );
    while (*it)
    {
        if ( (*it)->data( 0, Qt::UserRole ) == station.title() )
            (*it)->setFlags( (*it)->flags() & ~Qt::ItemIsEnabled );
        else
            (*it)->setFlags( (*it)->flags() | Qt::ItemIsEnabled );

        ++it;
    }
}


void
StationWidget::onItemDoubleClicked( QTreeWidgetItem* item, int column )
{
    if ( column == 0 && ( item->flags() & Qt::ItemIsEnabled ) )
    {
        // Enable all items
        QTreeWidgetItemIterator it( ui->treeWidget );
        while (*it)
            (*it++)->setFlags( item->flags() | Qt::ItemIsEnabled );

        // disable the selected item
        item->setFlags( item->flags() & ~Qt::ItemIsEnabled );

        RadioStation station( item->data( column, Qt::UserRole + 1 ).toString() );
        station.setTitle( item->data( column, Qt::UserRole ).toString() );
        radio->play( station );
    }
}


void
StationWidget::resizeEvent( QResizeEvent * event )
{
    ui->treeWidget->setColumnWidth( 0, (width() * 4) / 10 );
}
