#include "UserManager.h"

#include <QHBoxLayout>
#include <lastfm/User>
#include <QLabel>
#include <QApplication>
#include <QMouseEvent>
#include <QPushButton>
#include <QTimer>
#include <QDebug>


#include <QVBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QDialogButtonBox>

#include "lib/unicorn/QMessageBoxBuilder.h"
#include "lib/unicorn/UnicornSettings.h"
#include "lib/unicorn/dialogs/LoginDialog.h"
#include "lib/unicorn/dialogs/LoginContinueDialog.h"
#include "lib/unicorn/dialogs/WelcomeDialog.h"

using lastfm::UserDetails;
UserRadioButton::UserRadioButton( const User& user )
                :m_userName( user.name() )
{
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );

    if( user == User()) setChecked( true );

    QHBoxLayout* l = new QHBoxLayout();
    l->setContentsMargins( 20, 0, 0, 0 );
    l->addWidget( m_image = new QLabel(), 0, Qt::AlignTop);
    m_image->setFrameShape( QFrame::Box );
    m_image->setFrameShadow( QFrame::Plain );
    unsigned int framewidth = m_image->lineWidth() * 2;
    m_image->setMinimumSize( QSize( 35+framewidth, 35+ framewidth ));
    m_image->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::MinimumExpanding );
    
    installEventFilter( this );
    if( user.mediumImageUrl().isEmpty()) {
        QNetworkReply* reply = UserDetails::getInfo( user.name() );
        connect( reply, SIGNAL(finished()), SLOT( onUserDetailsFetched()));
    } else {
        QNetworkReply* reply = lastfm::nam()->get( QNetworkRequest( user.mediumImageUrl()));
        connect( reply, SIGNAL(finished()), SLOT( onImageLoaded()));
    }
    QGridLayout* gl = new QGridLayout();
    gl->setSpacing( 0 );
    gl->addWidget( m_name = new QLabel(user.name()), 0, 0, Qt::AlignTop );
    gl->addWidget( m_realName = new QLabel(), 0, 1, Qt::AlignTop );
    gl->addWidget( m_loggedIn = new QLabel(), 1, 0, 1, 2, Qt::AlignTop );
    m_realName->setObjectName( "realname" );
    l->addLayout( gl );
    l->addStretch();
    QPushButton* remove;
    l->addWidget( remove = new QPushButton( tr("Remove") ));
    connect( remove, SIGNAL(clicked()), SLOT(removeMe()));
    QVBoxLayout* vl = new QVBoxLayout(this);
    vl->addLayout( l );
}

void 
UserRadioButton::onUserDetailsFetched()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>( sender());
    Q_ASSERT( reply );

    UserDetails user( reply );

    m_name->setText( user.name());
    if( !user.realName().isEmpty())
        m_realName->setText( QString( " (%1)" ).arg(user.realName()));
    
    if( user == User())
        m_loggedIn->setText( "(" + tr( "currently logged in" ) + ")" );

    reply = lastfm::nam()->get( QNetworkRequest( user.mediumImageUrl()));
    connect( reply, SIGNAL(finished()), SLOT( onImageLoaded()));

    m_userName = user.name();
}

void 
UserRadioButton::onImageLoaded()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if( !reply ) return;

    QPixmap pm;
    pm.loadFromData( reply->readAll());
    m_image->setPixmap( pm.scaledToWidth( 35, Qt::SmoothTransformation ));
}

bool 
UserRadioButton::eventFilter( QObject* obj, QEvent* event )
{
    QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>( event );
    if( !mouseEvent ) return false;

    int l,t,r,b;
    getContentsMargins( &l, &t, &r, &b );
    QMouseEvent mEvent( mouseEvent->type(), QPoint( l, height()/2 ), mouseEvent->button(), mouseEvent->buttons(), mouseEvent->modifiers());
    
    // Remove the event filter so that the fake mouse event can actually
    // be passed to the radio button and then set the eventFilter again.
    removeEventFilter( this );
    if( this )
        QCoreApplication::sendEvent( this, &mEvent );
    installEventFilter( this );

    return true;
}

void
UserRadioButton::removeMe()
{
    int result =
        QMessageBoxBuilder( this ).setTitle( tr( "Removing %1" ).arg( m_name->text() ) )
                                  .setText( tr( "Are you sure you want to remove this user? All user settings will be lost and you will need to re authenticate in order to scrobble in the future." ))
                                  .setIcon( QMessageBox::Question )
                                  .setButtons( QMessageBox::Yes | QMessageBox::No )
                                  .exec();
    if( result != QMessageBox::Yes ) return;

    unicorn::Settings().remove( m_name->text() );

    if( isChecked()) {
        foreach (UserRadioButton* b, parentWidget()->findChildren<UserRadioButton*>()) {
            if( b->user() == User().name()) {
                b->click();
            }
        }
    }
    deleteLater();
}


const QString 
UserRadioButton::user() const
{
    return m_userName;
}


UserManager::UserManager( QWidget* parent )
            :QDialog( parent ), m_buttonGroup( new QButtonGroup( this ) )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setSpacing( 10 );
    layout->addWidget( new QLabel( tr( "Users authenticated with this application" )));

    layout->addWidget( ui.groupBox = new QGroupBox());
    new QVBoxLayout( ui.groupBox );
    ui.groupBox->setTitle( tr( "Log me in as:" ));

    QList<lastfm::User> roster = unicorn::Settings().userRoster();
    
    foreach( lastfm::User u, roster ) {
        UserRadioButton* b = new UserRadioButton( u );
        add( b, false );
    }

    qobject_cast<QBoxLayout*>(ui.groupBox->layout())->addStretch();
    ui.groupBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding );

    QHBoxLayout* actionButtons = new QHBoxLayout();
    QDialogButtonBox* bb;
    actionButtons->addWidget( bb = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel ));
    
    connect( bb->addButton( tr( "Add new user" ), QDialogButtonBox::ActionRole ),
             SIGNAL(clicked()),
             SLOT(onAddUserClicked()));

    connect( bb, SIGNAL( accepted()), SLOT( onAccept()));
    connect( bb, SIGNAL( rejected()), SLOT( reject()));
    layout->addLayout( actionButtons );
}


UserManager::~UserManager()
{
    disconnect( this );
}

void 
UserManager::onAccept()
{
    unicorn::Settings s;
    UserRadioButton* urb = qobject_cast<UserRadioButton*>( m_buttonGroup->checkedButton());
    if( !urb || urb->user() == User().name()) return QDialog::reject();

    s.setValue( "Username", urb->user());
    QDialog::accept();
}

void
UserManager::onAddUserClicked()
{
    LoginDialog* ld = new LoginDialog( this );
    ld->setWindowFlags( Qt::Sheet );
    ld->open();
    connect( ld, SIGNAL( accepted()), SLOT( onLoginDialogAccepted()));
}


void
UserManager::onLoginDialogAccepted()
{
    LoginDialog* ld = qobject_cast<LoginDialog*>(sender());
    Q_ASSERT( ld );
    
    QString token = ld->token();
    ld->deleteLater();

    LoginContinueDialog* lcd = new LoginContinueDialog( token, this );
    lcd->setWindowFlags( Qt::Sheet );
    lcd->open();
    connect( lcd, SIGNAL( accepted()), SLOT( onUserAdded()));
}


void 
UserManager::onUserAdded()
{
    LoginContinueDialog* lcd = qobject_cast<LoginContinueDialog*>(sender());
    Q_ASSERT( lcd );

    const unicorn::Session& s = lcd->session();
    
    User user( s.username());
    UserRadioButton* urb = new UserRadioButton( user );

    add( urb );
    if( ui.groupBox->layout()->count() <= 1 ) urb->click();

    WelcomeDialog( user ).exec();
}

void 
UserManager::add( UserRadioButton* urb, bool announce )
{
    // The user is added 1 widget from last 
    // (the last widget being the stretch)
    qobject_cast<QBoxLayout*>(ui.groupBox->layout())->insertWidget( ui.groupBox->layout()->count() - 1, 
                                                                    urb);
    m_buttonGroup->addButton( urb );

    if( announce )
        emit rosterUpdated();

    connect( urb, SIGNAL( destroyed(QObject*)), SIGNAL( rosterUpdated()));
}

