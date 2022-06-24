#include <QDateTime>
#include <QFile>
#include "logger.h"

Logger::Logger(QString log_name)
{
    log_file = new QFile(log_name);
    log_file->open(QIODevice::ReadWrite);
    log_file->resize(0);
}

Logger::~Logger()
{
    log_file->close();
    delete(log_file);
}

void Logger::write(log_level_t level, QString content)
{
    QDateTime dateTime = dateTime.currentDateTime();
    QString currentDateTime = dateTime.toString("yyyy-MM-dd HH:mm:ss");
    QString level_str;

    if (level == INFO) level_str = "INFO";
    else if(level == WARNING) level_str = "WARNING";
    else if(level == ERROR) level_str = "ERROR";
    QString data = currentDateTime + " [ " + level_str + " ] " + content + "\n";
    log_file->write(data.toUtf8());
}
