#include <QTimer>
#include <QList>
#include <QFileDialog>
#include <qwt/qwt_legend.h>
#include <qwt/qwt_plot_grid.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    series_BUS_VOLTAGE = new QLineSeries();
    chart_BUS_VOLTAGE = new QChart();
    chart_BUS_VOLTAGE->addSeries(series_BUS_VOLTAGE);
    chart_BUS_VOLTAGE->legend()->hide();
    chart_BUS_VOLTAGE->createDefaultAxes();
    chart_BUS_VOLTAGE->axisX()->setRange(0, 1000);
    chart_BUS_VOLTAGE->axisX()->setTitleText("SECONDS (s)");
    chart_BUS_VOLTAGE->axisY()->setRange(-0.3,0.3);
    chart_BUS_VOLTAGE->axisY()->setTitleText("VOLTAGE (V)");

    QChartView *chartView = new QChartView(chart_BUS_VOLTAGE);
    chartView->chart()->setTheme(QChart::ChartThemeDark);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->tab->layout()->addWidget(chartView);

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
        m_mod = 1000;
        m_sampleCounter = 0;
        chart_BUS_VOLTAGE->axisX()->setRange(0, 1000);
        series_BUS_VOLTAGE->clear();

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

    series_BUS_VOLTAGE->append(m_sampleCounter, shuntVoltage.toDouble());

    if((m_sampleCounter > 0) && (m_sampleCounter % m_mod == 0))
    {
        chart_BUS_VOLTAGE->axisX()->setRange(m_sampleCounter - 1000, m_sampleCounter);
        m_mod = 1;
        //series_BUS_VOLTAGE->removePoints(0, 900);
    }

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
