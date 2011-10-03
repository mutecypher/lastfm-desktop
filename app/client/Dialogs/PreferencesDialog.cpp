
#include <QToolBar>

#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

PreferencesDialog::PreferencesDialog( QMenuBar* menuBar )
    :unicorn::MainWindow( menuBar ),
    ui( new Ui::PreferencesDialog )
{
    ui->setupUi( this );

    setAttribute( Qt::WA_DeleteOnClose, true );

    setUnifiedTitleAndToolBarOnMac( true );

    QToolBar* toolBar = addToolBar( "tabs" );
    toolBar->addWidget( ui->tabFrame );

    connect( this, SIGNAL( saveNeeded() ), ui->general, SLOT( saveSettings() ) );
    connect( this, SIGNAL( saveNeeded() ), ui->scrobbling, SLOT( saveSettings() ) );
    connect( this, SIGNAL( saveNeeded() ), ui->ipod, SLOT( saveSettings() ) );
    connect( this, SIGNAL( saveNeeded() ), ui->accounts, SLOT( saveSettings() ) );
    connect( this, SIGNAL( saveNeeded() ), ui->advanced, SLOT( saveSettings() ) );

    connect( ui->general, SIGNAL( settingsChanged() ), SLOT( onSettingsChanged() ) );
    connect( ui->scrobbling, SIGNAL( settingsChanged() ), SLOT( onSettingsChanged() ) );
    connect( ui->ipod, SIGNAL( settingsChanged() ), SLOT( onSettingsChanged() ) );
    connect( ui->accounts, SIGNAL( settingsChanged() ), SLOT( onSettingsChanged() ) );
    connect( ui->advanced, SIGNAL( settingsChanged() ), SLOT( onSettingsChanged() ) );

    connect( ui->generalButton, SIGNAL(clicked()), SLOT(onTabButtonClicked()));
    connect( ui->scrobblingButton, SIGNAL(clicked()), SLOT(onTabButtonClicked()));
    connect( ui->ipodButton, SIGNAL(clicked()), SLOT(onTabButtonClicked()));
    connect( ui->accountsButton, SIGNAL(clicked()), SLOT(onTabButtonClicked()));
    connect( ui->advancedButton, SIGNAL(clicked()), SLOT(onTabButtonClicked()));

//    connect( ui.buttons, SIGNAL( accepted() ), this, SLOT( onAccepted() ) );
//    connect( ui.buttons, SIGNAL( rejected() ), this, SLOT( reject() ) );
//    connect( ui.buttons->button( QDialogButtonBox::Apply ), SIGNAL( clicked() ), SLOT( onApplyButtonClicked() ) );

    ui->generalButton->click();
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}


void
PreferencesDialog::closeEvent( QCloseEvent* )
{
    emit saveNeeded();
}

void
PreferencesDialog::onTabButtonClicked()
{
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());

    setWindowTitle( clickedButton->text() );

    if ( clickedButton == ui->generalButton )
        ui->stackedWidget->setCurrentWidget( ui->general );
    else if ( clickedButton == ui->accountsButton )
        ui->stackedWidget->setCurrentWidget( ui->accounts );
    else if ( clickedButton == ui->ipodButton )
        ui->stackedWidget->setCurrentWidget( ui->ipod );
    else if ( clickedButton == ui->advancedButton )
        ui->stackedWidget->setCurrentWidget( ui->advanced );
    else if ( clickedButton == ui->scrobblingButton )
        ui->stackedWidget->setCurrentWidget( ui->scrobbling );
}

void
PreferencesDialog::onAccepted()
{
    emit saveNeeded();
    close();
}

void
PreferencesDialog::onSettingsChanged()
{
    //ui->buttons->button( QDialogButtonBox::Apply )->setEnabled( true );
}

void
PreferencesDialog::onApplyButtonClicked()
{
    emit saveNeeded();
    //ui->buttons->button( QDialogButtonBox::Apply )->setEnabled( false );
}
