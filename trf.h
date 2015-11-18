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
