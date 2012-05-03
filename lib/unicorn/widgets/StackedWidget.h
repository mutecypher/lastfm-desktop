#ifndef STACKEDWIDGET_H
#define STACKEDWIDGET_H

#include <QStackedWidget>

#include "lib/DllExportMacro.h"

namespace unicorn
{

class UNICORN_DLLEXPORT StackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit StackedWidget(QWidget *parent = 0);
    
private slots:
    void onCurrentChanged( int index );

private:
    QSize sizeHint() const;
};

}

#endif // STACKEDWIDGET_H
