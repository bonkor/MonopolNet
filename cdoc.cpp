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
    CPlayer *p = &m_p[pNu];
    int curPos = p->pos;

    if (st == 0) return curPos;

    if (inCrest(curPos))
    {
        switch (p->crestDir) {
        case D_Left:
            if (curPos == 15)
                curPos = 49 - st;
            else {
                curPos -= st;
                if (curPos < 40) {
                    if (p->stepsBack == 0)
                        curPos = 39 - curPos + 35;
                    else
                        curPos = 35 + curPos - 39;
                }
            }
            break;
        case D_Right:
            if (curPos == 35)
                curPos = 39 + st;
            else {
                curPos += st;
                if (curPos > 48) {
                    if (p->stepsBack == 0)
                        curPos = curPos - 49 + 15;
                    else
                        curPos = 15 - curPos + 49;
                }
            }
            break;
        case D_Up:
            if (curPos == 25) {
                curPos = 57 - st;
                if (curPos == 52)
                    curPos = 44;
                else if (curPos == 51)
                    curPos = 52;
            } else if (curPos > 52 && curPos < 57) {
                curPos -= st;
                if (curPos == 52)
                    curPos = 44;
                else if (curPos > 47 && curPos < 52)
                    curPos += 1;
                else if (curPos == 47)
                    curPos = 5;
            } else if (curPos == 44) {
                curPos = 53 - st;
                if (curPos < 49) {
                    if (p->stepsBack == 0)
                        curPos = 48 - curPos + 5;
                    else
                        curPos = 5 + curPos - 48;
                }
            } else {
                curPos -= st;
                if (curPos < 49) {
                    if (p->stepsBack == 0)
                        curPos = 48 - curPos + 5;
                    else
                        curPos = 5 + curPos - 48;
                }
            }
            break;
        case D_Down:
            if (curPos == 5) {
                curPos = 48 + st;
                if (curPos == 53)
                    curPos = 44;
                else if (curPos == 54)
                    curPos = 53;
            } else if (curPos > 48 && curPos < 53) {
                curPos += st;
                if (curPos == 53)
                    curPos = 44;
                else if (curPos > 53 && curPos < 58)
                    curPos -= 1;
                else if (curPos == 58)
                    curPos = 25;
            } else if (curPos == 44) {
                curPos = 52 + st;
                if (curPos > 56) {
                    if (p->stepsBack == 0)
                        curPos = curPos - 57 + 25;
                    else
                        curPos = 25 - curPos + 57;
                }
            } else {
                curPos += st;
                if (curPos > 56) {
                    if (p->stepsBack == 0)
                        curPos = curPos - 57 + 25;
                    else
                        curPos = 25 - curPos + 57;
                }
            }
        }
    } else
    {
        QString name = p->name;
        int oldPos = curPos;
        if (p->stepsBack == 0) {
            curPos += st;
            if (curPos > 39) curPos -= 40;
            if (p->turnToStart > 0) {
                if (oldPos < 5 && curPos > 5) {
                    emit sendLog(name + tr(" свернул к старту"));
                    p->turnToStart--;
                    curPos = 49 + curPos - 6;
                    if (curPos == 53) curPos = 44;
                    p->crestDir = D_Down;
                } else if (oldPos < 15 && curPos > 15) {
                    emit sendLog(name + tr(" свернул к старту"));
                    p->turnToStart--;
                    curPos = 48 - curPos + 16;
                    p->crestDir = D_Left;
                } else if (oldPos < 25 && curPos > 25) {
                    emit sendLog(name + tr(" свернул к старту"));
                    p->turnToStart--;
                    curPos = 56 - curPos + 26;
                    if (curPos == 53) curPos = 44;
                    p->crestDir = D_Up;
                } else if (oldPos < 35 && curPos > 35) {
                    emit sendLog(name + tr(" свернул к старту"));
                    p->turnToStart--;
                    curPos = 40 + curPos - 36;
                    p->crestDir = D_Right;
                }
            }
        } else {
            curPos -= st;
            if (curPos < 0) curPos += 40;
            if (p->turnToStart > 0) {
                if (oldPos > 5 && curPos < 5) {
                    emit sendLog(name + tr(" свернул к старту"));
                    p->turnToStart--;
                    curPos = 49 - curPos + 4;
                    if (curPos == 53) curPos = 44;
                    p->crestDir = D_Down;
                } else if (oldPos > 15 && curPos < 15) {
                    emit sendLog(name + tr(" свернул к старту"));
                    p->turnToStart--;
                    curPos = 48 + curPos - 14;
                    p->crestDir = D_Left;
                } else if (oldPos > 25 && curPos < 25) {
                    emit sendLog(name + tr(" свернул к старту"));
                    p->turnToStart--;
                    curPos = 56 + curPos - 24;
                    if (curPos == 53) curPos = 44;
                    p->crestDir = D_Up;
                } else if (oldPos > 35 && curPos < 35) {
                    emit sendLog(name + tr(" свернул к старту"));
                    p->turnToStart--;
                    curPos = 40 - curPos + 34;
                    p->crestDir = D_Right;
                }
            }
        }
    }

    return curPos;
}
