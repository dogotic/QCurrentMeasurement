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
    void setItemIdx(int idx) { m_item_idx = idx; }
    int getItemIdx() { return m_item_idx; }
    void setCurve(QwtPlotCurve *curve);

private:
    QwtPlotCurve *m_curve;
    QwtWeedingCurveFitter *fitter;
    QPolygonF points;
    int m_point_counter = 0;
    int m_item_idx = 0;

public slots:
    void setDataSamples(QByteArray samples);
};

#endif // GRAPHPLOTTER_H
