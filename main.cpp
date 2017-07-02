#include "mainwindow.h"
#include "commonhelper.h"
#include "sqlitedatabase.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /// 程序单例运行
    QCoreApplication::setOrganizationName("Weinbery");
    QCoreApplication::setApplicationName("PCIeController");
    QString strKey = QCoreApplication::organizationName() + QCoreApplication::applicationName();
    QSharedMemory sharedMemory(strKey);
    if (!sharedMemory.create(512, QSharedMemory::ReadWrite))
    {
        QMessageBox::information(NULL, QStringLiteral("提示"), QStringLiteral("程序已运行！"));
        exit(0);
    }
    ///
    CommonHelper::setStyle("PCIeController.qss");
    /// 创建数据库连接
    if (!createSqliteConnection())
    {
        return -1;
    }
    /// fusion windows
    QApplication::setStyle(QStyleFactory::create("fusion"));
    ///
    MainWindow w;
    w.show();

    return a.exec();
}
