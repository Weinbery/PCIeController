/* Copyright (C) Jungo 2002 - 2005 */

#ifndef _WD_USB_H_
#define _WD_USB_H_

#include "windrvr.h"

#if defined(__cplusplus)
    extern "C" {
#endif

typedef PVOID WDU_DEVICE_HANDLE;
typedef PVOID WDU_DRIVER_HANDLE; 

typedef WORD WDU_LANGID;

// User Callbacks

// Function typedef: WDU_ATTACH_CALLBACK()
//   WinDriver calls this function with any new device that is attached, 
//   matches the given criteria, and if WD_ACKNOWLEDGE was passed to WDU_Init()
//   in dwOptions - not controlled yet by another driver.
//   This callback is called once for each matching interface.
// Parameters:
//   [in] hDevice: a unique identifier for the device/interface
//   [in] pDeviceInfo: ptr to device configuration details, this ptr is valid 
//   until the end of the function
//   [in] pUserData: ptr that was passed to WDU_Init
// Return Value:
//   if WD_ACKNOWLEDGE was passed to WDU_Init(), the implementor should check & 
//   return if he wants to control the device
typedef BOOL (DLLCALLCONV *WDU_ATTACH_CALLBACK)(WDU_DEVICE_HANDLE hDevice, 
    WDU_DEVICE *pDeviceInfo, PVOID pUserData);

// Function typedef: WDU_DETACH_CALLBACK()
//   WinDriver calls this function when a controlled device has been detached 
//   from the system.
// Parameters:
//   [in] hDevice: a unique identifier for the device/interface
//   [in] pUserData: ptr that was passed to WDU_Init (in the event table)
// Return Value:
//   None.
typedef void (DLLCALLCONV *WDU_DETACH_CALLBACK)(WDU_DEVICE_HANDLE hDevice, PVOID pUserData);

// Function typedef: WDU_POWER_CHANGE_CALLBACK()
//   (currently (CE.Net 4.10) pRegisterNotificationRoutine will pass only 
//   USB_CLOSE_DEVICE event (see microsoft documentation))
// Parameters:
//   [in] hDevice: a unique identifier for the device/interface
//   [in] dwPowerState: number of the power state selected
//   [in] pUserData: ptr that was passed to WDU_Init (in the event table)
// Return Value:
//   TRUE/FALSE; Currently there is no significance to the return value.
typedef BOOL (DLLCALLCONV *WDU_POWER_CHANGE_CALLBACK)(WDU_DEVICE_HANDLE hDevice, DWORD dwPowerState, PVOID pUserData);

// struct definitions
typedef struct
{
    WDU_ATTACH_CALLBACK pfDeviceAttach;
    WDU_DETACH_CALLBACK pfDeviceDetach;
    WDU_POWER_CHANGE_CALLBACK pfPowerChange;
    PVOID pUserData; // pointer to pass in each callback
} WDU_EVENT_TABLE;

// API Functions  

// Function: WDU_Init()
//   Starts listening to devices matching a criteria, and registers
//   notification callbacks for those devices
// Parameters:
//   [out] phDriver: handle to this registration of events & criteria
//   [in] pMatchTables: array of match tables defining the devices-criteria
//   [in] dwNumMatchTables: number of elements in pMatchTables
//   [in] pEventTable: notification callbackss when the device's status changes
//   [in] sLicense: WinDriver's license string
//   [in] dwOptions: can be 0 or:
//          WD_ACKNOWLEDGE - The user can seize control over the device in 
//                           WDU_ATTACH_CALLBACK return value
// Return Value:
//   WinDriver Error Code
DWORD DLLCALLCONV WDU_Init(WDU_DRIVER_HANDLE *phDriver,
    WDU_MATCH_TABLE *pMatchTables, DWORD dwNumMatchTables,
    WDU_EVENT_TABLE *pEventTable, const char *sLicense, DWORD dwOptions);

// Function: WDU_Uninit()
//   Stops listening to devices matching the criteria, and unregisters the 
//   notification callbacks for those devices
// Parameters:
//   [in] hDriver: handle to the registration, received from WDU_Init
// Return Value:
//   None.
void DLLCALLCONV WDU_Uninit(WDU_DRIVER_HANDLE hDriver);

// Function: WDU_GetDeviceAddr()
//   Gets USB address that the device uses. The address number is written to
//   the caller supplied pAddress.
// Parameters:
//   [in] hDevice: a unique identifier for the device/interface
//   [out] pAddress: ptr to ULONG, in which result will be returned.
// Return Value:
//   WinDriver Error Code         
// NOTE: this function is supported on Windows only
DWORD DLLCALLCONV WDU_GetDeviceAddr(WDU_DEVICE_HANDLE hDevice, ULONG *pAddress);

// Function: WDU_GetDeviceInfo()
//   Gets configuration information from the device including all the 
//   descriptors in a WDU_DEVICE struct. The caller should free *ppDeviceInfo 
//   after using it by calling WDU_PutDeviceInfo().
// Parameters:
//   [in]  hDevice: a unique identifier for the device/interface
//   [out] ppDeviceInfo: ptr to ptr to a buffer containing the device information
// Return Value:
//   WinDriver Error Code
DWORD DLLCALLCONV WDU_GetDeviceInfo(WDU_DEVICE_HANDLE hDevice, WDU_DEVICE **ppDeviceInfo);

// Function: WDU_PutDeviceInfo()
//   Receives a device information pointer, allocated with a previous 
//   WDU_GetDeviceInfo() call, in order to perform the necessary cleanup
// Parameters:
//   [in] pDeviceInfo: ptr to a buffer containing the device 
//   information, as returned by a previous call to WDU_GetDeviceInfo()
// Return Value:
//   None.
void DLLCALLCONV WDU_PutDeviceInfo(WDU_DEVICE *pDeviceInfo);

// Function: WDU_SetInterface()
//   Sets the alternate setting for the specified interface
// Parameters:
//   [in] hDevice: a unique identifier for the device/interface
//   [in] dwInterfaceNum: the interface's number
//   [in] dwAlternateSetting: the desired alternate setting value
// Return Value:
//   WinDriver Error Code
DWORD DLLCALLCONV WDU_SetInterface(WDU_DEVICE_HANDLE hDevice, DWORD dwInterfaceNum, DWORD dwAlternateSetting);

// NOT IMPLEMENTED YET
DWORD DLLCALLCONV WDU_SetConfig(WDU_DEVICE_HANDLE hDevice, DWORD dwConfigNum);
// NOT IMPLEMENTED YET

// Function: WDU_ResetPipe()
//   Resets a pipe
// Parameters:
//   [in] hDevice: a unique identifier for the device/interface
//   [in] dwPipeNum: pipe number
// Return Value:
//   WinDriver Error Code
DWORD DLLCALLCONV WDU_ResetPipe(WDU_DEVICE_HANDLE hDevice, DWORD dwPipeNum);

// Function: WDU_ResetDevice()
//   Resets a device (supported only on Windows)
// Parameters:
//   [in] hDevice: a unique identifier for the device/interface
//   [in] dwOptions: can be 0 or:
//          WD_USB_HARD_RESET - will reset the device
//              even if it is not disabled.
//              After using this option it is advised to set the
//              interface of the device (WDU_SetInterface())
// Return Value:
//   WinDriver Error Code
// NOTE: this function is supported on Windows only
DWORD DLLCALLCONV WDU_ResetDevice(WDU_DEVICE_HANDLE hDevice, DWORD dwOptions);

// Function: WDU_Wakeup()
//   Enables/Disables wakeup feature
// Parameters:
//   [in] hDevice: a unique identifier for the device/interface
//   [in] dwOptions: can be:
//          WDU_WAKEUP_ENABLE - will enable wakeup
//          or:
//          WDU_WAKEUP_DISABLE - will disbale wakeup
// Return Value:
//   WinDriver Error Code
// NOTE: this function is supported on Windows only
DWORD DLLCALLCONV WDU_Wakeup(WDU_DEVICE_HANDLE hDevice, DWORD dwOptions);

// Function: WDU_Transfer()
//   Transfers data to/from a device
// Parameters:
//   [in] hDevice: a unique identifier for the device/interface
//   [in] dwPipeNum: number of the pipe through which the data is transferred
//   [in] fRead: TRUE for read, FALSE for write
//   [in] dwOptions: can be:
//              USB_ISOCH_NOASAP - For isochronous data transfers. Setting this option   
//                      instructs the lower driver (usbd.sys) to use a preset frame number 
//                      (instead of the next available frame) while performing the data transfer. 
//                      Use this flag if you notice unused frames during the transfer, on low-speed
//                      or full-speed devices (USB 1.1 only) and on Windows only (excluding CE).
//              USB_ISOCH_RESET - Resets the isochronous pipe before the data transfer. It 
//                      also resets the pipe after minor errors (consequently allowing to continue 
//                      with the transfer).
//              USB_ISOCH_FULL_PACKETS_ONLY - When set, do not transfer less than packet size 
//                      on isochronous pipes.                                   
//   [in] pBuffer: location of the data buffer
//   [in] dwBufferSize: number of the bytes to transfer
//   [out] pdwBytesTransferred: number of bytes actually transferred
//   [in] pSetupPacket: 8-bytes packet to transfer to control pipes
//   [in] dwTimeout: time in miliseconds to complete the transfer
// Return Value:
//   WinDriver Error Code
DWORD DLLCALLCONV WDU_Transfer(WDU_DEVICE_HANDLE hDevice, DWORD dwPipeNum, DWORD fRead, 
    DWORD dwOptions, PVOID pBuffer, DWORD dwBufferSize, PDWORD pdwBytesTransferred, 
    PBYTE pSetupPacket, DWORD dwTimeout);

// Function: WDU_HaltTransfer()
//   Halts the transfer on the specified pipe (only one simultaneous transfer
//   per-pipe is allowed by WinDriver)
// Parameters:
//   [in] hDevice: a unique identifier for the device/interface
//   [in] dwPipeNum: pipe number
// Return Value:
//   WinDriver Error Code
DWORD DLLCALLCONV WDU_HaltTransfer(WDU_DEVICE_HANDLE hDevice, DWORD dwPipeNum);

//
// simplified transfers
//

DWORD DLLCALLCONV WDU_TransferDefaultPipe(WDU_DEVICE_HANDLE hDevice, DWORD fRead, 
    DWORD dwOptions, PVOID pBuffer, DWORD dwBufferSize, PDWORD pdwBytesTransferred, 
    PBYTE pSetupPacket, DWORD dwTimeout);

DWORD DLLCALLCONV WDU_TransferBulk(WDU_DEVICE_HANDLE hDevice, DWORD dwPipeNum, DWORD fRead, 
    DWORD dwOptions, PVOID pBuffer, DWORD dwBufferSize, PDWORD pdwBytesTransferred, 
    DWORD dwTimeout);

DWORD DLLCALLCONV WDU_TransferIsoch(WDU_DEVICE_HANDLE hDevice, DWORD dwPipeNum, DWORD fRead, 
    DWORD dwOptions, PVOID pBuffer, DWORD dwBufferSize, PDWORD pdwBytesTransferred, 
    DWORD dwTimeout);

DWORD DLLCALLCONV WDU_TransferInterrupt(WDU_DEVICE_HANDLE hDevice, DWORD dwPipeNum, DWORD fRead, 
    DWORD dwOptions, PVOID pBuffer, DWORD dwBufferSize, PDWORD pdwBytesTransferred, 
    DWORD dwTimeout);

// Function: WDU_GetLangIDs()
//   Reads a list of supported language IDs and/or the number of supported 
//   language IDs from a device
// Parameters:
//   [in] hDevice: a unique identifier for the device/interface
//   [out] pbNumSupportedLangIDs: paramater to receive number of 
//           supported language IDs. Can be NULL if bNumLangIDs is 
//           not 0 and pLangIDs is not NULL. If NULL, the function 
//           will not return the number of supported language IDs 
//           for the device.
//   [out] pLangIDs: array of language IDs. If bNumLangIDs is not 
//           0 the function will fill this array with the supported 
//           language IDs for the device. If bNumLangIDs < the number 
//           of supported language IDs for the device, only the first 
//           bNumLangIDs supported language IDs will be read from the 
//           device and returned in the pLangIDs array   
//   [in] bNumLangIDs: number of IDs in pLangIDs array. If 0, the 
//          function will only return the number of supported language 
//          IDs    
// Return Value:
//   WinDriver Error Code
// NOTE: If no language IDs are supported for the device 
//       (*pbNumSupportedLangIDs == 0) the function returns 
//       WD_STATUS_SUCCESS   
DWORD DLLCALLCONV WDU_GetLangIDs(WDU_DEVICE_HANDLE hDevice, 
    PBYTE pbNumSupportedLangIDs, WDU_LANGID *pLangIDs, BYTE bNumLangIDs);

// Function: WDU_GetStringDesc()
//   Reads a string descriptor from a device by string index
//   (the descriptor is returned as a bytes stream)
// Parameters:
//   [in] hDevice: a unique identifier for the device/interface
//   [in] bStrIndex: string index 
//   [out] pbBuf: bytes array to hold the string descriptor that is read from 
//           the device. If dwBufSize < the size of the string descriptor, 
//           the returned descriptor will be truncated to dwBufSize bytes.
//   [in] dwBufSize: size of the pbBuf array
//   [in] langID: the language ID to be used in the get string descriptor 
//          request that is sent to the device. If langID is 0, the function 
//          will use the first supported language ID returned from the device 
//          (if exists)
//   [out] pdwDescSize: if this parameter is not NULL, in case of a successful 
//           read the function will update this parameter with the size of the 
//           string descriptor that was read from the device.
// Return Value:
//   WinDriver Error Code
DWORD DLLCALLCONV WDU_GetStringDesc(WDU_DEVICE_HANDLE hDevice, BYTE bStrIndex, 
    PBYTE pbBuf, DWORD dwBufSize, WDU_LANGID langID, PDWORD pdwDescSize);

#ifdef __cplusplus
}
#endif

#endif
