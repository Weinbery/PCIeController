#include "highspeedwindow.h"
#include "ui_highspeedwindow.h"
#include <QFileDialog>

int HighSpeedWindow::sequenceNumber = 1;

HighSpeedWindow::HighSpeedWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HighSpeedWindow)
{
    ui->setupUi(this);

    strWindowTitle = tr("高速PCIe窗口%1").arg(sequenceNumber++);
    setWindowTitle(strWindowTitle);

    QStringList comboSendType;
    comboSendType << "PCIe发送" << "发送文件";
    ui->comboBoxSendType->addItems(comboSendType);
    ui->lineEditChooseFile->setVisible(false);
    ui->pushButtonChooseFile->setVisible(false);

    ui->radioButtonAcc->setChecked(true);
    //ui->radioButtonAcc->setVisible(false);
    if (mapCardStrParameter.size() > 0)
    {
        QStringList pcieList;
        map<std::string, PPCIeParameter>::iterator iter = mapCardStrParameter.begin();
        for (; iter != mapCardStrParameter.end(); iter++)
        {
            pcieList << iter->first.c_str();
        }
        // 添加pcie总线列表
        ui->comboBoxPCIeBus->addItems(pcieList);
    }
    //
    pciexpress = new HighSpeedOperation();
}

HighSpeedWindow::~HighSpeedWindow()
{
    delete ui;
    if (NULL != pciexpress)
    {
        delete pciexpress;
        pciexpress = NULL;
    }
}

QSize HighSpeedWindow::sizeHint() const
{
    return QSize(320, 240);
}

QString HighSpeedWindow::getWindowTitle() const
{
    return strWindowTitle;
}

void HighSpeedWindow::setWorkspace(QString strDir)
{
    strWorkspace = strDir;
}

void HighSpeedWindow::initDialogParameter()
{

}

void HighSpeedWindow::initRegisterParameter()
{
    CoupleParameter config;

    pciexpress->m_ConfigLock.lock();
    pciexpress->m_mapCardConfig.clear();
    // 默认不变参数，直接写死在程序里面
    config.nOffset = strtol("44", NULL, 16);
    config.nValue = strtol("0", NULL, 16);
    pciexpress->m_mapCardConfig.insert(make_pair(global_PCIeParameterName[0], config));

    config.nOffset = strtol("48", NULL, 16);
    config.nValue = strtol("0", NULL, 16);
    pciexpress->m_mapCardConfig.insert(make_pair(global_PCIeParameterName[1], config));

    config.nOffset = strtol("4c", NULL, 16);
    config.nValue = strtol("0", NULL, 16);
    pciexpress->m_mapCardConfig.insert(make_pair(global_PCIeParameterName[2], config));

    config.nOffset = strtol("50", NULL, 16);
    config.nValue = strtol("0", NULL, 16);
    pciexpress->m_mapCardConfig.insert(make_pair(global_PCIeParameterName[3], config));

    config.nOffset = strtol("54", NULL, 16);
    config.nValue = strtol("1", NULL, 16);
    pciexpress->m_mapCardConfig.insert(make_pair(global_PCIeParameterName[4], config));

    config.nOffset = strtol("58", NULL, 16);
    config.nValue = strtol("0", NULL, 16);
    pciexpress->m_mapCardConfig.insert(make_pair(global_PCIeParameterName[5], config));

    config.nOffset = strtol("5c", NULL, 16);
    config.nValue = strtol("0", NULL, 16);
    pciexpress->m_mapCardConfig.insert(make_pair(global_PCIeParameterName[6], config));
    // 从界面获取参数
    config.nOffset = ui->labelSrcAddress->text().toInt(0, 16);
    config.nValue = ui->lineEditSrcAddress->text().toInt(0, 16);
    pciexpress->m_mapCardConfig.insert(make_pair(global_PCIeParameterName[7], config));
    //
    config.nOffset = ui->labelDstAddress->text().toInt(0, 16);
    config.nValue = ui->lineEditDstAddress->text().toInt(0, 16);
    pciexpress->m_mapCardConfig.insert(make_pair(global_PCIeParameterName[8], config));
    //
    config.nOffset = ui->labelTypeState->text().toInt(0, 16);
    config.nValue = ui->lineEditTypeState->text().toInt(0, 16);
    pciexpress->m_mapCardConfig.insert(make_pair(global_PCIeParameterName[9], config));
    //
    config.nOffset = ui->labelVelocityId->text().toInt(0, 16);
    config.nValue = ui->lineEditVelocityId->text().toInt(0, 16);
    pciexpress->m_mapCardConfig.insert(make_pair(global_PCIeParameterName[10], config));
    //
    config.nOffset = ui->labelValidSize->text().toInt(0, 16);
    config.nValue = ui->lineEditValidSize->text().toInt(0, 16);
    pciexpress->m_mapCardConfig.insert(make_pair(global_PCIeParameterName[11], config));
    //
    config.nOffset = ui->labelTimeGap->text().toInt(0, 16);
    config.nValue = ui->lineEditTimeGap->text().toInt(0, 16);
    pciexpress->m_mapCardConfig.insert(make_pair(global_PCIeParameterName[12], config));
    //
    config.nOffset = ui->labelStartOrStop->text().toInt(0, 16);
    config.nValue = ui->lineEditStartOrStop->text().toInt(0, 16);
    pciexpress->m_mapCardConfig.insert(make_pair(global_PCIeParameterName[13], config));
}

QString HighSpeedWindow::getFile() const
{
    return strFile;
}

void HighSpeedWindow::on_pushButtonChooseFile_clicked()
{
    strFile = QFileDialog::getOpenFileName(
                this,
                tr("选择需要发送的文件"),
                tr("."),
                tr("二进制文件(*.dat);;所有文件(*)")
                );
    if (strFile.isEmpty())
    {
        return;
    }
    else
    {
        ui->lineEditChooseFile->setText(strFile);
    }
}

void HighSpeedWindow::on_comboBoxSendType_currentIndexChanged(int index)
{
    if (index == 0)
    {
        ui->lineEditChooseFile->setVisible(false);
        ui->pushButtonChooseFile->setVisible(false);
        ui->radioButtonAcc->setVisible(true);
        ui->radioButtonRandom->setVisible(true);
        ui->radioButtonBlank->setVisible(true);
    }
    else if (index == 1)
    {
        ui->lineEditChooseFile->setVisible(true);
        ui->pushButtonChooseFile->setVisible(true);
        ui->radioButtonAcc->setVisible(false);
        ui->radioButtonRandom->setVisible(false);
        ui->radioButtonBlank->setVisible(false);
    }
}

void HighSpeedWindow::on_pushButtonOpenPCIe_clicked()
{
    QString strPCIeCard = ui->comboBoxPCIeBus->currentText();
    map<string, PPCIeParameter>::iterator iter;
    iter = mapCardStrParameter.find(strPCIeCard.toLocal8Bit().data());
    if (iter == mapCardStrParameter.end())
    {
        return;
    }
    pciexpress->setCard(iter->second);
    if (!pciexpress->openCard())
    {
        return;					// 打开失败
    }
    // 初始化PCIe寄存器参数配置
    initRegisterParameter();
    // 打开板卡即默认接收
    on_pushButtonStartRecv_clicked();
    //
    ui->pushButtonOpenPCIe->setEnabled(false);
    ui->comboBoxPCIeBus->setEnabled(false);

    emit loggerWrite(tr("%1，打开PCIe板卡：%2").arg(strWindowTitle).arg(strPCIeCard));
}

void HighSpeedWindow::on_pushButtonClosePCIe_clicked()
{
    if (NULL != pciexpress)
    {
        if (!pciexpress->m_bWriteStop)
        {
            on_pushButtonStartSend_clicked();
        }
        if (!pciexpress->m_bRecvStop)
        {
            on_pushButtonStartRecv_clicked();
        }
        //
        Sleep(100);
        //
        pciexpress->closeCard();
        ui->pushButtonOpenPCIe->setEnabled(true);
        ui->comboBoxPCIeBus->setEnabled(true);
        //
        emit loggerWrite(tr("%1，关闭PCIe板卡").arg(strWindowTitle));
    }
}

void HighSpeedWindow::on_pushButtonStartRecv_clicked()
{
    if (NULL != pciexpress)
    {
        if (!pciexpress->m_bCardOpen)
        {
            return;
        }
        if (!pciexpress->m_bRecvStop)
        {
            pciexpress->stopReceive();
            ui->pushButtonStartRecv->setText(tr("开始接收"));
            emit loggerWrite(tr("%1，PCIe板卡：%2，停止接收").arg(strWindowTitle).arg(pciexpress->m_pCard->pcieCardInfo.pciSlot.dwBus));
        }
        QString strSavePath = strWorkspace + "/" + strWindowTitle;
        if (!createMultipleDirectory(strSavePath))
        {
            emit loggerWrite(tr("%1，创建目录：%2失败").arg(strWindowTitle).arg(strSavePath));
            return;
        }
        // 截取时间戳
        QString strTimeStamp = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
        pciexpress->setTimeStamp(strTimeStamp);
        //
        pciexpress->setSaveDirectory(strSavePath);
        if (!pciexpress->startReceive())
        {
            emit loggerWrite(tr("%1，PCIe板卡：%2，接收失败").arg(strWindowTitle).arg(pciexpress->m_pCard->pcieCardInfo.pciSlot.dwBus));
            return;
        }
        emit loggerWrite(tr("%1，PCIe板卡：%2，开始接收").arg(strWindowTitle).arg(pciexpress->m_pCard->pcieCardInfo.pciSlot.dwBus));
    }
}

void HighSpeedWindow::on_pushButtonStartSend_clicked()
{
    if (NULL != pciexpress)
    {
        if (!pciexpress->m_bCardOpen)
        {
            return;
        }
        TRANSFER_PARAMETER transferParam;
        int index = ui->comboBoxSendType->currentIndex();
        if (index == 0)
        {
            transferParam.eTransferType = PCIE_AUTO_TRANSFER;
            if (ui->radioButtonAcc->isChecked())
                transferParam.eDataField = ACCUMULATION_FIELD;
            else if (ui->radioButtonRandom->isChecked())
                transferParam.eDataField = RANDOM_NUMBER_FIELD;
            else if (ui->radioButtonBlank->isChecked())
                transferParam.eDataField = BLANK_FRAME_FIELD;
            pciexpress->setTransferParameter(transferParam);
        }
        else
        {
            transferParam.eTransferType = UPPER_MONITER_TRANSFER;
            transferParam.strTransferFile = ui->lineEditChooseFile->text();
            pciexpress->setTransferParameter(transferParam);
        }
        if (!pciexpress->m_bWriteStop)
        {
            pciexpress->stopWrite();
            ui->pushButtonStartSend->setText(tr("开始发送"));
            emit loggerWrite(tr("%1，PCIe板卡：%2，停止发送").arg(strWindowTitle).arg(pciexpress->m_pCard->pcieCardInfo.pciSlot.dwBus));
            return;
        }
        //
        initRegisterParameter();
        //
        if (!pciexpress->startWrite())
        {
            emit loggerWrite(tr("%1，PCIe板卡：%2，发送失败").arg(strWindowTitle).arg(pciexpress->m_pCard->pcieCardInfo.pciSlot.dwBus));
            return;
        }
        emit loggerWrite(tr("%1，PCIe板卡：%2，开始发送").arg(strWindowTitle).arg(pciexpress->m_pCard->pcieCardInfo.pciSlot.dwBus));
    }
}

void HighSpeedWindow::on_pushButtonReadRegister_clicked()
{
    if (NULL != pciexpress && NULL != pciexpress->m_pCard)
    {
        if (!pciexpress->m_pCard->bOpen)
        {
            return ;
        }
        quint32 nOffset = ui->lineEditRegisterOffset->text().toInt(0, 16);
        quint32 nValue = ReadRegister(pciexpress->m_pCard, nOffset);
        ui->lineEditRegisterValue->setText(QString::number(nValue, 16));
    }
}

void HighSpeedWindow::on_pushButtonWriteRegister_clicked()
{
    if (NULL != pciexpress && NULL != pciexpress->m_pCard)
    {
        if (!pciexpress->m_pCard->bOpen)
        {
            return ;
        }
        quint32 nOffset = ui->lineEditRegisterOffset->text().toInt(0, 16);
        quint32 nValue = ui->lineEditRegisterValue->text().toInt(0, 16);

        if (WriteRegister(pciexpress->m_pCard, nOffset, nValue))
        {
            emit loggerWrite(tr("%1，寄存器写入，nOffset = 0x%2，nValue = 0x%3")
                             .arg(getWindowTitle())
                             .arg(QString::number(nOffset, 16))
                             .arg(QString::number(nValue, 16)));
        }
    }
}
