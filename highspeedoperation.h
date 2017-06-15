#pragma once

#include "common.h"
#include "pciexpress.h"

/*
1，寄存器读写板卡模式
[0] 接收使能，RW   //bit0：‘0’—不使能；‘1’—使能  bit31~bit1：预留
[1] 接收模式设置，RW //bit[2:0]: //‘001’：主份模式  //‘010’：备份模式 //‘100’：主备模式 //bit31~bit3：预留
[2] 发送使能，RW   //bit0：‘0’—不使能；‘1’—使能  //bit31~bit1：预留
[3] 发送模式设置，RW //bit[2:0]: //‘001’：主份模式  //‘010’：备份模式 //‘100’：主备模式 //bit31~bit3：预留
[4] fifo可编程空标志：RO //‘1’：空； ‘0’：不空 //不空即可从0x5c端口读数据 
[5] fifo可读数据量：32bit数据个数RO
[6] 数据端口：RO //当0x54不空或者0x58数据量不为零，即可从此地址端口读数据，存文件

2，DMA读写板卡模式
[0] 接收使能，RW   //bit0：‘0’—不使能；‘1’—使能  bit31~bit1：预留
[1] 接收模式设置，RW //bit[2:0]: //‘001’：主份模式  //‘010’：备份模式 //‘100’：主备模式 //bit31~bit3：预留
[2] 发送使能，RW   //bit0：‘0’—不使能；‘1’—使能  //bit31~bit1：预留
[3] 发送模式设置，RW //bit[2:0]: //‘001’：主份模式  //‘010’：备份模式 //‘100’：主备模式 //bit31~bit3：预留
[4] [DMA复用] us_count 0x54 PCIE上行存储空间，RO(字节数)   -->接收数据地址
[5] [DMA复用] us_count 0x54 PCIE下行存储空间，RO(字节数)   -->发送数据地址
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
	CString					m_strSavePath;      // 保存文件的路径
	CString					m_strTimeStamp;
	IFastFileHelper*		m_pFileHelper;		// 快速存文件
	IThreadCreatorHelper*	m_pSaveThread;

	static list<PCIeParameter>					m_pCardList;
	static map<std::string, PPCIeParameter>		m_mapCardStrParameter;

public:
	BYTE*					m_pBuffer;
	size_t					m_nOffset;
	MXLock					m_lock;
	CoupleParameterMap		m_mapCardConfig;
	PCIeDeviceInfo			m_pcieDeviceInfo;	// 记录当前PCIe板卡的参数信息

	PPCIeParameter			m_pCard;			// PCIE卡 HANDLE,类中操作操作的核心对象，只维护一个
	BOOL					m_bCardOpen;
	size_t					m_nBlockSize;
	size_t					m_nCardIndex;		// 当前对象操作板卡序号

	MXLock					m_ConfigLock;
	MXLock					m_RegisterLock;

	CFile					m_CFile;			// 发送文件
	size_t					m_nTotalSize;		// 发送文件的总字节数
	TRANSFER_PARAMETER		m_transferParameter;

	HANDLE					m_hRecvEvent[PHYSIC_MEMORY_NUMBER];

	IndexParameter			m_pThis[PHYSIC_MEMORY_NUMBER];
};
