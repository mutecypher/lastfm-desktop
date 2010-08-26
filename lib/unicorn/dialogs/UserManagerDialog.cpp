#include "UserManagerDialog.h"

#include <lastfm/User>

#include "lib/unicorn/QMessageBoxBuilder.h"
#include "lib/unicorn/UnicornSettings.h"
#include "lib/unicorn/widgets/UserManagerWidget.h"

#include <QApplication>
#include <QDebug>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>


UserManagerDialog::UserManagerDialog( QWidget* parent )
            :QDialog( parent )
{
    m_users = new UserManagerWidget( this );
    QVBoxLayout* layout = new QVBoxLayout();
    QHBoxLayout* actionButtons = new QHBoxLayout();
    QDialogButtonBox* bb;
    actionButtons->addWidget( bb = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel ));

    layout->addWidget( m_users );
    layout->addLayout( actionButtons );
    setLayout( layout );

    connect( m_users, SIGNAL( rosterUpdated() ), this, SIGNAL( rosterUpdated() ) );
    connect( bb, SIGNAL( accepted()), SLOT( onAccept()));
    connect( bb, SIGNAL( rejected()), SLOT( reject()));
}


UserManagerDialog::~UserManagerDialog()
{
}

void 
UserManagerDialog::onAccept()
{
    unicorn::Settings s;
    UserRadioButton* urb = qobject_cast<UserRadioButton*>( m_users->checkedButton() );
    if( !urb || urb->user() == User().name()) return QDialog::reject();

    s.setValue( "Username", urb->user());
    QDialog::accept();
}
