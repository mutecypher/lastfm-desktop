#include "StationWidget.h"
#include "ui_StationWidget.h"

StationWidget::StationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StationWidget)
{
    ui->setupUi(this);
}

StationWidget::~StationWidget()
{
    delete ui;
}

void
StationWidget::addStation( const RadioStation& station )
{
    new QListWidgetItem( station.title(), ui->listWidget );
}
