#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <QtCore>
#include <QThread>
#include <windows.h>

using namespace std;

#define		THREAD_STACK_SIZE				1 * 1024 * 1024		// �̶߳�ջ��С
#define		DMA_DATA_SIZE					1 * 1024 * 1024		// ÿ�δ�DMA�ж�ȡ��������
#define		DMA_TRANSFER_SIZE				(224 * 1024)		// ÿ���´���DMA��������
#define		DMA_PARAM_SIZE					24					// �Ĵ������ò����ĸ���
#define		DMA_WRITE_BUFFER_MAX_SIZE		64 * 1024 * 1024	// Ӳ������128M���ڴ�ȡ1/2�͹����ˡ�
#define		FIFO_OVER_FLOW					255 * 1024 * 1024	// FIFO����Ľ���
#define		DDR_BUFFER_SIZE					128
#define		TRIPLE_WIRE_READ_SIZE           (224 * 768)			// ������
// ������ͨ��1��Ϣ
#define		REGISTER_SEND_ADDR_CH1			0x7C
#define		REGISTER_RECV_ADDR_CH1			0x80
#define		REGISTER_CRC_ADDR_CH1			0x84
// ������ͨ��2��Ϣ
#define		REGISTER_SEND_ADDR_CH2			0x8C
#define		REGISTER_RECV_ADDR_CH2			0x90
#define		REGISTER_CRC_ADDR_CH2			0x94

// �����ļ���Ҫ��������ƫ��
#define		REGISTER_TRANSFER_TYPE			0xC0	// ���巢������
#define		REGISTER_DATA_FIELD				0xC4	// ������������־
#define		REGISTER_TRANSFER_VALID_SIZE	0xC8	// ���η��͵���Ч���ݳ���
#define		REGISTER_TRANSFER_SIZE			0xCC	// ���η��͵����ݳ���
#define		REGISTER_TRANSFER_FILE_TOTAL	0xD0	// �����ļ������ֽ���
#define		REGISTER_TRANSFER_FILE_OVER		0xD4	// �ļ�������ɱ�־��0x01��ʾ�ļ��Ѿ�������ɻ���ֹͣ�����ļ�
#define		REGISTER_TRANSFER_TOTAL_FRAME	0xD8	// ÿ�η����ļ�����֡��

#define		PCIE_PATH_SIZE					256		// �Զ���·����󳤶ȣ�����MAX_PATH
#define		PCIE_HIGH_SPEED_FRAME_SIZE		276		// ������/2G���ٵ���Ч���ݳ���
#define		PCIE_MEDIUM_SPEED_FRAME_SIZE	3069	// ������/2G���ٵ���Ч���ݳ��� 1023 * 18bit
#define		PCIE_SINGLE_WIRE_FRAME_SIZE		274		// ������/2G���ٵ���Ч���ݳ���
#define		PCIEtrRIPLE_WIRE_FRAME_SIZE		273		// ��������Ч���ݳ���

#define		PCIE_TRIPLE_BUFFER_SIZE			16 * 1024 * 1024	// �����ƻ���
#define		PCIE_SINGLE_BUFFER_SIZE			32 * 1024 * 1024	// �����ƻ���
#define		PCIE_MEDIUM_BUFFER_SIZE			128 * 1024 * 1024	// ����1Gbps����
#define		PCIE_HIGH_BUFFER_SIZE			256 * 1024 * 1024	// ����2Gbps����
//
#define		PCIE_BELOW_NORMAL_FILE_SIZE		256 * 1024 * 1024	// �ͳ����ļ���С
#define		PCIE_NORMAL_FILE_SIZE			512 * 1024 * 1024	// �����ļ���С
#define		PCIE_ABOVE_NORMAL_FILE_SIZE		1024 * 1024 * 1024	// �������ļ���С

#define		PCIE_CONFIG_FILE				tr("PCIeController.ini")            // �����ļ�
#define		PCIE_LOG_FILE					tr("PCIeController.log")            // ��־�ļ�
#define		PCIE_APP_MANUAL					tr("PCIe�������û��ֲ�.pdf")         // �û��ֲ�

#define		PCIE_SAVE_PATH					tr("SAVE_PATH")
#define		PCIE_TRANSFER_FILE				tr("TRANSFER_FILE")
// ���ٳ������
#define		PCIE_SRC_ADDR_OFFSET			tr("SRC_ADDR_OFFSET")
#define		PCIE_SRC_ADDR_VALUE				tr("SRC_ADDR_VALUE")
#define		PCIE_DST_ADDR_OFFSET			tr("DST_ADDR_OFFSET")
#define		PCIE_DST_ADDR_VALUE				tr("DST_ADDR_VALUE")
#define		PCIE_TYPE_STATE_OFFSET			tr("TYPE_STATE_OFFSET")
#define		PCIE_TYPE_STATE_VALUE			tr("TYPE_STATE_VALUE")
#define		PCIE_RATE_ID_OFFSET				tr("RATE_ID_OFFSET")
#define		PCIE_RATE_ID_VALUE				tr("RATE_ID_VALUE")
#define		PCIE_VALID_LENGTH_OFFSET		tr("VALID_LENGTH_OFFSET")
#define		PCIE_VALID_LENGTH_VALUE			tr("VALID_LENGTH_VALUE")
#define		PCIE_FRAME_GAP_OFFSET			tr("FRAME_GAP_OFFSET")
#define		PCIE_FRAME_GAP_VALUE			tr("FRAME_GAP_VALUE")
#define		PCIE_START_STOP_OFFSET			tr("START_STOP_OFFSET")
#define		PCIE_START_STOP_VALUE			tr("START_STOP_VALUE")
// 0233�������ò���
#define		PCIE_SEND_CHANNEL_OFFSET		tr("SEND_CHANNEL_OFFSET")
#define		PCIE_SEND_CHANNEL_VALUE_A		tr("SEND_CHANNEL_VALUE_A")
#define		PCIE_SEND_CHANNEL_VALUE_B		tr("SEND_CHANNEL_VALUE_B")
#define		PCIE_RECV_CHANNEL_OFFSET		tr("RECV_CHANNEL_OFFSET")
#define		PCIE_RECV_CHANNEL_VALUE_A		tr("RECV_CHANNEL_VALUE_A")
#define		PCIE_RECV_CHANNEL_VALUE_B		tr("RECV_CHANNEL_VALUE_B")
#define		PCIE_SRC_ADDR_OFFSET_CH2		tr("SRC_ADDR_OFFSET_CH2")
#define		PCIE_SRC_ADDR_VALUE_CH2			tr("SRC_ADDR_VALUE_CH2")
#define		PCIE_DST_ADDR_OFFSET_CH2		tr("DST_ADDR_OFFSET_CH2")
#define		PCIE_DST_ADDR_VALUE_CH2			tr("DST_ADDR_VALUE_CH2")
#define		PCIE_TYPE_STATE_OFFSET_CH2		tr("TYPE_STATE_OFFSET_CH2")
#define		PCIE_TYPE_STATE_VALUE_CH2		tr("TYPE_STATE_VALUE_CH2")
#define		PCIE_RATE_ID_OFFSET_CH2			tr("RATE_ID_OFFSET_CH2")
#define		PCIE_RATE_ID_VALUE_CH2			tr("RATE_ID_VALUE_CH2")
#define		PCIE_VALID_LENGTH_OFFSET_CH2	tr("VALID_LENGTH_OFFSET_CH2")
#define		PCIE_VALID_LENGTH_VALUE_CH2		tr("VALID_LENGTH_VALUE_CH2")
#define		PCIE_FRAME_GAP_OFFSET_CH2		tr("FRAME_GAP_OFFSET_CH2")
#define		PCIE_FRAME_GAP_VALUE_CH2		tr("FRAME_GAP_VALUE_CH2")
// 0215����֡ͷ
#define		PCIE_FRAME_HEAD_OFFSET			tr("FRAME_HEAD_OFFSET")
#define		PCIE_FRAME_HEAD_VALUE			tr("FRAME_HEAD_VALUE")

#define		PCIE_TOTAL_FRAME_OFFSET			tr("TOTAL_FRAME_OFFSET")
#define		PCIE_TOTAL_FRAME_VALUE			tr("TOTAL_FRAME_VALUE")

#define		PCIE_RECV_START					"rec_start"				// ����ʹ�ܣ�bit0����0������ʹ�ܣ���1����ʹ�ܡ�bit31~bit1��Ԥ��
#define		PCIE_RECV_MODE					"rec_mode"				// ����ģʽ���ã�bit[2:0]: //��001��������ģʽ  //��010��������ģʽ //��100��������ģʽ //bit31~bit3��Ԥ��
#define		PCIE_SEND_START					"tra_start"				// ����ʹ�ܣ�bit0����0������ʹ�ܣ���1����ʹ��  //bit31~bit1��Ԥ��
#define		PCIE_SEND_MODE					"tra_mode"				// ����ģʽ���ã�bit[2:0]: //��001��������ģʽ  //��010��������ģʽ //��100��������ģʽ //bit31~bit3��Ԥ��
#define		PCIE_POST_PRO					"post_prog_empty"		// fifo�ɱ�̿ձ�־����1�����գ� ��0�������� //���ռ��ɴ�0x5c�˿ڶ����� 
																	// [DMA����] us_count 0x54 PCIE���д洢�ռ䣬RO(�ֽ���)   -->�������ݵ�ַ
#define		PCIE_POST_RD_DATA_CONFIG		"post_rd_data_config"	// fifo�ɶ���������32bit���ݸ���RO
																	// [DMA����] us_count 0x58 PCIE���д洢�ռ䣬RO(�ֽ���)   -->�������ݵ�ַ
#define		PCIE_POST_DOUT					"post_dout"				// ���ݶ˿ڣ�RO //��0x54���ջ���0x58��������Ϊ�㣬���ɴӴ˵�ַ�˿ڶ����ݣ����ļ�
#define		PCIE_DATA_SRC					"data_src"
#define		PCIE_DATA_DST					"data_dst"
#define		PCIE_TYPE_STATE					"data_info"
#define		PCIE_RATE_ID					"rate_identifier"	
#define		PCIE_VALID_LENGTH				"valid_length"
#define		PCIE_FRAME_GAP					"frame_gap"
#define		PCIE_START_STOP					"start_stop"
// 0233����(100Mbps)�����Ĵ�����д����
#define		PCIE_DATA_SRC_CH2				"data_src_ch2"
#define		PCIE_DATA_DST_CH2				"data_dst_ch2"
#define		PCIE_TYPE_STATE_CH2				"data_info_ch2"
#define		PCIE_RATE_ID_CH2				"rate_identifier_ch2"	
#define		PCIE_VALID_LENGTH_CH2			"valid_length_ch2"
#define		PCIE_FRAME_GAP_CH2				"frame_gap_ch2"

#define		PCIE_SEND_CHANNEL				"send_channel"
#define		PCIE_RECV_CHANNEL				"recv_channel"
// 0215����(1024Mbps)�����Ĵ�����д����
#define		PCIE_FRAME_HEAD					"frame_head"

#define		PCIE_TOTAL_FRAME				"total_frame"

typedef struct _PCIeDeviceInfo
{
	float		fRecvRate;				// ʵʱ��������
	float		fStorageQuantity;		// ���ݴ洢����
	DWORD		dwGroupAWriteCount;		// Aͨ������֡����
	DWORD		dwGroupBWriteCount;
	DWORD		dwGroupARecvCount;		// Aͨ������֡����
	DWORD		dwGroupBRecvCount;
	DWORD		dwGroupACrcErrorCount;	// Aͨ��CRC��֡����
	DWORD		dwGroupBCrcErrorCount;
	DWORD		dwTotalData;			// ��ǰ������������
	DWORD		dwLastTotalData;		// ��һʱ�̽��յ��������������ڼ����������
	DWORD		dwFifoCount;			// �忨FiFo�ռ��С��������Ϊ524296[80008]
	DWORD		dwFifoOverFlow;			// �忨FiFo�ռ����������ü�������1���ʾ��һ�������
	DWORD		dwStart;				// ��ʼʱ�䣬���ڼ����������
	DWORD		dwFileCount;			// �洢���ļ�����
	DWORD		dwBufferOverFlow;		// ��������Ĵ���
	_PCIeDeviceInfo() {
		fRecvRate = 0.0f;
		fStorageQuantity = 0.0f;
		dwGroupAWriteCount = 0;
		dwGroupBWriteCount = 0;
		dwGroupARecvCount = 0;
		dwGroupBRecvCount = 0;
		dwGroupACrcErrorCount = 0;
		dwGroupBCrcErrorCount = 0;
		dwTotalData = 0;
		dwLastTotalData = 0;
		dwFifoCount = 0;
		dwFifoOverFlow = 0;
		dwStart = 0;
		dwFileCount = 0;
		dwBufferOverFlow = 0;
	}
}PCIeDeviceInfo, *PPCIeDeviceInfo;

// �Ĵ������ò���ģ��
typedef struct _CoupleParameter
{
	unsigned int	nOffset;
	unsigned int	nValue;
}CoupleParameter, *PCoupleParameter;

typedef std::map<std::string, CoupleParameter> CoupleParameterMap;

typedef enum _TRANSFER_TYPE
{
    NONE_TYPE_TRANSFER = 0,
    PCIE_AUTO_TRANSFER,			// PCIe�Զ�����ģʽ
    UPPER_MONITER_TRANSFER,		// ��λ���������ģʽ
}TRANSFER_TYPE;

typedef enum _DATA_FIELD
{
    NONE_TYPE_FIELD = 0,		// ������
	ACCUMULATION_FIELD,			// �ۼ���
	RANDOM_NUMBER_FIELD,		// �����
	BLANK_FRAME_FIELD,			// ��֡
}DATA_FIELD;

typedef struct _TRANSFER_PARAMETER
{
    TRANSFER_TYPE	eTransferType;
	DATA_FIELD		eDataField;
    QString			strTransferFile;
    _TRANSFER_PARAMETER() {
        eTransferType = NONE_TYPE_TRANSFER;
        eDataField = NONE_TYPE_FIELD;
        strTransferFile = "";
	}
}TRANSFER_PARAMETER, *PTRANSFER_PARAMETER;

extern char* global_PCIeParameterName[DMA_PARAM_SIZE];

// ��ʼ�������üĴ�������
void initPCIeConfigParameter();

// �����༶Ŀ¼����
bool createMultipleDirectory(const QString strPath);

#endif // COMMON_H
