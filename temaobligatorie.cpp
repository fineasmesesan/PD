#include <iostream>
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <vector>
#include <string>

#pragma comment(lib, "setupapi.lib")

// Function to retrieve the name of a USB device
std::string fetchUSBDeviceName(HDEVINFO deviceInfoSet, SP_DEVINFO_DATA& deviceInfoData) {
    char nameBuffer[256] = "";
    
    if (!SetupDiGetDeviceRegistryPropertyA(deviceInfoSet, &deviceInfoData, SPDRP_FRIENDLYNAME, nullptr, 
                                           (PBYTE)nameBuffer, sizeof(nameBuffer), nullptr)) {
        if (!SetupDiGetDeviceRegistryPropertyA(deviceInfoSet, &deviceInfoData, SPDRP_DEVICEDESC, nullptr, 
                                               (PBYTE)nameBuffer, sizeof(nameBuffer), nullptr)) {
            return "Unrecognized USB Device";
        }
    }
    return std::string(nameBuffer);
}

// Function to scan and display all connected USB devices
void scanAndDisplayUSBDevices() {
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&GUID_DEVCLASS_USB, nullptr, nullptr, DIGCF_PRESENT);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        std::cerr << "Error retrieving USB device data." << std::endl;
        return;
    }
    
    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(deviceInfoData);
    DWORD deviceIndex = 0;
    
    while (SetupDiEnumDeviceInfo(deviceInfoSet, deviceIndex, &deviceInfoData)) {
        std::cout << "Detected USB Device: " << fetchUSBDeviceName(deviceInfoSet, deviceInfoData) << std::endl;
        deviceIndex++;
    }
    
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
}

int main() {
    scanAndDisplayUSBDevices();
    return 0;
}
