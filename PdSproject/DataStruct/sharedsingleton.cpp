#include "sharedsingleton.h"

using namespace std;

mutex ConnectionSingleton::istantiation_mutex;

void ConnectionSingleton::pushNew(User user){
    lock_guard<mutex> lg(m);
    newConnection.push(user);
    cv.notify_one();
}

void ConnectionSingleton::pushDeleted(User user){
    lock_guard<mutex> lg(m);
    deletedConnection.push(user);
    cv.notify_one();
}

bool ConnectionSingleton::popNew(User& user){
    unique_lock<mutex> ul(m);
    cv.wait(ul);
    if (newConnection.empty())
        return false;
    user = newConnection.front();
    newConnection.pop();
    return true;
}

bool ConnectionSingleton::popDeleted(User& user){
    unique_lock<mutex> ul(m);
    cv.wait(ul);
    if (deletedConnection.empty())
        return false;
    user = deletedConnection.front();
    deletedConnection.pop();
    return true;
}
