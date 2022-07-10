#include <QDebug>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include "dataacquisition.h"

DataAcquisition::DataAcquisition()
{
    port = new QSerialPort();
    connect(port,SIGNAL(errorOccurred(QSerialPort::SerialPortError)),this,SLOT(handleError(QSerialPort::SerialPortError)));
    connect(port,SIGNAL(readyRead()),this,SLOT(readIncommingData()));
}

DataAcquisition::~DataAcquisition()
{

}

void DataAcquisition::startCommunicationOnPort(QString port_name)
{
#ifdef __linux__
    port->setPortName("/dev/" + port_name);
#elif _WIN32
    port->setPortName(port_name);
#endif
    port->setBaudRate(115200);
    port->setDataBits(QSerialPort::Data8);
    port->setStopBits(QSerialPort::OneStop);
    port->setParity(QSerialPort::NoParity);
    port->open(QIODevice::ReadWrite);
}

void DataAcquisition::stopCommunicationOnPort()
{
    port->close();
    emit notifyDAQConnected(false);
}

void DataAcquisition::handleError(QSerialPort::SerialPortError error)
{
    qDebug() << error;
    if (error != QSerialPort::SerialPortError::NoError)
    {
        if (port->isOpen())
        {
            port->close();
        }
        emit notifyDAQConnected(false);
    }
}

void DataAcquisition::readIncommingData()
{
    //data_in = port->readAll();
    data_in = port->readLine();
    if (data_in.contains("QCurrentMeasurement"))
    {
        emit notifyDAQConnected(true);
        emit sendDataSamples(data_in);
    }
    else
    {
        emit notifyDAQConnected(false);
    }
}

