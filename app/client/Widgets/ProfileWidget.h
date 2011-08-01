#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>

#include "lib/unicorn/UnicornSession.h"

class ProfileWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProfileWidget(QWidget *parent = 0);

private slots:
    void onSessionChanged( unicorn::Session* session );

};

#endif // PROFILEWIDGET_H
