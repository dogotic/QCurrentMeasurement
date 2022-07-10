#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dataacquisition.h"
#include "datarecorder.h"
#include "serialportlistpopulator.h"
#include "chart.h"

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

    void on_actionExport_Samples_as_CSV_triggered();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();

    void on_horizontalSlider_LOAD_VOLTAGE_MIN_sliderMoved(int position);

    void on_horizontalSlider_LOAD_VOLTAGE_MAX_sliderMoved(int position);

    void on_horizontalSlider_BUS_VOLTAGE_MIN_sliderMoved(int position);

    void on_horizontalSlider_BUS_VOLTAGE_MAX_sliderMoved(int position);

    void on_horizontalSlider_SHUNT_VOLTAGE_MIN_sliderMoved(int position);

    void on_horizontalSlider_SHUNT_VOLTAGE_MAX_sliderMoved(int position);

    void on_horizontalSlider_CURRENT_MIN_sliderMoved(int position);

    void on_horizontalSlider_CURRENT_MAX_sliderMoved(int position);

    void on_horizontalSlider_POWER_MIN_sliderMoved(int position);

    void on_horizontalSlider_POWER_MAX_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
    DataAcquisition *daq;
    DataRecorder *recorder;
    serialPortListPopulator *populatorThread;

    Chart *m_chart_busVoltage;
    Chart *m_chart_loadVoltage;
    Chart *m_chart_shuntVoltage;
    Chart *m_chart_current;
    Chart *m_chart_power;

    bool connected = false;
    int m_sampleCounter = 0;

signals:
    void startCommunication(QString port_name);
    void stopCommunication();
};
#endif // MAINWINDOW_H
