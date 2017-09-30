#include "sharedsingleton.h"

using namespace std;

mutex UserListSingleton::istantiation_mutex;

void UserListSingleton::pushNew(User user){
    lock_guard<mutex> lg(m);
    newConnection.push(user);
    cv.notify_one();
}

void UserListSingleton::pushDeleted(User user){
    lock_guard<mutex> lg(m);
    deletedConnection.push(user);
    cv.notify_one();
}

bool UserListSingleton::popNew(User& user){
    unique_lock<mutex> ul(m);
    cv.wait(ul);
    if (newConnection.empty())
        return false;
    user = newConnection.front();
    newConnection.pop();
    return true;
}

bool UserListSingleton::popDeleted(User& user){
    unique_lock<mutex> ul(m);
    cv.wait(ul);
    if (deletedConnection.empty())
        return false;
    user = deletedConnection.front();
    deletedConnection.pop();
    return true;
}
