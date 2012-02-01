
#include <lastfm/User.h>

#include "lib/unicorn/DesktopServices.h"

#include "../Application.h"

#include "BetaDialog.h"
#include "ui_BetaDialog.h"

BetaDialog::BetaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BetaDialog)
{
    ui->setupUi(this);

    this->setAttribute( Qt::WA_DeleteOnClose );

    ui->description->setText( "This is a beta version of the new Last.fm Desktop App. Please be gentle!" );

    if ( aApp->currentSession()->userInfo().type() == lastfm::User::TypeStaff )
    {
        ui->feedback->setText( "If you notice and problems, create a Jira issue and we'll fix them immediately. Thanks!" );

    }
    else
    {
        ui->createIssue->hide();
        ui->feedback->setText( "This beta isn't public yet so you probably know Mr. Michael P. Coffey MEng. Tell him your problems with the app and he'll fix them immediately." );
    }

    connect( ui->createIssue, SIGNAL(clicked()), SLOT(createIssue()) );
}

void
BetaDialog::createIssue()
{
    unicorn::DesktopServices::openUrl( QUrl( "https://jira.last.fm/secure/CreateIssue.jspa?pid=10011&issuetype=1&Create=Create" ) );
}

BetaDialog::~BetaDialog()
{
    delete ui;
}

