

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
////BLE Example C++
//#include <stdio.h>
//#include <windows.h>
//#include <setupapi.h>
//#include <devguid.h>
//#include <regstr.h>
//#include <bthdef.h>
//#include <Bluetoothleapis.h>
//#pragma comment(lib, "SetupAPI")
//#pragma comment(lib, "BluetoothApis.lib")
//#define TO_SEARCH_DEVICE_UUID "{0000180D-0000-1000-8000-00805F9B34FB}" //we use UUID for an HR BLE device 
//
//
////this is the notification function
////the way ValueChangedEventParameters is utilized is shown in
////HealthHeartRateService::HeartRateMeasurementEvent()
////a function in Windows Driver Kit (WDK) 8.0 Samples.zip\C++\WDK 8.0 Samples\Bluetooth Low Energy (LE) Generic Attribute (GATT) Profile Drivers\Solution\WpdHealthHeartRate\HealthHeartRateService.cpp
//void SomethingHappened(BTH_LE_GATT_EVENT_TYPE EventType, PVOID EventOutParameter, PVOID Context)
//{
//  printf("notification obtained ");
//  PBLUETOOTH_GATT_VALUE_CHANGED_EVENT ValueChangedEventParameters = (PBLUETOOTH_GATT_VALUE_CHANGED_EVENT)EventOutParameter;
//
//  HRESULT hr;
//  if (0 == ValueChangedEventParameters->CharacteristicValue->DataSize) {
//    hr = E_FAIL;
//    printf("datasize 0\n");
//  }
//  else {
//    printf("HR ");
//    //for(int i=0; i<ValueChangedEventParameters->CharacteristicValue->DataSize;i++) {
//    //	printf("%0x",ValueChangedEventParameters->CharacteristicValue->Data[i]);
//    //}
//    // if the first bit is set, then the value is the next 2 bytes.  If it is clear, the value is in the next byte
//    //The Heart Rate Value Format bit (bit 0 of the Flags field) indicates if the data format of 
//    //the Heart Rate Measurement Value field is in a format of UINT8 or UINT16. 
//    //When the Heart Rate Value format is sent in a UINT8 format, the Heart Rate Value 
//    //Format bit shall be set to 0. When the Heart Rate Value format is sent in a UINT16 
//    //format, the Heart Rate Value Format bit shall be set to 1
//    //from this PDF https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=239866
//    unsigned heart_rate;
//    if (0x01 == (ValueChangedEventParameters->CharacteristicValue->Data[0] & 0x01)) {
//      heart_rate = ValueChangedEventParameters->CharacteristicValue->Data[1] * 256 + ValueChangedEventParameters->CharacteristicValue->Data[2];
//    }
//    else {
//      heart_rate = ValueChangedEventParameters->CharacteristicValue->Data[1];
//    }
//    printf("%d\n", heart_rate);
//  }
//}
//
////this function works to get a handle for a BLE device based on its GUID
////copied from http://social.msdn.microsoft.com/Forums/windowshardware/en-US/e5e1058d-5a64-4e60-b8e2-0ce327c13058/erroraccessdenied-error-when-trying-to-receive-data-from-bluetooth-low-energy-devices?forum=wdk
////credits to Andrey_sh
//HANDLE GetBLEHandle(__in GUID AGuid)
//{
//  HDEVINFO hDI;
//  SP_DEVICE_INTERFACE_DATA did;
//  SP_DEVINFO_DATA dd;
//  GUID BluetoothInterfaceGUID = AGuid;
//  HANDLE hComm = NULL;
//
//  hDI = SetupDiGetClassDevs(&BluetoothInterfaceGUID, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
//
//  if (hDI == INVALID_HANDLE_VALUE) return NULL;
//
//  did.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
//  dd.cbSize = sizeof(SP_DEVINFO_DATA);
//
//  for (DWORD i = 0; SetupDiEnumDeviceInterfaces(hDI, NULL, &BluetoothInterfaceGUID, i, &did); i++)
//  {
//    SP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData;
//
//    DeviceInterfaceDetailData.cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
//
//    DWORD size = 0;
//
//    if (!SetupDiGetDeviceInterfaceDetail(hDI, &did, NULL, 0, &size, 0))
//    {
//      int err = GetLastError();
//
//      if (err == ERROR_NO_MORE_ITEMS) break;
//
//      PSP_DEVICE_INTERFACE_DETAIL_DATA pInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)GlobalAlloc(GPTR, size);
//
//      pInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
//
//      if (!SetupDiGetDeviceInterfaceDetail(hDI, &did, pInterfaceDetailData, size, &size, &dd))
//        break;
//
//      hComm = CreateFile(
//        pInterfaceDetailData->DevicePath,
//        GENERIC_WRITE | GENERIC_READ,
//        FILE_SHARE_READ | FILE_SHARE_WRITE,
//        NULL,
//        OPEN_EXISTING,
//        0,
//        NULL);
//
//      GlobalFree(pInterfaceDetailData);
//    }
//  }
//
//  SetupDiDestroyDeviceInfoList(hDI);
//  return hComm;
//}

//int main(int argc, char *argv[], char *envp[])
//{
//  //Step 1: find the BLE device handle from its GUID
//  GUID AGuid;
//  //GUID can be constructed from "{xxx....}" string using CLSID
//  LPCOLESTR myOleStr;
//  myOleStr = OLESTR("{0000180D-0000-1000-8000-00805F9B34FB}");
//  CLSIDFromString(myOleStr, &AGuid);
//
//  //now get the handle 
//  HANDLE hLEDevice = GetBLEHandle(AGuid);
//
//
//  //Step 2: Get a list of services that the device advertises
//  // first send 0,NULL as the parameters to BluetoothGATTServices inorder to get the number of
//  // services in serviceBufferCount
//  USHORT serviceBufferCount;
//  ////////////////////////////////////////////////////////////////////////////
//  // Determine Services Buffer Size
//  ////////////////////////////////////////////////////////////////////////////
//
//  HRESULT hr = BluetoothGATTGetServices(
//    hLEDevice,
//    0,
//    NULL,
//    &serviceBufferCount,
//    BLUETOOTH_GATT_FLAG_NONE);
//
//  if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr) {
//    printf(" BluetoothGATTGetServices - Buffer Size %d", hr);
//  }
//
//  PBTH_LE_GATT_SERVICE pServiceBuffer = (PBTH_LE_GATT_SERVICE)
//    malloc(sizeof(BTH_LE_GATT_SERVICE) * serviceBufferCount);
//
//  if (NULL == pServiceBuffer) {
//    printf("pServiceBuffer out of memory\r\n");
//  }
//  else {
//    RtlZeroMemory(pServiceBuffer,
//      sizeof(BTH_LE_GATT_SERVICE) * serviceBufferCount);
//  }
//
//  ////////////////////////////////////////////////////////////////////////////
//  // Retrieve Services
//  ////////////////////////////////////////////////////////////////////////////
//
//  USHORT numServices;
//  hr = BluetoothGATTGetServices(
//    hLEDevice,
//    serviceBufferCount,
//    pServiceBuffer,
//    &numServices,
//    BLUETOOTH_GATT_FLAG_NONE);
//
//  if (S_OK != hr) {
//    printf("BluetoothGATTGetServices - Buffer Size %d", hr);
//  }
//
//
//  //Step 3: now get the list of charactersitics. note how the pServiceBuffer is required from step 2
//  ////////////////////////////////////////////////////////////////////////////
//  // Determine Characteristic Buffer Size
//  ////////////////////////////////////////////////////////////////////////////
//
//  USHORT charBufferSize;
//  hr = BluetoothGATTGetCharacteristics(
//    hLEDevice,
//    pServiceBuffer,
//    0,
//    NULL,
//    &charBufferSize,
//    BLUETOOTH_GATT_FLAG_NONE);
//
//  if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr) {
//    printf("BluetoothGATTGetCharacteristics - Buffer Size %d", hr);
//  }
//
//  PBTH_LE_GATT_CHARACTERISTIC pCharBuffer;
//  if (charBufferSize > 0) {
//    pCharBuffer = (PBTH_LE_GATT_CHARACTERISTIC)
//      malloc(charBufferSize * sizeof(BTH_LE_GATT_CHARACTERISTIC));
//
//    if (NULL == pCharBuffer) {
//      printf("pCharBuffer out of memory\r\n");
//    }
//    else {
//      RtlZeroMemory(pCharBuffer,
//        charBufferSize * sizeof(BTH_LE_GATT_CHARACTERISTIC));
//    }
//
//    ////////////////////////////////////////////////////////////////////////////
//    // Retrieve Characteristics
//    ////////////////////////////////////////////////////////////////////////////
//    USHORT numChars;
//    hr = BluetoothGATTGetCharacteristics(
//      hLEDevice,
//      pServiceBuffer,
//      charBufferSize,
//      pCharBuffer,
//      &numChars,
//      BLUETOOTH_GATT_FLAG_NONE);
//
//    if (S_OK != hr) {
//      printf("BluetoothGATTGetCharacteristics - Actual Data %d", hr);
//    }
//
//    if (numChars != charBufferSize) {
//      printf("buffer size and buffer size actual size mismatch\r\n");
//    }
//  }
//
//
//  //Step 4: now get the list of descriptors. note how the pCharBuffer is required from step 3
//  //descriptors are required as we descriptors that are notification based will have to be written
//  //once IsSubcribeToNotification set to true, we set the appropriate callback function
//  //need for setting descriptors for notification according to
//  //http://social.msdn.microsoft.com/Forums/en-US/11d3a7ce-182b-4190-bf9d-64fefc3328d9/windows-bluetooth-le-apis-event-callbacks?forum=wdk
//  PBTH_LE_GATT_CHARACTERISTIC currGattChar;
//  for (int ii = 0; ii <charBufferSize; ii++) {
//    currGattChar = &pCharBuffer[ii];
//    USHORT charValueDataSize;
//    PBTH_LE_GATT_CHARACTERISTIC_VALUE pCharValueBuffer;
//
//
//    ///////////////////////////////////////////////////////////////////////////
//    // Determine Descriptor Buffer Size
//    ////////////////////////////////////////////////////////////////////////////
//    USHORT descriptorBufferSize;
//    hr = BluetoothGATTGetDescriptors(
//      hLEDevice,
//      currGattChar,
//      0,
//      NULL,
//      &descriptorBufferSize,
//      BLUETOOTH_GATT_FLAG_NONE);
//
//    if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr) {
//      printf("BluetoothGATTGetDescriptors - Buffer Size %d", hr);
//    }
//
//    PBTH_LE_GATT_DESCRIPTOR pDescriptorBuffer;
//    if (descriptorBufferSize > 0) {
//      pDescriptorBuffer = (PBTH_LE_GATT_DESCRIPTOR)
//        malloc(descriptorBufferSize
//          * sizeof(BTH_LE_GATT_DESCRIPTOR));
//
//      if (NULL == pDescriptorBuffer) {
//        printf("pDescriptorBuffer out of memory\r\n");
//      }
//      else {
//        RtlZeroMemory(pDescriptorBuffer, descriptorBufferSize);
//      }
//
//      ////////////////////////////////////////////////////////////////////////////
//      // Retrieve Descriptors
//      ////////////////////////////////////////////////////////////////////////////
//
//      USHORT numDescriptors;
//      hr = BluetoothGATTGetDescriptors(
//        hLEDevice,
//        currGattChar,
//        descriptorBufferSize,
//        pDescriptorBuffer,
//        &numDescriptors,
//        BLUETOOTH_GATT_FLAG_NONE);
//
//      if (S_OK != hr) {
//        printf("BluetoothGATTGetDescriptors - Actual Data %d", hr);
//      }
//
//      if (numDescriptors != descriptorBufferSize) {
//        printf("buffer size and buffer size actual size mismatch\r\n");
//      }
//
//      for (int kk = 0; kk<numDescriptors; kk++) {
//        PBTH_LE_GATT_DESCRIPTOR  currGattDescriptor = &pDescriptorBuffer[kk];
//        ////////////////////////////////////////////////////////////////////////////
//        // Determine Descriptor Value Buffer Size
//        ////////////////////////////////////////////////////////////////////////////
//        USHORT descValueDataSize;
//        hr = BluetoothGATTGetDescriptorValue(
//          hLEDevice,
//          currGattDescriptor,
//          0,
//          NULL,
//          &descValueDataSize,
//          BLUETOOTH_GATT_FLAG_NONE);
//
//        if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr) {
//          printf("BluetoothGATTGetDescriptorValue - Buffer Size %d", hr);
//        }
//
//        PBTH_LE_GATT_DESCRIPTOR_VALUE pDescValueBuffer = (PBTH_LE_GATT_DESCRIPTOR_VALUE)malloc(descValueDataSize);
//
//        if (NULL == pDescValueBuffer) {
//          printf("pDescValueBuffer out of memory\r\n");
//        }
//        else {
//          RtlZeroMemory(pDescValueBuffer, descValueDataSize);
//        }
//
//        ////////////////////////////////////////////////////////////////////////////
//        // Retrieve the Descriptor Value
//        ////////////////////////////////////////////////////////////////////////////
//
//        hr = BluetoothGATTGetDescriptorValue(
//          hLEDevice,
//          currGattDescriptor,
//          (ULONG)descValueDataSize,
//          pDescValueBuffer,
//          NULL,
//          BLUETOOTH_GATT_FLAG_NONE);
//        if (S_OK != hr) {
//          printf("BluetoothGATTGetDescriptorValue - Actual Data %d", hr);
//        }
//        //you may also get a descriptor that is read (and not notify) andi am guessing the attribute handle is out of limits
//        // we set all descriptors that are notifiable to notify us via IsSubstcibeToNotification
//        if (currGattDescriptor->AttributeHandle < 255) {
//          BTH_LE_GATT_DESCRIPTOR_VALUE newValue;
//
//          RtlZeroMemory(&newValue, sizeof(newValue));
//
//          newValue.DescriptorType = ClientCharacteristicConfiguration;
//          newValue.ClientCharacteristicConfiguration.IsSubscribeToNotification = TRUE;
//
//          hr = BluetoothGATTSetDescriptorValue(
//            hLEDevice,
//            currGattDescriptor,
//            &newValue,
//            BLUETOOTH_GATT_FLAG_NONE);
//          if (S_OK != hr) {
//            printf("BluetoothGATTGetDescriptorValue - Actual Data %d", hr);
//          }
//          else {
//            printf("setting notification for serivice handle %d\n", currGattDescriptor->ServiceHandle);
//          }
//
//        }
//
//      }
//    }
//
//
//    //set the appropriate callback function when the descriptor change value
//    BLUETOOTH_GATT_EVENT_HANDLE EventHandle;
//
//    if (currGattChar->IsNotifiable) {
//      printf("Setting Notification for ServiceHandle %d\n", currGattChar->ServiceHandle);
//      BTH_LE_GATT_EVENT_TYPE EventType = CharacteristicValueChangedEvent;
//
//      BLUETOOTH_GATT_VALUE_CHANGED_EVENT_REGISTRATION EventParameterIn;
//      EventParameterIn.Characteristics[0] = *currGattChar;
//      EventParameterIn.NumCharacteristics = 1;
//      hr = BluetoothGATTRegisterEvent(
//        hLEDevice,
//        EventType,
//        &EventParameterIn,
//        SomethingHappened,
//        NULL,
//        &EventHandle,
//        BLUETOOTH_GATT_FLAG_NONE);
//
//      if (S_OK != hr) {
//        printf("BluetoothGATTRegisterEvent - Actual Data %d", hr);
//      }
//    }
//
//
//    if (currGattChar->IsReadable) {//currGattChar->IsReadable
//                                   ////////////////////////////////////////////////////////////////////////////
//                                   // Determine Characteristic Value Buffer Size
//                                   ////////////////////////////////////////////////////////////////////////////
//      hr = BluetoothGATTGetCharacteristicValue(
//        hLEDevice,
//        currGattChar,
//        0,
//        NULL,
//        &charValueDataSize,
//        BLUETOOTH_GATT_FLAG_NONE);
//
//      if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr) {
//        printf("BluetoothGATTGetCharacteristicValue - Buffer Size %d", hr);
//      }
//
//      pCharValueBuffer = (PBTH_LE_GATT_CHARACTERISTIC_VALUE)malloc(charValueDataSize);
//
//      if (NULL == pCharValueBuffer) {
//        printf("pCharValueBuffer out of memory\r\n");
//      }
//      else {
//        RtlZeroMemory(pCharValueBuffer, charValueDataSize);
//      }
//
//      ////////////////////////////////////////////////////////////////////////////
//      // Retrieve the Characteristic Value
//      ////////////////////////////////////////////////////////////////////////////
//
//      hr = BluetoothGATTGetCharacteristicValue(
//        hLEDevice,
//        currGattChar,
//        (ULONG)charValueDataSize,
//        pCharValueBuffer,
//        NULL,
//        BLUETOOTH_GATT_FLAG_NONE);
//
//      if (S_OK != hr) {
//        printf("BluetoothGATTGetCharacteristicValue - Actual Data %d", hr);
//      }
//
//      //print the characeteristic Value
//      //for an HR monitor this might be the body sensor location
//      printf("\n Printing a read (not notifiable) characterstic (maybe) body sensor value");
//      for (int iii = 0; iii< pCharValueBuffer->DataSize; iii++) {// ideally check ->DataSize before printing
//        printf("%d", pCharValueBuffer->Data[iii]);
//      }
//      printf("\n");
//
//
//      // Free before going to next iteration, or memory leak.
//      free(pCharValueBuffer);
//      pCharValueBuffer = NULL;
//    }
//
//  }
//
//  //go into an inf loop that sleeps. you will ideally see notifications from the HR device
//  while (1) {
//    Sleep(1000);
//    //printf("look for notification");
//  }
//
//  CloseHandle(hLEDevice);
//
//
//
//  if (GetLastError() != NO_ERROR &&
//    GetLastError() != ERROR_NO_MORE_ITEMS)
//  {
//    // Insert error handling here.
//    return 1;
//  }
//
//  return 0;
//}
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

//search for bluetooth devices connected to the computer
int ScanBLEDevices()
{
  static /*const*/ GUID GUID_DEVINTERFACE_USB_DEVICE =
  { 0xA5DCBF10L, 0x6530, 0x11D2,{ 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } };
  
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
      std::cout << "Path: " << pDevIntDetData->DevicePath << std::endl;
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
      printf("\n Press e to exit. \n Press s to search for devices. \n Press h again to show this message. \n");
    }
    //we scan again to remove the enter
    scanf("%c", &command);
  }
  
}