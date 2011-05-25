#ifndef RADIOWIDGET_H
#define RADIOWIDGET_H

#include <lastfm/RadioStation>

#include "lib/unicorn/UnicornMainWindow.h"

namespace Ui { class RadioWidget; }

class RadioWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RadioWidget();
    ~RadioWidget();

signals:
    void aboutToHide();

private slots:
    void onStartClicked();

    void onLibraryClicked();
    void onMixClicked();
    void onRecomendedClicked();

    void onStationEditTextChanged( const QString& text );

    void onEditClicked();
    void onFilterClicked();

private:
    Ui::RadioWidget* ui;
};

#endif // RadioWidget_H
