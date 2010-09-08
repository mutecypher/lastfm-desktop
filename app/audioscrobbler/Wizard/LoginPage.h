/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Jono Cole

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef LOGIN_PAGE_H_
#define LOGIN_PAGE_H_

#include <QWizardPage>
#include <QAbstractButton>

namespace unicorn
{
    class LoginProcess;
    class Session;
}

class LoginPage : public QWizardPage
{
    Q_OBJECT
public:
    LoginPage( QWidget* parent = 0 );

    virtual void initializePage();
    virtual void cleanupPage();
    virtual bool isComplete() const;
 
private slots:
    void authenticate();
    void onAuthenticated( unicorn::Session* session );

private:

    struct {
        class QLabel* description;
        class QLabel* errorMsg;
        class QLabel* continueMsg;
        class QLabel* browserMsg;
        class QLineEdit* loginUrl;
        class QPushButton* okButton;
    } ui;

    unicorn::LoginProcess* m_loginProcess;
    bool m_isComplete;
};

#endif //LOGIN_PAGE_H_
