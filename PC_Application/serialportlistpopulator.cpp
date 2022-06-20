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
        QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
        QStringList portNames;
        foreach (QSerialPortInfo info,infos)
        {
            portNames.append(info.portName());
        }
        emit sendPortList(portNames);
        QThread::sleep(1);
    }
}

void serialPortListPopulator::stop()
{
    m_running = false;
}
