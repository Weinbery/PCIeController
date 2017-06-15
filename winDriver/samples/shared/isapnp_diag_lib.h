////////////////////////////////////////////////////////////////
// File - isapnp_diag_lib.h
//
// Copyright (c) 2003 - 2005 Jungo Ltd.  http://www.jungo.com
//
////////////////////////////////////////////////////////////////

#ifndef _ISAPNP_DIAG_LIB_H_
#define _ISAPNP_DIAG_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

// Function: ISAPNP_Get_WD_handle()
//   Get handle to WinDriver
BOOL ISAPNP_Get_WD_handle(HANDLE *phWD);

// Function: ISAPNP_Print_all_cards_info()
//   Print all ISA PnP devices attached
void ISAPNP_Print_all_cards_info();

#ifdef __cplusplus
}
#endif

#endif
