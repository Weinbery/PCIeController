////////////////////////////////////////////////////////////////
// File - USB_DIAG_LIB.C
//
// Utility functions for communication with USB devices 
// using WinDriver's API.
// 
// Copyright (c) 2003 - 2005 Jungo Ltd.  http://www.jungo.com
// 
////////////////////////////////////////////////////////////////

#include "windrvr.h"
#include "wdu_lib.h"
#include "status_strings.h"
#include "utils.h"
#ifdef _USE_SPECIFIC_KERNEL_DRIVER_
    #undef WD_Open
    #define WD_Open WD_OpenKernelHandle
    #if defined(UNIX)
        #undef WD_FUNCTION
        #define WD_FUNCTION(wFuncNum,h,pParam,dwSize,fWait) ((ULONG) ioctl((int)(h), wFuncNum, pParam))
    #endif
#endif
#include "usb_diag_lib.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#if !defined(ERR)
#define ERR printf
#endif

#define TRANSFER_TIMEOUT 30000 // in msecs

typedef struct  
{
    HANDLE hDevice;
    WDU_PIPE_INFO *pPipe;
    PVOID pContext;
    BOOL fStopped;
    HANDLE hThread;
    DWORD dwError;
} USB_LISTEN_PIPE;

// Function: CloseListening()
//   Stop listening to USB device pipe
void CloseListening(USB_LISTEN_PIPE *pListenPipe);

// Function: ListenToPipe()
//   Start listening to a USB device pipe
void ListenToPipe(USB_LISTEN_PIPE *pListenPipe);

// Function: pipeType2Str()
//   Returns a string identifying the pipe type
// Parameters:
//   pipeType [in] pipe type
// Return Value:
//   String containing the description of the pipe
char *pipeType2Str(ULONG pipeType)
{
    char *res = "unknown";
    switch (pipeType)
    {
        case PIPE_TYPE_CONTROL: 
            res = "Control";
            break;
        case PIPE_TYPE_ISOCHRONOUS:
            res = "Isochronous";
            break;          
        case PIPE_TYPE_BULK:
            res = "Bulk";
            break;
        case PIPE_TYPE_INTERRUPT:
            res = "Interrupt";
            break;
    }
    return res;
}

// input of command from user
static char line[256];

// Function: CloseListening()
//   Stop listening to USB device pipe
// Parameters:
//   pListenPipe [in] pointer to USB device pipe
// Return Value:
//   None
void CloseListening(USB_LISTEN_PIPE* pListenPipe)
{
    if (!pListenPipe->hThread)
        return;

    printf("Stop listening to pipe\n");
    pListenPipe->fStopped = TRUE;

    WDU_HaltTransfer(pListenPipe->hDevice, pListenPipe->pPipe->dwNumber);

    ThreadWait(pListenPipe->hThread);
    pListenPipe->hThread = NULL;
}

// Function: PipeListenHandler()
//   Callback function, which listens to a pipe continuously when there is data 
//   available in the pipe
// Parameters:
//   pParam [in] contains the relevant pipe information
// Return Value:
//   None
void DLLCALLCONV PipeListenHandler(void * pParam)
{
    DWORD dwError;
    USB_LISTEN_PIPE *pListenPipe = (USB_LISTEN_PIPE*) pParam;
    DWORD dwBufsize = pListenPipe->pPipe->dwMaximumPacketSize;
    PVOID buf;
    DWORD dwOptions = 0;

    if (pListenPipe->pPipe->type == PIPE_TYPE_ISOCHRONOUS) 
    { 
        dwBufsize *= 8; // suit 8 minimum packets for high speed transfers 
        dwOptions |= USB_ISOCH_FULL_PACKETS_ONLY; 
    } 

    buf = malloc(dwBufsize);

    for (;;)
    {
        DWORD dwBytesTransferred;

        dwError = WDU_Transfer(pListenPipe->hDevice, pListenPipe->pPipe->dwNumber,
            TRUE, dwOptions, buf, dwBufsize,
            &dwBytesTransferred, NULL, TRANSFER_TIMEOUT);
        if (pListenPipe->fStopped)
            break;
        if (dwError)
        {
            pListenPipe->dwError = dwError;
            break;
        }
        DIAG_PrintHexBuffer(buf, dwBytesTransferred, TRUE);
    }
    free(buf);
}

// Function: ListenToPipe()
//   Start listening to a USB device pipe
// Parameters:
//   pListenPipe [in] pipe to listen to
// Return Value:
//   None
void ListenToPipe(USB_LISTEN_PIPE *pListenPipe)
{
    // start the running thread
    pListenPipe->fStopped = FALSE;
    printf("Start listening to pipe\n");

    pListenPipe->dwError = ThreadStart(&pListenPipe->hThread, 
        PipeListenHandler, (PVOID) pListenPipe);
}

// Function: PrintPipesInfo()
//   Prints the pipes information for the specified alternate setting
// Parameters:
//   pAltSet [in] pointer to the alternate setting information
// Return Value:
//   None
void PrintPipesInfo(WDU_ALTERNATE_SETTING *pAltSet)
{
    BYTE p;
    WDU_PIPE_INFO *pPipe;

    if (!pAltSet->Descriptor.bNumEndpoints)
    {
        printf("  no pipes are defined for this device other than the default pipe (number 0).\n");
        return;
    }
    for (p=0; p<pAltSet->Descriptor.bNumEndpoints; p++)
    {
        pPipe = &pAltSet->pPipes[p];

        printf("  pipe num. 0x%lx: packet size %ld, type %s, dir %s, interval %ld (ms)\n",
            pPipe->dwNumber,
            pPipe->dwMaximumPacketSize,
            pipeType2Str(pPipe->type),
            pPipe->direction==WDU_DIR_IN ? "In" : pPipe->direction==WDU_DIR_OUT ? "Out" : "In & Out",
            pPipe->dwInterval);
    }
}

static void PrintEndpoints(WDU_ALTERNATE_SETTING *pAltSet)
{
    BYTE endp;
    WDU_ENDPOINT_DESCRIPTOR *pEndp;

    for (endp=0; endp<pAltSet->Descriptor.bNumEndpoints; endp++)
    {
        pEndp = &pAltSet->pEndpointDescriptors[endp];
        printf("    end-point address: 0x%02x, attributes: 0x%x, max packet %d, Interval: %d\n",
            pEndp->bEndpointAddress,
            pEndp->bmAttributes,
            pEndp->wMaxPacketSize,
            pEndp->bInterval);
    }
}

// Function: PrintDeviceConfigurations()
//   Prints the device's configurations information
// Parameters:
//   hDevice [in] handle to the USB device
// Return Value:
//   None
void PrintDeviceConfigurations(HANDLE hDevice)
{
    DWORD dwError;
    WDU_DEVICE *pDevice = NULL;
    DWORD ifc, alt;
    UINT32 iConf;

    WDU_CONFIGURATION *pConf;
    WDU_INTERFACE *pInterface;
    WDU_ALTERNATE_SETTING *pAltSet;

    dwError = WDU_GetDeviceInfo(hDevice, &pDevice);
    if (dwError)
    {
        ERR("PrintDeviceConfigurations: WDU_GetDeviceInfo failed: error 0x%lx (\"%s\")\n",
            dwError, Stat2Str(dwError));
        goto Exit;
    }

    printf("This device has %d configurations:\n", pDevice->Descriptor.bNumConfigurations);
    for (iConf=0; iConf<pDevice->Descriptor.bNumConfigurations; iConf++)
    {
        printf("  %d. configuration value %d (has %ld interfaces)\n", 
            iConf, pDevice->pConfigs[iConf].Descriptor.bConfigurationValue,
            pDevice->pConfigs[iConf].dwNumInterfaces);
    }
    iConf = 0;
    if (pDevice->Descriptor.bNumConfigurations>1)
    {
        printf("Please enter the configuration index to display (dec - zero based): ");
        fgets(line, sizeof(line), stdin);
        sscanf(line, "%d", &iConf);
        if (iConf >= pDevice->Descriptor.bNumConfigurations)
        {
            printf("ERROR: invalid configuration index, valid values are 0-%d\n",
                pDevice->Descriptor.bNumConfigurations);
            goto Exit;
        }
    }
    pConf = &pDevice->pConfigs[iConf];

    printf("The configuration indexed %d has %ld interface(s):\n",
        iConf, pConf->dwNumInterfaces);

    for (ifc=0; ifc<pConf->dwNumInterfaces; ifc++)
    {
        pInterface = &pConf->pInterfaces[ifc];
        printf("interface no. %d:\n", 
            pInterface->pAlternateSettings[0].Descriptor.bInterfaceNumber);
        for (alt=0; alt<pInterface->dwNumAltSettings; alt++)
        {
            pAltSet = &pInterface->pAlternateSettings[alt];

            printf("  alternate: %d, endpoints: %d, class: 0x%x, subclass: 0x%x, protocol: 0x%x\n",
                pAltSet->Descriptor.bAlternateSetting,
                pAltSet->Descriptor.bNumEndpoints,
                pAltSet->Descriptor.bInterfaceClass,
                pAltSet->Descriptor.bInterfaceSubClass,
                pAltSet->Descriptor.bInterfaceProtocol);

            PrintEndpoints(pAltSet);
        }
        printf("\n");
    }
    printf("\n");

Exit:
    if (pDevice)
        WDU_PutDeviceInfo(pDevice);
}

// Function: ReadWritePipesMenu()
//   Displays menu to read/write from the device's pipes
// Parameters:
//   hDevice [in] handle to the USB device
// Return Value:
//   None
void ReadWritePipesMenu(HANDLE hDevice)
{
    DWORD dwError;
    WDU_DEVICE *pDevice;
    WDU_ALTERNATE_SETTING *pAltSet;
    WDU_PIPE_INFO *pPipes;
    BYTE  SetupPacket[8];
    DWORD cmd, dwPipeNum, dwSize, dwBytesTransferred;
    BYTE i=0;
    VOID *pBuffer = NULL;
    USB_LISTEN_PIPE listenPipe;
    int c;

    dwError = WDU_GetDeviceInfo(hDevice, &pDevice);
    if (dwError)
    {
        ERR("ReadWritePipesMenu: WDU_GetDeviceInfo() failed: error 0x%lx (\"%s\")\n",
            dwError, Stat2Str(dwError));
        return;
    }

    pAltSet = pDevice->pActiveInterface->pActiveAltSetting;
    pPipes = pAltSet->pPipes;

    PrintPipesInfo(pAltSet);

    do {
        printf("\n");
        printf("Read/Write from/to device's pipes\n");
        printf("---------------------\n");
        printf("1.  Read from pipe\n");
        printf("2.  Write to pipe\n");
        printf("3.  Listen to pipe (contiguous read)\n");
        printf("99. Main menu\n");
        printf("Enter option: ");
        cmd = 0;
        fgets(line, sizeof(line), stdin);
        sscanf(line, "%ld", &cmd);

        if (cmd==99)
            break;
        if (cmd<1 || cmd>3)
            continue;

        printf("Please enter the pipe number (hex): 0x");
        fgets(line, sizeof(line), stdin);
        dwPipeNum=0;
        sscanf(line, "%lx", &dwPipeNum);

        // search for the pipe
        if (dwPipeNum)
        {
            for (i=0; i<pAltSet->Descriptor.bNumEndpoints; i++)
                if (pPipes[i].dwNumber==dwPipeNum)
                    break;
            if (i >= pAltSet->Descriptor.bNumEndpoints)
            {
                printf("The pipe number 0x%lx does not exist, please try again.\n", dwPipeNum);
                continue;
            }
        }

        switch (cmd)
        {
        case 1:
        case 2:
            if (!dwPipeNum || pPipes[i].type==PIPE_TYPE_CONTROL)
            {
                printf("Please enter setup packet (hex - 8 bytes): ");
                DIAG_GetHexBuffer((PVOID) SetupPacket, 8);
            }
            printf("Please enter the size of the buffer (dec):  ");
            fgets(line, sizeof(line), stdin);
            sscanf(line, "%ld", &dwSize);
            if (dwSize)
            {
                pBuffer = malloc(dwSize);
                if (!pBuffer)
                {
                    ERR("cannot alloc memory\n");
                    break;
                }
                if (cmd==2)
                {
                    printf("Please enter the input buffer (hex): ");
                    DIAG_GetHexBuffer(pBuffer, dwSize);
                }
            }
            dwError = WDU_Transfer(hDevice, dwPipeNum? pPipes[i].dwNumber : 0, cmd==1, 0, pBuffer, 
                dwSize, &dwBytesTransferred, SetupPacket, TRANSFER_TIMEOUT);
            if (dwError)
                ERR("ReadWritePipesMenu: WDU_Transfer() failed: error 0x%lx (\"%s\")\n",
                    dwError, Stat2Str(dwError));
            else
            {
                printf("Transferred %ld bytes\n", dwBytesTransferred);
                if (cmd==1 && pBuffer)
                    DIAG_PrintHexBuffer(pBuffer, dwBytesTransferred, TRUE);
            }
            if (pBuffer)
                free(pBuffer);
            break;

        case 3:
            if (!dwPipeNum || pPipes[i].type==PIPE_TYPE_CONTROL)
            {
                printf("Cannot listen to control pipes.\n");
                break;
            }
            BZERO(listenPipe);
            listenPipe.hDevice = hDevice;
            listenPipe.pPipe = &pPipes[i];

            printf("Press <Enter> to start listening. While listening, press <Enter> to stop\n\n");
            getchar();
            ListenToPipe(&listenPipe);
            if (listenPipe.dwError)
            {
                ERR("ReadWritePipesMenu: error listening to pipe 0x%lx: error 0x%lx (\"%s\")\n",
                    dwPipeNum, listenPipe.dwError, Stat2Str(listenPipe.dwError));
                break;
            }

            while ((c=getchar())!=10) {}   // ESC code
            CloseListening(&listenPipe);
            if (listenPipe.dwError)
                ERR("ReadWritePipesMenu: WDU_Transfer failed: error 0x%lx (\"%s\")\n",
                    listenPipe.dwError, Stat2Str(listenPipe.dwError));
            break;
        }
    } while (1);

    if (pDevice)
        WDU_PutDeviceInfo(pDevice);
}

