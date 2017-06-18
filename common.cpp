#include "Common.h"

char* global_PCIeParameterName[DMA_PARAM_SIZE];

void initPCIeConfigParameter()
{
	//初始化PCIe板卡需要操作的参数名称
	global_PCIeParameterName[0] = PCIE_RECV_START;
	global_PCIeParameterName[1] = PCIE_RECV_MODE;
	global_PCIeParameterName[2] = PCIE_SEND_START;
	global_PCIeParameterName[3] = PCIE_SEND_MODE;
	global_PCIeParameterName[4] = PCIE_POST_PRO;
	global_PCIeParameterName[5] = PCIE_POST_RD_DATA_CONFIG;
	global_PCIeParameterName[6] = PCIE_POST_DOUT;
	//
	global_PCIeParameterName[7] = PCIE_DATA_SRC;
	global_PCIeParameterName[8] = PCIE_DATA_DST;
	global_PCIeParameterName[9] = PCIE_TYPE_STATE;
	global_PCIeParameterName[10] = PCIE_RATE_ID;
	global_PCIeParameterName[11] = PCIE_VALID_LENGTH;
	global_PCIeParameterName[12] = PCIE_FRAME_GAP;
	global_PCIeParameterName[13] = PCIE_START_STOP;
	// 0233项目中速寄存器读写参数
	global_PCIeParameterName[14] = PCIE_DATA_SRC_CH2;
	global_PCIeParameterName[15] = PCIE_DATA_DST_CH2;
	global_PCIeParameterName[16] = PCIE_TYPE_STATE_CH2;
	global_PCIeParameterName[17] = PCIE_RATE_ID_CH2;
	global_PCIeParameterName[18] = PCIE_VALID_LENGTH_CH2;
	global_PCIeParameterName[19] = PCIE_FRAME_GAP_CH2;

	global_PCIeParameterName[20] = PCIE_SEND_CHANNEL;
	global_PCIeParameterName[21] = PCIE_RECV_CHANNEL;
	// 添加0215项目中速寄存器读写参数
	global_PCIeParameterName[22] = PCIE_FRAME_HEAD;
	// 
	global_PCIeParameterName[23] = PCIE_TOTAL_FRAME;
}

// 用于创建多级目录
bool createMultipleDirectory(const QString strPath)
{
    QDir dir(strPath);
    if (dir.exists())
    {
        return true;
    }
    else
    {
        bool bOk = dir.mkpath(strPath); // 创建多级目录
        return bOk;
    }
}
