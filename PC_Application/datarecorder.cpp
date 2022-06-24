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

void DataRecorder::ReceiveDataSamples(QByteArray dataSamples)
{
    if (m_recording)
    {
        QString busVoltage;
        QString loadVoltage;
        QString shuntVoltage;
        QString current_mA;
        QString power_mW;

        QString input = QString(dataSamples);
        QStringList items = input.split(",");

        loadVoltage = items[1];
        busVoltage = items[2];
        shuntVoltage = items[3];
        current_mA = items[4];
        power_mW = items[5];

        m_csvBuffer.append(busVoltage + ",");
        m_csvBuffer.append(loadVoltage + ",");
        m_csvBuffer.append(shuntVoltage + ",");
        m_csvBuffer.append(current_mA + ",");
        m_csvBuffer.append(power_mW + "\n");
    }
}
