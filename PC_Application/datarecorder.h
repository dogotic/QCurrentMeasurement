#ifndef DATARECORDER_H
#define DATARECORDER_H

#include <QObject>
#include <QJsonArray>

class DataRecorder : public QObject
{
    Q_OBJECT
public:
    explicit DataRecorder(QObject *parent = nullptr);
    ~DataRecorder();
    void startRecording();
    void stopRecording();
    void storeCSVFile(QString filePathAndFileName);

private:
    bool m_recording = false;
    QString m_csvBuffer;

public slots:
    void ReceiveDataSamples(QJsonArray dataSamples);

signals:
    void RecordingStarted();
    void RecordingStopped();
};

#endif // DATARECORDER_H
