#include <QDebug>
#include "serialportlistpopulator.h"

serialPortListPopulator::serialPortListPopulator()
{
    m_running = true;
}

serialPortListPopulator::~serialPortListPopulator()
{
    m_running = false;
}

void serialPortListPopulator::run()
{
    while(m_running)
    {
        // list serialPorts
        const auto infos = QSerialPortInfo::availablePorts();
        for (const QSerialPortInfo &info : infos)
        {
            qDebug() << __FUNCTION__ << ", " << info.portName();
        }
        emit sendPortList(infos);
        QThread::sleep(1);
    }
}

void serialPortListPopulator::stop()
{
    m_running = false;
}
