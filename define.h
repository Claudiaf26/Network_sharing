#ifndef DEFINE_H
#define DEFINE_H

//contiene definizioni generali utilizzate da più oggetti


//flag per il passaggio dati da mainprogram a settings

enum Settings_Status {
    AUTOMATIC_FLAG = 1,
    NOTIFICATION_FLAG = 2,
    PRIVATE_FLAG = 4
};

typedef enum Settings_Status Settings_Status;

//flag per la ricezione/invio dei file
enum FileTransfer_Status {
    FT_COMPLETE = 100,
    FT_ERROR = 101,
};

typedef enum FileTransfer_Status FileTransfer_Status;

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


#endif // DEFINE_H
