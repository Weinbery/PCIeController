#include "mainwindow.h"
#include "sqlitedatabase.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
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
