#ifndef UNICORN_DIALOG_H_
#define UNICORN_DIALOG_H_

#include <QDialog>
#include <QApplication>
#include <QDesktopWidget>
#include "../UnicornMainWindow.h"
#include "../UnicornSettings.h"

namespace unicorn {
class Dialog : public QDialog
{
Q_OBJECT
public:
    Dialog( QWidget* parent = 0, Qt::WindowFlags f = 0 )
    :QDialog( parent, f ), m_firstShow( true ) {}

    virtual void setVisible( bool visible )
    {
        if( !visible || !m_firstShow ) return QDialog::setVisible( visible );
        m_firstShow = false;

        //Clever positioning
        QWidget* mw = findMainWindow();

        if( mw ) {
            resize( sizeHint() );

            AppSettings s;
            s.beginGroup( QString( metaObject()->className() ));
                QPoint offset = s.value( "position", QPoint( 40, 40 )).toPoint();
            s.endGroup();

            move( mw->pos() + offset);

            int screenNumber = qApp->desktop()->screenNumber( mw );
            QRect screenRect = qApp->desktop()->availableGeometry( screenNumber );
            if( !screenRect.contains( frameGeometry(), true )) {
                QRect diff = frameGeometry().intersected( screenRect );
                int xDir = (diff.left() == screenRect.left() ? 1 : -1 );
                int yDir = (diff.top() == screenRect.top() ? 1 : -1 );
                QPoint adjust = QPoint((frameGeometry().width() - diff.width() ) * xDir, (frameGeometry().height() - diff.height()) * yDir);
                qDebug() << "adjust =" << adjust;
                move( pos() + adjust );
            }
        }
        return setVisible( visible );
    }

protected:
    virtual void moveEvent( QMoveEvent* event )
    {
        using unicorn::MainWindow;
        QWidget* mw = findMainWindow();
        if( !mw ) return QDialog::moveEvent( event );

        AppSettings s;
        s.beginGroup( QString( metaObject()->className() ));
            s.setValue( "position", (pos() - mw->pos()));
        s.endGroup();
    }

private:
    bool m_firstShow;
    QWidget* findMainWindow() const
    {
        unicorn::MainWindow* mw = 0;
        foreach( QWidget* w, qApp->topLevelWidgets() ) {
            mw = qobject_cast<unicorn::MainWindow*>( w );
            if( mw ) break;
        }
        return mw;
    };
};

} //unicorn

#endif //UNICORN_DIALOG_H_
