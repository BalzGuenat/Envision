/***********************************************************************************************************************
 * MainView.cpp
 *
 *  Created on: Jan 10, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "views/MainView.h"
#include "Scene.h"

#include "Core/headers/global.h"

#include <QtGui/QWheelEvent>
#include <QtGui/QPrinter>
#include <QtSvg/QSvgGenerator>

namespace Visualization {

MainView::MainView(Scene *scene) :
	View(scene, NULL), miniMap(new MiniMap(scene, this)), scaleLevel(SCALING_FACTOR)
{
	setRenderHint(QPainter::Antialiasing);
	setRenderHint(QPainter::TextAntialiasing);

	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

	setMiniMapSize(MINIMAP_DEFAULT_WIDTH, MINIMAP_DEFAULT_HEIGHT);
}

MainView::~MainView()
{
	SAFE_DELETE(miniMap);
}

void MainView::setMiniMapSize(int width, int height)
{
	if ( miniMap )
	{
		miniMap->resize(width, height);
		miniMap->sceneRectChanged(scene()->sceneRect());
	}
}

bool MainView::event( QEvent *event )
{
	switch (event->type())
	{
		case QEvent::KeyPress :
		{
			QKeyEvent *k = (QKeyEvent *)event;
			keyPressEvent(k);
			if (k->key() == Qt::Key_Backtab || k->key() == Qt::Key_Tab ) event->accept();
			return true;
		}
		break;
		case QEvent::KeyRelease :
		{
			QKeyEvent *k = (QKeyEvent *)event;
			keyReleaseEvent(k);
			if (k->key() == Qt::Key_Backtab || k->key() == Qt::Key_Tab ) event->accept();
			return true;
		}
		break;
		default: return View::event( event );
	}
}

void MainView::resizeEvent(QResizeEvent *event)
{
	View::resizeEvent(event);
	if ( miniMap )
	{
		miniMap->updatePosition();
		miniMap->visibleRectChanged();
	}
}

void MainView::wheelEvent(QWheelEvent *event)
{
	if ( event->delta() > 0 ) scaleLevel--;
	else scaleLevel++;

	if ( scaleLevel <= 0 ) scaleLevel = 1;
	qreal scaleFactor = SCALING_FACTOR / (qreal) scaleLevel;
	setTransform(QTransform::fromScale(scaleFactor, scaleFactor));

	if ( miniMap ) miniMap->visibleRectChanged();
}

void MainView::scrollContentsBy(int dx, int dy)
{
	View::scrollContentsBy(dx, dy);
	if ( miniMap ) miniMap->visibleRectChanged();
}

void MainView::keyPressEvent(QKeyEvent *event)
{
	if ( (event->modifiers() & Qt::ControlModifier || event->modifiers() & Qt::ShiftModifier) && event->key() == Qt::Key_Print)
	{
		event->accept();

		QPrinter printer;
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setFullPage(true);
		printer.setResolution(300);

		QSvgGenerator svggen;
		svggen.setResolution(90);



		if (event->modifiers() & Qt::ShiftModifier)
		{
			// Print scene
			printer.setOutputFileName("screenshot-scene.pdf");
			printer.setPaperSize(scene()->sceneRect().size().toSize(), QPrinter::Point);
			QPainter painter(&printer);
			painter.setRenderHint(QPainter::Antialiasing);
			scene()->render( &painter );

			svggen.setFileName("screenshot-scene.svg");
			svggen.setSize(scene()->sceneRect().size().toSize());
			QPainter svgPainter(&svggen);
			svgPainter.setRenderHint(QPainter::Antialiasing);
			scene()->render(&svgPainter);

			QImage image(2 * scene()->sceneRect().size().toSize(), QImage::Format_ARGB32);
			image.fill(Qt::transparent);
			QPainter pmapPainter(&image);
			pmapPainter.setRenderHint(QPainter::Antialiasing);
			//pmapPainter.scale(2,2);
			scene()->render(&pmapPainter);
			image.save("screenshot-scene.png");
		}
		else
		{
			// Print view
			printer.setOutputFileName("screenshot-view.pdf");
			printer.setPaperSize(viewport()->rect().size(), QPrinter::Point);
			QPainter painter(&printer);
			painter.setRenderHint(QPainter::Antialiasing);
			render(&painter);

			svggen.setFileName("screenshot-view.svg");
			svggen.setSize(viewport()->rect().size());
			QPainter svgPainter(&svggen);
			svgPainter.setRenderHint(QPainter::Antialiasing);
			render(&svgPainter);

			QImage image(2 * viewport()->rect().size(), QImage::Format_ARGB32);
			image.fill(Qt::transparent);
			QPainter pmapPainter(&image);
			pmapPainter.setRenderHint(QPainter::Antialiasing);
			render(&pmapPainter);
			image.save("screenshot-view.png");
		}
	}
	else View::keyPressEvent(event);
}

}
