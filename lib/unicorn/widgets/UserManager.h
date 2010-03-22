#ifndef USER_MANAGER_H_
#define USER_MANAGER_H_



#include <QDialog>
#include <QRadioButton>

namespace lastfm{ class User; }
class QLabel;
class QFrame;
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


class UserManager : public QDialog
{
Q_OBJECT
public:
    UserManager( QWidget* parent = 0 );
    ~UserManager();

signals:
    void rosterUpdated();

protected slots:
    void onAddUserClicked();
    void onLoginDialogAccepted();
    void onUserAdded();
    void onAccept();

protected:
    void add( UserRadioButton*, bool = true );

    class QButtonGroup* m_buttonGroup;

    struct {
        class QGroupBox* groupBox;
    } ui;
};

#endif //USER_MANAGER_H_
