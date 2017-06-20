#ifndef HIGHSPEEDWINDOW_H
#define HIGHSPEEDWINDOW_H

#include <QWidget>
#include "highspeedoperation.h"

namespace Ui {
class HighSpeedWindow;
}

class HighSpeedWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HighSpeedWindow(QWidget *parent = 0);
    HighSpeedWindow(QString windowTitle = tr(""), QWidget *parent = 0);
    ~HighSpeedWindow();

    QSize sizeHint() const;
    static int sequenceNumber;
    QString getFile() const;
    QString getWindowTitle() const;

public:
    void setWorkspace(QString strDir);
    // 从数据库读取数据，初始化对话框的参数
    void initDialogParameter();
    // 初始化寄存器的配置
    void initRegisterParameter();

signals:
    void loggerWrite(const QString strContext);

private slots:
    void on_comboBoxSendType_currentIndexChanged(int index);
    void on_pushButtonChooseFile_clicked();
    void on_pushButtonOpenPCIe_clicked();
    void on_pushButtonClosePCIe_clicked();
    void on_pushButtonStartRecv_clicked();
    void on_pushButtonStartSend_clicked();
    void on_pushButtonReadRegister_clicked();
    void on_pushButtonWriteRegister_clicked();

private:
    Ui::HighSpeedWindow *ui;
    QString strWindowTitle;
    QString strFile;
    QString strWorkspace;
    //
    HighSpeedOperation *pciexpress;
};

#endif // HIGHSPEEDWINDOW_H
