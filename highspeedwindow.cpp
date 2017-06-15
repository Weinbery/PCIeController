#include "highspeedwindow.h"
#include "ui_highspeedwindow.h"
#include <QFileDialog>

int HighSpeedWindow::sequenceNumber = 1;

HighSpeedWindow::HighSpeedWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HighSpeedWindow)
{
    ui->setupUi(this);

    strCurrentWindow = tr("高速PCIe窗口%1").arg(sequenceNumber++);
    setWindowTitle(strCurrentWindow);

    QStringList comboSendType;
    comboSendType << "PCIe发送" << "发送文件";
    ui->comboBoxSendType->addItems(comboSendType);
    ui->lineEditChooseFile->setVisible(false);
    ui->pushButtonChooseFile->setVisible(false);

    ui->radioButtonAcc->setChecked(true);
    //ui->radioButtonAcc->setVisible(false);
}

HighSpeedWindow::~HighSpeedWindow()
{
    delete ui;
}

QSize HighSpeedWindow::sizeHint() const
{
    return QSize(320, 240);
}

QString HighSpeedWindow::getFile()
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
