#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <vector>
#include <string>

void PrintImagePaths() {
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services"), 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        std::wcerr << L"Eroare la deschiderea registry!\n";
        return;
    }

    DWORD index = 0;
    TCHAR subKeyName[256];
    DWORD subKeySize;
    FILETIME ftLastWriteTime;

    while (true) {
        subKeySize = 256;
        if (RegEnumKeyEx(hKey, index++, subKeyName, &subKeySize, NULL, NULL, NULL, &ftLastWriteTime) == ERROR_NO_MORE_ITEMS) {
            break;
        }

        HKEY hSubKey;
        if (RegOpenKeyEx(hKey, subKeyName, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
            TCHAR imagePath[512];
            DWORD imagePathSize = sizeof(imagePath);
            DWORD type;

            if (RegQueryValueEx(hSubKey, TEXT("ImagePath"), NULL, &type, reinterpret_cast<LPBYTE>(imagePath), &imagePathSize) == ERROR_SUCCESS) {
                std::wcout << L"Subcheie: " << subKeyName << L"\nImagePath: " << imagePath << L"\n\n";
            }
            RegCloseKey(hSubKey);
        }
    }
    RegCloseKey(hKey);
}

int main() {
    PrintImagePaths();
    return 0;
}
