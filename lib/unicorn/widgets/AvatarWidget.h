#ifndef AVATARWIDGET_H
#define AVATARWIDGET_H

#include <lastfm/User.h>

#include "lib/unicorn/widgets/HttpImageWidget.h"

#include "lib/DllExportMacro.h"


class UNICORN_DLLEXPORT AvatarWidget : public HttpImageWidget
{
    Q_OBJECT
public:
    explicit AvatarWidget( QWidget* parent = 0 );

    void setUser( const lastfm::User& user );

private:
    void paintEvent( QPaintEvent *paintEvent );

private:
    lastfm::User m_user;
};

#endif // AVATARWIDGET_H
