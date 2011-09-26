#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

PreferencesDialog::PreferencesDialog( QWidget* parent )
    :unicorn::Dialog( parent ),
    ui( new Ui::PreferencesDialog )
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex( 0 );

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

//    connect( ui.buttons, SIGNAL( accepted() ), this, SLOT( onAccepted() ) );
//    connect( ui.buttons, SIGNAL( rejected() ), this, SLOT( reject() ) );
//    connect( ui.buttons->button( QDialogButtonBox::Apply ), SIGNAL( clicked() ), SLOT( onApplyButtonClicked() ) );
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void
PreferencesDialog::onAccepted()
{
    emit saveNeeded();
    accept();
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
