#include <math.h>
#include <qwt.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_legend.h>
#include <qwt_plot_curve.h>
#include "graphplotter.h"

GraphPlotter::GraphPlotter()
{
    setTitle("This is an Example");
}

GraphPlotter::GraphPlotter(QString title)
{
    setTitle(title);
    setCanvasBackground(Qt::white);

    setAxisScale(0,0.0,14.0,1.0);   // bus voltage
    setAxisScale(1,0.0,14.0,1.0);   // load voltage
    setAxisScale(2,0.0,1.0,0.1);   // shunt voltage
    setAxisScale(3,0.0,2000.0,1.0);   // current_mA
    setAxisScale(4,0.0,10000.0,1.0);   // power_mW

    insertLegend( new QwtLegend() );
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->attach(this);

    curve_busVoltage = new QwtPlotCurve();
    curve_busVoltage->setTitle( "Bus voltage (V) " );
    curve_busVoltage->setPen( Qt::blue, 4 ),
    curve_busVoltage->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    curve_loadVoltage = new QwtPlotCurve();
    curve_loadVoltage->setTitle( "Load voltage (V) " );
    curve_loadVoltage->setPen( Qt::red, 4 ),
    curve_loadVoltage->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    curve_shuntVoltage = new QwtPlotCurve();
    curve_shuntVoltage->setTitle( "Shunt voltage (V) " );
    curve_shuntVoltage->setPen( Qt::yellow, 4 ),
    curve_shuntVoltage->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    curve_current_mA = new QwtPlotCurve();
    curve_current_mA->setTitle( "Current (mA) " );
    curve_current_mA->setPen( Qt::green, 4 ),
    curve_current_mA->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    curve_power_mW = new QwtPlotCurve();
    curve_power_mW->setTitle( "Power (mW) " );
    curve_power_mW->setPen( Qt::black, 4 ),
    curve_power_mW->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    resize(1024,768);
}

void GraphPlotter::setDataSamples(QList<QJsonObject> samples)
{
    m_dataSamples = samples;

    QPolygonF points_shuntVoltage;
    QPolygonF points_busVoltage;
    QPolygonF points_loadVoltage;
    QPolygonF points_current;
    QPolygonF points_power;
    int point_counter = 0;


    foreach (QJsonObject obj, samples)
    {
        QPointF point_shuntVoltage(point_counter, obj["shuntvoltage"].toDouble());
        QPointF point_busVoltage(point_counter,obj["busvoltage"].toDouble());
        QPointF point_loadtVoltage(point_counter,obj["loadvoltage"].toDouble());
        QPointF point_current(point_counter,obj["current_mA"].toDouble());
        QPointF point_power(point_counter,obj["power_mW"].toDouble());

        points_shuntVoltage << point_shuntVoltage;
        points_busVoltage << point_busVoltage;
        points_loadVoltage << point_loadtVoltage;
        points_current << point_current;
        points_power << point_power;

        point_counter++;
    }

    curve_busVoltage->setSamples(points_busVoltage);
    curve_shuntVoltage->setSamples(points_shuntVoltage);
    curve_loadVoltage->setSamples(points_loadVoltage);
    curve_current_mA->setSamples(points_current);
    curve_power_mW->setSamples(points_power);

    curve_busVoltage->attach(this);
    curve_shuntVoltage->attach(this);
    curve_loadVoltage->attach(this);
    curve_current_mA->attach(this);
    curve_power_mW->attach(this);

    resize(1024,768);

    // Show the plots
    replot();
}

