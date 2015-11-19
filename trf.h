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

#ifndef TRF_H
#define TRF_H

#include <QObject>

class Trf : public QObject
{
    Q_OBJECT

public:
    explicit Trf(QObject *parent = 0);
    enum Type {Type1, Type2, Type3, Type4};
    Trf::Type getType();
    void setType(Type t);
    double getTau();
    void setTau(double t);

private:
    Type type;
    double tau;
};

#endif // TRF_H
