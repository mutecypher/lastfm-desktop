#ifndef RADIOWIDGET_H
#define RADIOWIDGET_H

#include <QWidget>

namespace unicorn { class Session; };

class RadioWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RadioWidget(QWidget *parent = 0);

signals:

private slots:
    void onSessionChanged( unicorn::Session* session );

};

#endif // RADIOWIDGET_H
