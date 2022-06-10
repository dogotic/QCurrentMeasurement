#ifndef DATAACQUISITIONTHREAD_H
#define DATAACQUISITIONTHREAD_H

#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtSerialPort/QSerialPort>

#include "logger.h"

class DataAcquisitionThread : public QThread
{
    Q_OBJECT
public:
    DataAcquisitionThread();
    ~DataAcquisitionThread();
    void run();
    void startRecording();
    void stopRecording();
    void stop();
    QList<QJsonObject> getDataSamples();

private:
    bool m_recording = false;
    bool m_running  = false;
    QSerialPort *port;
    int m_sampleCounter = 0;
    double busVoltage_avg = 0.0;
    double shuntVoltage_avg = 0.0;
    double loadVoltage_avg = 0.0;
    double current_mA_avg = 0.0;
    double power_mW_avg = 0.0;
    QString m_csvBuffer;
    Logger *m_logger;
    QList<QJsonObject> *m_dataSamples;

signals:
    void notifyDAQConnected(bool status);
    void sendMeasurementResults(QJsonObject data);

public slots:
    void storeCSVFile(QString filePathAndFileName);
    void startCommunicationOnPort(QString port_name);
    void stopCommunicationOnPort();
    void handleError(QSerialPort::SerialPortError error);
};

#endif // DATAACQUISITIONTHREAD_H
