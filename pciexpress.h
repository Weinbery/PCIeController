#pragma once

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

#define		VENDORID				0x10EE  // 厂商编号
#define		DEVICEID				0x0007	// 设备编号

#define		WDC_LICENSESTR			"6C3CC2CFE89E7AD0424A070D434A6F6DC495654A.gkhy" // 6C3CC2CFE89E7AD0424A070D434A6F6DC495654A.gkhy

typedef struct _PCIeResult
{
	DWORD						dwLost;
	DWORD						dwCounter;
	WD_INTERRUPT_WAIT_RESULT	waitResult;
}PCIeResult, *PPCIeResult;
/**
 * 中断函数指针定义
 */
typedef void (*PCIINT_HANDLER)(WDC_DEVICE_HANDLE hDev, PPCIeResult pResult);
/**
 * 事件触发函数指针定义
 */
typedef void (*PCIEVENT_HANDLER)(WDC_DEVICE_HANDLE hDev, DWORD dwAction);
/**
* 设备上下文结构体
*/
typedef struct _PCIeDeviceCTX
{
	UINT				nReadSize;		// 当前读长度，对于DMA来说是写DMA的长度
	UINT				nReadCount;		// 当前读块数，对于DMA来说是写块数
	UINT				nWriteCount;	// 当前写块数，对于DMA来说是读块数
	HANDLE				hReadEvent;		// 读中断事件
	HANDLE				hWriteEvent;	// 写中断事件
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

#ifdef __cplusplus
extern "C" {
#endif
	BOOL __declspec(dllexport) DisplayCards(std::list<WD_PCI_CARD_INFO> &pcieCardlist);
	BOOL __declspec(dllexport) GetCards(std::list<PCIeParameter> &pcieCardlist);
	BOOL __declspec(dllexport) DisplayCard(WD_PCI_CARD_INFO *pCardInfo);
	BOOL __declspec(dllexport) GetCard(PCIeParameter *pCard);
	void __declspec(dllexport) InitDevice(PCIeParameter *pCard);
	BOOL __declspec(dllexport) InitHandle(PCIeParameter *pCard);
	BOOL __declspec(dllexport) Open(PCIeParameter *pCard);
	BOOL __declspec(dllexport) PrepareTransfer(PCIeParameter *pCard);
	UINT __declspec(dllexport) Write(PCIeParameter *pCard, BYTE* pBuffer, UINT nSize);
	BOOL __declspec(dllexport) CompleteTransfer(PCIeParameter *pCard);
	BOOL __declspec(dllexport) PrepareRead(PCIeParameter *pCard);
	UINT __declspec(dllexport) Read(PCIeParameter *pCard, BYTE* pBuf, UINT nSize, int nIndex);
	BOOL __declspec(dllexport) CompleteRead(PCIeParameter *pCard);  
	void __declspec(dllexport) StartRead(PCIeParameter *pCard);
	void __declspec(dllexport) StopRead(PCIeParameter *pCard);
	void __declspec(dllexport) StartTransfer(PCIeParameter *pCard);
	void __declspec(dllexport) StopTransfer(PCIeParameter *pCard);
	UINT __declspec(dllexport) ReadRegister(PCIeParameter *pCard, UINT nOffset);
	BOOL __declspec(dllexport) WriteRegister(PCIeParameter *pCard, UINT nOffset, UINT nValue);
	void __declspec(dllexport) Close(PCIeParameter *pCard);
#ifdef __cplusplus
}
#endif
