#include "AvatarWidget.h"

AvatarWidget::AvatarWidget( QWidget *parent ) :
    HttpImageWidget(parent)
{
}

void
AvatarWidget::setUserDetails( const lastfm::UserDetails& user )
{
    m_user = user;
    update();
}

void
AvatarWidget::paintEvent( QPaintEvent* paintEvent )
{
    HttpImageWidget::paintEvent( paintEvent );

    if ( m_user.isSubscriber() )
    {
        QPainter p( this );

        p.setPen( QColor( 0xffffff ) );

        QFont font = p.font();
        font.setPixelSize( 8 );
        p.setFont( font );

        p.drawText( rect(), Qt::AlignHCenter | Qt::AlignBottom, tr( "Subscriber" ) );
    }
}
