

////BUETOOTH TEST
//
//#include <WinSock2.h>
//#include <WS2tcpip.h>
//#include <ws2bth.h>
//#include <bluetoothleapis.h>
//#include <stdio.h>
//#include <bluetoothleapis.h>
//#include <setupapi.h>
//#include <bthdef.h>
//#include <devguid.h>
//#include "types.h"
//
//#define CXN_SUCCESS                       0
//#define CXN_ERROR                         1
//
//#pragma comment(lib,"Ws2_32.lib")
//
//void printLastError() {
//  LPVOID lpMsgBuf;
//  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
//    FORMAT_MESSAGE_FROM_SYSTEM |
//    FORMAT_MESSAGE_IGNORE_INSERTS,
//    NULL, GetLastError(), 0, (LPTSTR)&lpMsgBuf, 0, NULL);
//  fprintf(stderr, "%s\n", lpMsgBuf);
//  free(lpMsgBuf);
//  
//}
//
//int main() {
//
//  WSADATA wsaData{};
//  WSAQUERYSET query{};
//  DWORD controlFlags{};
//  HANDLE queryHandle{};
//
//  char msg[50];
//
//  char    buffer[4096] = {};
//  DWORD   swSize = sizeof(buffer);
//
//  s32 iResult = CXN_SUCCESS;
//
//  u8 exit = 0;
//
//  //Message initialization
//  memset(msg, 0, sizeof(msg));
//  //Initialize Winsock
//  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
//
//  if (iResult != NO_ERROR) {
//    printf("WSAStartup failed: %d\n", iResult);
//    printLastError();
//    return 1;
//  }
//  query.dwSize = sizeof(WSAQUERYSET);
//  query.dwNameSpace = NS_BTH;
//  controlFlags = LUP_CONTAINERS | LUP_RETURN_ADDR | LUP_FLUSHCACHE;
//  while (!exit) {
//    //Find bluetooth devices
//    iResult = WSALookupServiceBegin(&query, controlFlags, &queryHandle);
//    if (SOCKET_ERROR == iResult) {
//      wprintf(L"Error looking for service \n");
//      printLastError();
//      return 1;
//    }
//    if (iResult == ERROR || queryHandle == nullptr) {
//      printf("Couldn't find bluetooth device. Try again? [y,n] \n");
//      fgets(msg, sizeof(msg), stdin);
//      //As fgets also gets the input of the new line character we clean it with a 0
//      msg[strcspn(msg, "\n")] = 0;
//      if (memcmp(msg, "n", strlen(msg)) == 0) {
//        exit = 1;
//      }
//      else {
//        exit = 0;
//      }
//    }
//    else {
//      while (WSALookupServiceNext(queryHandle, controlFlags, &swSize, &query)) {
//        wprintf(L"Device name:%s\n", query.lpszServiceInstanceName);
//
//      }
//    }
//  }
//  return 0;
//}
//
////How to get notification values from a HR BLE monitor
////Ensure that you have paired the HR BLE monitor with the computer
//
//

#pragma warning (disable: 4068)


#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>
#include <bthdef.h>
#include <bluetoothleapis.h>

#include <iostream>
#include <sstream>
#include <string>
#include <locale>
//GUID_DEVINTERFACE_* values
#include <uuids.h>

//For now is a global viable
//TODO once the classes are reestructured should be an attribute of a class
PBTH_LE_GATT_SERVICE pServiceBuffer;

//For now is a global viable
//TODO once the classes are reestructured should be an attribute of a class
PBTH_LE_GATT_CHARACTERISTIC pCharacteristicsBuffer;

//Determining the Services Buffer Size
//Based in the example provided by Microsoft at the oficial API
//https://docs.microsoft.com/en-us/windows/win32/api/bluetoothleapis/nf-bluetoothleapis-bluetoothgattgetservices
HRESULT GetBLEGattServices(HANDLE service_handle) {
  
  USHORT serviceBufferCount;
  HRESULT result = BluetoothGATTGetServices(
    service_handle,
    0,
    NULL,
    &serviceBufferCount,
    BLUETOOTH_GATT_FLAG_NONE
  );
  //The error is not caused by a lack of space which is the
  //one we are expecting after the call
  if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != result) {
    //Error
    return result;
  }

  std::cout << "The device has  " << serviceBufferCount << " Services."<< std::endl;
  //Made a global as we need it to get the characteristics
  //PBTH_LE_GATT_SERVICE pServiceBuffer;
  pServiceBuffer = (PBTH_LE_GATT_SERVICE)malloc(sizeof(BTH_LE_GATT_SERVICE) * serviceBufferCount);
  if (nullptr == pServiceBuffer) {
    //Error no more memory
    return result;
  }
  //We set the initial value of the buffer to 0
  memset(pServiceBuffer, 0, sizeof(BTH_LE_GATT_SERVICE) * serviceBufferCount);

  //We retrieve the service
  USHORT num_services = 0;
  result = BluetoothGATTGetServices(
    service_handle,
    serviceBufferCount,
    pServiceBuffer,
    &num_services,
    BLUETOOTH_GATT_FLAG_NONE
  );

  if (num_services != serviceBufferCount) {
    //Missmatch between buffer size and actual buffer size
  }

  for (USHORT i = 0; i < num_services; i++) {
    GUID guid = (pServiceBuffer + i)->ServiceUuid.Value.LongUuid;

    OLECHAR* guidString;
    StringFromCLSID(guid, &guidString);
    std::cout << " Service " << i << ": Service Short UUID(" << (pServiceBuffer + i)->ServiceUuid.Value.ShortUuid <<") Service Long UUID(" << guidString << ") with attribute handle " << (pServiceBuffer + i)->AttributeHandle << std::endl;
  }

  return result;
}

//Get the characteristics of a device
//Similar to get services, an example can alos be found at the oficial API
//https://docs.microsoft.com/en-us/windows/win32/api/bluetoothleapis/nf-bluetoothleapis-bluetoothgattgetcharacteristics
HRESULT GetBLECharacteristics(HANDLE service_handle) {
  USHORT charBufferSize;

  HRESULT result = BluetoothGATTGetCharacteristics(
    service_handle,
    pServiceBuffer,
    0,
    NULL,
    &charBufferSize,
    BLUETOOTH_GATT_FLAG_NONE
  );

  if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != result) {
    //Error
    return result;
  }

  //Made a global as we need it to get the characteristics
  //PBTH_LE_GATT_CHARACTERISTIC pCharacteristicsBuffer;
  if (charBufferSize > 0) {
    std::cout << "The device has  " << charBufferSize << " Characteristics." << std::endl;
    pCharacteristicsBuffer = (PBTH_LE_GATT_CHARACTERISTIC)malloc(sizeof(BTH_LE_GATT_CHARACTERISTIC)*charBufferSize);
  }
  if (nullptr == pCharacteristicsBuffer) {
    //Error no more memory
    return result;
  }

  //We set the initial value of the buffer to 0
  memset(pCharacteristicsBuffer, 0, sizeof(BTH_LE_GATT_CHARACTERISTIC) * charBufferSize);

  //We retrieve the characteristics
  USHORT num_characteristics = 0;
  result = BluetoothGATTGetCharacteristics(
    service_handle,
    pServiceBuffer,
    charBufferSize,
    pCharacteristicsBuffer,
    &num_characteristics,
    BLUETOOTH_GATT_FLAG_NONE
  );

  for (USHORT i = 0; i < charBufferSize; i++) {
    GUID guid = (pCharacteristicsBuffer + i)->CharacteristicUuid.Value.LongUuid;

    OLECHAR* guidString;
    StringFromCLSID(guid, &guidString);
    std::cout << " Characteristic " << i << ": Characteristic Short UUID(" << (pCharacteristicsBuffer + i)->CharacteristicUuid.Value.ShortUuid << ") Characteristic Long UUID(" << guidString << ") with attribute handle "
      << (pCharacteristicsBuffer + i)->AttributeHandle << " from service: " << (pCharacteristicsBuffer + i)->ServiceHandle << std::endl;
  }

  if (num_characteristics != charBufferSize) {
    //Missmatch between buffer size and actual buffer size
  }
  return result;
}

//Get the descriptors of a characteristic
//Similar to get services, an example can alos be found at the oficial API
//https://docs.microsoft.com/en-us/windows/win32/api/bluetoothleapis/nf-bluetoothleapis-bluetoothgattgetdescriptors
HRESULT GetBLEDescriptors(HANDLE service_handle) {

  USHORT descriptorBufferSize;
  PBTH_LE_GATT_CHARACTERISTIC currGattChar;
  currGattChar = &pCharacteristicsBuffer[2];
  HRESULT result = BluetoothGATTGetDescriptors(
    service_handle,
    currGattChar,
    0,
    NULL,
    &descriptorBufferSize,
    BLUETOOTH_GATT_FLAG_NONE
  );

  if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != result) {
    //Error
    return result;
  }

  PBTH_LE_GATT_DESCRIPTOR pDescriptorBuffer;

  if (descriptorBufferSize > 0) {
    pDescriptorBuffer = (PBTH_LE_GATT_DESCRIPTOR)malloc(sizeof(BTH_LE_GATT_DESCRIPTOR)*descriptorBufferSize);
  }
  if (nullptr == pCharacteristicsBuffer) {
    //Error no more memory
    return result;
  }

  //We set the initial value of the buffer to 0
  memset(pDescriptorBuffer, 0, sizeof(BTH_LE_GATT_DESCRIPTOR) * descriptorBufferSize);

  //We retrieve the descriptors
  USHORT num_descriptors;
  result = BluetoothGATTGetDescriptors(
    service_handle,
    pCharacteristicsBuffer,
    descriptorBufferSize,
    pDescriptorBuffer,
    &num_descriptors,
    BLUETOOTH_GATT_FLAG_NONE
  );
  if (num_descriptors != descriptorBufferSize) {
    //Missmatch between buffer size and actual buffer size
  }
  return result;
}

//search for bluetooth devices connected to the computer
int ScanBLEDevices()
{
  static /*const*/ GUID GUID_DEVINTERFACE_USB_DEVICE =
  { 0xA5DCBF10L, 0x6530, 0x11D2,{ 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } };
  
  CHAR* path;

  HDEVINFO hDI;
  SP_DEVINFO_DATA deviceInfoData;
  DWORD memberIndex = 0;

  //https://github.com/crosswalk-project/chromium-crosswalk/blob/master/device/bluetooth/bluetooth_low_energy_win.cc
  // Create a HDEVINFO with all bluetooth present devices.
  // Enumerate known Bluetooth low energy devices or Bluetooth low energy GATT
  // service devices according to |device_interface_guid|.
  // Note: |device_interface_guid| = GUID_BLUETOOTHLE_DEVICE_INTERFACE corresponds
  // Bluetooth low energy devices. |device_interface_guid| =
  // GUID_BLUETOOTH_GATT_SERVICE_DEVICE_INTERFACE corresponds Bluetooth low energy
  // Gatt service devices.


  //hDI = SetupDiGetClassDevs(&GUID_DEVCLASS_BLUETOOTH, NULL, NULL, DIGCF_PRESENT );
  //hDI = SetupDiGetClassDevs(&GUID_BLUETOOTH_GATT_SERVICE_DEVICE_INTERFACE, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
  hDI = SetupDiGetClassDevs(&GUID_BLUETOOTHLE_DEVICE_INTERFACE, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
  if (hDI == INVALID_HANDLE_VALUE)
  {
    return 1;
  }
  //Handle for the device
  HANDLE deviceHandle = NULL;

  // Enumerate through all devices in Set.
  memset(&deviceInfoData, 0, sizeof(SP_DEVINFO_DATA));
  deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    
  while (SetupDiEnumDeviceInfo(hDI, memberIndex, &deviceInfoData)) {
    memberIndex++;
    DWORD attributeIndex = 0;
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    memset(&deviceInterfaceData, 0, sizeof(SP_DEVICE_INTERFACE_DATA));
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    while (SetupDiEnumDeviceInterfaces(
      hDI,
      &deviceInfoData,
      &GUID_BLUETOOTHLE_DEVICE_INTERFACE,
      attributeIndex,
      &deviceInterfaceData
    )) {

      attributeIndex++;
      // Get the size required for the structure.
      DWORD RequiredSize;
      SetupDiGetDeviceInterfaceDetail(hDI, &deviceInterfaceData, NULL, NULL, &RequiredSize, NULL);
      PSP_DEVICE_INTERFACE_DETAIL_DATA pDevIntDetData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(
        sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + RequiredSize
      );
      memset(pDevIntDetData, 0, sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + RequiredSize);
      pDevIntDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
      SetupDiGetDeviceInterfaceDetail(
        hDI,
        &deviceInterfaceData,
        pDevIntDetData, RequiredSize,
        &RequiredSize,
        &deviceInfoData
      );
      path = pDevIntDetData->DevicePath;
      std::cout << "Path: " << pDevIntDetData->DevicePath << std::endl;
      //TODO create a function that returns this device Paths so a user could choose which one he desires to use to make a Handle
      //The path can be used to create a handle which is what we need to access the services and all the other info from the GATT
      //deviceHandle = CreateFile(pDevIntDetData->DevicePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    }

    bool hasError = false;

    DWORD nameData;
    LPTSTR nameBuffer = NULL;
    DWORD nameBufferSize = 0;

    while (!SetupDiGetDeviceRegistryProperty(
      hDI,
      &deviceInfoData,
      SPDRP_FRIENDLYNAME,
      &nameData,
      (PBYTE)nameBuffer,
      nameBufferSize,
      &nameBufferSize))
    {
      if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
      {
        if (nameBuffer) delete(nameBuffer);
        nameBuffer = new TCHAR[nameBufferSize * 2];
      }
      else
      {
        hasError = true;
        break;
      }
    }

    //TODO SetupDiGetDeviceProperty could have important information

    DWORD GUID;
    LPTSTR GUIDBuffer = NULL;
    DWORD GUIDBufferSize = 0;
    while (!SetupDiGetDeviceRegistryProperty(
      hDI,
      &deviceInfoData,
      SPDRP_CLASSGUID,
      &GUID,
      (PBYTE)GUIDBuffer,
      GUIDBufferSize,
      &GUIDBufferSize))
    {
      if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
      {
        if (GUIDBuffer) delete(GUIDBuffer);
        GUIDBuffer = new TCHAR[GUIDBufferSize * 2];
      }
      else
      {
        hasError = true;
        break;
      }
    }

    DWORD addressData;
    LPTSTR addressBuffer = NULL;
    DWORD addressBufferSize = 0;

    while (!SetupDiGetDeviceRegistryProperty(
      hDI,
      &deviceInfoData,
      SPDRP_HARDWAREID,
      &addressData,
      (PBYTE)addressBuffer,
      addressBufferSize,
      &addressBufferSize))
    {
      if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
      {
        if (addressBuffer) delete(addressBuffer);
        addressBuffer = new TCHAR[addressBufferSize * 2];
      }
      else
      {
        hasError = true;
        break;
      }
    }

    LPTSTR deviceIdBuffer = NULL;
    DWORD deviceIdBufferSize = 0;

    while (!SetupDiGetDeviceInstanceId(
      hDI,
      &deviceInfoData,
      deviceIdBuffer,
      deviceIdBufferSize,
      &deviceIdBufferSize))
    {
      if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
      {
        if (deviceIdBuffer) delete(deviceIdBuffer);
        deviceIdBuffer = new TCHAR[deviceIdBufferSize * 2];
      }
      else
      {
        hasError = true;
        break;
      }
    }

    if (hasError)
    {
      if (nameBuffer) delete(nameBuffer);
      if (addressBuffer) delete(addressBuffer);
      if (deviceIdBuffer) delete(deviceIdBuffer);
      if (GUIDBuffer) delete(GUIDBuffer);
      continue;
    }

    std::cout << "Found " << nameBuffer << " (" << deviceIdBuffer << ") with GUID : " << GUIDBuffer << std::endl;

    if (nameBuffer) delete(nameBuffer);
    if (addressBuffer) delete(addressBuffer);
    if (deviceIdBuffer) delete(deviceIdBuffer);
    if (GUIDBuffer) delete(GUIDBuffer);
  }
  //Microsoft API
  //The caller of SetupDiGetClassDevs must delete the returned device information set when it is no longer needed 
  SetupDiDestroyDeviceInfoList(hDI);
  HANDLE myHandle = CreateFile(path, GENERIC_WRITE | GENERIC_READ, NULL, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  
  if (myHandle == INVALID_HANDLE_VALUE) {
    //Error
    return -1;
  }
  std::cout << "Handle created for the paht  " << path << std::endl;

  GetBLEGattServices(myHandle);
  GetBLECharacteristics(myHandle);
  GetBLEDescriptors(myHandle);
  return 0;
}

int main(int argc, char *argv[], char *envp[]) {
  bool exit = false;
  char command = 'a';
  printf("Press h for help. \n");
  while (!exit) {
    scanf("%c", &command);
    if (!memcmp("s", &command, sizeof(char))) {
      int a = ScanBLEDevices();
    }
    else if (!memcmp("e", &command, sizeof(char))) {
      exit = true;
    }
    else if (!memcmp("h", &command, sizeof(char))) {
      printf("\n ****************************************************** \n");
      printf("\n Press e to exit. \n Press s to search for devices. \n Press h again to show this message. \n");
      printf("\n ****************************************************** \n");
    }
    //we scan again to remove the enter
    scanf("%c", &command);
  }
  
}