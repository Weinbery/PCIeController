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

#define		THREAD_STACK_SIZE				1 * 1024 * 1024		// 线程堆栈大小
#define		DMA_DATA_SIZE					1 * 1024 * 1024		// 每次从DMA中读取的数据量
#define		DMA_TRANSFER_SIZE				(224 * 1024)		// 每次下传至DMA的数据量
#define		DMA_PARAM_SIZE					24					// 寄存器配置参数的个数
#define		DMA_WRITE_BUFFER_MAX_SIZE		64 * 1024 * 1024	// 硬件本身128M，在此取1/2就够用了。
#define		FIFO_OVER_FLOW					255 * 1024 * 1024	// FIFO溢出的界限
#define		DDR_BUFFER_SIZE					128
#define		TRIPLE_WIRE_READ_SIZE           (224 * 768)			// 三线制
// 单线制通道1信息
#define		REGISTER_SEND_ADDR_CH1			0x7C
#define		REGISTER_RECV_ADDR_CH1			0x80
#define		REGISTER_CRC_ADDR_CH1			0x84
// 单线制通道2信息
#define		REGISTER_SEND_ADDR_CH2			0x8C
#define		REGISTER_RECV_ADDR_CH2			0x90
#define		REGISTER_CRC_ADDR_CH2			0x94

// 发送文件需要定义的相关偏移
#define		REGISTER_TRANSFER_TYPE			0xC0	// 定义发送类型
#define		REGISTER_DATA_FIELD				0xC4	// 数据域填充码标志
#define		REGISTER_TRANSFER_VALID_SIZE	0xC8	// 本次发送的有效数据长度
#define		REGISTER_TRANSFER_SIZE			0xCC	// 本次发送的数据长度
#define		REGISTER_TRANSFER_FILE_TOTAL	0xD0	// 发送文件的总字节数
#define		REGISTER_TRANSFER_FILE_OVER		0xD4	// 文件发送完成标志，0x01表示文件已经发送完成或者停止发送文件
#define		REGISTER_TRANSFER_TOTAL_FRAME	0xD8	// 每次发送文件的总帧数

#define		PCIE_PATH_SIZE					256		// 自定义路径最大长度，类似MAX_PATH
#define		PCIE_HIGH_SPEED_FRAME_SIZE		276		// 单线制/2G高速的有效数据长度
#define		PCIE_MEDIUM_SPEED_FRAME_SIZE	3069	// 单线制/2G高速的有效数据长度 1023 * 18bit
#define		PCIE_SINGLE_WIRE_FRAME_SIZE		274		// 单线制/2G高速的有效数据长度
#define		PCIEtrRIPLE_WIRE_FRAME_SIZE		273		// 三线制有效数据长度

#define		PCIE_TRIPLE_BUFFER_SIZE			16 * 1024 * 1024	// 三线制缓存
#define		PCIE_SINGLE_BUFFER_SIZE			32 * 1024 * 1024	// 单线制缓存
#define		PCIE_MEDIUM_BUFFER_SIZE			128 * 1024 * 1024	// 中速1Gbps缓存
#define		PCIE_HIGH_BUFFER_SIZE			256 * 1024 * 1024	// 高速2Gbps缓存
//
#define		PCIE_BELOW_NORMAL_FILE_SIZE		256 * 1024 * 1024	// 低常规文件大小
#define		PCIE_NORMAL_FILE_SIZE			512 * 1024 * 1024	// 常规文件大小
#define		PCIE_ABOVE_NORMAL_FILE_SIZE		1024 * 1024 * 1024	// 超常规文件大小

#define		PCIE_CONFIG_FILE				tr("PCIeController.ini")            // 配置文件
#define		PCIE_LOG_FILE					tr("PCIeController.log")            // 日志文件
#define		PCIE_APP_MANUAL					tr("PCIe控制器用户手册.pdf")         // 用户手册

#define		PCIE_SAVE_PATH					tr("SAVE_PATH")
#define		PCIE_TRANSFER_FILE				tr("TRANSFER_FILE")
// 高速常规参数
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
// 0233中速配置参数
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
// 0215中速帧头
#define		PCIE_FRAME_HEAD_OFFSET			tr("FRAME_HEAD_OFFSET")
#define		PCIE_FRAME_HEAD_VALUE			tr("FRAME_HEAD_VALUE")

#define		PCIE_TOTAL_FRAME_OFFSET			tr("TOTAL_FRAME_OFFSET")
#define		PCIE_TOTAL_FRAME_VALUE			tr("TOTAL_FRAME_VALUE")

#define		PCIE_RECV_START					"rec_start"				// 接收使能，bit0：‘0’—不使能；‘1’—使能。bit31~bit1：预留
#define		PCIE_RECV_MODE					"rec_mode"				// 接收模式设置，bit[2:0]: //‘001’：主份模式  //‘010’：备份模式 //‘100’：主备模式 //bit31~bit3：预留
#define		PCIE_SEND_START					"tra_start"				// 发送使能，bit0：‘0’—不使能；‘1’—使能  //bit31~bit1：预留
#define		PCIE_SEND_MODE					"tra_mode"				// 发送模式设置，bit[2:0]: //‘001’：主份模式  //‘010’：备份模式 //‘100’：主备模式 //bit31~bit3：预留
#define		PCIE_POST_PRO					"post_prog_empty"		// fifo可编程空标志，‘1’：空； ‘0’：不空 //不空即可从0x5c端口读数据 
																	// [DMA复用] us_count 0x54 PCIE上行存储空间，RO(字节数)   -->接收数据地址
#define		PCIE_POST_RD_DATA_CONFIG		"post_rd_data_config"	// fifo可读数据量：32bit数据个数RO
																	// [DMA复用] us_count 0x58 PCIE下行存储空间，RO(字节数)   -->发送数据地址
#define		PCIE_POST_DOUT					"post_dout"				// 数据端口：RO //当0x54不空或者0x58数据量不为零，即可从此地址端口读数据，存文件
#define		PCIE_DATA_SRC					"data_src"
#define		PCIE_DATA_DST					"data_dst"
#define		PCIE_TYPE_STATE					"data_info"
#define		PCIE_RATE_ID					"rate_identifier"	
#define		PCIE_VALID_LENGTH				"valid_length"
#define		PCIE_FRAME_GAP					"frame_gap"
#define		PCIE_START_STOP					"start_stop"
// 0233中速(100Mbps)新增寄存器读写参数
#define		PCIE_DATA_SRC_CH2				"data_src_ch2"
#define		PCIE_DATA_DST_CH2				"data_dst_ch2"
#define		PCIE_TYPE_STATE_CH2				"data_info_ch2"
#define		PCIE_RATE_ID_CH2				"rate_identifier_ch2"	
#define		PCIE_VALID_LENGTH_CH2			"valid_length_ch2"
#define		PCIE_FRAME_GAP_CH2				"frame_gap_ch2"

#define		PCIE_SEND_CHANNEL				"send_channel"
#define		PCIE_RECV_CHANNEL				"recv_channel"
// 0215中速(1024Mbps)新增寄存器读写参数
#define		PCIE_FRAME_HEAD					"frame_head"

#define		PCIE_TOTAL_FRAME				"total_frame"

typedef struct _PCIeDeviceInfo
{
	float		fRecvRate;				// 实时接收速率
	float		fStorageQuantity;		// 数据存储总量
	DWORD		dwGroupAWriteCount;		// A通道发送帧计数
	DWORD		dwGroupBWriteCount;
	DWORD		dwGroupARecvCount;		// A通道接收帧计数
	DWORD		dwGroupBRecvCount;
	DWORD		dwGroupACrcErrorCount;	// A通道CRC错帧计数
	DWORD		dwGroupBCrcErrorCount;
	DWORD		dwTotalData;			// 当前接收数据总量
	DWORD		dwLastTotalData;		// 上一时刻接收的数据总量，用于计算接收速率
	DWORD		dwFifoCount;			// 板卡FiFo空间大小，现满载为524296[80008]
	DWORD		dwFifoOverFlow;			// 板卡FiFo空间满计数，该计数增加1则表示有一次溢出。
	DWORD		dwStart;				// 开始时间，用于计算接收速率
	DWORD		dwFileCount;			// 存储的文件计数
	DWORD		dwBufferOverFlow;		// 缓存溢出的次数
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

// 寄存器配置参数模板
typedef struct _CoupleParameter
{
	unsigned int	nOffset;
	unsigned int	nValue;
}CoupleParameter, *PCoupleParameter;

typedef std::map<std::string, CoupleParameter> CoupleParameterMap;

typedef enum _TRANSFER_TYPE
{
    NONE_TYPE_TRANSFER = 0,
    PCIE_AUTO_TRANSFER,			// PCIe自动发送模式
    UPPER_MONITER_TRANSFER,		// 上位机软件发送模式
}TRANSFER_TYPE;

typedef enum _DATA_FIELD
{
    NONE_TYPE_FIELD = 0,		// 无类型
	ACCUMULATION_FIELD,			// 累加数
	RANDOM_NUMBER_FIELD,		// 随机数
	BLANK_FRAME_FIELD,			// 空帧
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

// 初始化各配置寄存器参数
void initPCIeConfigParameter();

// 创建多级目录函数
bool createMultipleDirectory(const QString strPath);

#endif // COMMON_H
