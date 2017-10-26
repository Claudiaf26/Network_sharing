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

//gestisce l'aggiunta al menù a tendina su windows e linux mediante polimorfismo
class ContextMenu{
private:
    unique_ptr<ContextMenu_Interface> m_base;
    bool m_added;

    //Can't be copied, can't be assigned
    ContextMenu(const ContextMenu& s);
    ContextMenu& operator=(const ContextMenu& s);

public:
    ContextMenu(wstring);
    bool addToContextMenu();        //aggiunge al menù a tendina disponibile con il click destro di qualunque file/cartella l'applicazione
    bool removeFromContextMenu();   //toglie dal menù a tendina
    bool isAddedToContextMenu(){return m_added;}
};

#endif // CONTEXTMENU_H
