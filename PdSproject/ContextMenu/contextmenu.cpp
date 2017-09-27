#include "contextmenu.h"

using namespace std;

ContextMenu::ContextMenu(wstring contextName){

#ifdef _WIN32
    base = make_unique<ContextMenu_Windows>(contextName);
#endif
#ifdef __linux__
    base = unique_ptr<ContextMenu_Linux>(new ContextMenu_Linux(contextName));
#endif
}

bool ContextMenu::addToContextMenu(){return base->addToContextMenu();}
bool ContextMenu::removeFromContextMenu(){return base->removeFromContextMenu();}


