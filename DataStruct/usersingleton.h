#ifndef USERSINGLETON_H
#define USERSINGLETON_H
#include "define.h"
#include "SharedMemory/sharedmem.h"
#include <memory>
#include <mutex>
#include <thread>
#include <string>
#include <vector>
#include <exception>

class UserSingleton {
private:
    //attributo che indica se la memoria condivisa è aperta
    bool shared;

    //gestore della memoria condivisa fra processi
     std::unique_ptr<SharedMem> m_sharedMemory;

    //il mutex per l'istanziazione
    static std::mutex m_istantiationMutex;

    std::thread* m_sendingThread;

    //serve per catturare l'eccezione catturata nel thread secondaria ed inviarla al main thread
    std::exception_ptr m_threadException;

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
        std::lock_guard<std::mutex> l( m_istantiationMutex );
        //questo è l'unico oggetto statico della classe, creato la prima volta che viene chiamata la funzione
        static UserSingleton istance;

        return istance;
      }

    bool initialize();                      //inizializza un'area di memoria condivisa
    void close();                           //chiude l'area di memoria condivisa
    bool open();                            //apre l'area di memoria condivisa
    std::vector<User> getList();            //ottiene una lista di utenti dall'area di memoria condivisa
    void setList(const std::vector<User>&); //serializza gli utenti connessi e li inserisce nell'area di memoria condivisa

};

#endif // USERSINGLETON_H
