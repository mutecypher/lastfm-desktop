#include <QEvent>

#include "Label.h"

Label::Label( QWidget *parent )
    :QLabel( parent )
{
    setTextFormat( Qt::RichText );
}

void
Label::setText( const QString& text )
{
    m_text = text;
    resizeEvent(0);
}

QString
Label::anchor( const QString& url, const QString& text )
{
    return QString( "<a title='yeah' href=\"%1\">%2</a>" ).arg( url, text );
}

void
Label::resizeEvent( QResizeEvent* event )
{
    if ( textFormat() != Qt::RichText )
        QLabel::setText( fontMetrics().elidedText( m_text, Qt::ElideRight, parentWidget()->contentsRect().width() ) );
    else
        QLabel::setText( QString( "<html><head><style type=text/css>"
                                 "a:link {color:white; text-decoration:none;}"
                                 "a:hover {color:white; text-decoration:none}"
                                 "</style></head><body>%1</body></html>" ).arg( m_text ) );
}

