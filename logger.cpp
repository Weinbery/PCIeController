#include "logger.h"
#include <QDebug>
#include <QMessageBox>

Logger::Logger(QObject *parent) : QObject(parent)
{

}

void Logger::loggerWrite(const QString strContext)
{
    /// 时间戳
    QString strTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ///
    QSqlQuery query;
    query.prepare("insert into tbl_logger(timestamp, context) values(:timestamp, :context)");
    query.bindValue(":timestamp", strTime);
    query.bindValue(":context", strContext);

    bool bOk = query.exec();
    if (!bOk)
    {
        QMessageBox::warning(NULL, "失败", "日志失败");
    }
}

