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

/**
 * @brief Plot::Plot
 * @param parent
 * Konstruktor
 */
Plot::Plot(QWidget *parent) :
    QGraphicsView(parent)
{
    fStart = 1;
    fStop = 1000000;
    lengthXDiv = 200;
    lengthYDiv = 200;

    scene = new QGraphicsScene(this);
    this->setScene(scene);

    penMagnitude.setWidth(2);
    penMagnitude.setColor(QColor::fromRgb(200, 0, 0));
    penMagnitude.setStyle(Qt::SolidLine);
    penPhase.setWidth(2);
    penPhase.setColor(QColor::fromRgb(0, 0, 200));
    penPhase.setStyle(Qt::DashLine);
    penAxis.setColor(QColor::fromRgb(128, 128, 128));

    this->setRenderHints(QPainter::Antialiasing);
    // transform the whole scene: y is up, x is right
    this->setTransform(QTransform::fromScale(1, -1));

    //connect(this->scene, SIGNAL(sceneRectChanged(QRectF))
}

/**
 * @brief Plot::calculateMagnitude
 * @param trfList
 * Berechnet den Betragsfrequenzgang
 */
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

    toLog(magnitudePoints, 2);

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

void Plot::calculatePhase(QList<Trf *> trfList)
{
    phasePoints.clear();

    for(unsigned int i = fStart; i <= fStop; i++) {
        QPointF *p = new QPointF();
        p->setX(i); // omega
        p->setY(0); // empty Phase
        phasePoints.append(p);
    }

    double p = 0;
    double re = 0;
    double im = 0;

    for(unsigned int i = 0; i <= fStop - fStart; i++) {
        foreach(Trf *trf, trfList) {
            if(trf->getTau() != 0) {
                switch(trf->getType()) {
                case Trf::Type1:
                    p = atan2(phasePoints.at(i)->x() * trf->getTau(), 0);
                    phasePoints.at(i)->setY(phasePoints.at(i)->y() + p);
                    break;
                case Trf::Type2:
                    p = atan2(phasePoints.at(i)->x() * trf->getTau(), 1);
                    phasePoints.at(i)->setY(phasePoints.at(i)->y() + p);
                    break;
                case Trf::Type3:
                    im = -1.0 / (phasePoints.at(i)->x() * trf->getTau());
                    re = 0;
                    p = atan2(im, re);
                    phasePoints.at(i)->setY(phasePoints.at(i)->y() + p);
                    break;
                case Trf::Type4:
                    re = 1.0 / (pow(phasePoints.at(i)->x() * trf->getTau(), 2));
                    im = -1.0 / (phasePoints.at(i)->x() * trf->getTau());
                    p = atan2(im, re);
                    phasePoints.at(i)->setY(phasePoints.at(i)->y() + p);
                    break;
                default:
                    break;
                }
            }
        }
    }

    toLog(phasePoints, 1);
}

/**
 * @brief Plot::calculateXAxis
 * Initialisiert das Array für die Omega-Achse
 */
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

/**
 * @brief Plot::toLog
 * @param linearPoints
 * Hilfsfunktion um die lineare XAchse in den logarithmischen Bereich zu
 * verschieben.
 */
void Plot::toLog(QVector<QPointF *> linearPoints, int log = 2)
{
    if(log == 2 || log == 1) {
        foreach(QPointF *p, linearPoints) {
            double lx = p->x();
            double la = (double) fStart;
            double ld = 200.0;
            p->setX(ld * log10(lx / la));

            if(log == 2) {
                double ua = p->y();
                double ue = 1.0;
                double lu = 10.0;
                p->setY(20.0 * lu * log10(ua / ue));
            }
        }
    } else {
        qDebug() << "ERROR: toLog() got wrong int log value!";
    }
}

/**
 * @brief Plot::plot
 * - Löscht alten Plot
 * - Erstellt Achsenkreuz
 * - Malt die Funktion
 * - Fügt Beschriftung der Achsen hinzu
 */
void Plot::plot()
{
    scene->clear();
    this->viewport()->update();

    // magnitude
    QPainterPath pathMagnitude;
    QPolygonF polygonMagnitude; // this is the plotted magnitude

    foreach(QPointF *p, magnitudePoints) {
        if(p->x() >= 100 && p->x() < 1000) {
            if((int) p->x() % 5 == 0) {
                polygonMagnitude.append(QPointF(p->x(), p->y()));
            }
        } else if(p->x() >= 1000 && p->x() < 10000) {
            if((int) p->x() % 50 == 0) {
                polygonMagnitude.append(QPointF(p->x(), p->y()));
            }
        } else if(p->x() >= 10000 && p->x() < 100000) {
            if((int) p->x() % 500 == 0) {
                polygonMagnitude.append(QPointF(p->x(), p->y()));
            }
        } else if(p->x() >= 100000) {
            if((int) p->x() % 10000 == 0) {
                polygonMagnitude.append(QPointF(p->x(), p->y()));
            }
        } else { // give up optimising, just add it
            polygonMagnitude.append(QPointF(p->x(), p->y()));
        }
    }

    pathMagnitude.addPolygon(polygonMagnitude);
    scene->addPath(pathMagnitude, penMagnitude);

    // phase
    QPainterPath pathPhase;
    QPolygonF polygonPhase; // this is the plotted phase

    foreach(QPointF *p, phasePoints) {
        if(p->x() >= 100 && p->x() < 1000) {
            if((int) p->x() % 5 == 0) {
                polygonPhase.append(QPointF(p->x(), lengthYDiv * 2.0/3.1415926 * p->y()));
            }
        } else if(p->x() >= 1000 && p->x() < 10000) {
            if((int) p->x() % 50 == 0) {
                polygonPhase.append(QPointF(p->x(), lengthYDiv * 2.0/3.1415926 * p->y()));
            }
        } else if(p->x() >= 10000 && p->x() < 100000) {
            if((int) p->x() % 500 == 0) {
                polygonPhase.append(QPointF(p->x(), lengthYDiv * 2.0/3.1415926 * p->y()));
            }
        } else if(p->x() >= 100000) {
            if((int) p->x() % 10000 == 0) {
                polygonPhase.append(QPointF(p->x(), lengthYDiv * 2.0/3.1415926 * p->y()));
            }
        } else { // give up optimising, just add it
            polygonPhase.append(QPointF(p->x(), lengthYDiv * 2.0/3.1415926 * p->y()));
        }
    }

    pathPhase.addPolygon(polygonPhase);
    scene->addPath(pathPhase, penPhase);

    // x axis
    for(int i = (int) (yMin / lengthYDiv) - 1;
        i <= (int) (yMax / lengthYDiv) + 1;
        i++) {
        scene->addLine(0, i * lengthYDiv,
                       lengthXDiv * log10(fStop / fStart), i * lengthYDiv,
                       penAxis);
    }

    // y axis
    int y1 = (int) yMin - ((int) yMin % (int) lengthYDiv) - (int) lengthYDiv;
    int y2 = (int) yMax - ((int) yMax % (int) lengthYDiv) + (int) lengthYDiv;

    for(int i = 0;
        i <= log10(fStop / fStart);
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
                              log10(fStop / fStart) * lengthXDiv + 2*50,
                              y2 - y1 + 100);
    this->scene->addRect(this->sceneRect(),
                         QPen(QColor::fromRgb(255, 15, 0, 64),
                              2, Qt::DotLine));
    qDebug() << "sceneRect"
             << this->sceneRect().x() << this->sceneRect().y()
             << this->sceneRect().width() << this->sceneRect().height();

    // text
    QGraphicsTextItem *textItemFrequency = new QGraphicsTextItem();
    textItemFrequency->setPlainText("ω : "
                                    + QString::number(fStart)
                                    + " ... "
                                    + QString::number(fStop)
                                    + " 1/s");
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

    QGraphicsTextItem *textItemPhase = new QGraphicsTextItem();
    textItemPhase->setPlainText("ϕ : "
                                + QString::number(y1 / 400.0)
                                + " ... "
                                + QString::number(y2 / 400.0)
                                + " π");
    textItemPhase->setPos(textItemPhase->boundingRect().height() / -2 - 75 + sceneRect().width(),
                          y1 + (y2 - y1) / 2
                          - textItemPhase->boundingRect().width() / 2);
    textItemPhase->setTransform(QTransform::fromScale(1, -1));
    textItemPhase->setRotation(-90);
    scene->addItem(textItemPhase);
}
