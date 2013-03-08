#include "cplayer.h"

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
    crestDir = 0;   // невыбрано
    mustSellMode = false;

    pbp = 0;
    pbq = 0;
    turnToStart = 5;
    stepsBack = 0;
}

void CPlayer::addMove(int mv)
{
    if (mv < 0)
        stepsBack += mv;
    for (int i=0; i<mv; i++)
        queue << Q_Move;
}

void CPlayer::addQues(int q)
{
    for (int i=0; i<q; i++)
        queue.insert(0, Q_Ques);
}

quint8 CPlayer::getNextAction(void)
{
    if (queue.size() == 0)
        return Q_No;
    return queue.takeFirst();
}
