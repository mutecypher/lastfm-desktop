#ifndef SIMILARARTISTWIDGET_H
#define SIMILARARTISTWIDGET_H

#include "lib/unicorn/widgets/HttpImageWidget.h"

class SimilarArtistWidget : public HttpImageWidget
{
    Q_OBJECT
public:
    explicit SimilarArtistWidget(QWidget *parent = 0);

    void setArtist( const QString& artist );
    
private:
    void paintEvent(QPaintEvent * event);

private:
    QString m_artist;
};

#endif // SIMILARARTISTWIDGET_H
