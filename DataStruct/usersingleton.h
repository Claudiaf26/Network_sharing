#ifndef USERSINGLETON_H
#define USERSINGLETON_H
#include "define.h"
#include "SharedMemory/sharedmem.h"
#include <memory>
#include <mutex>
#include <thread>
#include <string>
#include <vector>

class UserSingleton {
private:
    //attributo che indica se la memoria condivisa è aperta
    bool shared;

    //gestore della memoria condivisa fra processi
     std::unique_ptr<SharedMem> memory;

    //il mutex per l'istanziazione
    static std::mutex istantiation_mutex;

    std::thread* t;

    //costruttore dichiarato privato in modo che non si possano costruire esternamente oggetti di questa classe
    UserSingleton();
    //il costruttore di copia dichiarato privato in modo che non si possa acquisire un oggetto di questa classe altrove
    UserSingleton(const UserSingleton&);
    //overload dell'operatore di assegnazione dichiarato privato in modo che non si possa passare come argomento l'oggetto
    void operator=(const UserSingleton&);
    ~UserSingleton();

    //funzioni ausiliarie per eseguire certe operazioni in un thread separato
    std::vector<User> threadGetter();
    void threadSetter(const std::vector<User>&);


public:
    //il metodo statico che genera l'istanza la prima volta che viene richiamato e la restituisce ogni volta
    static UserSingleton& get_instance() {
        //rendiamo atomica l'istanziazione dell'oggetto singleton
        std::lock_guard<std::mutex> l( istantiation_mutex );
        //questo è l'unico oggetto statico della classe, creato la prima volta che viene chiamata la funzione
        static UserSingleton istance;

        return istance;
      }

    bool initialize();
    void close();
    bool open();
    std::vector<User> getList();
    void setList(const std::vector<User>&);

};

#endif // USERSINGLETON_H
