#ifdef _WIN32
#include "contextmenu_windows.h"
#include <Windows.h>
#pragma comment(lib,"Advapi32.lib")

inline void insertIntoString(wstring& str, wstring insert, wstring flag){
    size_t pos = str.find(flag);
    str.erase(pos, flag.size());
    str.insert(pos, insert);
}

inline void removeFromString(wstring& str, wstring remove){
    size_t pos = str.find(remove);
    str.erase(pos, remove.size());
}

ContextMenu_Windows::ContextMenu_Windows(wstring arg){
    contextString = arg;
    insertIntoString(fileRegister, contextString, L"%");
    insertIntoString(directoryRegister, contextString, L"%");
}

bool ContextMenu_Windows::addToContextMenu(){
    DWORD len = 1024;
    WCHAR path[1024];
    GetModuleFileNameW(NULL, path, len);
    wstring contextPath = path;
    contextPath += L" \"%1\"";

    HKEY fileHandle;
    HKEY directoryHandle;
    DWORD fileDisposition;
    DWORD directoryDisposition;
    LONG value = RegCreateKeyExW(HKEY_CLASSES_ROOT, fileRegister.c_str(), 0, NULL, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &fileHandle, &fileDisposition);
    if (value != ERROR_SUCCESS) return false;

    value = RegSetValueExW(fileHandle, NULL, 0, REG_SZ, (PBYTE)contextPath.c_str(), len);
    if (value != ERROR_SUCCESS) return false;

    value = RegCreateKeyExW(HKEY_CLASSES_ROOT, directoryRegister.c_str(), 0, NULL, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &directoryHandle, &directoryDisposition);
    if (value != ERROR_SUCCESS) return false;

    value = RegSetValueExW(directoryHandle, NULL, 0, REG_SZ, (PBYTE)contextPath.c_str(), len);
    if (value != ERROR_SUCCESS) return false;

    return true;
}

bool ContextMenu_Windows::removeFromContextMenu(){
    int value = RegDeleteKeyW(HKEY_CLASSES_ROOT, fileRegister.c_str());
    if (value != ERROR_SUCCESS) return false;

    value = RegDeleteKeyW(HKEY_CLASSES_ROOT, directoryRegister.c_str());
    if (value != ERROR_SUCCESS) return false;

    removeFromString(fileRegister, L"\\command");
    removeFromString(directoryRegister, L"\\command");

    value = RegDeleteKeyW(HKEY_CLASSES_ROOT, fileRegister.c_str());
    if (value != ERROR_SUCCESS) return false;

    value = RegDeleteKeyW(HKEY_CLASSES_ROOT, directoryRegister.c_str());
    if (value == ERROR_SUCCESS) return false;

    return true;
}
#endif
