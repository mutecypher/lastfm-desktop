#ifndef TAGFILTERDIALOG_H
#define TAGFILTERDIALOG_H

#include <QDialog>

#include <radio/RadioStation.h>

namespace Ui {
    class TagFilterDialog;
}

class TagFilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagFilterDialog( const lastfm::RadioStation& radioStation, QWidget *parent = 0);
    ~TagFilterDialog();

    QString tag() const;

private slots:
    void onGotTagSuggestions();

private:
    Ui::TagFilterDialog *ui;
};

#endif // TAGFILTERDIALOG_H
