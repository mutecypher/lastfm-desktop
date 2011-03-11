#ifndef EDITSTATIONDIALOG_H
#define EDITSTATIONDIALOG_H

#include <QDialog>

#include <lastfm/RadioStation>

namespace Ui {
    class EditStationDialog;
}

class EditStationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditStationDialog( const RadioStation& station, QWidget *parent = 0);
    ~EditStationDialog();

private:
    Ui::EditStationDialog *ui;
};

#endif // EDITSTATIONDIALOG_H
