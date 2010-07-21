#ifndef USER_MANAGER_WIDGET_H_
#define USER_MANAGER_WIDGET_H_



#include <QWidget>
#include <QRadioButton>
#include "lib/DllExportMacro.h"

namespace lastfm{ class User; }

class QAbstractButton;
class QButtonGroup;
class QFrame;
class QLabel;
class QPushButton;

class UserRadioButton : public QRadioButton
{
Q_OBJECT

public:
    UserRadioButton( const lastfm::User& user );
    const QString user() const;

protected slots:
    void onUserDetailsFetched();
    void onImageLoaded();
    void removeMe();

protected:
    bool eventFilter( QObject* obj, QEvent* event );

protected:
    QString m_userName;
    QLabel* m_name;
    QLabel* m_realName;
    QLabel* m_loggedIn;
    QLabel* m_image;
    QFrame* m_f;
};


class UNICORN_DLLEXPORT UserManagerWidget : public QWidget
{
Q_OBJECT
public:
    UserManagerWidget( QWidget* parent = 0 );
    ~UserManagerWidget();

    QAbstractButton* checkedButton() const;

signals:
    void rosterUpdated();

protected slots:
    void onAddUserClicked();
    void onLoginDialogAccepted();
    void onUserAdded();

protected:
    void add( UserRadioButton*, bool = true );

    struct {
        class QGroupBox* groupBox;
    } ui;

private:
    void setTabOrders();

private:
    QButtonGroup* m_buttonGroup;
    QPushButton* m_addUserButton;
};

#endif //USER_MANAGER_WIDGET_H_
