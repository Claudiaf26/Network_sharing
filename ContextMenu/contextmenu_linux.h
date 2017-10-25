#ifdef __linux__
#ifndef CONTEXTMENU_LINUX_H
#define CONTEXTMENU_LINUX_H

#include "ContextMenu/contextmenu_interface.h"
#include <string>

using namespace std;

class ContextMenu_Linux : public ContextMenu_Interface {
private:
    const string m_registrationPath = "/.local/share/file-manager/actions/";
    string m_completeRegistrationPath;
public:
    ContextMenu_Linux(wstring);
    virtual bool addToContextMenu();
    virtual bool removeFromContextMenu();
};

#endif // CONTEXTMENU_LINUX_H
#endif
