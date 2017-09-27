#ifndef SHAREDSINGLETON
#define SHAREDSINGLETON
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

using namespace std;

struct User {
    string ip;
    string name;
    int16_t age;
	string picture;

    bool operator==(const User& a) const {
        /*IMPORTANT, the age and the picture are not compared! It has no sense to compare based on them!*/
        return (!ip.compare(a.ip) && (!name.compare(a.name)));
    }
};


enum UDP_Discover_Status {
    UDS_STOP = 0,
    UDS_HIDDEN = 1,
    UDS_ACTIVE = 2,
};
typedef enum UDP_Discover_Status UDP_Discover_Status;

class ConnectionSingleton {
private:
    //la coda dei nuovi utenti connessi
    queue<User> newConnection;
    //la coda degli ultimi utenti disconnessi
    queue<User> deletedConnection;

    //il mutex e la condition variable per la sincronizzazione
    mutex m;
    condition_variable cv;
    static mutex istantiation_mutex;

    //costruttore dichiarato privato in modo che non si possano costruire esternamente oggetti di questa classe
    ConnectionSingleton() { };
    //il costruttore di copia dichiarato privato in modo che non si possa acquisire un oggetto di questa classe altrove
    ConnectionSingleton(const ConnectionSingleton&);
    //overload dell'operatore di assegnazione dichiarato privato in modo che non si possa passare come argomento l'oggetto
    void operator=(const ConnectionSingleton&);
public:
    //il metodo statico che genera l'istanza la prima volta che viene richiamato e la restituisce ogni volta
    static ConnectionSingleton& get_instance() {
        //rendiamo atomica l'istanziazione dell'oggetto singleton
        lock_guard<mutex> l( istantiation_mutex );
        //questo è l'unico oggetto statico della classe, creato la prima volta che viene chiamata la funzione
        static ConnectionSingleton istance;

        return istance;
      }

    void pushNew(User);
    void pushDeleted(User);
    bool popNew(User&);
    bool popDeleted(User&);
};

#endif
