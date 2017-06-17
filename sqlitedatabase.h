#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QtSql>
#include <QSqlQueryModel>
#include <QTableView>
#include <QMessageBox>

static bool createSqliteConnection()
{
    //创建DB文件，创建表格
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("PCIeController.db");
    if (!db.open())
    {
        QMessageBox::critical(NULL, QObject::tr("失败"), QObject::tr("连接数据库失败!"));
        return false;
    }
    QSqlQuery query;
    bool bOk = query.exec("create table if not exists tbl_logger(timestamp text, context text)");
    if (!bOk)
    {
        QMessageBox::critical(NULL, QObject::tr("失败"), QObject::tr("创建运行日志表失败!"));
        return false;
    }
    bOk = query.exec("create table if not exists tbl_mdiarea(diagram text PRIMARY KEY NOT NULL,"
                     "type INTEGER NOT NULL,"
                     "offset1 text, value1 text,"
                     "offset2 text, value2 text,"
                     "offset3 text, value3 text,"
                     "offset4 text, value4 text,"
                     "offset5 text, value5 text,"
                     "offset6 text, value6 text,"
                     "offset7 text, value7 text,"
                     "offset8 text, value8 text,"
                     "offset9 text, value9 text)");
    if (!bOk)
    {
        QMessageBox::critical(NULL, QObject::tr("失败"), QObject::tr("创建多文档区域表!"));
        return false;
    }
    /*
    /// tbl_appconf 应用程序配置项，暂时不确定需要多少项
    bOk = query.exec("create table if not exists tbl_config(currentuser varchar(24),"
                     "background text,"
                     "other text)");
    if (!bOk)
    {
        QMessageBox::critical(NULL, QObject::tr("失败"), QObject::tr("创建运行日志表失败!"));
        return false;
    }
    */
    return true;
}

#endif // SQLITEDATABASE_H
