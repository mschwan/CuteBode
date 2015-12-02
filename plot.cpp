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
    fStop = 10000;

    scene = new QGraphicsScene(this);
    this->setScene(scene);

    pen.setWidth(2);
    pen.setStyle(Qt::SolidLine);

    this->setRenderHints(QPainter::Antialiasing);
    this->setTransform(QTransform::fromScale(1, -1));
}

void Plot::calculateMagnitude(QList<Trf *> trfList)
{
    magnitude.clear();

    for(unsigned int i = fStart; i <= fStop; i++) {
        QPointF *p = new QPointF();
        p->setX(i); // omega
        p->setY(0); // empty magnitude
        magnitude.append(p);
    }

    double m = 0;
    double re = 0;
    double im = 0;

    for(unsigned int i = 0; i <= fStop - fStart; i++) {
        foreach(Trf *trf, trfList) {
            switch(trf->getType()) {
            case Trf::Type1:
                m = magnitude.at(i)->x() * trf->getTau();
                magnitude.at(i)->setY(magnitude.at(i)->y() + m);
                break;
            case Trf::Type2:
                m = sqrt(1 + pow(magnitude.at(i)->x() * trf->getTau(), 2));
                magnitude.at(i)->setY(magnitude.at(i)->y() + m);
                break;
            case Trf::Type3:
                m = 1.0 / (magnitude.at(i)->x() * trf->getTau());
                magnitude.at(i)->setY(magnitude.at(i)->y() + m);
                break;
            case Trf::Type4:
                re = 1.0 / (1 + pow(magnitude.at(i)->x() * trf->getTau(), 2));
                im = (magnitude.at(i)->x() * trf->getTau()) /
                     (1 + pow(magnitude.at(i)->x() * trf->getTau(), 2));
                m = sqrt(pow(re, 2) + pow(im, 2));
                magnitude.at(i)->setY(magnitude.at(i)->y() + m);
                break;
            default:
                break;
            }
        }
    }

    toLog(magnitude);
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

    QPainterPath painterPath;
    QPolygonF polygon;

    foreach(QPointF *p, magnitude) {
        polygon.append(QPointF(p->x(), p->y()));
    }

    painterPath.addPolygon(polygon);
    scene->addPath(painterPath, pen);
}
