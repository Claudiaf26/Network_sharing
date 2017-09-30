#ifndef USERLISTSINGLETON
#define USERLISTSINGLETON
#include "define.h"
#include <queue>
#include <mutex>
#include <condition_variable>

class UserListSingleton {
private:
    //la coda dei nuovi utenti connessi
    std::queue<User> newConnection;
    //la coda degli ultimi utenti disconnessi
    std::queue<User> deletedConnection;

    //il mutex e la condition variable per la sincronizzazione
    std::mutex m;
    std::condition_variable cv;
    static std::mutex istantiation_mutex;

    //costruttore dichiarato privato in modo che non si possano costruire esternamente oggetti di questa classe
    UserListSingleton() { };
    //il costruttore di copia dichiarato privato in modo che non si possa acquisire un oggetto di questa classe altrove
    UserListSingleton(const UserListSingleton&);
    //overload dell'operatore di assegnazione dichiarato privato in modo che non si possa passare come argomento l'oggetto
    void operator=(const UserListSingleton&);
public:
    //il metodo statico che genera l'istanza la prima volta che viene richiamato e la restituisce ogni volta
    static UserListSingleton& get_instance() {
        //rendiamo atomica l'istanziazione dell'oggetto singleton
        std::lock_guard<std::mutex> l( istantiation_mutex );
        //questo è l'unico oggetto statico della classe, creato la prima volta che viene chiamata la funzione
        static UserListSingleton istance;

        return istance;
      }

    void pushNew(User);
    void pushDeleted(User);
    bool popNew(User&);
    bool popDeleted(User&);
};

#endif
