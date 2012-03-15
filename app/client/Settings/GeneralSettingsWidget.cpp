
#include <QComboBox>

#include "lib/unicorn/UnicornSettings.h"
#include "lib/unicorn/QMessageBoxBuilder.h"

#include "../Application.h"

#include "ui_GeneralSettingsWidget.h"
#include "GeneralSettingsWidget.h"


GeneralSettingsWidget::GeneralSettingsWidget( QWidget* parent )
    :SettingsWidget( parent ),
      ui( new Ui::GeneralSettingsWidget )
{
    ui->setupUi( this );

    populateLanguages();

    connect( ui->languages, SIGNAL( currentIndexChanged( int ) ), SLOT( onSettingsChanged() ) );

    ui->showAs->setChecked( unicorn::Settings().value( SETTING_SHOW_AS, ui->showAs->isChecked() ).toBool() );
    ui->launch->setChecked( unicorn::Settings().value( SETTING_LAUNCH_ITUNES, ui->launch->isChecked() ).toBool() );
    ui->notifications->setChecked( unicorn::Settings().value( SETTING_NOTIFICATIONS, ui->notifications->isChecked() ).toBool() );
    ui->lastRadio->setChecked( unicorn::Settings().value( SETTING_LAST_RADIO, ui->lastRadio->isChecked() ).toBool() );
    ui->sendCrashReports->setChecked( unicorn::Settings().value( SETTING_SEND_CRASH_REPORTS, ui->sendCrashReports->isChecked() ).toBool() );
    ui->updates->setChecked( unicorn::Settings().value( SETTING_CHECK_UPDATES, ui->updates->isChecked() ).toBool() );

    connect( ui->showAs, SIGNAL(stateChanged(int)), SLOT( onSettingsChanged() ) );
    connect( ui->launch, SIGNAL(stateChanged(int) ), SLOT( onSettingsChanged() ) );
    connect( ui->notifications, SIGNAL(stateChanged(int)), SLOT( onSettingsChanged() ) );
    connect( ui->lastRadio, SIGNAL(stateChanged(int)), SLOT( onSettingsChanged() ) );
    connect( ui->sendCrashReports, SIGNAL(stateChanged(int)), SLOT( onSettingsChanged() ) );
    connect( ui->updates, SIGNAL(stateChanged(int)), SLOT( onSettingsChanged() ) );

#ifdef Q_OS_MAC
    ui->hideDock->setChecked( unicorn::Settings().value( SETTING_HIDE_DOCK, ui->hideDock->isChecked() ).toBool() );
    connect( ui->hideDock, SIGNAL(stateChanged(int)), SLOT( onSettingsChanged() ) );
#else
    ui->hideDock->hide();
#endif
}

void
GeneralSettingsWidget::populateLanguages()
{
    ui->languages->addItem( tr( "System Language" ), "" );
    ui->languages->addItem( "English", QLocale( QLocale::English ).name().left( 2 ) );
    ui->languages->addItem( QString::fromUtf8( "FranÃ§ais" ), QLocale( QLocale::French ).name().left( 2 ) );
    ui->languages->addItem( "Italiano", QLocale( QLocale::Italian ).name().left( 2 )  );
    ui->languages->addItem( "Deutsch", QLocale( QLocale::German ).name().left( 2 ) );
    ui->languages->addItem( QString::fromUtf8( "EspaÃ±ol" ), QLocale( QLocale::Spanish ).name().left( 2 ) );
    ui->languages->addItem( QString::fromUtf8( "PortuguÃ©s" ), QLocale( QLocale::Portuguese ).name().left( 2 ) );
    ui->languages->addItem( "Polski", QLocale( QLocale::Polish ).name().left( 2 ) );
    ui->languages->addItem( "Svenska", QLocale( QLocale::Swedish ).name().left( 2 ) );
    ui->languages->addItem( QString::fromUtf8( "TÃ¼kÃ§e" ), QLocale( QLocale::Turkish ).name().left( 2 ) );
    ui->languages->addItem( QString::fromUtf8( "Ð ÑÑÑÐºÐ¸Ð¹" ), QLocale( QLocale::Russian ).name().left( 2 ) );
    ui->languages->addItem( QString::fromUtf8( "ä¸­æ" ), QLocale( QLocale::Chinese ).name().left( 2 ) );
    ui->languages->addItem( QString::fromUtf8( "æ¥æ¬èª" ), QLocale( QLocale::Japanese ).name().left( 2 ) );

    QString currLanguage = unicorn::AppSettings().value( "language", "" ).toString();
    int index = ui->languages->findData( currLanguage );
    if ( index != -1 )
    {
        ui->languages->setCurrentIndex( index );
    }
}

void
GeneralSettingsWidget::saveSettings()
{
    qDebug() << "has unsaved changes?" << hasUnsavedChanges();
    if ( hasUnsavedChanges() )
    {
        int currIndex = ui->languages->currentIndex();
        QString currLanguage = ui->languages->itemData( currIndex ).toString();

        if ( unicorn::AppSettings().value( "language", "" ) != currLanguage )
        {
            unicorn::AppSettings().setValue( "language", currLanguage );
            QMessageBoxBuilder( 0 )
                .setIcon( QMessageBox::Information )
                .setTitle( tr( "Restart needed" ) )
                .setText( tr( "You need to restart the application for the language change to take effect." ) )
                .exec();
        }

        unicorn::Settings().setValue( SETTING_SHOW_AS, ui->showAs->isChecked() );
        aApp->showAs( ui->showAs->isChecked() );

        // setting is for the 'Client' aplication for compatibility with old media player plugins
        unicorn::AppSettings( "Client" ).setValue( SETTING_LAUNCH_ITUNES, ui->launch->isChecked() );

        //
        unicorn::Settings().setValue( SETTING_NOTIFICATIONS, ui->notifications->isChecked() );
        unicorn::Settings().setValue( SETTING_LAST_RADIO, ui->lastRadio->isChecked() );
        unicorn::Settings().setValue( SETTING_SEND_CRASH_REPORTS, ui->sendCrashReports->isChecked() );
        unicorn::Settings().setValue( SETTING_CHECK_UPDATES, ui->updates->isChecked() );
#ifdef Q_OS_MAC
        unicorn::Settings().setValue( SETTING_HIDE_DOCK, ui->hideDock->isChecked() );
        aApp->hideDockIcon( ui->hideDock->isChecked() );
#endif

        onSettingsSaved();
    }
}
