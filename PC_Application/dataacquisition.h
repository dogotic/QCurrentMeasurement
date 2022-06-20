#ifndef DATAACQUISITION_H
#define DATAACQUISITION_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtSerialPort/QSerialPort>

#include "logger.h"

class DataAcquisition : public QObject
{
    Q_OBJECT
public:
    DataAcquisition();
    ~DataAcquisition();

private:
    QSerialPort *port;
    int m_sampleCounter = 0;
    double busVoltage_avg = 0.0;
    double shuntVoltage_avg = 0.0;
    double loadVoltage_avg = 0.0;
    double current_mA_avg = 0.0;
    double power_mW_avg = 0.0;
    QJsonArray m_dataSamples;
    QString m_deviceName;
    QByteArray data_in;

signals:
    void notifyDAQConnected(bool status);
    void sendDataSamples(QJsonArray dataSamples);

public slots:
    void startCommunicationOnPort(QString port_name);
    void stopCommunicationOnPort();
    void handleError(QSerialPort::SerialPortError error);
    void readIncommingData();
};

#endif // DATAACQUISITION_H
