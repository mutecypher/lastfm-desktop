#include <QEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QToolTip>
#include <QUrl>
#include <QGraphicsDropShadowEffect>
#include <QDateTime>
#include <QTimer>

#include "../DesktopServices.h"

#include "Label.h"

unicorn::Label::Label( QWidget* parent )
    :QLabel( parent ), m_linkColor( QRgb( 0x333333 ) )
{
    setAttribute( Qt::WA_LayoutUsesWidgetRect );
    setAttribute( Qt::WA_MacNoClickThrough );

    setOpenExternalLinks( false );

    connect( this, SIGNAL(linkHovered(QString)), SLOT(onHovered(QString)));
    connect( this, SIGNAL(linkActivated(QString)), SLOT(onActivated(QString)));
}


unicorn::Label::Label( const QString& text, QWidget* parent )
    :QLabel( parent )
{
    setText( text );
    setAttribute( Qt::WA_LayoutUsesWidgetRect );
    setAttribute( Qt::WA_MacNoClickThrough );
    setOpenExternalLinks( false );

    connect( this, SIGNAL(linkHovered(QString)), SLOT(onHovered(QString)));
    connect( this, SIGNAL(linkActivated(QString)), SLOT(onActivated(QString)));
}

void
unicorn::Label::onActivated( const QString& url )
{
    unicorn::DesktopServices::openUrl( url );
}

void
unicorn::Label::onHovered( const QString& url )
{   
    QUrl displayUrl( url );
    QToolTip::showText( cursor().pos(), displayUrl.toString(), this, QRect() );
}

QString
unicorn::Label::boldLinkStyle( const QString& text, QColor linkColor )
{
    return QString( "<html><head><style type=text/css>"
                     "a:link {color:%1; font-weight: bold; text-decoration:none;}"
                     "a:hover {color:%1; font-weight: bold; text-decoration:none;}"
                     "</style></head><body>%2</body></html>" ).arg( linkColor.name(), text );
}

QString
unicorn::Label::boldLinkStyle( const QString& text )
{
    return boldLinkStyle( text, m_linkColor );
}

QString
unicorn::Label::text() const
{
    return m_text;
}

void
unicorn::Label::setText( const QString& text )
{
    m_text = text;

    if ( textFormat() == Qt::RichText )
        QLabel::setText( boldLinkStyle( m_text ) );
    else
        QLabel::setText( ""  );

    update();
}

void
unicorn::Label::setLinkColor( QColor linkColor )
{
    m_linkColor = linkColor;
}

QString
unicorn::Label::anchor( const QString& url, const QString& text )
{
    return QString( "<a href=\"%1\">%2</a>" ).arg( url, text );
}

void
unicorn::Label::prettyTime( Label& timestampLabel, const QDateTime& timestamp, QTimer* callback )
{
    QDateTime now = QDateTime::currentDateTime();

    // Full time in the tool tip
    timestampLabel.setToolTip( timestamp.toString( Qt::DefaultLocaleLongDate ) );

    int secondsAgo = timestamp.secsTo( now );

    if ( secondsAgo < (60 * 60) )
    {
        // Less than an hour ago
        int minutesAgo = ( timestamp.secsTo( now ) / 60 );
        timestampLabel.setText( tr( "%n minute(s) ago", "", minutesAgo ) );
        if ( callback ) callback->start( now.secsTo( timestamp.addSecs(((minutesAgo + 1 ) * 60 ) + 1 ) ) * 1000 );
    }
    else if ( secondsAgo < (60 * 60 * 6) || now.date() == timestamp.date() )
    {
        // Less than 6 hours ago or on the same date
        int hoursAgo = ( timestamp.secsTo( now ) / (60 * 60) );
        timestampLabel.setText( tr( "%n hour(s) ago", "", hoursAgo ) );
        if ( callback ) callback->start( now.secsTo( timestamp.addSecs( ( (hoursAgo + 1) * 60 * 60 ) + 1 ) ) * 1000 );
    }
    else if ( secondsAgo < (60 * 60 * 24 * 365) )
    {
        // less than a year ago
        timestampLabel.setText( timestamp.toString( Qt::DefaultLocaleShortDate ) );
        // We don't need to set the timer because this date will never change (well, it might in a year's time)
    }
    else
    {
        timestampLabel.setText( timestamp.toString( Qt::DefaultLocaleLongDate ) );
        // We don't need to set the timer because this date will never change
    }
}



QSize
unicorn::Label::sizeHint() const
{
    QSize sizeHint = QLabel::sizeHint();

    if ( textFormat() != Qt::RichText )
        sizeHint.setWidth( qMin ( sizeHint.width(), fontMetrics().width( m_text ) + 1 ) );

    return sizeHint;
}

void
unicorn::Label::paintEvent( QPaintEvent* event )
{
    if ( textFormat() == Qt::RichText )
        QLabel::paintEvent( event );
    else
    {
        QFrame::paintEvent(event);
        QPainter p(this);
        p.drawText( rect(), fontMetrics().elidedText( m_text, Qt::ElideRight, contentsRect().width() ) );
    }
}


