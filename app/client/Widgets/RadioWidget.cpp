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
#include "../Services/RadioService.h"
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
    RadioService::instance().play( RadioStation::library( User().name() ) );
}


void
RadioWidget::onMixClicked()
{
    RadioService::instance().play( RadioStation::mix( User().name() ) );
}


void
RadioWidget::onRecomendedClicked()
{
    RadioService::instance().play( RadioStation::recommendations( User().name() ) );
}


void
RadioWidget::onStartClicked()
{
    QString trimmedText = ui->stationEdit->text().trimmed();

    if( trimmedText.startsWith("lastfm://")) {
        RadioService::instance().play( RadioStation( trimmedText ) );
        return;
    }

    if ( ui->stationEdit->text().length() )
    {
        StationSearch* s = new StationSearch();
        connect(s, SIGNAL(searchResult(RadioStation)), &RadioService::instance(), SLOT(play(RadioStation)));
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
//        if ( RadioService::instance().state() == Radio::Stopped )
//            RadioService::instance().play( RadioStation( "" ) );
//        else
//        {
//            RadioService::instance().resume();
//        }
//    }
//    else
//    {
//        RadioService::instance().pause();
//    }
//}


void
RadioWidget::onFilterClicked()
{
    TagFilterDialog tagFilter( RadioService::instance().station(), this );
    if ( tagFilter.exec() == QDialog::Accepted )
    {
        RadioStation station = RadioService::instance().station();
        station.setTagFilter( tagFilter.tag() );
        RadioService::instance().playNext( station );
    }
}


void
RadioWidget::onEditClicked()
{

}

