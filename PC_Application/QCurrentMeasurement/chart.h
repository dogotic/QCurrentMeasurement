#ifndef CHART_H
#define CHART_H

#include <QString>
#include <QChart>
#include <QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

class Chart
{
private:
    QChart *m_chart;
    QChartView *m_chartView;
    QLineSeries *m_series;
    QString m_xAxisTitle;
    QString m_yAxisTitle;
    QVariant m_xAxisMin;
    QVariant m_xAxisMax;
    QVariant m_yAxisMin;
    QVariant m_yAxisMax;
    int m_mod = 0;

public:
    Chart(QWidget *parent = nullptr,
          QString xAxisTitle = "",
          QString yAxisTitle = "",
          QVariant xAxisMin = 0,
          QVariant xAxisMax = 0,
          QVariant yAxisMin = 0,
          QVariant yAxisMax = 0);
    ~Chart();
    void Reset();
    void Update(int x_value, double y_value);
    void Update(int x_value, int y_value);
};

#endif // CHART_H
