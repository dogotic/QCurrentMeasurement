#include <math.h>
#include <qwt/qwt.h>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_grid.h>
#include <qwt/qwt_legend.h>
#include <qwt/qwt_plot_curve.h>
#include "graphplotter.h"

GraphPlotter::GraphPlotter(QWidget *parent, QString title)
{
    m_curve = new QwtPlotCurve();
}

void GraphPlotter::setDataSamples(QByteArray samples)
{
    QString input = QString(samples);
    QStringList items = input.split(",");

    qDebug() << getItemIdx()+1;
    QPointF point(m_point_counter,items[getItemIdx()+1].toDouble());
    points << point;

    m_point_counter++;

    m_curve->setSamples(points);
    //m_curve->attach(this);
    replot();
}

void GraphPlotter::setCurve(QwtPlotCurve *curve)
{
    m_curve = curve;
}

