#include <QTimer>
#include <QList>
#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_chart_busVoltage = new Chart(ui->tab,"TIME (s)", "VOLTAGE (V)", 0, 1000, -3.0, 20.0);
    m_chart_loadVoltage = new Chart(ui->tab_2,"TIME (s)", "VOLTAGE (V)", 0, 1000, -3.0, 20.0);
    m_chart_shuntVoltage = new Chart(ui->tab_3,"TIME (s)", "VOLTAGE (mV)", 0, 1000, -300.0, 300.0);
    m_chart_current = new Chart(ui->tab_4,"TIME (s)", "CURRENT (mA)", 0, 1000, -300.0, 5000.0);
    m_chart_power = new Chart(ui->tab_5,"TIME (s)", "POWER (mW)", 0, 1000, 0.0, 20000.0);

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
        m_sampleCounter = 0;
        m_chart_busVoltage->Reset();
        m_chart_loadVoltage->Reset();
        m_chart_shuntVoltage->Reset();
        m_chart_current->Reset();
        m_chart_power->Reset();

        daq->startCommunicationOnPort(ui->comboBox_SERIAL_PORTS->currentText());
        recorder->startRecording();
    }
    else
    {
        daq->stopCommunicationOnPort();
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

    m_chart_busVoltage->Update(m_sampleCounter,busVoltage.toDouble());
    m_chart_loadVoltage->Update(m_sampleCounter,loadVoltage.toDouble());
    m_chart_shuntVoltage->Update(m_sampleCounter,shuntVoltage.toDouble());
    m_chart_current->Update(m_sampleCounter,current_mA.toDouble());
    m_chart_power->Update(m_sampleCounter,power_mW.toDouble());

    ui->lcdNumber_BUS_VOLTAGE->display(busVoltage);
    ui->lcdNumber_LOAD_VOLTAGE->display(loadVoltage);
    ui->lcdNumber_SHUNT_VOLTAGE->display(shuntVoltage);
    ui->lcdNumber_CURRENT->display(current_mA);
    ui->lcdNumber_POWER->display(power_mW);

    m_sampleCounter++;
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

void MainWindow::SetDeviceConnected(bool status)
{
    ui->pushButton->setEnabled(true);

    if (status == true)
    {
        connected = true;
        ui->pushButton->setText("STOP ACQUISITION");
    }
    else
    {
        connected = false;
        ui->pushButton->setText("START ACQUISITION");
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
