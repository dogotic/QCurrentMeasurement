#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QLineSeries>

#include "dataacquisition.h"
#include "datarecorder.h"
#include "serialportlistpopulator.h"
#include "graphplotter.h"

QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void SetDeviceConnected(bool status);
    void ReceiveMeasurements(QByteArray samples);

private slots:
    void on_pushButton_clicked();
    void displayFileDialog();
    void populateSerialPorts(QStringList list);
    void exitProgram();

private:
    Ui::MainWindow *ui;
    DataAcquisition *daq;
    DataRecorder *recorder;
    serialPortListPopulator *populatorThread;

    QChart *chart_BUS_VOLTAGE;
    QLineSeries *series_BUS_VOLTAGE;

    bool connected = false;
    int m_sampleCounter = 0;
    int m_mod = 1000;

signals:
    void startCommunication(QString port_name);
    void stopCommunication();
};
#endif // MAINWINDOW_H
