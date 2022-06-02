#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonDocument>
#include "dataacquisitionthread.h"


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
    bool recording = false;

public slots:
    void SetDeviceConnected(bool status);
    void ReceiveMeasurements(QJsonObject data);

private slots:
    void on_pushButton_START_MEASUREMENT_clicked();
    void displayFileDialog();
};
#endif // MAINWINDOW_H
