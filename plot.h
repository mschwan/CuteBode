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

#ifndef PLOT_H
#define PLOT_H

#include <QGraphicsView>
#include <cmath>

#include "trf.h"

#include <QDebug>

class Plot : public QGraphicsView
{
    Q_OBJECT

public:
    explicit Plot(QWidget *parent = 0);
    void calculateMagnitude(QList<Trf *> trfList);
    void plot();
    QVector<QPointF *> magnitude;

private:
    QGraphicsScene *scene;
    //int fStart = 1;
    //int fStop = 10;
};

#endif // PLOT_H
