#ifndef CONTEXTMENU_INTERFACE_H
#define CONTEXTMENU_INTERFACE_H

#include <string>

class ContextMenu_Interface{
protected:
    std::wstring m_contextString;
public:
    virtual bool addToContextMenu() = 0;
    virtual bool removeFromContextMenu() = 0;
};

#endif // CONTEXTMENU_INTERFACE_H
