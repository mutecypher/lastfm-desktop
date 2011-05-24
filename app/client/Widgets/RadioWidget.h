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
//    void onRadioTick( qint64 tick );
//    void onTrackSpooled( const Track& track );
//    void onTuningIn( const RadioStation& station );
//    void onError( int error , const QVariant& errorData );
//    void onStopped();

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
