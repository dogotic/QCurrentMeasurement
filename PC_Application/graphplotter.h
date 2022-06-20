#ifndef GRAPHPLOTTER_H
#define GRAPHPLOTTER_H

#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QWidget>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_curve_fitter.h>

class GraphPlotter : public QwtPlot
{
    Q_OBJECT
public:
    GraphPlotter(QWidget *parent, QString title="");
    void setDataSamples(QJsonArray samples);
    void resetPointCounter() { m_point_counter = 0; }

private:
    QList<QJsonObject> m_dataSamples;
    QwtPlotCurve *curve_busVoltage;
    QwtPlotCurve *curve_loadVoltage;
    QwtPlotCurve *curve_shuntVoltage;
    QwtPlotCurve *curve_current_mA;
    QwtPlotCurve *curve_power_mW;
    QwtWeedingCurveFitter *fitter;
    int m_point_counter = 0;
};

#endif // GRAPHPLOTTER_H
