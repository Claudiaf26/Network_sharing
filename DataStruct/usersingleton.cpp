#include "usersingleton.h"
#include <sstream>
#include <future>
#include <locale>
#include <codecvt>
#include <algorithm>

using namespace std;

mutex UserSingleton::m_istantiationMutex;

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

UserSingleton::UserSingleton() : shared(false), m_sendingThread(nullptr), m_threadException(nullptr) {
    m_sharedMemory = unique_ptr<SharedMem>(new SharedMem());
}

UserSingleton::~UserSingleton(){
    if (m_sendingThread != nullptr)
        delete m_sendingThread;
}

bool UserSingleton::initialize(){
    auto result = async(std::launch::async, &SharedMem::createMem, m_sharedMemory.get());
    result.wait();
    shared = result.get();

    return shared;
}

void UserSingleton::close(){
    if(shared){
        m_sharedMemory->releaseMem();
        shared = false;
    }
}

bool UserSingleton::open(){
    auto result = async(std::launch::async, &SharedMem::openMem, m_sharedMemory.get());
    result.wait();
    shared = result.get();

    return shared;
}

vector<User> UserSingleton::threadGetter(){
    vector<User> newList;

    wstringstream stringRead(m_sharedMemory->getContent());
    wstring currentLine;
    getline(stringRead, currentLine);
    while(currentLine != L""){
        User newUser;
        newUser.name = WStringToString(currentLine);
        getline(stringRead, currentLine);;
        newUser.ip = WStringToString(currentLine);
        getline(stringRead, currentLine);
        newUser.age = stoi(currentLine);
        getline(stringRead, currentLine);
        newUser.picture = WStringToString(currentLine);
        newList.push_back(newUser);

        getline(stringRead, currentLine);
        getline(stringRead, currentLine);
    }

    return newList;
}

void UserSingleton::threadSetter(const vector<User>& newList){
    wstring serializedUser;

    for (auto it = newList.begin(); it != newList.end(); it++){
        User currentUser = *it;
        wstring currentline = StringToWString(currentUser.name+'\n');
        serializedUser.append(currentline);
        currentline = StringToWString(currentUser.ip+'\n');
        serializedUser.append(currentline);
        serializedUser.append(to_wstring(currentUser.age)+L"\n");
        currentline = StringToWString(currentUser.picture+'\n');
        serializedUser.append(currentline);
        serializedUser.append(L"\n");
    }

    serializedUser.append(L"\n");
    try{
    m_sharedMemory->setContent(serializedUser);
    }catch(runtime_error error){
        m_threadException = current_exception();
    }
}

vector<User> UserSingleton::getList(){
    auto result = async(std::launch::async, &UserSingleton::threadGetter, this);
    result.wait();
    try{
        return result.get();
    }catch(runtime_error err){
        throw err;
    }
}

void UserSingleton::setList(const vector<User>& newList){
    m_sendingThread = new thread(&UserSingleton::threadSetter, this, newList);
    m_sendingThread->join();

    if(m_threadException)
        rethrow_exception(m_threadException);
}
