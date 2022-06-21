#include <math.h>
#include <qwt/qwt.h>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_grid.h>
#include <qwt/qwt_legend.h>
#include <qwt/qwt_plot_curve.h>
#include "graphplotter.h"

GraphPlotter::GraphPlotter(QWidget *parent, QString title)
{
    setTitle(title);
    setCanvasBackground(Qt::white);

    setAxisScale(0,-2.0,100.0,10.0);
    setAxisAutoScale(true);

    insertLegend( new QwtLegend() );
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->attach(this);

    QwtWeedingCurveFitter *fitter = new QwtWeedingCurveFitter();
    fitter->setTolerance(0.1);

    curve_busVoltage = new QwtPlotCurve();
    curve_busVoltage->setTitle( "Bus voltage (V) " );
    curve_busVoltage->setPen( Qt::blue, 4 ),
    curve_busVoltage->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve_busVoltage->setCurveFitter(fitter);
    curve_busVoltage->setCurveAttribute(QwtPlotCurve::Fitted, true);

    curve_loadVoltage = new QwtPlotCurve();
    curve_loadVoltage->setTitle( "Load voltage (V) " );
    curve_loadVoltage->setPen( Qt::red, 4 ),
    curve_loadVoltage->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve_loadVoltage->setCurveFitter(fitter);
    curve_loadVoltage->setCurveAttribute(QwtPlotCurve::Fitted, true);

    curve_shuntVoltage = new QwtPlotCurve();
    curve_shuntVoltage->setTitle( "Shunt voltage (V) " );
    curve_shuntVoltage->setPen( Qt::yellow, 4 ),
    curve_shuntVoltage->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve_shuntVoltage->setCurveFitter(fitter);
    curve_shuntVoltage->setCurveAttribute(QwtPlotCurve::Fitted, true);

    curve_current_mA = new QwtPlotCurve();
    curve_current_mA->setTitle( "Current (mA) " );
    curve_current_mA->setPen( Qt::green, 4 ),
    curve_current_mA->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve_current_mA->setCurveFitter(fitter);
    curve_current_mA->setCurveAttribute(QwtPlotCurve::Fitted, true);

    curve_power_mW = new QwtPlotCurve();
    curve_power_mW->setTitle( "Power (mW) " );
    curve_power_mW->setPen( Qt::black, 4 ),
    curve_power_mW->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve_power_mW->setCurveFitter(fitter);
    curve_power_mW->setCurveAttribute(QwtPlotCurve::Fitted, true);

}

void GraphPlotter::setDataSamples(QJsonArray samples)
{
    QPolygonF points_shuntVoltage;
    QPolygonF points_busVoltage;
    QPolygonF points_loadVoltage;
    QPolygonF points_current;
    QPolygonF points_power;

    for (int point_counter=0; point_counter < samples.count(); point_counter++)
    {
        QJsonObject obj = samples[point_counter].toObject();

        QPointF point_shuntVoltage(m_point_counter + point_counter, obj["shuntvoltage"].toDouble());
        QPointF point_busVoltage(m_point_counter + point_counter,obj["busvoltage"].toDouble());
        QPointF point_loadtVoltage(m_point_counter + point_counter,obj["loadvoltage"].toDouble());
        QPointF point_current(m_point_counter + point_counter,obj["current_mA"].toDouble());
        QPointF point_power(m_point_counter + point_counter,obj["power_mW"].toDouble());

        points_shuntVoltage << point_shuntVoltage;
        points_busVoltage << point_busVoltage;
        points_loadVoltage << point_loadtVoltage;
        points_current << point_current;
        points_power << point_power;
    }

    m_point_counter += samples.count();

    // curve_busVoltage->setSamples(points_busVoltage);
    // curve_shuntVoltage->setSamples(points_shuntVoltage);
    // curve_loadVoltage->setSamples(points_loadVoltage);
    curve_current_mA->setSamples(points_current);
    //curve_power_mW->setSamples(points_power);

    //curve_busVoltage->attach(this);
    //curve_shuntVoltage->attach(this);
    //curve_loadVoltage->attach(this);
    curve_current_mA->attach(this);
    //curve_power_mW->attach(this);

    // Show the plots
    replot();
}

