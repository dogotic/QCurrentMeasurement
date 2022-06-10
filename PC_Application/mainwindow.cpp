#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFileDialog>
#include <QDateTime>
#include <QSerialPortInfo>
#include <qwt.h>
#include <qwt_plot.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        ui->comboBox_SERIAL_PORTS->addItem(info.portName());
    }

    populatorThread = new serialPortListPopulator();
    populatorThread->start();

    connect(ui->actionSave_As,SIGNAL(triggered(bool)),this,SLOT(displayFileDialog()));
    connect(ui->actionExit,SIGNAL(triggered(bool)),this,SLOT(exitProgram()));

    daq_thread = new DataAcquisitionThread();
    connect(daq_thread,SIGNAL(notifyDAQConnected(bool)),this, SLOT(SetDeviceConnected(bool)));
    connect(daq_thread,SIGNAL(sendMeasurementResults(QJsonObject)), this, SLOT(ReceiveMeasurements(QJsonObject)));
    connect(this,SIGNAL(startCommunication(QString)),daq_thread,SLOT(startCommunicationOnPort(QString)));
    connect(this,SIGNAL(stopCommunication()),daq_thread,SLOT(stopCommunicationOnPort()));
    daq_thread->start();

    timer = new QTimer();
    timer->setSingleShot(false);
    timer->setInterval(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateRecordingDuration()));
}

MainWindow::~MainWindow()
{
    daq_thread->stop();
    daq_thread->terminate();

    populatorThread->stop();
    populatorThread->terminate();

    delete ui;
}

void MainWindow::SetDeviceConnected(bool status)
{
    ui->pushButton_CONNECT->setEnabled(true);

    if (status == true)
    {
        connected = true;
        ui->pushButton_CONNECT->setText("DISCONNECT");
        ui->label_CONNECTION_STATUS->setText("CONNECTED");
        ui->label_CONNECTION_STATUS->setStyleSheet("QLabel { background-color : green; color : yellow; }");
        ui->pushButton_START_MEASUREMENT->setEnabled(true);
    }
    else
    {
        connected = false;
        ui->pushButton_CONNECT->setText("CONNECT");
        ui->pushButton_START_MEASUREMENT->setEnabled(false);
        ui->label_CONNECTION_STATUS->setText("DISCONNECTED");
        ui->label_CONNECTION_STATUS->setStyleSheet("QLabel { background-color : red; color : white; }");
        ui->comboBox_SERIAL_PORTS->setEnabled(true);
    }
}

void MainWindow::ReceiveMeasurements(QJsonObject data)
{
    float busVoltage_now = data["busvoltage_now"].toDouble();
    float loadVoltage_now = data["loadvoltage_now"].toDouble();
    float shuntVoltage_now = data["shuntvoltage_now"].toDouble();
    float current_mA_now = data["current_mA_now"].toDouble();
    float power_mW_now = data["power_mW_now"].toDouble();

    float busVoltage_avg = data["busvoltage_avg"].toDouble();
    float loadVoltage_avg = data["loadvoltage_avg"].toDouble();
    float shuntVoltage_avg = data["shuntvoltage_avg"].toDouble();
    float current_mA_avg = data["current_mA_avg"].toDouble();
    float power_mW_avg = data["power_mW_avg"].toDouble();

    ui->label_BUS_VOLTAGE_NOW->setText(QString::number(busVoltage_now));
    ui->label_SHUNT_VOLTAGE_NOW->setText(QString::number(shuntVoltage_now));
    ui->label_LOAD_VOLTAGE_NOW->setText(QString::number(loadVoltage_now));
    ui->label_CURRENT_NOW->setText(QString::number(current_mA_now));
    ui->label_POWER_NOW->setText(QString::number(power_mW_now));

    ui->label_BUS_VOLTAGE_AVG->setText(QString::number(busVoltage_avg));
    ui->label_SHUNT_VOLTAGE_AVG->setText(QString::number(shuntVoltage_avg));
    ui->label_LOAD_VOLTAGE_AVG->setText(QString::number(loadVoltage_avg));
    ui->label_CURRENT_AVG->setText(QString::number(current_mA_avg));
    ui->label_POWER_AVG->setText(QString::number(power_mW_avg));
}

void MainWindow::on_pushButton_START_MEASUREMENT_clicked()
{
    if (!recording)
    {
        ui->pushButton_START_MEASUREMENT->setText("STOP MEASUREMENT RECORDING");
        ui->label_RECORDING_STATUS->setText("RECORDING IN PROGRESS");
        ui->label_RECORDING_STATUS->setStyleSheet("QLabel { background-color : green; color : yellow; }");
        daq_thread->startRecording();
        recording = true;
        recording_duration = 0;
        timer->start();
    }
    else
    {
        ui->pushButton_START_MEASUREMENT->setText("START MEASUREMENT RECORDING");
        ui->label_RECORDING_STATUS->setText("---");
        daq_thread->stopRecording();
        recording = false;
        timer->stop();
    }
}

void MainWindow::displayFileDialog()
{
    QFileDialog fileDialog;
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    QString filename = fileDialog.getSaveFileName();
    daq_thread->storeCSVFile(filename);
}

void MainWindow::updateRecordingDuration()
{
    recording_duration++;
    ui->label_RECORDING_DURATION->setStyleSheet("QLabel { background-color : green; color : yellow; }");
    ui->label_RECORDING_DURATION->setText(QDateTime::fromSecsSinceEpoch(recording_duration).toUTC().toString("hh:mm:ss"));

    QList<QJsonObject> samples = daq_thread->getDataSamples();
    ui->plotter->setDataSamples(samples);
    ui->plotter->show();
}

void MainWindow::on_pushButton_CONNECT_clicked()
{
    ui->label_CONNECTION_STATUS->setText("IN PROGRESS...");
    ui->label_CONNECTION_STATUS->setStyleSheet("QLabel { background-color : grey; color : yellow; }");

    ui->pushButton_CONNECT->setEnabled(false);
    ui->comboBox_SERIAL_PORTS->setEnabled(false);
    // ovo sranje mi treba da bi se label update-ao i gumb i comboBox....
    qApp->processEvents();
    if (!connected)
    {
        daq_thread->startCommunicationOnPort(ui->comboBox_SERIAL_PORTS->currentText());
    }
    else
    {
        daq_thread->stopCommunicationOnPort();
    }
}

void MainWindow::populateSerialPorts(QList<QSerialPortInfo> list)
{
    for (const QSerialPortInfo &info : list)
    {
        ui->comboBox_SERIAL_PORTS->addItem(info.portName());
    }
}

void MainWindow::exitProgram()
{
    QApplication::exit();
}
