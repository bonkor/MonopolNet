#include <QDebug>
#include "cmoney.h"

CMoney::CMoney(void)
{
    cents = 0;
}

CMoney::CMoney(const CMoney & m)
{
    cents = m.cents;
}

CMoney::CMoney(qint32 m)
{
    cents = m * 100;
}

CMoney::CMoney(double m)
{
    if (m >= 0)
        cents = quint32(m * 100. + .5);
    else
        cents = quint32(m * 100. - .5);
}

QString CMoney::toString(void)
{
    qint32 d = cents / 100;
    qint32 c = cents % 100;
    QString s;
    s.setNum(abs(d));
    if (cents < 0)
        s = "-" + s;
    if (c != 0) {
        QString s1;
        s1.setNum(abs(c));
        if (s1.at(1) == '0')
            s1.chop(1);
        s += "." + s1;
    }
    return s;
}

void CMoney::fromCents(qint32 c)
{
    cents = c;
}

bool CMoney::positive(void)
{
    return (cents >= 0);
}

void CMoney::add(CMoney s)
{
    cents += s.cents;
}

void CMoney::minus(CMoney s)
{
    cents -= s.cents;
}

void CMoney::mul(qint8 m)
{
    cents *= m;
}

void CMoney::div(qint8 m)
{
    cents /= m;
}

CMoney & CMoney::operator+= (const CMoney & s)
{
    add(s);
}

CMoney & CMoney::operator-= (const CMoney & s)
{
    minus(s);
}

CMoney & CMoney::operator*= (const qint8 & s)
{
    mul(s);
}

const CMoney & operator+ (const CMoney & s1, const CMoney & s2)
{
    CMoney s;
    s.add(s1);
    s.add(s2);
    return CMoney(s);
}

const CMoney & operator- (const CMoney & s1, const CMoney & s2)
{
    CMoney s;
    s.add(s1);
    s.minus(s2);
    return CMoney(s);
}

const CMoney & operator* (const CMoney & s1, const qint8 & m)
{
    CMoney s;
    s.add(s1);
    s.mul(m);
    return CMoney(s);
}

const CMoney & operator/ (const CMoney & s1, const qint8 & m)
{
    CMoney s;
    s.add(s1);
    s.div(m);
    return CMoney(s);
}

const bool operator== (const CMoney & s1, const CMoney & s2)
{
    return s1.cents == s2.cents;
}

const bool operator!= (const CMoney & s1, const CMoney & s2)
{
    return s1.cents != s2.cents;
}

const bool operator>= (const CMoney & s1, const CMoney & s2)
{
    return s1.cents >= s2.cents;
}

const bool operator<= (const CMoney & s1, const CMoney & s2)
{
    return s1.cents <= s2.cents;
}

const bool operator> (const CMoney & s1, const CMoney & s2)
{
    return s1.cents > s2.cents;
}

const bool operator< (const CMoney & s1, const CMoney & s2)
{
    return s1.cents < s2.cents;
}


