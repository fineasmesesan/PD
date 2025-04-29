#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

void ProcessSubKey(HKEY hKeyServices, const std::wstring& subKeyName);

int wmain() {
    HKEY hKeyServices;
    LONG lResult = RegOpenKeyExW(
        HKEY_LOCAL_MACHINE,
        L"SYSTEM\\CurrentControlSet\\Services",
        0,
        KEY_READ,
        &hKeyServices
    );

    if (lResult != ERROR_SUCCESS) {
        std::wcerr << L"Erroare la deschiderea cheii Services: " << lResult << std::endl;
        return 1;
    }

    DWORD cSubKeys = 0, maxSubKeyLen = 0;
    lResult = RegQueryInfoKeyW(
        hKeyServices, nullptr, nullptr, nullptr,
        &cSubKeys, &maxSubKeyLen, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr
    );

    if (lResult != ERROR_SUCCESS) {
        std::wcerr << L"Nu s-au putut obține informații despre cheie." << std::endl;
        RegCloseKey(hKeyServices);
        return 1;
    }

    maxSubKeyLen++; // pentru null-terminator
    std::vector<wchar_t> subKeyName(maxSubKeyLen);

    for (DWORD i = 0; i < cSubKeys; ++i) {
        DWORD nameLen = maxSubKeyLen;
        if (RegEnumKeyExW(
            hKeyServices,
            i,
            subKeyName.data(),
            &nameLen,
            nullptr,
            nullptr,
            nullptr,
            nullptr
        ) == ERROR_SUCCESS) {
            ProcessSubKey(hKeyServices, std::wstring(subKeyName.data(), nameLen));
        }
    }

    RegCloseKey(hKeyServices);
    return 0;
}

void ProcessSubKey(HKEY hKeyServices, const std::wstring& subKeyName) {
    HKEY hSubKey;
    if (RegOpenKeyExW(hKeyServices, subKeyName.c_str(), 0, KEY_READ, &hSubKey) != ERROR_SUCCESS) {
        std::wcerr << L"Nu s-a putut deschide subcheia: " << subKeyName << std::endl;
        return;
    }

    DWORD type = 0;
    DWORD dataSize = 0;
    LONG lResult = RegQueryValueExW(hSubKey, L"ImagePath", nullptr, &type, nullptr, &dataSize);
    if (lResult == ERROR_SUCCESS && (type == REG_SZ || type == REG_EXPAND_SZ)) {
        std::unique_ptr<wchar_t[]> imagePath(new wchar_t[dataSize / sizeof(wchar_t)]);
        lResult = RegQueryValueExW(hSubKey, L"ImagePath", nullptr, nullptr,
                                   reinterpret_cast<LPBYTE>(imagePath.get()), &dataSize);
        if (lResult == ERROR_SUCCESS) {
            std::wcout << subKeyName << L": " << imagePath.get() << std::endl;
        }
    }

    RegCloseKey(hSubKey);
}
