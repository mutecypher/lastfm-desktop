#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QPushButton>

class PushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit PushButton(QWidget *parent = 0);
    
private:
    bool event(QEvent *e);
    void paintEvent(QPaintEvent *);

private:
    bool m_hovered;
};

#endif // PUSHBUTTON_H
