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
    fStop = 500;

    scene = new QGraphicsScene(this);
    this->setScene(scene);

    pen.setWidth(2);
    pen.setCapStyle(Qt::FlatCap);
    pen.setStyle(Qt::DashLine);

    this->setRenderHints(QPainter::Antialiasing);
    this->setTransform(QTransform::fromScale(1, -1));

    for(int i = 0; i <= fStop - fStart; i++) {
        QPointF *p = new QPointF();
        p->setX(i + fStart); // omega
        p->setY(0);
        magnitude.append(p);
    }
}

void Plot::calculateMagnitude(QList<Trf *> trfList)
{
    // clear everything
    scene->clear();
    this->viewport()->update();

    foreach(QPointF *p, magnitude) {
        p->setY(0);
    }

    double m = 0;
    double re = 0;
    double im = 0;

    for(int i = 0; i <= fStop - fStart; i++) {
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
}

void Plot::plot()
{
    QPainterPath painterPath;
    QPolygonF polygon;

    foreach(QPointF *p, magnitude) {
        polygon.append(QPointF(p->x(), p->y()));
    }

    painterPath.addPolygon(polygon);
    scene->addPath(painterPath, pen);

    // IMPORTANT: stop one item before the end!
    /*for(int i = 0; i < fStop - fStart; i++) {
        scene->addLine(magnitude.at(i)->x(),
                       magnitude.at(i)->y(),
                       magnitude.at(i + 1)->x(),
                       magnitude.at(i + 1)->y(),
                       QPen(Qt::blue));
    }*/
}
