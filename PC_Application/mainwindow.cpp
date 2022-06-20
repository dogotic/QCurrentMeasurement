#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFileDialog>
#include <QDateTime>
#include <QList>
#include <QSerialPortInfo>
#include <qwt/qwt.h>
#include <qwt/qwt_plot.h>

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

    connect(ui->actionSave_As,SIGNAL(triggered(bool)),this,SLOT(displayFileDialog()));
    connect(ui->actionExit,SIGNAL(triggered(bool)),this,SLOT(exitProgram()));

    populatorThread = new serialPortListPopulator();
    connect(populatorThread,SIGNAL(sendPortList(QStringList)),this,SLOT(populateSerialPorts(QStringList)));
    populatorThread->start();

    daq = new DataAcquisition();
    connect(daq,SIGNAL(notifyDAQConnected(bool)),this, SLOT(SetDeviceConnected(bool)));
    connect(daq,SIGNAL(sendDataSamples(QJsonArray)), this, SLOT(ReceiveMeasurements(QJsonArray)));
    connect(this,SIGNAL(startCommunication(QString)),daq,SLOT(startCommunicationOnPort(QString)));
    connect(this,SIGNAL(stopCommunication()),daq,SLOT(stopCommunicationOnPort()));

    recorder = new DataRecorder();
    connect(daq,SIGNAL(sendDataSamples(QJsonArray)),recorder,SLOT(ReceiveDataSamples(QJsonArray)));
    connect(recorder,SIGNAL(RecordingStarted()),this,SLOT(HandleRecordingStarted()));
    connect(recorder,SIGNAL(RecordingStopped()),this,SLOT(HandleRecordingStopped()));

    timer = new QTimer();
    timer->setSingleShot(false);
    timer->setInterval(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateRecordingDuration()));
}

MainWindow::~MainWindow()
{
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

void MainWindow::ReceiveMeasurements(QJsonArray data)
{
    double busVoltage_now = 0.0;
    double loadVoltage_now = 0.0;
    double shuntVoltage_now = 0.0;
    double current_mA_now = 0.0;
    double power_mW_now = 0.0;

    QJsonObject obj;
    for (int i=0; i<data.count(); i++)
    {
        obj = data[i].toObject();
        busVoltage_now += obj["busvoltage"].toDouble();
        loadVoltage_now += obj["loadvoltage"].toDouble();
        shuntVoltage_now += obj["shuntvoltage"].toDouble();
        current_mA_now += obj["current_mA"].toDouble();
        power_mW_now += obj["power_mW"].toDouble();
    }

    busVoltage_now /= data.count();
    loadVoltage_now /= data.count();
    shuntVoltage_now /= data.count();
    current_mA_now /= data.count();
    power_mW_now /= data.count();

    ui->label_BUS_VOLTAGE_NOW->setText(QString::number(busVoltage_now));
    ui->label_SHUNT_VOLTAGE_NOW->setText(QString::number(shuntVoltage_now));
    ui->label_LOAD_VOLTAGE_NOW->setText(QString::number(loadVoltage_now));
    ui->label_CURRENT_NOW->setText(QString::number(current_mA_now));
    ui->label_POWER_NOW->setText(QString::number(power_mW_now));
}

void MainWindow::on_pushButton_START_MEASUREMENT_clicked()
{
    if (!recording)
    {
        recorder->startRecording();
    }
    else
    {
        recorder->stopRecording();
    }
}

void MainWindow::displayFileDialog()
{
    QFileDialog fileDialog;
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    QString filename = fileDialog.getSaveFileName();
    recorder->storeCSVFile(filename);
}

void MainWindow::updateRecordingDuration()
{
    ui->label_RECORDING_DURATION->setStyleSheet("QLabel { background-color : green; color : yellow; }");
    ui->label_RECORDING_DURATION->setText(QDateTime::fromSecsSinceEpoch(recording_duration).toUTC().toString("hh:mm:ss"));
    recording_duration++;

    /*
    QJsonArray samples = daq_thread->getDataSamples();
    qDebug() << samples.count();
    ui->plotter->setDataSamples(samples);
    ui->plotter->show();
    */
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
        daq->startCommunicationOnPort(ui->comboBox_SERIAL_PORTS->currentText());
    }
    else
    {
        daq->stopCommunicationOnPort();
        if (recording)
        {
            recorder->stopRecording();
        }
    }
}

void MainWindow::populateSerialPorts(QStringList list)
{
    ui->comboBox_SERIAL_PORTS->clear();
    ui->comboBox_SERIAL_PORTS->addItems(list);
}

void MainWindow::exitProgram()
{
    QApplication::exit();
}

void MainWindow::HandleRecordingStarted()
{
    ui->pushButton_START_MEASUREMENT->setText("STOP MEASUREMENT RECORDING");
    ui->label_RECORDING_STATUS->setText("RECORDING IN PROGRESS");
    ui->label_RECORDING_STATUS->setStyleSheet("QLabel { background-color : green; color : yellow; }");
    recording_duration = 0;
    timer->start();
    recording = true;
}

void MainWindow::HandleRecordingStopped()
{
    /* stop recorder if recording in progress */
    if (recording)
    {
        ui->pushButton_START_MEASUREMENT->setText("START MEASUREMENT RECORDING");
        ui->label_RECORDING_STATUS->setText("---");
        timer->stop();
        recording = false;
    }
}
