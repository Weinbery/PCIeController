////////////////////////////////////////////////////////////////
// File - pcmcia_diag_lib.h
//
// Copyright (c) 2003 - 2005 Jungo Ltd.  http://www.jungo.com
//
////////////////////////////////////////////////////////////////

#ifndef _PCMCIA_DIAG_LIB_H_
#define _PCMCIA_DIAG_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

// Function: PCMCIA_Get_WD_handle()
//   Get handle to WinDriver
BOOL PCMCIA_Get_WD_handle(HANDLE *phWD);
// Function: PCMCIA_Print_all_cards_info()
//   Print all PCMCIA devices attached
void PCMCIA_Print_all_cards_info();

#ifdef __cplusplus
}
#endif

#endif
