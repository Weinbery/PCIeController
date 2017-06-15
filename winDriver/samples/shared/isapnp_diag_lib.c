////////////////////////////////////////////////////////////////
// File - ISAPNP_DIAG_LIB.C
//
// Utility functions for printing card information,
// detecting PCI cards, and accessing PCI configuration
// registers.
// 
// Copyright (c) 2003 - 2005 Jungo Ltd.  http://www.jungo.com
// 
////////////////////////////////////////////////////////////////

#include "windrvr.h"
#ifdef _USE_SPECIFIC_KERNEL_DRIVER_
    #undef WD_Open
    #define WD_Open WD_OpenKernelHandle
    #if defined(UNIX)
        #undef WD_FUNCTION
        #define WD_FUNCTION(wFuncNum,h,pParam,dwSize,fWait) ((ULONG) ioctl((int)(h), wFuncNum, pParam))
    #endif
#endif
#include "print_struct.h"
#include <stdio.h>
#include "isapnp_diag_lib.h"

// Function: ISAPNP_Get_WD_handle()
//   Get handle to WinDriver
// Parameters:
//   phWD [out] pointer to handle to be received
// Return Value:
//   Success/fail

BOOL ISAPNP_Get_WD_handle(HANDLE *phWD)
{
    WD_VERSION ver;

    *phWD = INVALID_HANDLE_VALUE;
    *phWD = WD_Open();

    // Check whether handle is valid and version OK
    if (*phWD==INVALID_HANDLE_VALUE) 
    {
        printf("Failed opening " WD_PROD_NAME " device\n");
        return FALSE;
    }

    BZERO(ver);
    WD_Version(*phWD,&ver);
    if (ver.dwVer<WD_VER) 
    {
        printf("Incorrect " WD_PROD_NAME " version\n");
        WD_Close (*phWD);
        *phWD = INVALID_HANDLE_VALUE;
        return FALSE;
    }

    return TRUE;
}

// Function: ISAPNP_Print_all_cards_info()
//   Print all ISA PnP devices attached
// Parameters:
//   None
// Return Value:
//   None

void ISAPNP_Print_all_cards_info()
{
    HANDLE hWD;
    WD_ISAPNP_SCAN_CARDS scanCards;
    WD_ISAPNP_CARD_INFO cardInfo;
    DWORD i, j;

    if (!ISAPNP_Get_WD_handle (&hWD)) return;

    printf ("ISA PnP bus scan:\n\n");
    BZERO(scanCards);
    WD_IsapnpScanCards (hWD, &scanCards);
    for (i=0; i<scanCards.dwCards; i++)
    {
        printf ("Card %ld: %s\n", i, scanCards.Card[i].cIdent);
        for (j=0; j<scanCards.Card[i].dwLogicalDevices; j++)
        {
            BZERO(cardInfo);
            cardInfo.cardId = scanCards.Card[i].cardId;
            cardInfo.dwLogicalDevice = j;
            WD_IsapnpGetCardInfo(hWD, &cardInfo);
            if (strlen(cardInfo.cIdent))
                printf ("Device %ld: %s, ", j, cardInfo.cIdent);
            printf ("Vendor ID: %s, ", scanCards.Card[i].cardId.cVendor );
            printf ("Serial number: %lx\n", scanCards.Card[i].cardId.dwSerial );
            WD_CARD_print(&cardInfo.Card, "   ");
            printf ("\n");
        }
    }
    WD_Close (hWD);
}

