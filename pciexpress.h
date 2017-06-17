#ifndef PCIEXPRESS_H
#define PCIEXPRESS_H

#include <windows.h>
#include <list>
#include <stdlib.h>
#include "wdc_defs.h"
#include "wdc_lib.h"

using namespace std;

#define		PHYSIC_MEMORY_NUMBER	2
#define		DMA_WRITE_SIZE			512
#define		DMA_BLOCK_SIZE			(224 * 1024)
#define		DMA_BUFFER_SIZE			(1024 * 1024)

#define		VENDORID				0x10EE
#define		DEVICEID				0x0007

#define		WDC_LICENSESTR			"6C3CC2CFE89E7AD0424A070D434A6F6DC495654A.gkhy" // 6C3CC2CFE89E7AD0424A070D434A6F6DC495654A.gkhy

typedef struct _PCIeResult
{
	DWORD						dwLost;
	DWORD						dwCounter;
	WD_INTERRUPT_WAIT_RESULT	waitResult;
}PCIeResult, *PPCIeResult;

typedef void (*PCIINT_HANDLER)(WDC_DEVICE_HANDLE hDev, PPCIeResult pResult);

typedef void (*PCIEVENT_HANDLER)(WDC_DEVICE_HANDLE hDev, DWORD dwAction);

typedef struct _PCIeDeviceCTX
{
    UINT				nReadSize;
    UINT				nReadCount;
    UINT				nWriteCount;
    HANDLE				hReadEvent;
    HANDLE				hWriteEvent;
	WD_TRANSFER*		pIntTransCmds;
	PCIINT_HANDLER		funcDiagIntHandler;
	PCIEVENT_HANDLER	funcDiagEventHandler;
}PCIeDeviceCTX, *PPCIeDeviceCTX;

typedef struct _PCIeDevice
{
	PVOID		pBuffer[PHYSIC_MEMORY_NUMBER];
	WD_DMA*		pWd_Dma[PHYSIC_MEMORY_NUMBER];
}PCIeDevice, *PPCIeDevice;

typedef struct _PCIeParameter
{
	BOOL				bOpen;
	BOOL				bReadStop;
	BOOL				bWriteStop;
	HANDLE				hReadEvent;
	HANDLE				hWriteEvent;
	PCIeDevice			devReadTransfer;
	PCIeDevice			devWriteTransfer;
	WD_PCI_CARD_INFO	pcieCardInfo;
	CRITICAL_SECTION	criticalSection;
	WDC_DEVICE_HANDLE	hDevHandle;
}PCIeParameter, *PPCIeParameter;


BOOL DisplayCards(std::list<WD_PCI_CARD_INFO> &pcieCardlist);
BOOL GetCards(std::list<PCIeParameter> &pcieCardlist);
BOOL DisplayCard(WD_PCI_CARD_INFO *pCardInfo);
BOOL GetCard(PCIeParameter *pCard);
void InitDevice(PCIeParameter *pCard);
BOOL InitHandle(PCIeParameter *pCard);
BOOL Open(PCIeParameter *pCard);
BOOL PrepareTransfer(PCIeParameter *pCard);
UINT Write(PCIeParameter *pCard, BYTE* pBuffer, UINT nSize);
BOOL CompleteTransfer(PCIeParameter *pCard);
BOOL PrepareRead(PCIeParameter *pCard);
UINT Read(PCIeParameter *pCard, BYTE* pBuf, UINT nSize, int nIndex);
BOOL CompleteRead(PCIeParameter *pCard);
void StartRead(PCIeParameter *pCard);
void StopRead(PCIeParameter *pCard);
void StartTransfer(PCIeParameter *pCard);
void StopTransfer(PCIeParameter *pCard);
UINT ReadRegister(PCIeParameter *pCard, UINT nOffset);
BOOL WriteRegister(PCIeParameter *pCard, UINT nOffset, UINT nValue);
void Close(PCIeParameter *pCard);

#endif // PCIEXPRESS_H
