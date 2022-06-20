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
    port->setBaudRate(921600);
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
    // fill read buffer until valid json is completely received
    data_in.append(port->readAll());
    QJsonDocument doc = QJsonDocument::fromJson(data_in);
    if (doc.isArray())
    {
        m_dataSamples = doc.array();
        data_in.clear();
        m_deviceName = m_dataSamples[0].toObject()["device_name"].toString();
        if (m_deviceName == "QCurrentMeasurement")
        {
            emit notifyDAQConnected(true);
            emit sendDataSamples(m_dataSamples);
        }
        else
        {
            notifyDAQConnected(false);
        }
    }
}

