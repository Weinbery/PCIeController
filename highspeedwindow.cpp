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
    pciexpress = new HighSpeedOperation();
}

HighSpeedWindow::~HighSpeedWindow()
{
    delete ui;
}

QSize HighSpeedWindow::sizeHint() const
{
    return QSize(320, 240);
}

QString HighSpeedWindow::getWindowTitle() const
{
    return strWindowTitle;
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

}

void HighSpeedWindow::on_pushButtonClosePCIe_clicked()
{

}

void HighSpeedWindow::on_pushButtonStartRecv_clicked()
{

}

void HighSpeedWindow::on_pushButtonStartSend_clicked()
{

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
    quint32 nOffset = ui->lineEditRegisterOffset->text().toInt(0, 16);
    quint32 nValue = ui->lineEditRegisterValue->text().toInt(0, 16);

    emit loggerWrite(tr("%1，寄存器写入，nOffset = 0x%2，nValue = 0x%3")
                         .arg(getWindowTitle())
                         .arg(QString::number(nOffset, 16))
                         .arg(QString::number(nValue, 16)));

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
