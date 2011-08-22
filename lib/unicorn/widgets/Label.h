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

    static QString anchor( const QString& url, const QString& text );
    static QString boldLinkStyle( const QString& text );

private:
    void paintEvent( QPaintEvent* event );

private:
    QString m_text;
};

#endif // LABEL_H
