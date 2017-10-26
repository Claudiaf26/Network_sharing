#include "contextmenu.h"

using namespace std;

ContextMenu::ContextMenu(wstring contextName): m_added(false){

#ifdef _WIN32
    m_base = make_unique<ContextMenu_Windows>(contextName);
#endif
#ifdef __linux__
    m_base = unique_ptr<ContextMenu_Linux>(new ContextMenu_Linux(contextName));
#endif
}

bool ContextMenu::addToContextMenu(){m_added = m_base->addToContextMenu(); return m_added;}
bool ContextMenu::removeFromContextMenu(){m_added = false; return m_base->removeFromContextMenu();}


