#include "trf.h"

Trf::Trf(QObject *parent) :
    QObject(parent)
{
}

Trf::Type Trf::getType()
{
    return this->type;
}

void Trf::setType(Type t)
{
    this->type = t;
}

double Trf::getTau()
{
    return this->tau;
}

void Trf::setTau(double t)
{
    this->tau = t;
}
