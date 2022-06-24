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
    connect(daq,SIGNAL(sendDataSamples(QByteArray)), this, SLOT(ReceiveMeasurements(QByteArray)));
    connect(daq,SIGNAL(sendDataSamples(QByteArray)),ui->plotter,SLOT(setDataSamples(QByteArray)));
    connect(this,SIGNAL(startCommunication(QString)),daq,SLOT(startCommunicationOnPort(QString)));
    connect(this,SIGNAL(stopCommunication()),daq,SLOT(stopCommunicationOnPort()));

    recorder = new DataRecorder();
    connect(daq,SIGNAL(sendDataSamples(QByteArray)),recorder,SLOT(ReceiveDataSamples(QByteArray)));
    connect(recorder,SIGNAL(RecordingStarted()),this,SLOT(HandleRecordingStarted()));
    connect(recorder,SIGNAL(RecordingStopped()),this,SLOT(HandleRecordingStopped()));

    timer = new QTimer();
    timer->setSingleShot(false);
    timer->setInterval(40);
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

void MainWindow::ReceiveMeasurements(QByteArray samples)
{
    QString busVoltage;
    QString loadVoltage;
    QString shuntVoltage;
    QString current_mA;
    QString power_mW;

    QString input = QString(samples);
    QStringList items = input.split(",");

    busVoltage = items[1];
    loadVoltage = items[2];
    shuntVoltage = items[3];
    current_mA = items[4];
    power_mW = items[5];

    ui->label_BUS_VOLTAGE_NOW->setText(busVoltage);
    ui->label_SHUNT_VOLTAGE_NOW->setText(shuntVoltage);
    ui->label_LOAD_VOLTAGE_NOW->setText(loadVoltage);
    ui->label_CURRENT_NOW->setText(current_mA);
    ui->label_POWER_NOW->setText(power_mW);
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
