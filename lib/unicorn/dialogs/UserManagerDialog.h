#ifndef USER_MANAGER_DIALOG_H_
#define USER_MANAGER_DIALOG_H_



#include <QDialog>
#include <QRadioButton>
#include "lib/DllExportMacro.h"

namespace lastfm{ class User; }

class UserManagerWidget;

class QLabel;
class QFrame;
class QPushButton;

class UNICORN_DLLEXPORT UserManagerDialog : public QDialog
{
Q_OBJECT
public:
    UserManagerDialog( QWidget* parent = 0 );
    ~UserManagerDialog();

signals:
    void rosterUpdated();

protected slots:
    void onAccept();

private:
    UserManagerWidget* m_users;
};

#endif //USER_MANAGER_DIALOG_H_
