#ifndef LOGGER_H
#define LOGGER_H

#include <QFile>

enum log_level_t
{
    INFO,
    WARNING,
    ERROR
};

class Logger
{

private:
    QFile *log_file;

public:
    Logger(QString log_name);
    ~Logger();
    void write(log_level_t level, QString content);
};

#endif // LOGGER_H
