#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>

namespace unicorn { class Session; }

class SideBar : public QWidget
{
    Q_OBJECT
private:
    struct
    {
        class QRadioButton* np;
        class QRadioButton* s;
        class QRadioButton* r;
    } ui;

public:
    explicit SideBar(QWidget *parent = 0);

signals:
    void currentChanged( int index );

public slots:
    void onButtonClicked();
    void onSessionChanged( unicorn::Session* newSession );
    void onGotAvatar();
};

#endif // SIDEBAR_H
