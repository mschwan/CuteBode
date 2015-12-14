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
    lengthXDiv = 200;
    lengthYDiv = 200;

    scene = new QGraphicsScene(this);
    this->setScene(scene);

    penMagnitude.setWidth(2);
    penMagnitude.setStyle(Qt::SolidLine);
    penAxis.setColor(QColor::fromRgb(64, 64, 64));

    this->setRenderHints(QPainter::Antialiasing);
    this->setTransform(QTransform::fromScale(1, -1));

    //connect(this->scene, SIGNAL(sceneRectChanged(QRectF))
}

void Plot::calculateMagnitude(QList<Trf *> trfList)
{
    magnitudePoints.clear();

    for(unsigned int i = fStart; i <= fStop; i++) {
        QPointF *p = new QPointF();
        p->setX(i); // omega
        p->setY(1); // empty magnitude
        magnitudePoints.append(p);
    }

    double m = 0;
    double re = 0;
    double im = 0;

    for(unsigned int i = 0; i <= fStop - fStart; i++) {
        foreach(Trf *trf, trfList) {
            if(trf->getTau() != 0) {
                switch(trf->getType()) {
                case Trf::Type1:
                    m = magnitudePoints.at(i)->x() * trf->getTau();
                    magnitudePoints.at(i)->setY(magnitudePoints.at(i)->y() * m);
                    break;
                case Trf::Type2:
                    m = sqrt(1 + pow(magnitudePoints.at(i)->x() * trf->getTau(), 2));
                    magnitudePoints.at(i)->setY(magnitudePoints.at(i)->y() * m);
                    break;
                case Trf::Type3:
                    m = 1.0 / (magnitudePoints.at(i)->x() * trf->getTau());
                    magnitudePoints.at(i)->setY(magnitudePoints.at(i)->y() * m);
                    break;
                case Trf::Type4:
                    re = 1.0 / (1 + pow(magnitudePoints.at(i)->x() * trf->getTau(), 2));
                    im = (magnitudePoints.at(i)->x() * trf->getTau()) /
                         (1 + pow(magnitudePoints.at(i)->x() * trf->getTau(), 2));
                    m = sqrt(pow(re, 2) + pow(im, 2));
                    magnitudePoints.at(i)->setY(magnitudePoints.at(i)->y() * m);
                    break;
                default:
                    break;
                }
            }
        }
    }

    toLog(magnitudePoints);

    yMin = magnitudePoints.at(0)->y();
    yMax = magnitudePoints.at(0)->y();

    for(unsigned int i = 1; i <= fStop - fStart; i++) {
        if(magnitudePoints.at(i)->y() < yMin) {
            yMin = magnitudePoints.at(i)->y();
        }

        if(magnitudePoints.at(i)->y() > yMax) {
            yMax = magnitudePoints.at(i)->y();
        }
    }
}

void Plot::calculateXAxis()
{
    xAxisPoints.clear();

    qDebug() << "--- axis";

    for(unsigned int i = 0; i < log10(fStop); i++) {
        qDebug() << i;
        QPointF *p = new QPointF(i * lengthXDiv, 0);
        xAxisPoints.append(p);
    }
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

    // magnitude

    QPainterPath pathMagnitude;
    QPolygonF polygonMagnitude; // this is the plotted line

    foreach(QPointF *p, magnitudePoints) {
        polygonMagnitude.append(QPointF(p->x(), p->y()));
    }

    pathMagnitude.addPolygon(polygonMagnitude);
    scene->addPath(pathMagnitude, penMagnitude);

    // axis


    // x axis
    for(int i = (int) (yMin / lengthYDiv) - 1;
        i <= (int) (yMax / lengthYDiv) + 1;
        i++) {
        scene->addLine(0, i * lengthYDiv,
                       lengthXDiv * (log10(fStop) - 1), i * lengthYDiv,
                       penAxis);
    }

    // y axis
    int y1 = (int) yMin - ((int) yMin % (int) lengthYDiv) - (int) lengthYDiv;
    int y2 = (int) yMax - ((int) yMax % (int) lengthYDiv) + (int) lengthYDiv;

    for(int i = 0;
        i <= 2;
        i++) {
        scene->addLine(i * lengthXDiv, y1,
                       i * lengthXDiv, y2,
                       penAxis);
    }

    qDebug() << "Y min/max:" << yMin << yMax;
    qDebug() << "y1" << y1 << "\n"
             << "y2" << y2;

    // resize scene rectangle

    this->scene->setSceneRect(-50,
                              y1 -50,
                              2 * lengthXDiv + 2*50,
                              y2 - y1 +100);
    this->scene->addRect(this->sceneRect(), QPen(QColor::fromRgb(255, 15, 0, 64), 2, Qt::DotLine));
    qDebug() << "sceneRect"
             << this->sceneRect().x() << this->sceneRect().y()
             << this->sceneRect().width() << this->sceneRect().height();

    // text
    QGraphicsTextItem *textItemFrequency = new QGraphicsTextItem();
    textItemFrequency->setPlainText("f : "
                                    + QString::number(fStart)
                                    + " ... "
                                    + QString::number(fStop)
                                    + " Hz");
    textItemFrequency->setPos(lengthXDiv, sceneRect().top() + 50);
    textItemFrequency->setTransform(QTransform::fromScale(1, -1));
    scene->addItem(textItemFrequency);

    QGraphicsTextItem *textItemMagnitude = new QGraphicsTextItem();
    textItemMagnitude->setPlainText("|H| : "
                                    + QString::number(y1 / 10)
                                    + " ... "
                                    + QString::number(y2 / 10)
                                    + " dB");
    textItemMagnitude->setPos(textItemMagnitude->boundingRect().height() / -2 - 25,
                              y1 + (y2 - y1) / 2
                              - textItemMagnitude->boundingRect().width() / 2);
    textItemMagnitude->setTransform(QTransform::fromScale(1, -1));
    textItemMagnitude->setRotation(-90);
    scene->addItem(textItemMagnitude);
}
