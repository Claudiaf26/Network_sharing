#include "usersingleton.h"
#include <sstream>
#include <future>
#include <codecvt>
#include <algorithm>

using namespace std;

mutex UserSingleton::istantiation_mutex;

inline string WStringToString(const wstring& s){
    wstring_convert<codecvt_utf8_utf16<wchar_t>> convert;
    string temp = convert.to_bytes(s);
    return temp;
 }

inline wstring StringToWString(const string& s){
 wstring temp(s.length(),' ');
 copy(s.begin(), s.end(), temp.begin());
 return temp;
 }

UserSingleton::UserSingleton() : shared(false) {
    t = nullptr;
    memory = unique_ptr<SharedMem>(new SharedMem());
}

UserSingleton::~UserSingleton(){
    if (t != nullptr)
        delete t;
    if(shared)
        memory->releaseMem();
    memory.release();
}

bool UserSingleton::initialize(){
    auto result = async(std::launch::async, &SharedMem::createMem, memory.get());
    result.wait();
    shared = result.get();

    return shared;
}

bool UserSingleton::open(){
    auto result = async(std::launch::async, &SharedMem::openMem, memory.get());
    result.wait();
    shared = result.get();

    return shared;
}

vector<User> UserSingleton::getThread(){
    vector<User> newList;

    wstringstream stringRead(memory->getContent());
    wstring currentLine;
    getline(stringRead, currentLine);
    while(currentLine != L""){
        User newUser;
        newUser.name = WStringToString(currentLine);
        getline(stringRead, currentLine);;
        newUser.ip = WStringToString(currentLine);
        getline(stringRead, currentLine);
        newUser.age = stoi(currentLine);
        newList.push_back(newUser);

        getline(stringRead, currentLine);
        getline(stringRead, currentLine);
    }

    return newList;
}

void UserSingleton::setThread(const vector<User>& newList){
    wstring serializedUser;

    for (auto it = newList.begin(); it != newList.end(); it++){
        User currentUser = *it;
        wstring currentline = StringToWString(currentUser.name+'\n');
        serializedUser.append(currentline);
        currentline = StringToWString(currentUser.ip+'\n');
        serializedUser.append(currentline);
        serializedUser.append(to_wstring(currentUser.age)+L"\n");
        serializedUser.append(L"\n");
    }

    serializedUser.append(L"\n");

    memory->setContent(serializedUser);
}

vector<User> UserSingleton::getList(){
    auto result = async(std::launch::async, &UserSingleton::getThread, this);
    result.wait();
    return result.get();
}

void UserSingleton::setList(const vector<User>& newList){
    t = new thread(&UserSingleton::setThread, this, newList);
    t->detach();
}
