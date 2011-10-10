
#include <QComboBox>

#include "lib/unicorn/UnicornSettings.h"
#include "lib/unicorn/QMessageBoxBuilder.h"

#include "GeneralSettingsWidget.h"

GeneralSettingsWidget::GeneralSettingsWidget( QWidget* parent )
    :SettingsWidget( parent )
{
    ui.languages = new QComboBox( this );

    populateLanguages();

    connect( ui.languages, SIGNAL( currentIndexChanged( int ) ), this, SLOT( onSettingsChanged() ) );
}

void
GeneralSettingsWidget::populateLanguages()
{
    ui.languages->addItem( tr( "System Language" ), "" );
    ui.languages->addItem( "English", QLocale( QLocale::English ).name().left( 2 ) );
    ui.languages->addItem( QString::fromUtf8( "FranÃ§ais" ), QLocale( QLocale::French ).name().left( 2 ) );
    ui.languages->addItem( "Italiano", QLocale( QLocale::Italian ).name().left( 2 )  );
    ui.languages->addItem( "Deutsch", QLocale( QLocale::German ).name().left( 2 ) );
    ui.languages->addItem( QString::fromUtf8( "EspaÃ±ol" ), QLocale( QLocale::Spanish ).name().left( 2 ) );
    ui.languages->addItem( QString::fromUtf8( "PortuguÃ©s" ), QLocale( QLocale::Portuguese ).name().left( 2 ) );
    ui.languages->addItem( "Polski", QLocale( QLocale::Polish ).name().left( 2 ) );
    ui.languages->addItem( "Svenska", QLocale( QLocale::Swedish ).name().left( 2 ) );
    ui.languages->addItem( QString::fromUtf8( "TÃ¼kÃ§e" ), QLocale( QLocale::Turkish ).name().left( 2 ) );
    ui.languages->addItem( QString::fromUtf8( "Ð ÑÑÑÐºÐ¸Ð¹" ), QLocale( QLocale::Russian ).name().left( 2 ) );
    ui.languages->addItem( QString::fromUtf8( "ä¸­æ" ), QLocale( QLocale::Chinese ).name().left( 2 ) );
    ui.languages->addItem( QString::fromUtf8( "æ¥æ¬èª" ), QLocale( QLocale::Japanese ).name().left( 2 ) );

    QString currLanguage = unicorn::AppSettings().value( "language", "" ).toString();
    int index = ui.languages->findData( currLanguage );
    if ( index != -1 )
    {
        ui.languages->setCurrentIndex( index );
    }
}

void
GeneralSettingsWidget::saveSettings()
{
    qDebug() << "has unsaved changes?" << hasUnsavedChanges();
    if ( hasUnsavedChanges() )
    {
        int currIndex = ui.languages->currentIndex();
        QString currLanguage = ui.languages->itemData( currIndex ).toString();

        if ( unicorn::AppSettings().value( "language", "" ) != currLanguage )
        {
            unicorn::AppSettings().setValue( "language", currLanguage );
            QMessageBoxBuilder( 0 )
                .setIcon( QMessageBox::Information )
                .setTitle( tr( "Restart needed" ) )
                .setText( tr( "You need to restart the application for the language change to take effect." ) )
                .exec();
        }
        onSettingsSaved();
    }
}
