#ifndef GRAPHPLOTTER_H
#define GRAPHPLOTTER_H

#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QWidget>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_curve_fitter.h>

class GraphPlotter : public QwtPlot
{
    Q_OBJECT
public:
    GraphPlotter(QWidget *parent, QString title="");
    void resetPointCounter() { m_point_counter = 0; }

private:
    QwtPlotCurve *curve_busVoltage;
    QwtPlotCurve *curve_loadVoltage;
    QwtPlotCurve *curve_shuntVoltage;
    QwtPlotCurve *curve_current_mA;
    QwtPlotCurve *curve_power_mW;
    QwtWeedingCurveFitter *fitter;
    QPolygonF points_shuntVoltage;
    QPolygonF points_busVoltage;
    QPolygonF points_loadVoltage;
    QPolygonF points_current;
    QPolygonF points_power;
    int m_point_counter = 0;

public slots:
    void setDataSamples(QByteArray samples);
};

#endif // GRAPHPLOTTER_H
