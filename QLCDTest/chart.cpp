#include <QLayout>
#include <QDebug>
#include "chart.h"

Chart::Chart(QWidget *parent,
             QString xAxisTitle,
             QString yAxisTitle,
             QVariant xAxisMin,
             QVariant xAxisMax,
             QVariant yAxisMin,
             QVariant yAxisMax)
{
    m_xAxisMin = xAxisMin;
    m_xAxisMax = xAxisMax;
    m_yAxisMin = yAxisMin;
    m_yAxisMax = yAxisMax;
    m_xAxisTitle = xAxisTitle;
    m_yAxisTitle = yAxisTitle;

    m_series = new QLineSeries();
    m_chart = new QChart();
    m_chart->addSeries(m_series);
    m_chart->legend()->hide();
    m_chart->createDefaultAxes();
    m_chart->axisX()->setRange(m_xAxisMin,m_xAxisMax);
    m_chart->axisX()->setTitleText(m_xAxisTitle);
    m_chart->axisY()->setRange(m_yAxisMin,m_yAxisMax);
    m_chart->axisY()->setTitleText(m_yAxisTitle);

    m_chartView = new QChartView(m_chart);
    m_chartView->chart()->setTheme(QChart::ChartThemeDark);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    parent->layout()->addWidget(m_chartView);
}

void Chart::Reset()
{
    m_mod = m_xAxisMax.toInt();
    m_chart->axisX()->setRange(m_xAxisMin,m_xAxisMax);
    m_series->clear();
}

void Chart::Update(int x_value, double y_value)
{
    m_series->append(x_value, y_value);

    if ((x_value > 0) && (x_value % m_mod == 0))
    {
        m_chart->axisX()->setRange(x_value - m_xAxisMax.toInt(), x_value);
        m_mod = 1;
    }
}
