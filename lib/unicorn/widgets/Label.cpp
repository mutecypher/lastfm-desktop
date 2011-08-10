#include <QEvent>
#include <QResizeEvent>
#include <QPainter>

#include "Label.h"

Label::Label( QWidget* parent )
    :QLabel( parent )
{
    setAttribute( Qt::WA_LayoutUsesWidgetRect );
}


Label::Label( const QString& text, QWidget* parent )
    :QLabel( parent )
{
    setText( text );
    setAttribute( Qt::WA_LayoutUsesWidgetRect );
}


void
Label::setText( const QString& text )
{
    QLabel::setText( text );

    if ( textFormat() == Qt::RichText )
        QLabel::setText( QString( "<html><head><style type=text/css>"
                                 "a:link {color:white; text-decoration:none;}"
                                 "a:hover {color:white; text-decoration:none}"
                                 "</style></head><body>%1</body></html>" ).arg( m_text ) );
    else
        QLabel::setText( "" );

    m_text = text;
}

QString
Label::anchor( const QString& url, const QString& text )
{
    return QString( "<a title='yeah' href=\"%1\">%2</a>" ).arg( url, text );
}

void
Label::paintEvent( QPaintEvent* event )
{
    QLabel::paintEvent( event );

    if ( textFormat() == Qt::RichText )
        QLabel::paintEvent( event );
    else
    {
        QFrame::paintEvent(event);
        QPainter p(this);
        p.drawText( rect(), fontMetrics().elidedText( m_text, Qt::ElideRight, contentsRect().width() ) );
    }
}


