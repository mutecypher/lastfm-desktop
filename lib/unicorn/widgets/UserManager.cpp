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

using lastfm::UserDetails;
UserRadioButton::UserRadioButton( const User& user )
{
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding );

    QHBoxLayout* l = new QHBoxLayout();
    l->setContentsMargins( 20, 0, 0, 0 );
    l->addWidget( m_image = new QLabel(), 0, Qt::AlignTop);
    m_image->setFrameShape( QFrame::Box );
    m_image->setFrameShadow( QFrame::Plain );
    m_image->setMinimumSize( QSize( 35, 35 ));
    m_image->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    
    installEventFilter( this );
    if( user.mediumImageUrl().isEmpty()) {
        QNetworkReply* reply = UserDetails::getInfo( user.name() );
        connect( reply, SIGNAL(finished()), SLOT( onUserDetailsFetched()));
    } else {
        QNetworkReply* reply = lastfm::nam()->get( QNetworkRequest( user.mediumImageUrl()));
        connect( reply, SIGNAL(finished()), SLOT( onImageLoaded()));
    }
    l->addWidget( m_name = new QLabel( user.name()), 0, Qt::AlignTop );
    l->addStretch();
    QPushButton* remove;
    l->addWidget( remove = new QPushButton( tr("Remove") ));
    connect( remove, SIGNAL(clicked()), SLOT(deleteLater()));
    QVBoxLayout* vl = new QVBoxLayout(this);
    vl->addStretch();
    vl->addLayout( l );
    vl->addStretch();
}

void 
UserRadioButton::onUserDetailsFetched()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>( sender());
    Q_ASSERT( reply );

    UserDetails user( reply );

    m_name->setText( QString( "%1 (%2)").arg( user.name()).arg(user.realName()));
    
    reply = lastfm::nam()->get( QNetworkRequest( user.mediumImageUrl()));
    connect( reply, SIGNAL(finished()), SLOT( onImageLoaded()));
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
    if( mouseEvent ) {
        int l,t,r,b;
        getContentsMargins( &l, &t, &r, &b );
        QMouseEvent mEvent( mouseEvent->type(), QPoint( l, height()/2 ), mouseEvent->button(), mouseEvent->buttons(), mouseEvent->modifiers());
        removeEventFilter( this );
        QCoreApplication::sendEvent( this, &mEvent );
        installEventFilter( this );
        return true;
    }
    return false;
}

UserManager::UserManager( QWidget* parent )
            :QDialog( parent )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setSpacing( 7 );
    layout->addWidget( new QLabel( tr( "Users authenticated with this application" )));

    QGroupBox* groupBox;
    layout->addWidget( groupBox = new QGroupBox());
    new QVBoxLayout( groupBox );
    //groupBox->setContentsMargins( 0, 0, 0, 0 );
    groupBox->setTitle( tr( "Log me in as:" ));

    groupBox->layout()->addWidget( new UserRadioButton( User() ));
    groupBox->layout()->addWidget( new UserRadioButton( User( "han")));
    groupBox->layout()->addWidget( new UserRadioButton( User( "eartle")));
    groupBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding );

    QHBoxLayout* actionButtons = new QHBoxLayout();
    QDialogButtonBox* bb;
    actionButtons->addWidget( bb = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel ));
    
    connect( bb->addButton( "+ " + tr( "Add new user" ), QDialogButtonBox::ActionRole ),
             SIGNAL(clicked()),
             SLOT(onAddUserClicked()));

    connect( bb, SIGNAL( accepted()), SLOT( accept()));
    connect( bb, SIGNAL( rejected()), SLOT( reject()));
    layout->addLayout( actionButtons );
}


void
UserManager::onAddUserClicked()
{
    qDebug() << "Adding user";
}
