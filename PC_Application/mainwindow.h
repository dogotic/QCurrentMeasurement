#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonDocument>
#include <QTimer>
#include <QList>
#include "dataacquisitionthread.h"
#include "serialportlistpopulator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    DataAcquisitionThread *daq_thread;
    serialPortListPopulator *populatorThread;
    bool recording = false;
    bool connected = false;
    QTimer *timer;
    int recording_duration = 0;

public slots:
    void SetDeviceConnected(bool status);
    void ReceiveMeasurements(QJsonObject data);

private slots:
    void on_pushButton_START_MEASUREMENT_clicked();
    void displayFileDialog();
    void updateRecordingDuration();
    void on_pushButton_CONNECT_clicked();
    void populateSerialPorts(QList<QSerialPortInfo> list);

signals:
    void startCommunication(QString port_name);
    void stopCommunication();
};
#endif // MAINWINDOW_H
