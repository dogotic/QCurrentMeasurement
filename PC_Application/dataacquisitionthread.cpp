#include <QDebug>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include "dataacquisitionthread.h"

DataAcquisitionThread::DataAcquisitionThread()
{
    m_running = true;
    port = new QSerialPort("/dev/ttyUSB0");
    port->setBaudRate(QSerialPort::Baud115200);
    port->setDataBits(QSerialPort::Data8);
    port->setStopBits(QSerialPort::OneStop);
    port->setParity(QSerialPort::NoParity);
    port->open(QIODevice::ReadWrite);
    logger = new Logger("log.txt");
}

DataAcquisitionThread::~DataAcquisitionThread()
{
    delete logger;
    m_recording = false;
    m_running = false;
}

void DataAcquisitionThread::run()
{
    if (port->isOpen())
    {
        emit notifyDAQConnected(true);
    }
    else
    {
        emit notifyDAQConnected(false);
    }

    while(m_running)
    {
        /* format data as json to easily retrieve voltages and currents */
        QByteArray data_in = port->readAll();

        if (data_in.length() > 0)
        {
            QJsonDocument doc = QJsonDocument::fromJson(data_in);
            QJsonObject obj = doc.object();

            double busVoltage = obj["busvoltage"].toDouble();
            double loadVoltage = obj["loadvoltage"].toDouble();
            double shuntVoltage = obj["shuntvoltage"].toDouble();
            double current_mA = obj["current_mA"].toDouble();
            double power_mW = obj["power_mW"].toDouble();

            if (m_recording)
            {
                /* store to csv buffer */
                csv_buffer.append(QString::number(busVoltage) + ",");
                csv_buffer.append(QString::number(loadVoltage) + ",");
                csv_buffer.append(QString::number(shuntVoltage) + ",");
                csv_buffer.append(QString::number(current_mA) + ",");
                csv_buffer.append(QString::number(power_mW) + "\n");
            }

            /* prevent divison by zero */
            if  (sample_counter > 0)
            {
                busVoltage_avg += busVoltage;
                busVoltage_avg /= (double)sample_counter;

                shuntVoltage_avg += shuntVoltage;
                shuntVoltage_avg /= sample_counter;

                loadVoltage_avg += loadVoltage;
                loadVoltage_avg /= sample_counter;

                current_mA_avg += current_mA;
                current_mA_avg /= sample_counter;

                power_mW_avg += power_mW;
                power_mW_avg /= sample_counter;
            }

            QJsonObject data_out;
            data_out.insert("busvoltage_now",busVoltage);
            data_out.insert("loadvoltage_now",loadVoltage);
            data_out.insert("shuntvoltage_now",shuntVoltage);
            data_out.insert("power_mW_now",power_mW);
            data_out.insert("current_mA_now",current_mA);

            data_out.insert("busvoltage_avg",busVoltage_avg);
            data_out.insert("loadvoltage_avg",loadVoltage_avg);
            data_out.insert("shuntvoltage_avg",shuntVoltage_avg);
            data_out.insert("power_mW_avg",power_mW_avg);
            data_out.insert("current_mA_avg",current_mA_avg);

            /* ignore first result as it returns 0 for all measurements */
            if (sample_counter > 0)
            {
                emit sendMeasurementResults(data_out);
            }

            sample_counter++;
        }
        else
        {
            logger->write(WARNING,"ZERO LENGTH DATA RECEIVED");
        }

        QThread::sleep(1);
    }
}

void DataAcquisitionThread::storeCSVFile(QString filePathAndFileName)
{
    QFile csvFile(filePathAndFileName);
    csvFile.open(QIODevice::ReadWrite | QIODevice::Text);
    csvFile.resize(0);
    csvFile.write("BUS VOLTAGE,SHUNT VOLTAGE,LOAD VOLTAGE, CURRENT, POWER\n");
    csvFile.write(csv_buffer.toUtf8());
    csvFile.flush();
    csvFile.close();
}


void DataAcquisitionThread::startRecording()
{
    m_recording = true;
    csv_buffer.clear();
}

void DataAcquisitionThread::stopRecording()
{
    m_recording = false;
}