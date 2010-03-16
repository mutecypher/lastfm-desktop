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

protected slots:
    void onUserDetailsFetched();
    void onImageLoaded();

protected:
    bool eventFilter( QObject* obj, QEvent* event );

protected:
    QLabel* m_name;
    QLabel* m_image;
    QFrame* m_f;
};


class UserManager : public QDialog
{
Q_OBJECT
public:
    UserManager( QWidget* parent = 0 );

protected slots:
    void onAddUserClicked();
    void onLoginDialogAccepted();
    void onUserAdded();

protected:
    struct {
        class QGroupBox* groupBox;
    } ui;
};

#endif //USER_MANAGER_H_
