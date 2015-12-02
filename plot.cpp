/*
    This file is part of CuteBode.

    CuteBode is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CuteBode is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CuteBode.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "plot.h"

Plot::Plot(QWidget *parent) :
    QGraphicsView(parent)
{
    fStart = 10;
    fStop = 1000;

    scene = new QGraphicsScene(this);
    this->setScene(scene);

    penMagnitude.setWidth(2);
    penMagnitude.setStyle(Qt::SolidLine);
    penAxis.setColor(QColor::fromRgb(64, 64, 64));

    this->setRenderHints(QPainter::Antialiasing);
    this->setTransform(QTransform::fromScale(1, -1));
}

void Plot::calculateMagnitude(QList<Trf *> trfList)
{
    magnitudePoints.clear();

    for(unsigned int i = fStart; i <= fStop; i++) {
        QPointF *p = new QPointF();
        p->setX(i); // omega
        p->setY(0); // empty magnitude
        magnitudePoints.append(p);
    }

    double m = 0;
    double re = 0;
    double im = 0;

    for(unsigned int i = 0; i <= fStop - fStart; i++) {
        foreach(Trf *trf, trfList) {
            switch(trf->getType()) {
            case Trf::Type1:
                m = magnitudePoints.at(i)->x() * trf->getTau();
                magnitudePoints.at(i)->setY(magnitudePoints.at(i)->y() + m);
                break;
            case Trf::Type2:
                m = sqrt(1 + pow(magnitudePoints.at(i)->x() * trf->getTau(), 2));
                magnitudePoints.at(i)->setY(magnitudePoints.at(i)->y() + m);
                break;
            case Trf::Type3:
                m = 1.0 / (magnitudePoints.at(i)->x() * trf->getTau());
                magnitudePoints.at(i)->setY(magnitudePoints.at(i)->y() + m);
                break;
            case Trf::Type4:
                re = 1.0 / (1 + pow(magnitudePoints.at(i)->x() * trf->getTau(), 2));
                im = (magnitudePoints.at(i)->x() * trf->getTau()) /
                     (1 + pow(magnitudePoints.at(i)->x() * trf->getTau(), 2));
                m = sqrt(pow(re, 2) + pow(im, 2));
                magnitudePoints.at(i)->setY(magnitudePoints.at(i)->y() + m);
                break;
            default:
                break;
            }
        }
    }

    toLog(magnitudePoints);
}

void Plot::calculateXAxis()
{
    xAxisPoints.clear();

    qDebug() << "--- axis";

    unsigned int i = fStart;

    while(i <= 100) {
        qDebug() << i;
        QPointF *p = new QPointF();
        p->setX(i);
        p->setY(1);
        xAxisPoints.append(p);
        i += 10;
    }

    i += 100 - 10;

    while(i <= 1000 && i > 100) {
        qDebug() << i;
        QPointF *p = new QPointF();
        p->setX(i);
        p->setY(1);
        xAxisPoints.append(p);
        i += 100;
    }

    toLog(xAxisPoints);
}

void Plot::toLog(QVector<QPointF *> linearPoints)
{
    foreach(QPointF *p, linearPoints) {
        double lx = p->x();
        double la = (double) fStart;
        double ld = 200.0;
        p->setX(ld * log10(lx / la));

        double ua = p->y();
        double ue = 1.0;
        double lu = 10.0;
        p->setY(20.0 * lu * log10(ua / ue));
    }
}

void Plot::plot()
{
    scene->clear();
    this->viewport()->update();

    QPainterPath pathMagnitude;
    QPainterPath pathXAxis;
    QPolygonF polygonMagnitude; // this is the plotted line
    QPolygonF polygonXAxis;

    foreach(QPointF *p, magnitudePoints) {
        polygonMagnitude.append(QPointF(p->x(), p->y()));
    }

    foreach(QPointF *p, xAxisPoints) {
        polygonXAxis.append(QPointF(p->x(), p->y()));
    }

    for(unsigned int i = 0; i <= 400; i += 200) {
        QGraphicsTextItem *textItem = new QGraphicsTextItem();
        textItem->setPlainText(QString::number(10 * pow(10, i / 200)));
        textItem->setPos(i, 0);
        textItem->setTransform(QTransform::fromScale(1, -1));
        scene->addItem(textItem);
    }

    pathMagnitude.addPolygon(polygonMagnitude);
    pathXAxis.addPolygon(polygonXAxis);

    scene->addPath(pathMagnitude, penMagnitude);
    scene->addPath(pathXAxis, penAxis);
}
