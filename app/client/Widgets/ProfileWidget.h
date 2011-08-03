#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QPointer>
#include <QWidget>

#include "lib/unicorn/UnicornSession.h"

class ProfileWidget : public QWidget
{
    Q_OBJECT
private:
    struct
    {
        class HttpImageWidget* avatar;
        class QLabel* name;
        class QLabel* scrobbleCount;
        class QLabel* scrobbles;

        class StylableWidget* topWeeklyArtists;
        class StylableWidget* topOverallArtists;
    } ui;

public:
    explicit ProfileWidget(QWidget *parent = 0);

private slots:
    void onSessionChanged( unicorn::Session* session );

    void onGotTopWeeklyArtists();
    void onGotTopOverallArtists();

private:
    QPointer<QWidget> m_main;
};

#endif // PROFILEWIDGET_H
