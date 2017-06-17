#ifndef HIGHSPEEDOPERATION_H
#define HIGHSPEEDOPERATION_H

#include "common.h"
#include "pciexpress.h"

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

public:
    BOOL startReceive();
    BOOL stopReceive();

    BOOL startWrite();
    BOOL stopWrite();

public:
    void buildEvent();
    void clearEvent();

    BOOL openCard();
    BOOL closeCard();
    BOOL setCard(PPCIeParameter pCard);

    void createFileDirectory(const QString strSavePath);

    void setCardIndex(int nCardIndex) {
		m_nCardIndex = nCardIndex;
	}

    void setTimeStamp(QString strTimeStamp) {
        m_strTimeStamp = strTimeStamp;
	}

    PPCIeDeviceInfo getPCIeDeviceInfo() {
		return &m_pcieDeviceInfo;
	}

    void setTransferParameter(TRANSFER_PARAMETER transferParameter) {
		m_transferParameter = transferParameter;
	}

    static DWORD WINAPI onTransfer(LPVOID lpParameter);

    static DWORD WINAPI onReceive(LPVOID lpParameter);

protected:
    void run();

public:
    FILE*                   m_pFile;
    quint32                 m_nFileSize;
	BOOL					m_bRecvStop;
	BOOL					m_bSaveStop;
	BOOL					m_bWriteStop;
    QString					m_strSavePath;
    QString					m_strTimeStamp;

public:
	BYTE*					m_pBuffer;
	size_t					m_nOffset;
    QMutex					m_lock;
	CoupleParameterMap		m_mapCardConfig;
    PCIeDeviceInfo			m_pcieDeviceInfo;

    PPCIeParameter			m_pCard;
	BOOL					m_bCardOpen;
	size_t					m_nBlockSize;
    size_t					m_nCardIndex;

    QMutex					m_ConfigLock;
    QMutex					m_RegisterLock;

    FILE*					m_qFile;
    size_t					m_nTotalSize;
	TRANSFER_PARAMETER		m_transferParameter;

	HANDLE					m_hRecvEvent[PHYSIC_MEMORY_NUMBER];

	IndexParameter			m_pThis[PHYSIC_MEMORY_NUMBER];
};

#endif // HIGHSPEEDOPERATION_H
