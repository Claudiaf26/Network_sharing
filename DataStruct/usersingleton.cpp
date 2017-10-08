#include "usersingleton.h"
#include <sstream>
#include <future>
#include <codecvt>
#include <algorithm>

using namespace std;

mutex UserSingleton::istantiation_mutex;

inline string WStringToString(const wstring& s){
    #ifdef _WIN32
    wstring_convert<codecvt_utf8_utf16<wchar_t>> convert;
    string temp = convert.to_bytes(s);
    #endif
    #ifdef __linux__
    setlocale(LC_ALL, "");
    const locale locale("");
    typedef codecvt<wchar_t, char, mbstate_t> converter_type;
    const converter_type& converter = use_facet<converter_type>(locale);
    vector<char> to(s.length() * converter.max_length());
    mbstate_t state;
    const wchar_t* from_next;
    char* to_next;
    converter.out(state, s.data(), s.data() + s.length(), from_next, &to[0], &to[0] + to.size(), to_next);
    string temp(to.begin(), to.end());
    #endif
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

vector<User> UserSingleton::threadGetter(){
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

void UserSingleton::threadSetter(const vector<User>& newList){
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
    auto result = async(std::launch::async, &UserSingleton::threadGetter, this);
    result.wait();
    return result.get();
}

void UserSingleton::setList(const vector<User>& newList){
    t = new thread(&UserSingleton::threadSetter, this, newList);
    t->detach();
}
