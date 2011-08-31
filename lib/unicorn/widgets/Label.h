#ifndef LABEL_H
#define LABEL_H

#include <QLabel>

#include "lib/DllExportMacro.h"

class UNICORN_DLLEXPORT Label : public QLabel
{
    Q_OBJECT
public:
    explicit Label( QWidget* parent = 0 );
    explicit Label( const QString& text, QWidget* parent = 0 );

    void setText( const QString& text );
    void setLinkColor( QColor linkColor );

    static QString anchor( const QString& url, const QString& text );
    static QString boldLinkStyle( const QString& text, QColor linkColor );

private:
    void paintEvent( QPaintEvent* event );

    QString boldLinkStyle( const QString& text );

private slots:
    void onHovered( const QString& url );

private:
    QString m_text;
    QColor m_linkColor;
};

#endif // LABEL_H
