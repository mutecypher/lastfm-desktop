#ifndef REFRESHBUTTON_H
#define REFRESHBUTTON_H

#include <QPushButton>
#include <QPixmap>

class RefreshButton : public QPushButton
{
    Q_OBJECT
public:
    explicit RefreshButton(QWidget *parent = 0);

private:
    void paintEvent(QPaintEvent *);

private:
    QPixmap m_pixmap;
};

#endif // REFRESHBUTTON_H
