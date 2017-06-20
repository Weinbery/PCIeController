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
    bOk = query.exec("create table if not exists tbl_workspace(workspace text PRIMARY KEY)");
    if (!bOk)
    {
        QMessageBox::critical(NULL, QObject::tr("失败"), QObject::tr("创建工作空间表失败!"));
        return false;
    }
    bOk = query.exec("create table if not exists tbl_highspeed(windowTitle text PRIMARY KEY NOT NULL,"
                     "typeId INTEGER NOT NULL,"
                     "srcAddrOffset varchar(10), srcAddrValue varchar(10),"
                     "dstAddrOffset varchar(10), dstAddrValue varchar(10),"
                     "typeStateOffset varchar(10), typeStateValue varchar(10),"
                     "velocityIdOffset varchar(10), velocityIdValue varchar(10),"
                     "validSizeOffset5 varchar(10), validSizeValue varchar(10),"
                     "timeGapOffset varchar(10), timeGapValue varchar(10),"
                     "startOrstopOffset varchar(10), startOrstopValue varchar(10))");
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
