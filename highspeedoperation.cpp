#include "highspeedoperation.h"

std::list<PCIeParameter> CHighSpeedOperation::m_pCardList;
std::map<std::string, PPCIeParameter> CHighSpeedOperation::m_mapCardStrParameter;

HighSpeedOperation::HighSpeedOperation()
{
	m_nOffset = 0;
	m_pCard = NULL;
	m_bRecvStop = TRUE;
	m_bSaveStop = TRUE;
	m_bWriteStop = TRUE;
	m_pFileHelper = NULL;
	m_pSaveThread = NULL;
	m_nBlockSize = DMA_DATA_SIZE;
	for(size_t i = 0; i < PHYSIC_MEMORY_NUMBER; i++)
	{
		m_hRecvEvent[i] = NULL;
	}
	try
	{
		m_pBuffer = new BYTE[PCIE_HIGH_BUFFER_SIZE];
		memset(m_pBuffer, 0x00, PCIE_HIGH_BUFFER_SIZE);
	}
	catch(...)
	{
		CTime ct = CTime::GetCurrentTime();
		CString strlog;
		strlog.Format(_T("[%4d-%02d-%02d %02d:%02d:%02d] 内存申请失败: m_pBuffer = new BYTE[PCIE_BUFFER_SIZE].\n"), 
			ct.GetYear(), ct.GetMonth(), ct.GetDay(), ct.GetHour(), ct.GetMinute(), ct.GetSecond());
		theApp.PushOutputInfo(strlog);
	}
};

HighSpeedOperation::~HighSpeedOperation(void)
{
	Destroy();
    SAFE_DELETE_ARRAY(m_pBuffer);
}

void HighSpeedOperation::run()
{

}

void HighSpeedOperation::CreateFileDirectory(const CString strSavePath)
{
	CString strDir = _T("high_speed");
	m_strSavePath = strSavePath + strDir;
	// 
	CFileFind fileFind;
	if (!fileFind.FindFile(m_strSavePath))
	{
		if (!CreateMultipleDirectory(m_strSavePath))
		{
			CTime ct = CTime::GetCurrentTime();
			CString strlog;
			strlog.Format(_T("[%4d-%02d-%02d %02d:%02d:%02d] 创建目录:%s 失败.\n"), ct.GetYear(), ct.GetMonth(), 
				ct.GetDay(), ct.GetHour(), ct.GetMinute(), ct.GetSecond(), m_strSavePath.GetBuffer());
			theApp.PushOutputInfo(strlog);
		}
	}
}

BOOL HighSpeedOperation::SetCard(PPCIeParameter pCard)
{
	if (NULL == pCard)
	{
		return FALSE;
	}
	else
	{
		m_pCard = pCard;
	}

	return TRUE;
}

int HighSpeedOperation::Init()
{
	ProduceEntity(IID_IFastFileHelper, (void**)&m_pFileHelper);
	if (m_pFileHelper == NULL)
	{
		return -1;
	}
	ProduceEntity(IID_IThreadCreatorHelper, (void**)&m_pSaveThread);
	if(m_pSaveThread == NULL)
	{
		return -1;
	}

	return 0;
}

int HighSpeedOperation::Start()
{
	return 0;
}

void HighSpeedOperation::Stop()
{
	// 停止保存
	m_bSaveStop = TRUE;
	if (m_pSaveThread != NULL)
	{
		m_pSaveThread->Destroy();
	}
	// 关闭文件
	if (m_pFileHelper != NULL)
	{
		m_pFileHelper->Close();
	}
}

void HighSpeedOperation::Destroy()
{
	Stop();
	SAFE_RELEASE(m_pSaveThread);
	SAFE_RELEASE(m_pFileHelper);
}

void HighSpeedOperation::BuildEvent()
{
	ClearEvent();
	for(size_t i = 0; i < PHYSIC_MEMORY_NUMBER; i++)
	{
		m_hRecvEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
}

void HighSpeedOperation::ClearEvent()
{
	for(size_t i = 0; i < PHYSIC_MEMORY_NUMBER; i++)
	{
		CloseHandle(m_hRecvEvent[i]);
		m_hRecvEvent[i] = NULL;
	}
}

BOOL HighSpeedOperation::OpenCard()
{
	if (theApp.m_nBus == m_pCard->pcieCardInfo.pciSlot.dwBus)
	{
		CTime ct = CTime::GetCurrentTime();
		CString strlog;
		strlog.Format(_T("[%4d-%02d-%02d %02d:%02d:%02d] PCIe 总线%d, 正在使用, 请重新选择PCIe板卡.\n"), ct.GetYear(), 
			ct.GetMonth(), ct.GetDay(), ct.GetHour(), ct.GetMinute(), ct.GetSecond(), m_pCard->pcieCardInfo.pciSlot.dwBus);
		theApp.PushOutputInfo(strlog);
		return FALSE;
	}
	if(!Open(m_pCard))
	{
		return FALSE;
	}
	if(NULL == m_pCard->criticalSection.DebugInfo)
	{
		::InitializeCriticalSection(&m_pCard->criticalSection);
	}
	m_pCard->bReadStop = FALSE;
	m_pCard->bWriteStop = FALSE;
	theApp.m_nBus = m_pCard->pcieCardInfo.pciSlot.dwBus;

	m_bCardOpen = TRUE;

	return TRUE;
}

BOOL HighSpeedOperation::CloseCard()
{
	if (m_pCard && m_bCardOpen)
	{
		m_pCard->bOpen = FALSE;
		m_bCardOpen = FALSE;
		Close(m_pCard);
	}

	theApp.m_nBus = 0xFF;

	return TRUE;
}

BOOL HighSpeedOperation::InitPCIeDeviceList()
{
	if(!GetCards(m_pCardList))
	{
		return false;
	}
	std::list<PCIeParameter>::iterator iter = m_pCardList.begin();
	for(; iter != m_pCardList.end(); iter++)
	{
		std::string strIndex;
		CString strItem;
		WD_PCI_CARD_INFO *pCardInfo = NULL;
		WD_PCI_SLOT *pciSlot = NULL;
		pCardInfo = &(iter->pcieCardInfo);
		pciSlot = &(pCardInfo->pciSlot);
		strItem.Format(_T("PCIe 总线:%d"), pciSlot->dwBus);
		strIndex = CT2A(strItem);
		std::map<std::string, PPCIeParameter>::iterator iterm;
		iterm = m_mapCardStrParameter.find(strIndex);
		if(iterm != m_mapCardStrParameter.end()) 
		{
			continue;
		}
		m_mapCardStrParameter.insert(make_pair(strIndex, (PPCIeParameter)&(*iter)));
	}

	return TRUE;
}

BOOL HighSpeedOperation::StartReceive()
{
	if (!m_bCardOpen || !m_pCard)
	{
		return FALSE;
	}
	if(PrepareRead(m_pCard))
	{
		StartRead(m_pCard);
	}
	else
	{
		CoupleParameterMap::iterator iter;
		CoupleParameter* pCoupleParameter = NULL;
		m_ConfigLock.lock();
		iter = m_mapCardConfig.find(global_PCIeParameterName[13]);
		pCoupleParameter = &(iter->second);
		pCoupleParameter->nValue = 0;
		WriteRegister(m_pCard, pCoupleParameter->nOffset, pCoupleParameter->nValue); 
		m_ConfigLock.unlock();

		CTime ct = CTime::GetCurrentTime();
		CString strlog;
		strlog.Format(_T("[%4d-%02d-%02d %02d:%02d:%02d] PCIe 总线:%d, DMA准备读写失败.\n"), ct.GetYear(), 
			ct.GetMonth(), ct.GetDay(), ct.GetHour(), ct.GetMinute(), ct.GetSecond(), m_pCard->pcieCardInfo.pciSlot.dwBus);
		theApp.PushOutputInfo(strlog);

		return FALSE;
	}

	m_pcieDeviceInfo.dwFifoOverFlow = 0;
	m_pcieDeviceInfo.dwBufferOverFlow = 0;
	m_pcieDeviceInfo.fStorageQuantity = 0.0f;
	m_pcieDeviceInfo.dwFileCount = 0;

	BuildEvent();

	m_bRecvStop = FALSE; // 开始接收

	DWORD dwThreadID = 0;
	
	m_pThis[0].nIndex = 0;
	m_pThis[0].pThis = this;
	HANDLE hRecvMasterThread = CreateThread(NULL, THREAD_STACK_SIZE, (LPTHREAD_START_ROUTINE)OnReceive, &m_pThis[0], 0, &dwThreadID);
	SetThreadPriority(hRecvMasterThread, THREAD_PRIORITY_HIGHEST);
	SetThreadAffinityMask(hRecvMasterThread, 1); // CPU:1
	// 
	m_pThis[1].nIndex = 1;
	m_pThis[1].pThis = this;
	HANDLE hRecvMinorThread = CreateThread(NULL, THREAD_STACK_SIZE, (LPTHREAD_START_ROUTINE)OnReceive, &m_pThis[1], 0, &dwThreadID);
	SetThreadPriority(hRecvMinorThread, THREAD_PRIORITY_HIGHEST);
	SetThreadAffinityMask(hRecvMinorThread, 2); // CPU:2
	// 
	SetEvent(m_hRecvEvent[0]);
	// 
	m_bSaveStop = FALSE;
	if (m_pSaveThread != NULL)
	{
		m_pSaveThread->RegistUser(this);
		m_pSaveThread->Create(TRUE);
		m_pSaveThread->SetPriority(THREAD_PRIORITY_HIGHEST);
		m_pSaveThread->SetCpuNum(3); // CPU:3
		m_pSaveThread->Resume();
	}

	return TRUE;
}

BOOL HighSpeedOperation::StopReceive()
{
	if(!m_bRecvStop)
	{
		for(int i = 0; i < PHYSIC_MEMORY_NUMBER; i++)
		{
			SetEvent(m_hRecvEvent[i]);
		}
		m_bRecvStop = TRUE; // 停止接收
	}

	StopRead(m_pCard);
	CompleteRead(m_pCard);

	m_RegisterLock.lock();

	m_pcieDeviceInfo.dwGroupAWriteCount = ReadRegister(m_pCard, REGISTER_SEND_ADDR_CH1);
	m_pcieDeviceInfo.dwGroupARecvCount = ReadRegister(m_pCard, REGISTER_RECV_ADDR_CH1);
	m_pcieDeviceInfo.dwGroupACrcErrorCount = ReadRegister(m_pCard, REGISTER_CRC_ADDR_CH1);

	m_RegisterLock.unlock();

	Stop();

	return TRUE;
}

BOOL HighSpeedOperation::StartWrite()
{
	// 对于需要上位机发送文件的模式
	if(m_transferParameter.eTransferType == UPPER_MONITER_TRANSFER)
	{
		if (!m_bCardOpen || !m_pCard)
		{
			return FALSE;
		}
		if(PrepareTransfer(m_pCard))
		{
			StartTransfer(m_pCard);
		}
		else
		{
			CoupleParameterMap::iterator iter;
			CoupleParameter* pCoupleParameter = NULL;
			m_ConfigLock.lock();
			iter = m_mapCardConfig.find(global_PCIeParameterName[13]);
			pCoupleParameter = &(iter->second);
			pCoupleParameter->nValue = 0;
			WriteRegister(m_pCard, pCoupleParameter->nOffset, pCoupleParameter->nValue); 
			m_ConfigLock.unlock();

			CTime ct = CTime::GetCurrentTime();
			CString strlog;
			strlog.Format(_T("[%4d-%02d-%02d %02d:%02d:%02d] PCIe 总线:%d, DMA准备读写失败.\n"), ct.GetYear(), 
				ct.GetMonth(), ct.GetDay(), ct.GetHour(), ct.GetMinute(), ct.GetSecond(), m_pCard->pcieCardInfo.pciSlot.dwBus);
			theApp.PushOutputInfo(strlog);

			return FALSE;
		}
		WriteRegister(m_pCard, REGISTER_TRANSFER_TYPE, 0x01); // 文件发送类型
		//
		if(!m_CFile.Open(m_transferParameter.strTransferFile, CFile::modeRead | CFile::typeBinary))
		{
			CTime ct = CTime::GetCurrentTime();
			CString strlog;
			strlog.Format(_T("[%4d-%02d-%02d %02d:%02d:%02d] PCIe 总线:%d, 打开文件:%s失败.\n"), ct.GetYear(), 
				ct.GetMonth(), ct.GetDay(), ct.GetHour(), ct.GetMinute(), ct.GetSecond(), m_pCard->pcieCardInfo.pciSlot.dwBus, 
				m_transferParameter.strTransferFile.GetBuffer());
			theApp.PushOutputInfo(strlog);

			return FALSE;
		}

		size_t nFileSize = m_CFile.GetLength();
		if (nFileSize > 64 * 1024 * 1024)
		{
			CTime ct = CTime::GetCurrentTime();
			CString strlog;
			strlog.Format(_T("[%4d-%02d-%02d %02d:%02d:%02d] PCIe 总线:%d, 发送文件:%s不能大于64M.\n"), ct.GetYear(), 
				ct.GetMonth(), ct.GetDay(), ct.GetHour(), ct.GetMinute(), ct.GetSecond(), m_pCard->pcieCardInfo.pciSlot.dwBus, 
				m_transferParameter.strTransferFile.GetBuffer());
			theApp.PushOutputInfo(strlog);
			return FALSE;
		}

		size_t nTotalFrame = nFileSize / PCIE_HIGH_SPEED_FRAME_SIZE;
		WriteRegister(m_pCard, REGISTER_TRANSFER_TOTAL_FRAME, nTotalFrame);

		m_nTotalSize = 0;
		// 开始发送
		m_bWriteStop = FALSE; 

		DWORD dwThreadID = 0;
		HANDLE hTransferThread = CreateThread(NULL, THREAD_STACK_SIZE, (LPTHREAD_START_ROUTINE)OnTransfer, this, 0, &dwThreadID);
	}
	else if (m_transferParameter.eTransferType == PCIE_AUTO_TRANSFER)
	{
		WriteRegister(m_pCard, REGISTER_TRANSFER_TYPE, 0x00); // PCIe自动发送
		if (m_transferParameter.eDataField == ACCUMULATION_FIELD)
			WriteRegister(m_pCard, REGISTER_DATA_FIELD, 0x00); // ...00000000B
		else if (m_transferParameter.eDataField == RANDOM_NUMBER_FIELD)
			WriteRegister(m_pCard, REGISTER_DATA_FIELD, 0x01); // ...00000001B
		else if (m_transferParameter.eDataField == BLANK_FRAME_FIELD)
			WriteRegister(m_pCard, REGISTER_DATA_FIELD, 0x02); // ...00000010B
		else
			WriteRegister(m_pCard, REGISTER_DATA_FIELD, 0x00);
		// 开始发送
		m_bWriteStop = FALSE;
	}

	CoupleParameterMap::iterator iter;
	CoupleParameter* pCoupleParameter = NULL;

	m_RegisterLock.lock();

	iter = m_mapCardConfig.find(global_PCIeParameterName[7]);
	pCoupleParameter = &(iter->second);
	WriteRegister(m_pCard, pCoupleParameter->nOffset, pCoupleParameter->nValue); 

	iter = m_mapCardConfig.find(global_PCIeParameterName[8]);
	pCoupleParameter = &(iter->second);
	WriteRegister(m_pCard, pCoupleParameter->nOffset, pCoupleParameter->nValue); 

	iter = m_mapCardConfig.find(global_PCIeParameterName[9]);
	pCoupleParameter = &(iter->second);
	WriteRegister(m_pCard, pCoupleParameter->nOffset, pCoupleParameter->nValue); 

	iter = m_mapCardConfig.find(global_PCIeParameterName[10]);
	pCoupleParameter = &(iter->second);
	WriteRegister(m_pCard, pCoupleParameter->nOffset, pCoupleParameter->nValue); 

	iter = m_mapCardConfig.find(global_PCIeParameterName[11]);
	pCoupleParameter = &(iter->second);
	WriteRegister(m_pCard, pCoupleParameter->nOffset, pCoupleParameter->nValue); 

	iter = m_mapCardConfig.find(global_PCIeParameterName[12]);
	pCoupleParameter = &(iter->second);
	WriteRegister(m_pCard, pCoupleParameter->nOffset, pCoupleParameter->nValue); 

	Sleep(10);

	iter = m_mapCardConfig.find(global_PCIeParameterName[13]);
	pCoupleParameter = &(iter->second);
	pCoupleParameter->nValue = 1;
	WriteRegister(m_pCard, pCoupleParameter->nOffset, pCoupleParameter->nValue); 

	m_RegisterLock.unlock();

	return TRUE;
}

BOOL HighSpeedOperation::StopWrite()
{
	m_bWriteStop = TRUE; // 停止发送

	CoupleParameterMap::iterator iter;
	CoupleParameter* pCoupleParameter = NULL;

	if (m_transferParameter.eTransferType == UPPER_MONITER_TRANSFER)
	{
		StopTransfer(m_pCard);
		CompleteTransfer(m_pCard);

		if(m_CFile.m_hFile != INVALID_HANDLE_VALUE)
		{
			m_CFile.Close();
		}
	}

	m_RegisterLock.lock();

	iter = m_mapCardConfig.find(global_PCIeParameterName[13]);
	pCoupleParameter = &(iter->second);
	pCoupleParameter->nValue = 0;
	WriteRegister(m_pCard, pCoupleParameter->nOffset, pCoupleParameter->nValue);

	m_RegisterLock.unlock();

	return TRUE;
}

DWORD WINAPI HighSpeedOperation::OnTransfer(LPVOID lpParameter)
{
	CHighSpeedOperation* pCPCIeCardOperation = (CHighSpeedOperation*)lpParameter;
	if(!pCPCIeCardOperation->m_bCardOpen)
	{
		pCPCIeCardOperation->m_bWriteStop = TRUE;
		return 0;
	}
	BYTE* pBuffer = NULL;
	try
	{
		pBuffer = new BYTE[DMA_TRANSFER_SIZE];
		memset(pBuffer, 0x00, DMA_TRANSFER_SIZE);
	}
	catch(...)
	{
		CTime ct = CTime::GetCurrentTime();
		CString strlog;
		strlog.Format(_T("[%4d-%02d-%02d %02d:%02d:%02d] 内存申请失败: pBuffer = new BYTE[DMA_TRANSFER_SIZE].\n"), 
			ct.GetYear(), ct.GetMonth(), ct.GetDay(), ct.GetHour(), ct.GetMinute(), ct.GetSecond());
		theApp.PushOutputInfo(strlog);
		return 0;
	}
	while(!pCPCIeCardOperation->m_bWriteStop)
	{
		// 等待写入时机
		while(!pCPCIeCardOperation->m_bWriteStop && pCPCIeCardOperation->m_bCardOpen)
		{
			pCPCIeCardOperation->m_RegisterLock.lock();

			size_t nWriteBufferSize = ReadRegister(pCPCIeCardOperation->m_pCard, 0x58);
			nWriteBufferSize *= DDR_BUFFER_SIZE;

			pCPCIeCardOperation->m_RegisterLock.unlock();

			if((DMA_WRITE_BUFFER_MAX_SIZE - nWriteBufferSize) < DMA_TRANSFER_SIZE)
			{
				Sleep(1);
				continue;
			}
			break;
		}
		if(pCPCIeCardOperation->m_bWriteStop || !pCPCIeCardOperation->m_bCardOpen)
		{
			break;
		}
		size_t nReadSize = pCPCIeCardOperation->m_CFile.Read(pBuffer, DMA_TRANSFER_SIZE); // 每次读取224K数据进行写入
		if(nReadSize <= 0)
		{
			break;
		}
		pCPCIeCardOperation->m_nTotalSize += nReadSize;
		// 不足224K的部分补0x00
		if(nReadSize < DMA_TRANSFER_SIZE)
		{
			BYTE* pFillBuffer = new BYTE[DMA_TRANSFER_SIZE - nReadSize];
			memset(pFillBuffer, 0x00, DMA_TRANSFER_SIZE - nReadSize);
			memcpy(pBuffer + nReadSize, pFillBuffer, DMA_TRANSFER_SIZE - nReadSize);
			delete pFillBuffer;
		}
		// 开始写入
		size_t nRealWrite = Write(pCPCIeCardOperation->m_pCard, pBuffer, DMA_TRANSFER_SIZE);
		if(nRealWrite != DMA_TRANSFER_SIZE)
		{
			pCPCIeCardOperation->m_bWriteStop = TRUE;
			break;
		}

		WriteRegister(pCPCIeCardOperation->m_pCard, REGISTER_TRANSFER_SIZE, DMA_TRANSFER_SIZE); // 告诉FPGA本次写入数据的实际长度
		WriteRegister(pCPCIeCardOperation->m_pCard, REGISTER_TRANSFER_VALID_SIZE, nReadSize);	// 告诉FPGA本次写入数据的有效长度
	}
	// 文件下传结束或者停止，想PCIe汇报传输的总字节数和停止标志
	WriteRegister(pCPCIeCardOperation->m_pCard, REGISTER_TRANSFER_FILE_OVER, 0x01);
	WriteRegister(pCPCIeCardOperation->m_pCard, REGISTER_TRANSFER_FILE_TOTAL, pCPCIeCardOperation->m_nTotalSize);

	SAFE_DELETE_ARRAY(pBuffer);

	return 0;
}

DWORD WINAPI HighSpeedOperation::OnReceive(LPVOID lpParameter)
{
	PIndexParameter pThis = (PIndexParameter)lpParameter;
	// 
	size_t nDmaIndex = pThis->nIndex; 
	CHighSpeedOperation* pCPCIeCardOperation = (CHighSpeedOperation*)pThis->pThis;
	if(!pCPCIeCardOperation->m_bCardOpen)
	{
		pCPCIeCardOperation->m_bRecvStop = TRUE;
		return 0;
	}
	BYTE* pBuffer = NULL;
	try
	{
		pBuffer = new BYTE[DMA_DATA_SIZE];
		memset(pBuffer, 0x00, DMA_DATA_SIZE);
	}
	catch(...)
	{
		CTime ct = CTime::GetCurrentTime();
		CString strlog;
		strlog.Format(_T("[%4d-%02d-%02d %02d:%02d:%02d] 内存申请失败: pBuffer = new BYTE[DMA_DATA_SIZE].\n"), 
			ct.GetYear(), ct.GetMonth(), ct.GetDay(), ct.GetHour(), ct.GetMinute(), ct.GetSecond());
		theApp.PushOutputInfo(strlog);
		return 0;
	}
	while(!pCPCIeCardOperation->m_bRecvStop)
	{
		if(nDmaIndex == 0)
		{
			if(::WaitForSingleObject(pCPCIeCardOperation->m_hRecvEvent[0], INFINITE) != WAIT_OBJECT_0)
			{
				break;
			}
			if(pCPCIeCardOperation->m_bRecvStop || !pCPCIeCardOperation->m_bCardOpen)
			{
				continue;
			}
		}
		if(nDmaIndex == 1)
		{
			if(::WaitForSingleObject(pCPCIeCardOperation->m_hRecvEvent[1], INFINITE) != WAIT_OBJECT_0)
			{
				break;
			}
			if(pCPCIeCardOperation->m_bRecvStop || !pCPCIeCardOperation->m_bCardOpen)
			{
				continue;
			}
		}
		while(!pCPCIeCardOperation->m_bRecvStop && pCPCIeCardOperation->m_bCardOpen)
		{
			pCPCIeCardOperation->m_RegisterLock.lock();
			// 获取计数
			pCPCIeCardOperation->m_pcieDeviceInfo.dwGroupAWriteCount = ReadRegister(pCPCIeCardOperation->m_pCard, REGISTER_SEND_ADDR_CH1);
			pCPCIeCardOperation->m_pcieDeviceInfo.dwGroupARecvCount = ReadRegister(pCPCIeCardOperation->m_pCard, REGISTER_RECV_ADDR_CH1);
			pCPCIeCardOperation->m_pcieDeviceInfo.dwGroupACrcErrorCount = ReadRegister(pCPCIeCardOperation->m_pCard, REGISTER_CRC_ADDR_CH1);
			//
			size_t nReadBufferSize = ReadRegister(pCPCIeCardOperation->m_pCard, 0x54); // nOffset = 0x54
			nReadBufferSize *= DDR_BUFFER_SIZE; // DDR缓存数值，1代表128个字节，转换为字节为：乘以128 才是数据量
			pCPCIeCardOperation->m_pcieDeviceInfo.dwFifoCount = nReadBufferSize; 
			if(pCPCIeCardOperation->m_pcieDeviceInfo.dwFifoCount >= FIFO_OVER_FLOW) 
			{
				pCPCIeCardOperation->m_pcieDeviceInfo.dwFifoOverFlow++;
			}
			// 
			pCPCIeCardOperation->m_RegisterLock.unlock();
			// 
			if(DMA_DATA_SIZE > nReadBufferSize) // fifo数据应当 >= 一次dma长度，才能发起dma，否则，死机。
			{
				Sleep(1);
				continue;
			}
			break;
		}
		if(pCPCIeCardOperation->m_bRecvStop || !pCPCIeCardOperation->m_bCardOpen)
		{
			break;
		}
		size_t nReadSize = Read(pCPCIeCardOperation->m_pCard, pBuffer, DMA_DATA_SIZE, nDmaIndex);
		if(pCPCIeCardOperation->m_bRecvStop || !pCPCIeCardOperation->m_bCardOpen)
		{
			break;
		}
		if(nDmaIndex == 0)
		{
			SetEvent(pCPCIeCardOperation->m_hRecvEvent[1]);
		}
		if(nDmaIndex == 1)
		{
			SetEvent(pCPCIeCardOperation->m_hRecvEvent[0]);
		}
		if(nReadSize > 0)
		{
			if(pCPCIeCardOperation->m_nOffset + nReadSize <= PCIE_HIGH_BUFFER_SIZE)
			{
				pCPCIeCardOperation->m_lock.lock();
				memcpy(pCPCIeCardOperation->m_pBuffer + pCPCIeCardOperation->m_nOffset, pBuffer, nReadSize);
				pCPCIeCardOperation->m_nOffset += nReadSize;
				pCPCIeCardOperation->m_lock.unlock();
				pCPCIeCardOperation->m_pcieDeviceInfo.dwTotalData += nReadSize;
			}
			else
			{
				pCPCIeCardOperation->m_pcieDeviceInfo.dwBufferOverFlow++; // 缓存溢出
			}
		}
	}

	SAFE_DELETE_ARRAY(pBuffer);

	return 0;
}

int HighSpeedOperation::OnSaveData()
{
	BYTE* pSaveBuffer = NULL;
	try
	{
		pSaveBuffer = new BYTE[PCIE_HIGH_BUFFER_SIZE];
	}
	catch(...)
	{
		CTime ct = CTime::GetCurrentTime();
		CString strlog;
		strlog.Format(_T("[%4d-%02d-%02d %02d:%02d:%02d] 内存申请失败: pSaveBuffer = new BYTE[PCIE_BUFFER_SIZE].\n"), 
			ct.GetYear(), ct.GetMonth(), ct.GetDay(), ct.GetHour(), ct.GetMinute(), ct.GetSecond());
		theApp.PushOutputInfo(strlog);
		return -1;
	}
	while(!m_bSaveStop)
	{
		if (m_nOffset <= 0)
		{
			Sleep(10);
		}
		else
		{
			size_t nWantWrite = 0;
			m_lock.lock();
			memcpy(pSaveBuffer, m_pBuffer, m_nOffset);
			nWantWrite = m_nOffset;
			m_nOffset = 0;
			m_lock.unlock();
			//
			if (!m_pFileHelper->IsOpen() || m_pFileHelper->GetLength() >= PCIE_ABOVE_NORMAL_FILE_SIZE)
			{
				m_pFileHelper->Close(true);
				CString	strFileName;
				strFileName.Format(_T("_%04d.dat"), m_pcieDeviceInfo.dwFileCount);
				strFileName = m_strSavePath + m_strTimeStamp + strFileName;
				if(m_pFileHelper->Open(strFileName.GetBuffer(), FALSE, PCIE_ABOVE_NORMAL_FILE_SIZE) != 0)
				{
					m_bRecvStop = TRUE;
				}
				m_pcieDeviceInfo.dwFileCount++; // 文件计数加1
			}
			// 
			m_pcieDeviceInfo.fStorageQuantity += (float)(nWantWrite / (1024.0 * 1024 * 1024)); // 数据存储量进行累加
			// 
			size_t nRealWrite = m_pFileHelper->Write(pSaveBuffer, nWantWrite);
			while(nRealWrite != 0 && nRealWrite < nWantWrite)
			{
				nWantWrite -= nRealWrite;
				nRealWrite = m_pFileHelper->Write((pSaveBuffer + nRealWrite), nWantWrite);
			}
		}
	}

	m_bSaveStop = TRUE;

	SAFE_DELETE_ARRAY(pSaveBuffer);

	return 0;
}
