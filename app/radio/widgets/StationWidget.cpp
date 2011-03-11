#include "StationWidget.h"
#include "ui_StationWidget.h"

#include "../radio.h"

StationWidget::StationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StationWidget)
{
    ui->setupUi(this);

    connect( ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(onItemClicked(QTreeWidgetItem*, int)));
    connect( radio, SIGNAL(tuningIn(RadioStation)), SLOT(onTuningIn(RadioStation)) );

    resizeEvent( 0 );
}


StationWidget::~StationWidget()
{
    delete ui;
}


QTreeWidgetItem*
StationWidget::createItem( const RadioStation& station )
{
    QIcon icon;
    icon.addFile(";/station-start-rest.png", QSize(17, 17), QIcon::Normal);
    icon.addFile(":/station-start-onhover.png", QSize(17, 17), QIcon::Active);
    icon.addFile(":/station-start-onpress.png", QSize(17, 17), QIcon::Selected);

    QStringList columns;
    columns << station.title();
    columns << "Context";
    QTreeWidgetItem* item = new QTreeWidgetItem( columns );
    item->setData( 0, Qt::UserRole, station.title());
    item->setData( 0, Qt::UserRole + 1, station.url());
    item->setIcon( 0, icon );

    return item;
}


void
StationWidget::addStation( const RadioStation& station )
{
    ui->treeWidget->addTopLevelItem( createItem( station ) );
}


void
StationWidget::recentStation( const RadioStation& station )
{
    // check if the widget is already in the recent list
    //

    QList<QTreeWidgetItem*> items = ui->treeWidget->findItems( station.title(), Qt::MatchExactly, 0 );

    if ( items.count() == 0 )
    {
        // not found so add to the beginning and remove the last element
        ui->treeWidget->insertTopLevelItem( 0, createItem( station ) );

        while ( ui->treeWidget->topLevelItemCount() > 20 )
            ui->treeWidget->takeTopLevelItem( ui->treeWidget->topLevelItemCount() - 1 );
    }
    else
    {
        // Found so move it to the begninning
        ui->treeWidget->insertTopLevelItem( 0, ui->treeWidget->takeTopLevelItem( ui->treeWidget->indexOfTopLevelItem( items[0] ) ) );
    }

    ui->treeWidget->setCurrentItem( ui->treeWidget->topLevelItem( 0 ) );
}


void
StationWidget::onTuningIn( const RadioStation& station )
{
    QList<QTreeWidgetItem*> items = ui->treeWidget->findItems( station.title(), Qt::MatchExactly, 0 );

    if ( items.count() == 0 )
        ui->treeWidget->setCurrentItem( 0 );
    else
        ui->treeWidget->setCurrentItem( items[0] );
}


void
StationWidget::onItemClicked( QTreeWidgetItem* item, int column )
{
    RadioStation station( item->data( column, Qt::UserRole + 1 ).toString() );
    station.setTitle( item->data( column, Qt::UserRole ).toString() );
    radio->play( station );
}


void
StationWidget::resizeEvent( QResizeEvent * event )
{
    ui->treeWidget->setColumnWidth( 0, (width() * 7) / 10 );
}
