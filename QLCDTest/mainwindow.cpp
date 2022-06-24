#include <QTimer>
#include <QList>
#include <QFileDialog>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>

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

    //connect(ui->actionSave_As,SIGNAL(triggered(bool)),this,SLOT(displayFileDialog()));
    //connect(ui->actionExit,SIGNAL(triggered(bool)),this,SLOT(exitProgram()));

    populatorThread = new serialPortListPopulator();
    connect(populatorThread,SIGNAL(sendPortList(QStringList)),this,SLOT(populateSerialPorts(QStringList)));
    populatorThread->start();

    daq = new DataAcquisition();
    connect(daq,SIGNAL(notifyDAQConnected(bool)),this, SLOT(SetDeviceConnected(bool)));
    connect(daq,SIGNAL(sendDataSamples(QByteArray)), this, SLOT(ReceiveMeasurements(QByteArray)));
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


void MainWindow::on_pushButton_clicked()
{

    ui->pushButton->setEnabled(false);
    ui->comboBox_SERIAL_PORTS->setEnabled(false);
    // ovo sranje mi treba da bi se label update-ao i gumb i comboBox....
    qApp->processEvents();
    if (!connected)
    {
        daq->startCommunicationOnPort(ui->comboBox_SERIAL_PORTS->currentText());
        timer->start();
    }
    else
    {
        daq->stopCommunicationOnPort();
        if (recording)
        {
            recorder->stopRecording();
        }
        timer->stop();
    }
}

void MainWindow::on_pushButton_2_clicked()
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

    ui->lcdNumber_BUS_VOLTAGE->display(busVoltage);
    ui->lcdNumber_LOAD_VOLTAGE->display(loadVoltage);
    ui->lcdNumber_SHUNT_VOLTAGE->display(shuntVoltage);
    ui->lcdNumber_CURRENT->display(current_mA);
    ui->lcdNumber_POWER->display(power_mW);
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
    ui->pushButton_2->setText("STOP MEASUREMENT RECORDING");
    recording_duration = 0;
    recording = true;
}

void MainWindow::HandleRecordingStopped()
{
    /* stop recorder if recording in progress */
    if (recording)
    {
        ui->pushButton_2->setText("START MEASUREMENT RECORDING");
        recording = false;
    }
}

void MainWindow::SetDeviceConnected(bool status)
{
    ui->pushButton->setEnabled(true);

    if (status == true)
    {
        connected = true;
        ui->pushButton->setText("STOP ACQUISITION");
        ui->pushButton_2->setEnabled(true);
    }
    else
    {
        connected = false;
        ui->pushButton->setText("START ACQUISITION");
        ui->pushButton_2->setEnabled(false);
        ui->comboBox_SERIAL_PORTS->setEnabled(true);
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
    // ui->label_RECORDING_DURATION->setStyleSheet("QLabel { background-color : green; color : yellow; }");
    // ui->label_RECORDING_DURATION->setText(QDateTime::fromSecsSinceEpoch(recording_duration).toUTC().toString("hh:mm:ss"));
    recording_duration++;
}
