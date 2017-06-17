#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include "sqlitedatabase.h"

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = 0);

signals:

public slots:
    void loggerWrite(const QString strContext/*日志内容*/);
};

#endif // LOGGER_H
