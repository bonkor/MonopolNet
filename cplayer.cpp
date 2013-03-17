#include "cplayer.h"

CLP::CLP()
{
    pls << 1; pls.clear();
}

void CLP::clear(void)
{
    pls.clear();
}

bool CLP::exist(void)
{
    if (pls.isEmpty())
        return false;
    else
        return true;
}

void CLP::set(quint8 p, CMoney s)
{
    pls.clear();
    pls << p;
    sum = s;
}

void CLP::set(QList<quint8> &l, CMoney s)
{
    pls = l;
    sum = s;
}

CPlayer::CPlayer(QObject *parent) :
    QObject(parent)
{
    pos = 44;
    investComplit = false;
    money = 75.;
    active = true;
    stay = false;
    canBuy = false;
    canInvest = false;
    crestDir = D_Center;   // невыбрано
    mustSellMode = false;
    mustSellQues = false;
    mustLoseQues = false;
    mustLoseMonQues = false;
    mustLoseMeson = false;

    pbp = 0;
    pbq = 0;
    turnToStart = 0;
    plusStart = 0;
    stepsBack = 0;
}

void CPlayer::addMove(int mv)
{
    if (mv < 0)
        stepsBack -= mv;
    for (int i=0; i<abs(mv); i++)
        queue << Q_Move;
}

void CPlayer::insertToQueue(QueueType q, quint8 nu)
{
    for (int i=0; i<nu; i++)
        queue.insert(0, q);
}

quint8 CPlayer::getNextAction(void)
{
    if (queue.size() == 0)
        return Q_No;
    return queue.takeFirst();
}
