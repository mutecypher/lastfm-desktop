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

        QString text = tr( "Subscriber" );
        QColor brush( Qt::black );

//        if ( m_user.isAlumi() )
//        {
//            text = m_user.gender().female() ? tr( "Alumna" ) : tr( "Alumnus" );
//            brush = QColor( 0x5336BD );
//        }
//        else if ( m_user.isStaff() )
//        {
//            text = tr( "Staff" );
//            brush = QColor( 0xD51007 );
//        }

        QFont font = p.font();
        font.setPixelSize( 10 );
        font.setWeight( QFont::Bold );
        p.setFont( font );

        QFontMetrics fm( font );
        int width = fm.width( text ) + 4;
        int height = fm.height() + 4;
        QRect rect( (70 / 2) - (width / 2), 70 - (height / 2), width, height ) ;

        p.setBrush( brush );
        p.drawRoundedRect( rect, 2, 2 );

        p.setPen( QColor( Qt::white ) );
        p.drawText( rect, Qt::AlignCenter, text );
    }
}
