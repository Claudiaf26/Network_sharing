#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#ifdef _WIN32
#include "ContextMenu/contextmenu_windows.h"
#endif

#ifdef __linux__
#include "ContextMenu/contextmenu_linux.h"
#endif

#include <memory>
#include <string>

class ContextMenu{
private:
    unique_ptr<ContextMenu_Interface> base;

    //Can't be copied, can't be assigned
    ContextMenu(const ContextMenu& s);
    ContextMenu& operator=(const ContextMenu& s);

public:
    ContextMenu(wstring);
    bool addToContextMenu();
    bool removeFromContextMenu();
};

#endif // CONTEXTMENU_H
