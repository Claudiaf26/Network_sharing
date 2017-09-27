#ifdef _WIN32
#ifndef CONTEXTMENU_WINDOWS_H
#define CONTEXTMENU_WINDOWS_H

#include "ContextMenu/contextmenu_interface.h"
#include <string>

using namespace std;

class ContextMenu_Windows : public ContextMenu_Interface{
private:
    wstring fileRegister = L"*\\shell\\%\\command";
    wstring directoryRegister = L"Directory\\shell\\%\\command";
public:
    ContextMenu_Windows(wstring);
    virtual bool addToContextMenu();
    virtual bool removeFromContextMenu();
};

#endif  // CONTEXTMENU_WINDOWS_H
#endif
