#include "highspeedoperation.h"

std::list<PCIeParameter> HighSpeedOperation::m_pCardList;
std::map<std::string, PPCIeParameter> HighSpeedOperation::m_mapCardStrParameter;

HighSpeedOperation::HighSpeedOperation()
{
	m_nOffset = 0;
	m_pCard = NULL;
	m_bRecvStop = TRUE;
	m_bSaveStop = TRUE;
	m_bWriteStop = TRUE;
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
        emit loggerWrite(tr("failed to new buffer"));
	}
}

HighSpeedOperation::~HighSpeedOperation(void)
{

}

void HighSpeedOperation::run()
{
    size_t nWantWrite = 0;
    BYTE* pSaveBuffer = NULL;
    try
    {
        pSaveBuffer = new BYTE[PCIE_HIGH_BUFFER_SIZE];
    }
    catch(...)
    {
        return;
    }
    while(!m_bSaveStop)
    {
        if (m_nOffset <= 0)
        {
           Sleep(10);
        }
        else
        {
            nWantWrite = 0;
            m_lock.lock();
            memcpy(pSaveBuffer, m_pBuffer, m_nOffset);
            nWantWrite = m_nOffset;
            m_nOffset = 0;
            m_lock.unlock();
            //
            if (m_nFileSize >= PCIE_ABOVE_NORMAL_FILE_SIZE)
            {
                fflush(m_pFile);
                fclose(m_pFile);
                QString	strFileName = tr("_%1.dat").arg(m_pcieDeviceInfo.dwFileCount);
                strFileName = m_strSavePath + m_strTimeStamp + strFileName;
                fopen_s(&m_pFile, strFileName.toLocal8Bit().data(), "wb");
                if (m_pFile == NULL)
                {
                    return;
                }
                m_pcieDeviceInfo.dwFileCount++;
            }
            //
            m_pcieDeviceInfo.fStorageQuantity += (float)(nWantWrite / (1024.0 * 1024 * 1024));
            //
            fwrite(pSaveBuffer, sizeof(char), nWantWrite, m_pFile);
        }
    }

    m_bSaveStop = TRUE;
}

void HighSpeedOperation::createFileDirectory(const QString strSavePath)
{
    QString strDir = tr("high_speed");
	m_strSavePath = strSavePath + strDir;
	// 
    if (!createMultipleDirectory(m_strSavePath))
    {
        emit loggerWrite(tr("failed to create high_speed"));
    }
}

BOOL HighSpeedOperation::setCard(PPCIeParameter pCard)
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

void HighSpeedOperation::buildEvent()
{
    clearEvent();
	for(size_t i = 0; i < PHYSIC_MEMORY_NUMBER; i++)
	{
		m_hRecvEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
}

void HighSpeedOperation::clearEvent()
{
	for(size_t i = 0; i < PHYSIC_MEMORY_NUMBER; i++)
	{
		CloseHandle(m_hRecvEvent[i]);
		m_hRecvEvent[i] = NULL;
	}
}

BOOL HighSpeedOperation::openCard()
{
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
    //m_pCard->pcieCardInfo.pciSlot.dwBus;

	m_bCardOpen = TRUE;

	return TRUE;
}

BOOL HighSpeedOperation::closeCard()
{
	if (m_pCard && m_bCardOpen)
	{
		m_pCard->bOpen = FALSE;
		m_bCardOpen = FALSE;
		Close(m_pCard);
	}

	return TRUE;
}

BOOL HighSpeedOperation::initPCIeDeviceList()
{
	if(!GetCards(m_pCardList))
	{
		return false;
	}
	std::list<PCIeParameter>::iterator iter = m_pCardList.begin();
	for(; iter != m_pCardList.end(); iter++)
	{
		std::string strIndex;
		WD_PCI_CARD_INFO *pCardInfo = NULL;
		WD_PCI_SLOT *pciSlot = NULL;
		pCardInfo = &(iter->pcieCardInfo);
		pciSlot = &(pCardInfo->pciSlot);
        QString strItem = tr("PCI Express Bus:%d").arg(pciSlot->dwBus);
        strIndex = strItem.toLocal8Bit().data();
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

BOOL HighSpeedOperation::startReceive()
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

		return FALSE;
	}

	m_pcieDeviceInfo.dwFifoOverFlow = 0;
	m_pcieDeviceInfo.dwBufferOverFlow = 0;
	m_pcieDeviceInfo.fStorageQuantity = 0.0f;
	m_pcieDeviceInfo.dwFileCount = 0;

    QString	strFileName = tr("_%1.dat").arg(m_pcieDeviceInfo.dwFileCount);
    strFileName = m_strSavePath + m_strTimeStamp + strFileName;
    fopen_s(&m_pFile, strFileName.toLocal8Bit().data(), "wb");
    if (m_pFile == NULL)
    {
        return FALSE;
    }

    buildEvent();

    m_bRecvStop = FALSE;

	DWORD dwThreadID = 0;
	
	m_pThis[0].nIndex = 0;
	m_pThis[0].pThis = this;
    HANDLE hRecvMasterThread = CreateThread(NULL, THREAD_STACK_SIZE, (LPTHREAD_START_ROUTINE)onReceive, &m_pThis[0], 0, &dwThreadID);
	SetThreadPriority(hRecvMasterThread, THREAD_PRIORITY_HIGHEST);
	SetThreadAffinityMask(hRecvMasterThread, 1); // CPU:1
	// 
	m_pThis[1].nIndex = 1;
	m_pThis[1].pThis = this;
    HANDLE hRecvMinorThread = CreateThread(NULL, THREAD_STACK_SIZE, (LPTHREAD_START_ROUTINE)onReceive, &m_pThis[1], 0, &dwThreadID);
	SetThreadPriority(hRecvMinorThread, THREAD_PRIORITY_HIGHEST);
	SetThreadAffinityMask(hRecvMinorThread, 2); // CPU:2
	// 
	SetEvent(m_hRecvEvent[0]);
    //
	m_bSaveStop = FALSE;
    start(QThread::HighestPriority);

	return TRUE;
}

BOOL HighSpeedOperation::stopReceive()
{
	if(!m_bRecvStop)
	{
		for(int i = 0; i < PHYSIC_MEMORY_NUMBER; i++)
		{
			SetEvent(m_hRecvEvent[i]);
		}
        m_bRecvStop = TRUE;
	}

	StopRead(m_pCard);
	CompleteRead(m_pCard);

	m_RegisterLock.lock();

	m_pcieDeviceInfo.dwGroupAWriteCount = ReadRegister(m_pCard, REGISTER_SEND_ADDR_CH1);
	m_pcieDeviceInfo.dwGroupARecvCount = ReadRegister(m_pCard, REGISTER_RECV_ADDR_CH1);
	m_pcieDeviceInfo.dwGroupACrcErrorCount = ReadRegister(m_pCard, REGISTER_CRC_ADDR_CH1);

	m_RegisterLock.unlock();

    m_bSaveStop = TRUE;

	return TRUE;
}

BOOL HighSpeedOperation::startWrite()
{
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

			return FALSE;
		}
        WriteRegister(m_pCard, REGISTER_TRANSFER_TYPE, 0x01);
		//
        fopen_s(&m_qFile, m_transferParameter.strTransferFile.toLocal8Bit().data(), "rb");
        if(m_qFile == NULL)
		{
			return FALSE;
		}
        fseek(m_qFile, 0L, SEEK_END);
        quint64 nFileSize = ftell(m_qFile);
        fseek(m_qFile, 0L, SEEK_SET);

        if (nFileSize > 64 * 1024 * 1024)
		{
			return FALSE;
		}

		size_t nTotalFrame = nFileSize / PCIE_HIGH_SPEED_FRAME_SIZE;
		WriteRegister(m_pCard, REGISTER_TRANSFER_TOTAL_FRAME, nTotalFrame);

		m_nTotalSize = 0;

		m_bWriteStop = FALSE; 

		DWORD dwThreadID = 0;
        HANDLE hTransferThread = CreateThread(NULL, THREAD_STACK_SIZE, (LPTHREAD_START_ROUTINE)onTransfer, this, 0, &dwThreadID);
	}
	else if (m_transferParameter.eTransferType == PCIE_AUTO_TRANSFER)
	{
        WriteRegister(m_pCard, REGISTER_TRANSFER_TYPE, 0x00);
		if (m_transferParameter.eDataField == ACCUMULATION_FIELD)
            WriteRegister(m_pCard, REGISTER_DATA_FIELD, 0x00);
		else if (m_transferParameter.eDataField == RANDOM_NUMBER_FIELD)
            WriteRegister(m_pCard, REGISTER_DATA_FIELD, 0x01);
		else if (m_transferParameter.eDataField == BLANK_FRAME_FIELD)
            WriteRegister(m_pCard, REGISTER_DATA_FIELD, 0x02);
		else
			WriteRegister(m_pCard, REGISTER_DATA_FIELD, 0x00);

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

BOOL HighSpeedOperation::stopWrite()
{
    m_bWriteStop = TRUE;

	CoupleParameterMap::iterator iter;
	CoupleParameter* pCoupleParameter = NULL;

	if (m_transferParameter.eTransferType == UPPER_MONITER_TRANSFER)
	{
		StopTransfer(m_pCard);
		CompleteTransfer(m_pCard);

        if (m_qFile == NULL)
        {
            fclose(m_qFile);
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

DWORD WINAPI HighSpeedOperation::onTransfer(LPVOID lpParameter)
{
    HighSpeedOperation* pCPCIeCardOperation = (HighSpeedOperation*)lpParameter;
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
		return 0;
	}
	while(!pCPCIeCardOperation->m_bWriteStop)
	{
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
        size_t nReadSize = fread((char*)pBuffer, sizeof(BYTE), DMA_TRANSFER_SIZE, pCPCIeCardOperation->m_qFile);
		if(nReadSize <= 0)
		{
			break;
		}
		pCPCIeCardOperation->m_nTotalSize += nReadSize;
		if(nReadSize < DMA_TRANSFER_SIZE)
		{
			BYTE* pFillBuffer = new BYTE[DMA_TRANSFER_SIZE - nReadSize];
			memset(pFillBuffer, 0x00, DMA_TRANSFER_SIZE - nReadSize);
			memcpy(pBuffer + nReadSize, pFillBuffer, DMA_TRANSFER_SIZE - nReadSize);
			delete pFillBuffer;
		}

		size_t nRealWrite = Write(pCPCIeCardOperation->m_pCard, pBuffer, DMA_TRANSFER_SIZE);
		if(nRealWrite != DMA_TRANSFER_SIZE)
		{
			pCPCIeCardOperation->m_bWriteStop = TRUE;
			break;
		}

        WriteRegister(pCPCIeCardOperation->m_pCard, REGISTER_TRANSFER_SIZE, DMA_TRANSFER_SIZE);
        WriteRegister(pCPCIeCardOperation->m_pCard, REGISTER_TRANSFER_VALID_SIZE, nReadSize);
	}
	WriteRegister(pCPCIeCardOperation->m_pCard, REGISTER_TRANSFER_FILE_OVER, 0x01);
	WriteRegister(pCPCIeCardOperation->m_pCard, REGISTER_TRANSFER_FILE_TOTAL, pCPCIeCardOperation->m_nTotalSize);

	return 0;
}

DWORD WINAPI HighSpeedOperation::onReceive(LPVOID lpParameter)
{
	PIndexParameter pThis = (PIndexParameter)lpParameter;
	// 
	size_t nDmaIndex = pThis->nIndex; 
    HighSpeedOperation* pCPCIeCardOperation = (HighSpeedOperation*)pThis->pThis;
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

			pCPCIeCardOperation->m_pcieDeviceInfo.dwGroupAWriteCount = ReadRegister(pCPCIeCardOperation->m_pCard, REGISTER_SEND_ADDR_CH1);
			pCPCIeCardOperation->m_pcieDeviceInfo.dwGroupARecvCount = ReadRegister(pCPCIeCardOperation->m_pCard, REGISTER_RECV_ADDR_CH1);
			pCPCIeCardOperation->m_pcieDeviceInfo.dwGroupACrcErrorCount = ReadRegister(pCPCIeCardOperation->m_pCard, REGISTER_CRC_ADDR_CH1);
			//
            size_t nReadBufferSize = ReadRegister(pCPCIeCardOperation->m_pCard, 0x54);
            nReadBufferSize *= DDR_BUFFER_SIZE;
            pCPCIeCardOperation->m_pcieDeviceInfo.dwFifoCount = nReadBufferSize;
			if(pCPCIeCardOperation->m_pcieDeviceInfo.dwFifoCount >= FIFO_OVER_FLOW) 
			{
				pCPCIeCardOperation->m_pcieDeviceInfo.dwFifoOverFlow++;
			}
			// 
			pCPCIeCardOperation->m_RegisterLock.unlock();
			// 
            if(DMA_DATA_SIZE > nReadBufferSize)
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
                pCPCIeCardOperation->m_pcieDeviceInfo.dwBufferOverFlow++;
            }
		}
	}

	return 0;
}
