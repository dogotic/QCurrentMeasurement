#ifndef SERIALPORTLISTPOPULATOR_H
#define SERIALPORTLISTPOPULATOR_H

#include <QThread>
#include <QSerialPortInfo>

class serialPortListPopulator : public QThread
{
    Q_OBJECT

public:
    serialPortListPopulator();
    ~serialPortListPopulator();
    void stop();
    void run();

private:
    bool m_running = false;

signals:
    void sendPortList(QList <QSerialPortInfo> list);
};

#endif // SERIALPORTLISTPOPULATOR_H
