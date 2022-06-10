#include <QDebug>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include "dataacquisitionthread.h"

DataAcquisitionThread::DataAcquisitionThread()
{
    m_logger = new Logger("log.txt");
    port = new QSerialPort();
    connect(port,SIGNAL(errorOccurred(QSerialPort::SerialPortError)),this,SLOT(handleError(QSerialPort::SerialPortError)));
    m_dataSamples = new QList<QJsonObject>;
}

DataAcquisitionThread::~DataAcquisitionThread()
{
    delete m_logger;
    m_recording = false;
    m_running = false;
}

void DataAcquisitionThread::run()
{
    while(true)
    {
        if (m_running)
        {
            /* format data as json to easily retrieve voltages and currents */
            QByteArray data_in = port->readAll();

            if (data_in.length() > 0)
            {
                QJsonDocument doc = QJsonDocument::fromJson(data_in);
                QJsonObject obj = doc.object();

                // check if we are connected to the correct device

                double busVoltage = obj["busvoltage"].toDouble();
                double loadVoltage = obj["loadvoltage"].toDouble();
                double shuntVoltage = obj["shuntvoltage"].toDouble();
                double current_mA = obj["current_mA"].toDouble();
                double power_mW = obj["power_mW"].toDouble();

                if (m_recording)
                {
                    /* store to csv buffer */
                    m_csvBuffer.append(QString::number(busVoltage) + ",");
                    m_csvBuffer.append(QString::number(loadVoltage) + ",");
                    m_csvBuffer.append(QString::number(shuntVoltage) + ",");
                    m_csvBuffer.append(QString::number(current_mA) + ",");
                    m_csvBuffer.append(QString::number(power_mW) + "\n");
                    m_dataSamples->append(obj);
                }

                /* prevent divison by zero */
                if  (m_sampleCounter > 0)
                {
                    busVoltage_avg += busVoltage;
                    busVoltage_avg /= m_sampleCounter;

                    shuntVoltage_avg += shuntVoltage;
                    shuntVoltage_avg /= m_sampleCounter;

                    loadVoltage_avg += loadVoltage;
                    loadVoltage_avg /= m_sampleCounter;

                    current_mA_avg += current_mA;
                    current_mA_avg /= m_sampleCounter;

                    power_mW_avg += power_mW;
                    power_mW_avg /= m_sampleCounter;
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
                if (m_sampleCounter > 0)
                {
                    emit sendMeasurementResults(data_out);
                }
                m_sampleCounter++;
            }
            else
            {
                m_logger->write(WARNING,"ZERO LENGTH DATA RECEIVED");
            }
        }
        QThread::msleep(100);
    }
}

void DataAcquisitionThread::storeCSVFile(QString filePathAndFileName)
{
    QFile csvFile(filePathAndFileName);
    csvFile.open(QIODevice::ReadWrite | QIODevice::Text);
    csvFile.resize(0);
    csvFile.write("BUS VOLTAGE,SHUNT VOLTAGE,LOAD VOLTAGE, CURRENT, POWER\n");
    csvFile.write(m_csvBuffer.toUtf8());
    csvFile.flush();
    csvFile.close();
}


void DataAcquisitionThread::startRecording()
{
    m_recording = true;
    m_csvBuffer.clear();
}

void DataAcquisitionThread::stopRecording()
{
    m_recording = false;
}

void DataAcquisitionThread::startCommunicationOnPort(QString port_name)
{
#ifdef __linux__
    port->setPortName("/dev/" + port_name);
#elif _WIN32
    port->setPortName(port_name);
#endif
    port->setBaudRate(QSerialPort::Baud115200);
    port->setDataBits(QSerialPort::Data8);
    port->setStopBits(QSerialPort::OneStop);
    port->setParity(QSerialPort::NoParity);
    port->open(QIODevice::ReadWrite);

    // try read one message to determine if the correct device is connected
    if (port->isOpen())
    {
        bool ok = port->waitForReadyRead(10000);
        if (!ok)
        {
            m_running = false;
            port->close();
            emit notifyDAQConnected(false);
        }
        else
        {
            QByteArray data_in = port->readAll();

            if (data_in.length() > 0)
            {
                QJsonDocument doc = QJsonDocument::fromJson(data_in);
                QJsonObject obj = doc.object();
                m_dataSamples->append(obj);
                if (obj["device_name"] == "QCurrentMeasurement")
                {
                    m_sampleCounter = 0;
                    m_running = true;
                    emit notifyDAQConnected(true);
                }
                else
                {
                    m_running = false;
                    port->close();
                    emit notifyDAQConnected(false);
                }
            }
        }
    }
    else if (!port->isOpen())
    {
        m_running = false;
        port->close();
        emit notifyDAQConnected(false);
    }
}

void DataAcquisitionThread::stopCommunicationOnPort()
{
    m_dataSamples->clear();
    m_running = false;
    port->close();
    emit notifyDAQConnected(false);
}

void DataAcquisitionThread::stop()
{
    m_running = false;
    m_dataSamples->clear();
    if (port->isOpen())
    {
        port->close();
    }
    emit notifyDAQConnected(false);
}

void DataAcquisitionThread::handleError(QSerialPort::SerialPortError error)
{
    qDebug() << error;
    if (error != QSerialPort::SerialPortError::NoError)
    {
        if (m_running == true)
        {
            if (port->isOpen())
            {
                m_running  = false;
                port->close();
                emit notifyDAQConnected(false);
            }
        }
    }
}

QList<QJsonObject> DataAcquisitionThread::getDataSamples()
{
    return *m_dataSamples;
}
