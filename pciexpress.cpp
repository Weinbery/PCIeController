// PCIECard.cpp : 定义 DLL 应用程序的导出函数。
//
#include "pciexpress.h"

void DLLCALLCONV InterruptHandler(PVOID pData)
{
	PWDC_DEVICE pDev = (PWDC_DEVICE)pData;
	PPCIeDeviceCTX pDevCtx = (PPCIeDeviceCTX)WDC_GetDevContext(pDev);
	PCIeResult pcieResult;
	BZERO(pcieResult);
	pcieResult.dwCounter = pDev->Int.dwCounter;
	pcieResult.dwLost = pDev->Int.dwLost;
	pcieResult.waitResult = (WD_INTERRUPT_WAIT_RESULT)pDev->Int.fStopped;

	/* Execute the diagnostics application's interrupt handler routine */
	pDevCtx->funcDiagIntHandler((WDC_DEVICE_HANDLE)pDev, &pcieResult);
}

// 中断处理程序，这里完成一次DMA读写则触发一个中断
void InterruptFunction(WDC_DEVICE_HANDLE hDev, PCIeResult *pIntResult)
{
	static UINT nCount = 0;
	nCount++;

	UINT nValue = 0;

	PPCIeDeviceCTX pDevCtx = (PPCIeDeviceCTX)WDC_GetDevContext(hDev);

	WDC_ReadAddr32(hDev, 0, 0x28, &nValue);		// 读出控制状态寄存器
	if((nValue & 8) == 8)						// DMA读完成标志
	{
		WDC_WriteAddr32(hDev, 0, 0x28, nValue);	// 清除DMA读完成标志位
		pDevCtx->nWriteCount++;
		SetEvent(pDevCtx->hWriteEvent);
	}
	else if((nValue & 2) == 2)					// DMA写完成标志
	{
		// 清除DMA写完成标志
		WDC_WriteAddr32(hDev, 0, 0x28, nValue);
		pDevCtx->nReadCount++;
		SetEvent(pDevCtx->hReadEvent);
	}
}

void InitDevice(PCIeParameter *pCard)
{
	// 复位PCIE卡
	WriteRegister(pCard, 0x2C, 0x01); // 施加复位
	Sleep(1);

	WriteRegister(pCard, 0x2C, 0x00); // 解除复位
	Sleep(1);

	UINT nCount = 0;

	while (true)
	{
		nCount++;
		UINT nLinkValue = ReadRegister(pCard, 0x38);// 用户状态/控制寄存器
		if((nLinkValue & 0x00010000) == 0x00010000)	// 检测PCIe链路初始化是否完成
		{
			break;
		}
		Sleep(1);
		if(nCount >= 1000)
		{
			break;
		}
	}
}

BOOL InitHandle(PCIeParameter *pCard)
{
	PPCIeDeviceCTX pDevCtx = (PPCIeDeviceCTX)WDC_GetDevContext(pCard->hDevHandle);
	// 初始化读内存中断
	WD_TRANSFER* pTransfer = NULL;
	//
	if(!WDC_IntIsEnabled(pCard->hDevHandle))
	{
		WriteRegister(pCard, 0x20, 0xFFFFFFFF);	// 屏蔽PCIE中断
		// 
		pTransfer = new WD_TRANSFER();
		PWDC_DEVICE pDev = (PWDC_DEVICE)pCard->hDevHandle;

		pTransfer->dwPort = pDev->pAddrDesc[0].kptAddr + 0x24;
		pTransfer->cmdTrans = WM_DWORD;
		pTransfer->Data.Dword = 3;
		// 中断函数
		pDevCtx->funcDiagIntHandler = InterruptFunction;
		// 
		DWORD dwStatus = WDC_IntEnable(pCard->hDevHandle, pTransfer, 1, INTERRUPT_CMD_COPY,
			InterruptHandler, (PVOID)pDev, WDC_IS_KP(pCard->hDevHandle));
		// 
		if(dwStatus != WD_STATUS_SUCCESS)
		{
			return FALSE;
		}
	}

	return TRUE;
}

// 扫描模拟源卡
BOOL DisplayCards(std::list<WD_PCI_CARD_INFO> &pcieCardlist)
{
	WDC_PCI_SCAN_RESULT displayResult;
	BZERO(displayResult);
	//
	static BOOL bFlag1 = FALSE;
	static BOOL bFlag2 = FALSE;
	if(!bFlag1)
	{
		DWORD dwResult = WDC_SetDebugOptions(WDC_DBG_DEFAULT, NULL);
		if(dwResult != WD_STATUS_SUCCESS)
		{
			return FALSE;
		}
		//
		dwResult = WDC_DriverOpen(WDC_DRV_OPEN_DEFAULT, WDC_LICENSESTR);
		if(dwResult != WD_STATUS_SUCCESS)
		{
			return FALSE;
		}
		bFlag1 = TRUE;
	}

	DWORD dwResult = WDC_PciScanDevices(VENDORID, DEVICEID, &displayResult);
	if(WD_STATUS_SUCCESS != dwResult)
	{
		return FALSE;
	}

	for(UINT i = 0; i < (displayResult.dwNumDevices); i++)
	{
		WD_PCI_CARD_INFO deviceInfo;
		BZERO(deviceInfo);
		deviceInfo.pciSlot = displayResult.deviceSlot[i];
		dwResult = WDC_PciGetDeviceInfo(&deviceInfo);
		if(WD_STATUS_SUCCESS != dwResult)
		{
			continue;
		}

		pcieCardlist.push_back(deviceInfo);

		bFlag2 = TRUE;
	}

	return (bFlag1 && bFlag2);
}

BOOL GetCards(std::list<PCIeParameter> &pcieCardlist)
{
	list<WD_PCI_CARD_INFO> pcieCardInfoList;
	if(!DisplayCards(pcieCardInfoList))
	{
		return FALSE;
	}
	//
	list<WD_PCI_CARD_INFO>::iterator iter = pcieCardInfoList.begin();
	for(; iter != pcieCardInfoList.end(); iter++)
	{
		PCIeParameter pcieParam;
		pcieParam.bOpen = FALSE;
		pcieParam.bReadStop = FALSE;
		pcieParam.bWriteStop = FALSE;
		::InitializeCriticalSection(&pcieParam.criticalSection);
		pcieParam.pcieCardInfo = *iter;
		pcieCardlist.push_back(pcieParam);
	}

	return TRUE;
}

BOOL DisplayCard(WD_PCI_CARD_INFO* pCardInfo)
{
	static BOOL bFlag = FALSE;
	//
	WDC_PCI_SCAN_RESULT displayResult;
	BZERO(displayResult);
	//
	if(!bFlag)
	{
		DWORD dwResult = WDC_SetDebugOptions(WDC_DBG_DEFAULT, NULL);
		if(dwResult != WD_STATUS_SUCCESS)
		{
			return FALSE;
		}
		dwResult = WDC_DriverOpen(WDC_DRV_OPEN_DEFAULT, WDC_LICENSESTR);
		if(dwResult != WD_STATUS_SUCCESS)
		{
			return FALSE;
		}
		bFlag = TRUE;
	}

	DWORD dwResult = WDC_PciScanDevices(VENDORID, DEVICEID, &displayResult);
	if(WD_STATUS_SUCCESS != dwResult)
	{
		return FALSE;
	}

	for(UINT i = 0; i < displayResult.dwNumDevices; i++)
	{
		WD_PCI_CARD_INFO deviceInfo;
		BZERO(deviceInfo);
		deviceInfo.pciSlot = displayResult.deviceSlot[i];
		dwResult = WDC_PciGetDeviceInfo(&deviceInfo);
		if(WD_STATUS_SUCCESS != dwResult)
		{
			continue;
		}
		//
		*pCardInfo = deviceInfo;
	}

	return TRUE;
}

BOOL GetCard(PCIeParameter *pCard)
{
	pCard->bOpen = FALSE;
	pCard->bReadStop = FALSE;
	pCard->bWriteStop = FALSE;
	//
	BOOL bFlag = DisplayCard(&pCard->pcieCardInfo);
	//
	if (!bFlag)
		return FALSE;
	else
		::InitializeCriticalSection(&pCard->criticalSection);

	return TRUE;
}

BOOL Open(PCIeParameter *pCard)
{
	PPCIeDeviceCTX pDevCtx = (PPCIeDeviceCTX)malloc(sizeof(PCIeDeviceCTX));
	//
	WORD wData = 0;
	DWORD dwResult = WDC_PciDeviceOpen(&pCard->hDevHandle, &pCard->pcieCardInfo, pDevCtx, NULL, NULL, NULL);
	if(WD_STATUS_SUCCESS != dwResult)
	{
		WDC_PciDeviceClose(pCard->hDevHandle);
		return FALSE;
	}
	if(WDC_PciReadCfg16(pCard->hDevHandle, 0x68, &wData) != WD_STATUS_SUCCESS)
	{
		WDC_PciDeviceClose(pCard->hDevHandle);
		return FALSE;
	}
	//
	wData = wData & 0xFFF;
	//
	if(WDC_PciWriteCfg16(pCard->hDevHandle, 0x68, wData) != WD_STATUS_SUCCESS)
	{
		WDC_PciDeviceClose(pCard->hDevHandle);
		return FALSE;
	}

	InitDevice(pCard);

	InitHandle(pCard);

	pCard->bOpen = TRUE;

	return TRUE;
}

UINT ReadRegister(PCIeParameter *pCard, UINT nOffset)
{ 
	UINT nValue = 0;
	//
	DWORD dwResult = WDC_ReadAddr32(pCard->hDevHandle, 0, nOffset, &nValue);
	//
	if(dwResult != WD_STATUS_SUCCESS)
	{
		return 0;
	}

	return nValue;
}

BOOL WriteRegister(PCIeParameter *pCard, UINT nOffset, UINT nValue)
{
	DWORD dwResult = WDC_WriteAddr32(pCard->hDevHandle, 0, nOffset, nValue);
	//
	if(dwResult != WD_STATUS_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL PrepareTransfer(PCIeParameter *pCard)
{
	// 首先准备用于传输的物理内存,这里为224k物理内存
	for(int i = 0; i < PHYSIC_MEMORY_NUMBER; i++)
	{
		DWORD dwResult = WDC_DMAContigBufLock(pCard->hDevHandle, &pCard->devWriteTransfer.pBuffer[i], 
			DMA_WRITE_TO_DEVICE, DMA_BLOCK_SIZE, &pCard->devWriteTransfer.pWd_Dma[i]);
		//
		if(dwResult != WD_STATUS_SUCCESS)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CompleteTransfer(PCIeParameter *pCard)
{
	for(int i = 0; i < PHYSIC_MEMORY_NUMBER; i++)
	{
		DWORD dwResult = WDC_DMABufUnlock(pCard->devWriteTransfer.pWd_Dma[i]);
		//
		if(dwResult != WD_STATUS_SUCCESS)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL PrepareRead(PCIeParameter *pCard)
{
	// 准备内核事件，一个是中断内核事件、一个是停止内核事件
	for(int i = 0; i < PHYSIC_MEMORY_NUMBER; i++)
	{
		DWORD dwResult = WDC_DMAContigBufLock(pCard->hDevHandle, &pCard->devReadTransfer.pBuffer[i], 
			DMA_READ_FROM_DEVICE, DMA_BUFFER_SIZE, &pCard->devReadTransfer.pWd_Dma[i]);
		//
		if(dwResult != WD_STATUS_SUCCESS)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CompleteRead(PCIeParameter *pCard)
{
	for(int i = 0; i < PHYSIC_MEMORY_NUMBER; i++)
	{
		DWORD dwResult = WDC_DMABufUnlock(pCard->devReadTransfer.pWd_Dma[i]);
		//
		if(dwResult != WD_STATUS_SUCCESS)
		{
			return FALSE;
		}
	}

	return TRUE;
}

void StartRead(PCIeParameter *pCard)
{
	pCard->hReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	pCard->bReadStop = FALSE;
}

void StartTransfer(PCIeParameter *pCard)
{
	pCard->hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	pCard->bWriteStop = FALSE;
}

void StopRead(PCIeParameter *pCard)
{
	if(pCard->bReadStop)
	{
		return;
	}

	pCard->bReadStop = TRUE;

	SetEvent(pCard->hReadEvent);

	WriteRegister(pCard, 0x28, 0x10); // DMA写终止操作
}

void StopTransfer(PCIeParameter *pCard)
{
	if(pCard->bWriteStop)
	{
		return;
	}

	pCard->bWriteStop = TRUE;

	SetEvent(pCard->hWriteEvent);

	WriteRegister(pCard, 0x28, 0x20); // DMA读终止操作
}

// 从板卡读数据
UINT Read(PCIeParameter *pCard, BYTE* pBuffer, UINT nSize, int nIndex)
{
	// 禁用写DMA的中断屏蔽
	UINT nValue = ReadRegister(pCard, 0x20);
	nValue &= 0x7ffffffe;
	WriteRegister(pCard, 0x20, nValue);
	// 
	nValue = ReadRegister(pCard, 0x38);
	// 构造使能记录值
	nValue |= 0x01;  
	// 使能记录 + 使能模拟
	WriteRegister(pCard, 0x38, nValue);

	PPCIeDeviceCTX pDevCtx = (PPCIeDeviceCTX)WDC_GetDevContext(pCard->hDevHandle);
	pDevCtx->hReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE hReadEvent[2] = {pDevCtx->hReadEvent, pCard->hReadEvent};
	pDevCtx->nReadCount = 0;

	while (!pCard->bReadStop)
	{
		// DMA参数
		WD_DMA* pDma = pCard->devReadTransfer.pWd_Dma[nIndex];
		// 写入DMA要使用到的物理内存地址
		WriteRegister(pCard, 0x4, pDma->Page[0].pPhysicalAddr);
		// 高4字节
		WriteRegister(pCard, 0x8, 0);
		// 写长度
		WriteRegister(pCard, 0x18, nSize);
		// 
		::EnterCriticalSection(&pCard->criticalSection);
		pDevCtx->nReadSize = 0;
		nValue = 0x01;
		// 开始DMA写内存
		WriteRegister(pCard, 0x28, nValue);
		// 等待写内存完成中断
		if(::WaitForMultipleObjects(2, hReadEvent, FALSE, INFINITE) != WAIT_OBJECT_0)
		{
			// 如果收到停止信号，则结束
			::LeaveCriticalSection(&pCard->criticalSection);
			break;
		}
		::LeaveCriticalSection(&pCard->criticalSection);
		memcpy(pBuffer, pCard->devReadTransfer.pBuffer[nIndex], nSize);
		memset(pCard->devReadTransfer.pBuffer[nIndex], 0xff, nSize);
		break;
	}

	CloseHandle(pDevCtx->hReadEvent);

	WriteRegister(pCard, 0x28, 0x10);	//  DMA写终止

	return nSize;
}

// 向板卡写数据
UINT Write(PCIeParameter *pCard, BYTE* pBuffer, UINT nSize)
{
	// 如果nSize不是DMA_WRITE_SIZE的整数倍，则返回，不进行DMA操作
	if(nSize % DMA_WRITE_SIZE != 0)
	{
		return 0;
	}
	// 禁用读DMA的中断屏蔽
	UINT nValue = ReadRegister(pCard, 0x20);
	// 构造读完成终端屏蔽值，高有效
	nValue &= 0x7ffffffd;
	WriteRegister(pCard, 0x20, nValue);
	// 
	PPCIeDeviceCTX pDevCtx = (PPCIeDeviceCTX)WDC_GetDevContext(pCard->hDevHandle);
	pDevCtx->hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	// 总块数
	UINT nMaxBlocks = (nSize / DMA_BLOCK_SIZE) + (nSize % DMA_BLOCK_SIZE) ? 1 : 0;
	// 初始化已经读内存数量
	pDevCtx->nWriteCount = 0;
	// 
	BYTE* pAgentBuffer = pBuffer;
	// 
	HANDLE hWriteEvent[2] = {pDevCtx->hWriteEvent, pCard->hWriteEvent}; 
	// DMA寄存器读取块长度
	do
	{
		// 所有的数据都读完了
		if(pDevCtx->nWriteCount == nMaxBlocks)
		{
			break;
		}
		// PHYSIC_MEMORY_NUMBER个物理内存的数据全部读取完了，要重新复制数据
		if(pDevCtx->nWriteCount % PHYSIC_MEMORY_NUMBER == 0)
		{
			for(UINT i = 0; (i < PHYSIC_MEMORY_NUMBER) && (i < nMaxBlocks); i++)
			{
				memcpy(pCard->devWriteTransfer.pBuffer[i], pAgentBuffer, nSize);
				pAgentBuffer += nSize;
			}
		}
		// 写入长度值
		WriteRegister(pCard, 0x1c, nSize);
		// 设置下一个block开始
		WD_DMA *pDma = pCard->devWriteTransfer.pWd_Dma[pDevCtx->nWriteCount % PHYSIC_MEMORY_NUMBER];
		WriteRegister(pCard, 0x0c, pDma->Page[0].pPhysicalAddr); // DMA寄存器读取内存物理地址低32位
		WriteRegister(pCard, 0x10, 0);							 // DMA寄存器读取内存物理地址高32位
		// 开始读内存
		::EnterCriticalSection(&pCard->criticalSection);
		nValue = ReadRegister(pCard, 0x28);
		// 构造DMA读开始控制值
		nValue |= 4;
		// 开始DMA写内存
		WriteRegister(pCard, 0x28, nValue);
		// 等待完成
		if(::WaitForMultipleObjects(2, hWriteEvent, FALSE, INFINITE) != WAIT_OBJECT_0)
		{
			// 如果收到停止信号，则结束
			::LeaveCriticalSection(&pCard->criticalSection);
			break;
		}
		::LeaveCriticalSection(&pCard->criticalSection);
		// 
	} while(true);

	CloseHandle(pDevCtx->hWriteEvent);

	return (pAgentBuffer - pBuffer);
}

void Close(PCIeParameter *pCard)
{
	// 启动中断掩码功能
	WriteRegister(pCard, 0x20, 0xffffffff);	

	WriteRegister(pCard, 0x38, 0x00);

	WDC_PciDeviceClose(pCard->hDevHandle);

	::DeleteCriticalSection(&pCard->criticalSection);
}
