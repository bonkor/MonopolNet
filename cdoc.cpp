#include "cdoc.h"

CPlayer *CDoc::getCurPlayer(void)
{
    return &m_p[curPl];
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
