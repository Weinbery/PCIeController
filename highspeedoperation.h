#pragma once

#include "common.h"
#include "pciexpress.h"

/*
1���Ĵ�����д�忨ģʽ
[0] ����ʹ�ܣ�RW   //bit0����0������ʹ�ܣ���1����ʹ��  bit31~bit1��Ԥ��
[1] ����ģʽ���ã�RW //bit[2:0]: //��001��������ģʽ  //��010��������ģʽ //��100��������ģʽ //bit31~bit3��Ԥ��
[2] ����ʹ�ܣ�RW   //bit0����0������ʹ�ܣ���1����ʹ��  //bit31~bit1��Ԥ��
[3] ����ģʽ���ã�RW //bit[2:0]: //��001��������ģʽ  //��010��������ģʽ //��100��������ģʽ //bit31~bit3��Ԥ��
[4] fifo�ɱ�̿ձ�־��RO //��1�����գ� ��0�������� //���ռ��ɴ�0x5c�˿ڶ����� 
[5] fifo�ɶ���������32bit���ݸ���RO
[6] ���ݶ˿ڣ�RO //��0x54���ջ���0x58��������Ϊ�㣬���ɴӴ˵�ַ�˿ڶ����ݣ����ļ�

2��DMA��д�忨ģʽ
[0] ����ʹ�ܣ�RW   //bit0����0������ʹ�ܣ���1����ʹ��  bit31~bit1��Ԥ��
[1] ����ģʽ���ã�RW //bit[2:0]: //��001��������ģʽ  //��010��������ģʽ //��100��������ģʽ //bit31~bit3��Ԥ��
[2] ����ʹ�ܣ�RW   //bit0����0������ʹ�ܣ���1����ʹ��  //bit31~bit1��Ԥ��
[3] ����ģʽ���ã�RW //bit[2:0]: //��001��������ģʽ  //��010��������ģʽ //��100��������ģʽ //bit31~bit3��Ԥ��
[4] [DMA����] us_count 0x54 PCIE���д洢�ռ䣬RO(�ֽ���)   -->�������ݵ�ַ
[5] [DMA����] us_count 0x54 PCIE���д洢�ռ䣬RO(�ֽ���)   -->�������ݵ�ַ
*/

class HighSpeedOperation : public QThread
{
    Q_OBJECT
public:
    HighSpeedOperation(void);
    virtual ~HighSpeedOperation(void);

	typedef struct _IndexParameter {
		size_t					nIndex;
        HighSpeedOperation*     pThis;
	}IndexParameter, *PIndexParameter;

signals:
    void loggerWrite(const QString strContext);

protected:
    void run();

public:
    BOOL StartReceive();
    BOOL StopReceive();

    BOOL StartWrite();
    BOOL StopWrite();

public:
	void BuildEvent();
	void ClearEvent();

    BOOL OpenCard();
	BOOL CloseCard();
	BOOL SetCard(PPCIeParameter pCard);

    void CreateFileDirectory(const QString strSavePath);

	void SetCardIndex(int nCardIndex) {
		m_nCardIndex = nCardIndex;
	}

    void SetTimeStamp(QString strTimeStamp) {
        m_strTimeStamp = strTimeStamp;
	}

	PPCIeDeviceInfo GetPCIeDeviceInfo() {
		return &m_pcieDeviceInfo;
	}

	void SetTransferParameter(TRANSFER_PARAMETER transferParameter) {
		m_transferParameter = transferParameter;
	}

	static BOOL InitPCIeDeviceList();

	static DWORD WINAPI OnTransfer(LPVOID lpParameter);

	static DWORD WINAPI OnReceive(LPVOID lpParameter);

protected:
	virtual int UserProc(IThreadCreatorHelper* pCreatorHelper)
	{
		if (m_pSaveThread == pCreatorHelper)
		{
			return OnSaveData();
		}

		return 0;
	}

protected:
	int OnSaveData();

public:
	BOOL					m_bRecvStop;
	BOOL					m_bSaveStop;
	BOOL					m_bWriteStop;
	CString					m_strSavePath;      // �����ļ���·��
	CString					m_strTimeStamp;
	IFastFileHelper*		m_pFileHelper;		// ���ٴ��ļ�
	IThreadCreatorHelper*	m_pSaveThread;

	static list<PCIeParameter>					m_pCardList;
	static map<std::string, PPCIeParameter>		m_mapCardStrParameter;

public:
	BYTE*					m_pBuffer;
	size_t					m_nOffset;
	MXLock					m_lock;
	CoupleParameterMap		m_mapCardConfig;
	PCIeDeviceInfo			m_pcieDeviceInfo;	// ��¼��ǰPCIe�忨�Ĳ�����Ϣ

	PPCIeParameter			m_pCard;			// PCIE�� HANDLE,���в��������ĺ��Ķ���ֻά��һ��
	BOOL					m_bCardOpen;
	size_t					m_nBlockSize;
	size_t					m_nCardIndex;		// ��ǰ��������忨���

	MXLock					m_ConfigLock;
	MXLock					m_RegisterLock;

	CFile					m_CFile;			// �����ļ�
	size_t					m_nTotalSize;		// �����ļ������ֽ���
	TRANSFER_PARAMETER		m_transferParameter;

	HANDLE					m_hRecvEvent[PHYSIC_MEMORY_NUMBER];

	IndexParameter			m_pThis[PHYSIC_MEMORY_NUMBER];
};
