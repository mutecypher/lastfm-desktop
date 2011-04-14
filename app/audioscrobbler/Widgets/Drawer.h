#ifndef DRAWER_H_
#define DRAWER_H_

#include <QWidget>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QTimeLine>
#include <QApplication>

#ifdef Q_OS_WIN
#define FAKE_DRAWER
#include <windows.h>
#endif

class Drawer : public QWidget {
Q_OBJECT
public:
    Drawer( QWidget* parent );

protected:

#ifdef FAKE_DRAWER
    bool eventFilter( QObject* obj, QEvent* event );
    bool event( QEvent* event );
#endif

private slots:
#ifdef FAKE_DRAWER
    void onFrameChanged( int frame );
    void onTimeLineFinished();
#endif
private:
#ifdef FAKE_DRAWER
    QTimeLine* timeLine;
#endif
};

#endif //DRAWER_H_
