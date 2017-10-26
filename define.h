#ifndef DEFINE_H
#define DEFINE_H

//contiene definizioni generali utilizzate da più oggetti

//controlla che l'oggetto sia stato creato prima di cancellarlo
template <class T>
inline void safeDelete(T object){
    if (object != nullptr)
        delete object;
}

//flag per il passaggio dati da mainprogram a settings
enum CurrentUserFlags {
    AUTOMATIC_FLAG  =       1,
    NOTIFICATION_FLAG   =   2,
    PRIVATE_FLAG        =   4,
};
typedef enum CurrentUserFlags CurrentUserFlags;

//struttura comune per gli utenti
#include <string>
struct User {
    std::string ip;
    std::string name;
    int16_t age;
    std::string picture;

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

enum FileTransfer_Status {
    FT_COMPLETE = 100,
    FT_ERROR = 101,
};
typedef enum FileTransfer_Status FileTransfer_Status;

enum FileTransfer_Type {
	FT_DIRECTORY = 0,
	FT_FILE = 1,
};
typedef enum FileTransfer_Type FileTransfer_Type;

const std::string iconString = ":/images/icons/Icons/";

#endif // DEFINE_H
