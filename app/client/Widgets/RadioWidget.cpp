#include <QShortcut>
#include <QToolBar>
#include <QListWidgetItem>
#include <QMenuBar>
#include <QProcess>
#include <QTcpSocket>

#ifdef Q_OS_MAC
    #include <CoreServices/CoreServices.h>
#endif

#include "RadioWidget.h"
#include "ui_RadioWidget.h"

#include "TagFilterDialog.h"

#include "PlayableItemWidget.h"

#include "../Application.h"
#include "Services/RadioService.h"
#include "../StationSearch.h"

#include <lastfm/XmlQuery>
#include <lastfm/User>

#ifdef Q_OS_MAC
#include "ApplicationServices/ApplicationServices.h"
#endif


RadioWidget::RadioWidget()
    :ui(new Ui::RadioWidget)
{
    ui->setupUi(this);

    //ui->trackTitle->setOpenExternalLinks( true);
    //ui->album->setOpenExternalLinks( true);
    //ui->context->setOpenExternalLinks( true);

    ui->stationEdit->setHelpText( tr("Type an artist or tag and press play") );
    ui->stationEdit->setAttribute( Qt::WA_MacShowFocusRect, false );

    connect( ui->start, SIGNAL(clicked()), SLOT(onStartClicked()));
    connect( ui->stationEdit, SIGNAL(returnPressed()), ui->start, SLOT(click()));
    connect( ui->stationEdit, SIGNAL(textChanged(QString)), SLOT(onStationEditTextChanged(QString)));

    connect( ui->library, SIGNAL(clicked()), SLOT(onLibraryClicked()) );
    connect( ui->mix, SIGNAL(clicked()), SLOT(onMixClicked()) );
    connect( ui->recommended, SIGNAL(clicked()), SLOT(onRecomendedClicked()) );
}


RadioWidget::~RadioWidget()
{
    delete ui;
}


void
RadioWidget::onLibraryClicked()
{
    radio->play( RadioStation::library( User().name() ) );
}


void
RadioWidget::onMixClicked()
{
    radio->play( RadioStation::mix( User().name() ) );
}


void
RadioWidget::onRecomendedClicked()
{
    radio->play( RadioStation::recommendations( User().name() ) );
}


void
RadioWidget::onStartClicked()
{
    QString trimmedText = ui->stationEdit->text().trimmed();

    if( trimmedText.startsWith("lastfm://")) {
        radio->play( RadioStation( trimmedText ) );
        return;
    }

    if ( ui->stationEdit->text().length() )
    {
        StationSearch* s = new StationSearch();
        connect(s, SIGNAL(searchResult(RadioStation)), radio, SLOT(play(RadioStation)));
        s->startSearch(ui->stationEdit->text());
    }

    ui->stationEdit->clear();
}


void
RadioWidget::onStationEditTextChanged( const QString& text )
{
    ui->start->setEnabled( text.count() > 0 );
}




//void
//RadioWidget::onPlayClicked( bool checked )
//{
//    if ( checked )
//    {
//        if ( radio->state() == Radio::Stopped )
//            radio->play( RadioStation( "" ) );
//        else
//        {
//            radio->resume();
//        }
//    }
//    else
//    {
//        radio->pause();
//    }
//}


void
RadioWidget::onFilterClicked()
{
    TagFilterDialog tagFilter( radio->station(), this );
    if ( tagFilter.exec() == QDialog::Accepted )
    {
        RadioStation station = radio->station();
        station.setTagFilter( tagFilter.tag() );
        radio->playNext( station );
    }
}


void
RadioWidget::onEditClicked()
{

}

