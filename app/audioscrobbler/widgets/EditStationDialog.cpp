#include "EditStationDialog.h"
#include "ui_EditStationDialog.h"

EditStationDialog::EditStationDialog( const RadioStation& station, QWidget *parent ) :
    QDialog(parent),
    ui(new Ui::EditStationDialog)
{
    ui->setupUi(this);
}

EditStationDialog::~EditStationDialog()
{
    delete ui;
}
