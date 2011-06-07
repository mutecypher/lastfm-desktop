#ifndef DRAWER_H_
#define DRAWER_H_

#include <QDockWidget>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QTimeLine>
#include <QApplication>

#ifdef Q_OS_WIN
#define FAKE_DRAWER
#include <windows.h>
#endif

class Drawer : public QDockWidget {
Q_OBJECT
public:
    Drawer( QWidget* parent );

#ifdef FAKE_DRAWER
protected:
    bool eventFilter( QObject* obj, QEvent* event );
    void showEvent( QShowEvent* event );
    void closeEvent( QCloseEvent* event );

private slots:
    void onFrameChanged( int frame );
    void onTimeLineFinished();
    void onDockLocationChanged( Qt::DockWidgetArea );

private:
    QTimeLine* m_timeLine;
    Qt::DockWidgetArea m_dockWidgetArea;

#endif
};

#endif //DRAWER_H_
