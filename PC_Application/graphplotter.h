#ifndef GRAPHPLOTTER_H
#define GRAPHPLOTTER_H

#include <QStringList>
#include <QJsonObject>
#include <QWidget>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_curve_fitter.h>

class GraphPlotter : public QwtPlot
{
    Q_OBJECT
public:
    GraphPlotter(QWidget *parent, QString title="");
    void setDataSamples(QList<QJsonObject> samples);

private:
    QList<QJsonObject> m_dataSamples;
    QwtPlotCurve *curve_busVoltage;
    QwtPlotCurve *curve_loadVoltage;
    QwtPlotCurve *curve_shuntVoltage;
    QwtPlotCurve *curve_current_mA;
    QwtPlotCurve *curve_power_mW;
    QwtWeedingCurveFitter *fitter;
};

#endif // GRAPHPLOTTER_H
