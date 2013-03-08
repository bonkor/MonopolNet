#include "pole.h"

CField::CField(QObject *parent)
{

}

CMezon::CMezon(QObject *parent)
{

}

CFirm::CFirm(CField *parent)
{
    owner = 4;
    cur_mz = 0;
}

CMonopol::CMonopol(QObject *parent)
{
    owner = 4;
}

quint8 CFirm::GetMultiplicator(void)
{
    quint8 multi = 1;
    foreach (CMonopol *m, listMon)
        if (m->owner != 4)
            multi *= 2;
    return multi;
}

CMoney CFirm::GetCurIncome(void)
{
    if (cur_mz != 0)
    {
        if (mz[0].type == 3) return mz[0].income*GetMultiplicator()*(cur_mz+1);
        return mz[cur_mz-1].income*GetMultiplicator();
    } else
        return binc*GetMultiplicator();
}

void CFirm::clear(void)
{
    cur_mz = 0;
}
