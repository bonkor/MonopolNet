#include "cdoc.h"

CDoc::CDoc(QObject *parent)
{
    nu_Players = 4;
    curPl = 0;

    nu_Monopols = 0;
}

CPlayer *CDoc::getCurPlayer(void)
{
    return &m_p[curPl];
}

quint8 CDoc::getActivePlayers(void)
{
    quint8 sum = 0;
    for (quint8 i=0; i<nu_Players; i++)
        if (m_p[i].active)
            sum++;
    return sum;
}

quint8 CDoc::getNextPlayer(int skip)
{
    quint8 ap = getActivePlayers();
    skip %= ap;

    quint8 cp = curPl;
    for (quint8 i=0; i<skip; i++) {
        do {
            cp++;
            if (cp == nu_Players)
                cp = 0;
        } while (! m_p[cp].active);
    }
    return cp;
}

quint8 CDoc::playersAtPoleExept(quint8 pNu, quint8 pos)
{
    quint8 sum = 0;
    for (quint8 i=0; i<nu_Players; i++)
        if (i != pNu && m_p[pNu].active && m_p[pNu].pos == pos)
            sum++;
    return sum;
}

quint8 CDoc::playerOwnerCount(quint8 pNu)
{
    return m_p[pNu].hash.size();
}

quint8 CDoc::playerMonCount(quint8 pNu)
{
    return m_p[pNu].listMon.size();
}

CMoney CDoc::playerSumCount(quint8 pNu)
{
    CPlayer *p = &m_p[pNu];
    CMoney sum = 0;

    foreach (quint8 fn, p->hash.keys()) {
        sum += p->hash.value(fn)->price;
    }
    return sum;
}

CMoney CDoc::playerCapital(quint8 pNu)
{
    return m_p[pNu].money + playerSumCount(pNu);
}

CFirm *CDoc::getFirm(quint8 fNu)
{
    return &m_f[fNu];
}

quint8 CDoc::getDir(void)
{
    CPlayer *p = &m_p[curPl];
    quint8 pos = p->pos;
    if (inCrest(pos))
        return p->crestDir;
    else {
        if (p->stepsBack == 0) {
            if (pos >= 0 && pos < 10)
                return D_Right;
            else if (pos >= 10 && pos < 20)
                return D_Down;
            else if (pos >= 20 && pos < 30)
                return D_Left;
            else if (pos >= 30 && pos < 40)
                return D_Up;
        } else {
            if (pos > 0 && pos <= 10)
                return D_Left;
            else if (pos > 10 && pos <= 20)
                return D_Up;
            else if (pos > 20 && pos <= 30)
                return D_Right;
            else if (pos == 0 || (pos > 30 && pos <= 40))
                return D_Down;
        }
    }
}

bool CDoc::inCrest(quint8 pos)
{
    if (pos >= 40 || pos == 5 || pos == 15 || pos == 25 || pos == 35)
        return true;
    else
        return false;
}

bool CDoc::setCurPlayer(int pNu)
{
    if (pNu >= nu_Players || ! m_p[pNu].active)
        return false;

    curPl = pNu;
    return true;
}

void CDoc::clearLastPay(quint8 pNu)
{
    m_p[pNu].lastPay.clear();
}

quint8 CDoc::calculateNextPos(quint8 pNu, quint8 st)
{

}
