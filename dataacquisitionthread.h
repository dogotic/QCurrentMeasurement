#ifndef DATAACQUISITIONTHREAD_H
#define DATAACQUISITIONTHREAD_H

#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtSerialPort/QSerialPort>

class DataAcquisitionThread : public QThread
{
    Q_OBJECT
public:
    DataAcquisitionThread();
    ~DataAcquisitionThread();
    void run();
    void stop() { m_running = false; }
    void startRecording();
    void stopRecording();

private:
    bool m_recording = false;
    bool m_running  = false;
    QSerialPort *port;
    int sample_counter = 0;
    double busVoltage_avg = 0.0;
    double shuntVoltage_avg = 0.0;
    double loadVoltage_avg = 0.0;
    double current_mA_avg = 0.0;
    double power_mW_avg = 0.0;
    QString csv_buffer;

signals:
    void notifyDAQConnected(bool status);
    void sendMeasurementResults(QJsonObject data);

public slots:
    void storeCSVFile(QString filePathAndFileName);
};

#endif // DATAACQUISITIONTHREAD_H
