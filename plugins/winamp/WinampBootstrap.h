#pragma once

#include "..\ScrobSub\Bootstrap.h"

class WinampBootStrap :
    public BootStrap
{
public:
    WinampBootStrap();
    ~WinampBootStrap(void);

    void sethWndWinampML( HWND hWndWinampML );

private:
    HWND m_hWndWinampML;
    bool start();
};
