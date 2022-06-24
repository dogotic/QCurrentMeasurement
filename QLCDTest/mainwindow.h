#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "dataacquisition.h"
#include "datarecorder.h"
#include "serialportlistpopulator.h"
#include "graphplotter.h"

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
    void on_pushButton_2_clicked();
    void displayFileDialog();
    void updateRecordingDuration();
    void populateSerialPorts(QStringList list);
    void exitProgram();
    void HandleRecordingStopped();
    void HandleRecordingStarted();

private:
    Ui::MainWindow *ui;
    DataAcquisition *daq;
    DataRecorder *recorder;
    serialPortListPopulator *populatorThread;

    bool recording = false;
    bool connected = false;

    QTimer *timer;
    int recording_duration = 0;

signals:
    void startCommunication(QString port_name);
    void stopCommunication();
};
#endif // MAINWINDOW_H
