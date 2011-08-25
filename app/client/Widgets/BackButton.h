#ifndef BACKBUTTON_H
#define BACKBUTTON_H

#include <QPushButton>
#include <QPixmap>

class BackButton : public QPushButton
{
    Q_OBJECT
public:
    explicit BackButton(QWidget *parent = 0);

    void setDescription( const QString& description );

private:
    void paintEvent(QPaintEvent *);

private:
    QString m_description;
};

#endif // BACKBUTTON_H
