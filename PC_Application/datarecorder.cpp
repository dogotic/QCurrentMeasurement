#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include "datarecorder.h"

DataRecorder::DataRecorder(QObject *parent) : QObject(parent)
{

}

DataRecorder::~DataRecorder()
{
    m_recording = false;
    emit RecordingStopped();
}

void DataRecorder::startRecording()
{
    m_recording = true;
    m_csvBuffer.clear();
    emit RecordingStarted();
}

void DataRecorder::stopRecording()
{
    m_recording = false;
    emit RecordingStopped();
}

void DataRecorder::storeCSVFile(QString filePathAndFileName)
{
    if (m_csvBuffer.count() > 0)
    {
        QFile csvFile(filePathAndFileName);
        csvFile.open(QIODevice::ReadWrite | QIODevice::Text);
        csvFile.resize(0);
        csvFile.write("BUS VOLTAGE,SHUNT VOLTAGE,LOAD VOLTAGE, CURRENT, POWER\n");
        csvFile.write(m_csvBuffer.toUtf8());
        csvFile.flush();
        csvFile.close();
    }
}

void DataRecorder::ReceiveDataSamples(QJsonArray dataSamples)
{
    if (m_recording)
    {
        QString busVoltage;
        QString loadVoltage;
        QString shuntVoltage;
        QString current_mA;
        QString power_mW;

        QJsonObject obj;
        for (int i=0; i<dataSamples.count(); i++)
        {
            obj = dataSamples[i].toObject();

            busVoltage = QString::number(obj["busvoltage"].toDouble());
            loadVoltage = QString::number(obj["loadvoltage"].toDouble());
            shuntVoltage = QString::number(obj["shuntvoltage"].toDouble());
            current_mA = QString::number(obj["current_mA"].toDouble());
            power_mW = QString::number(obj["power_mW"].toDouble());

            m_csvBuffer.append(busVoltage + ",");
            m_csvBuffer.append(loadVoltage + ",");
            m_csvBuffer.append(shuntVoltage + ",");
            m_csvBuffer.append(current_mA + ",");
            m_csvBuffer.append(power_mW + "\n");
        }
    }
}
