#ifndef CMONEY_H
#define CMONEY_H

#include <QObject>
#include <QString>

class CMoney
{
public:
    CMoney(void);
    CMoney(const CMoney & m);
    CMoney(qint32 m);
    CMoney(double m);
    bool positive(void);
    friend const CMoney & operator+ (const CMoney & s1, const CMoney & s2);
    friend const CMoney & operator- (const CMoney & s1, const CMoney & s2);
    friend const CMoney & operator* (const CMoney & s1, const qint8 & s2);
    friend const CMoney & operator/ (const CMoney & s1, const qint8 & s2);
    friend const bool operator== (const CMoney & s1, const CMoney & s2);
    friend const bool operator!= (const CMoney & s1, const CMoney & s2);
    friend const bool operator>= (const CMoney & s1, const CMoney & s2);
    friend const bool operator<= (const CMoney & s1, const CMoney & s2);
    friend const bool operator> (const CMoney & s1, const CMoney & s2);
    friend const bool operator< (const CMoney & s1, const CMoney & s2);
    CMoney & operator+= (const CMoney & s);
    CMoney & operator-= (const CMoney & s);
    CMoney & operator*= (const qint8 & s);
//    CMoney operator *(qint32){return CMoney();}
    QString toString(void);
private:
    qint32 cents;

    void fromCents(qint32 c);
    void add(CMoney s);
    void minus(CMoney s);
    void mul(qint8 m);
    void div(qint8 m);
};

#endif // CMONEY_H
