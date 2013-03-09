#include "cdoc.h"

CDoc::CDoc(QObject *parent)
{
    nu_Players = 4;
    curPl = 0;

    nu_Monopols = 28;
    FillFields();
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
        if (i != pNu && m_p[i].active && m_p[i].pos == pos)
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

CMoney CDoc::giveToBank(quint8 pNu, CMoney sum)
{
    CPlayer *p = &m_p[pNu];
    QString plName = p->name;

    if (p->seq == 0) {
        CMoney cap = playerCapital(pNu);
        if (sum <= cap) {
            p->money -= sum;
            emit sendLog(plName + tr(" платит ") + sum.toString());
            if (! p->money.positive())
                p->mustSellMode = true;
            return sum;
        } else {
            p->money += cap;
            emit sendLog(plName + tr(" платит только ") + cap.toString());
            sellAll(pNu);
            p->active = false;
            emit sendLog(plName + tr(" выходит из игры"));
            return cap;
        }
    } else {
        emit sendLog(plName + tr(" не платит ") + sum.toString() + tr(" из-за секвестра"));
        return sum;
    }
}

void CDoc::takeFromBank(quint8 pNu, CMoney sum)
{
    CPlayer *p = &m_p[pNu];
    QString plName = p->name;

    if (p->seq == 0) {
        p->money += sum;
        emit sendLog(plName + tr(" получает ") + sum.toString());
    } else
        emit sendLog(plName + tr(" не получает ") + sum.toString() + tr(" из-за секвестра"));
}

CMoney CDoc::transferMoney(quint8 fromPl, quint8 toPl, CMoney sum)
{
    CPlayer *fp = &m_p[fromPl];
    CPlayer *tp = &m_p[toPl];
    QString fromPlName = fp->name;
    QString toPlName = tp->name;

    if (fp->seq > 0) {
        emit sendLog(fromPlName + tr(" не платит ") + toPlName + sum.toString() + tr(" из-за секвестра"));
        return sum;
    }
    if (tp->seq > 0) {
        emit sendLog(toPlName + tr(" не получает от ") + fromPlName + tr(" ") + sum.toString() + tr(" из-за секвестра"));
        return sum;
    }

    CMoney cap = playerCapital(fromPl);
    if (sum <= cap) {
        fp->money -= sum;
        tp->money += sum;
        emit sendLog(fromPlName + tr(" платит ") + toPlName + tr(" ") + sum.toString());
        if (! fp->money.positive())
            fp->mustSellMode = true;
        return sum;
    } else {
        fp->money -= cap;
        tp->money += cap;
        emit sendLog(fromPlName + tr(" платит ")+ toPlName + tr(" только ") + cap.toString());
        sellAll(fromPl);
        fp->active = false;
        emit sendLog(fromPlName + tr(" выходит из игры"));
        return cap;
    }
}

void CDoc::sellAll(quint8 pNu)
{
    CPlayer *p = &m_p[pNu];

    foreach (quint8 fn, p->hash.keys()) {
        sellFirm(pNu, fn);
    }
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

bool CDoc::plusStart(quint8 oldPos, quint8 newPos)
{
    if (! inCrest(newPos)) return false;
    if (newPos == 44) return true;
    if (((newPos >= 44) && (newPos <= 48) || (newPos == 15)) &&
        (((oldPos < 44) && (oldPos >= 40)) || ((oldPos >= 34) && (oldPos <= 36)))) return true;
    if (((newPos <= 44) && (newPos >= 40) || (newPos == 35)) &&
        (((oldPos > 44) && (oldPos <= 48)) || ((oldPos >= 14) && (oldPos <= 16)))) return true;
    if (((newPos >= 53) && (newPos <= 56) || (newPos == 25) || (newPos == 44)) &&
        (((oldPos <= 52) && (oldPos >= 49)) || ((oldPos >= 4) && (oldPos <= 6)))) return true;
    if (((newPos <= 52) && (newPos >= 49) || (newPos == 5) || (newPos == 44)) &&
        (((oldPos >= 53) && (oldPos <= 56)) || ((oldPos >= 24) && (oldPos <= 26)))) return true;
    return false;
}

bool CDoc::go(quint8 pNu, quint8 st, int pos)
{
    CPlayer *p = &m_p[pNu];
    QString plName = p->name;

//    if (canTake(pNu, pos))
//        takeFirm(pNu, pos);

    quint8 oldPos = p->pos;
    quint8 newPos;
    if (pos == -1)
        newPos = calculateNextPos(pNu, st);
    else
        newPos = pos;

    qDebug() << "go" << pNu << "-" << st << "-" << pos << " || " << oldPos << "-" << newPos;
    p->pos = newPos;
    p->stay = false;

    if (p->stepsBack == 1) {
        switch (p->crestDir) {
        case D_Down:
            p->crestDir = D_Up;
            break;
        case D_Up:
            p->crestDir = D_Down;
            break;
        case D_Left:
            p->crestDir = D_Right;
            break;
        case D_Right:
            p->crestDir = D_Left;
            break;
        }
    }
    if (p->stepsBack > 0)
        p->stepsBack--;

    switch (newPos) {
    case 5:
        p->crestDir = D_Down;
        break;
    case 15:
        p->crestDir = D_Left;
        break;
    case 25:
        p->crestDir = D_Up;
        break;
    case 35:
        p->crestDir = D_Right;
        break;
    case 44:
        p->crestDir = D_Center;
        break;
    }

    CFirm *f = &m_f[newPos];
    quint8 t = f->m_type;

    if (plusStart(oldPos, newPos)) {
        if (p->seq == 0) {
            emit sendLog(plName + tr(" получает 25 за проход через СТАРТ"));
//            takeFromBank(pNu, 25);
            p->money += 25;
        } else {
            emit sendLog(plName + tr(" не получает 25 за проход через СТАРТ из-за секвестра"));
        }
    }

    bool res = true;
    switch (t) {
    case F_Firm:
        {
            QString fName = f->name;
            if (pos == -1 && st == 0)
                emit sendLog(plName + tr(" остается на ") + fName);
            else
                emit sendLog(plName + tr(" идет на ") + fName);
            quint8 ow = f->owner;
            if (ow != 4 && ow != pNu) {
                CMoney inc = f->GetCurIncome();
                if (transferMoney(pNu, ow, inc) != inc)
                    res = false;
            }
        }
        break;
    case F_3Ques:
        if (pos == -1 && st == 0)
            emit sendLog(plName + tr(" остается на Вопросах"));
        else
            emit sendLog(plName + tr(" идет на Вопросы"));
        break;
    case F_Ques:
        if (pos == -1 && st == 0)
            emit sendLog(plName + tr(" остается на вопросе"));
        else
            emit sendLog(plName + tr(" идет на вопрос"));
        break;
    case F_Birga:
        if (pos == -1 && st == 0)
            emit sendLog(plName + tr(" остается на Бирже"));
        else
            emit sendLog(plName + tr(" идет на Биржу"));
        if (giveToBank(pNu, 10) != 10)
            res = false;
        break;
    case F_Pip:
        if (pos == -1 && st == 0)
            emit sendLog(plName + tr(" остается на пипке"));
        else
            emit sendLog(plName + tr(" идет на пипку"));
        break;
    case F_Taxi:
        if (pos == -1 && st == 0)
            emit sendLog(plName + tr(" остается в Такси"));
        else
            emit sendLog(plName + tr(" идет в Такси"));
        p->stay = true;
        break;
    case F_Turma:
        if (pos == -1 && st == 0)
            emit sendLog(plName + tr(" остается в Тюрьме"));
        else
            emit sendLog(plName + tr(" садится в Тюрьму"));
        p->stay = true;
        break;
    case F_Start:
        if (pos == -1 && st == 0)
            emit sendLog(plName + tr(" остается на СТАРТе"));
        else
            emit sendLog(plName + tr(" идет на СТАРТ"));
        break;
    }

    p->investComplit = false;
    return res;
}

bool CDoc::canBuy(quint8 player, quint8 fNu)
{
    CPlayer *p = &m_p[player];
    CFirm *f = &m_f[fNu];

    if (f->m_type != F_Firm)
        return false;
    if (f->owner != 4)
        return false;
    if (p->investComplit)
        return false;
    if (playersAtPoleExept(player, fNu) != 0)
        return false;
    if (p->seq > 0)
        return false;
    if (p->pos != fNu && p->pos != 20 && ! p->canBuy)
        return false;
    if (p->money < f->price)
        return false;
    if (f->type == 1 && playerOwnerCount(player) == 0)
        return false;
    if (f->type == 2 && playerMonCount(player) == 0)
        return false;

    return true;
}

bool CDoc::canSell(quint8 player, quint8 fNu)
{
    CPlayer *p = &m_p[player];
    CFirm *f = &m_f[fNu];

    if (f->m_type != F_Firm)
        return false;
    if (f->owner != player)
        return false;
    if (p->seq > 0)
        return false;

    return true;
}

bool CDoc::canInvest(quint8 player, quint8 fNu)
{
    CPlayer *p = &m_p[player];
    CFirm *f = &m_f[fNu];

    if (f->m_type != F_Firm)
        return false;
    if (f->owner != player)
        return false;
    if (playersAtPoleExept(player, fNu) != 0)
        return false;
    if (p->seq > 0)
        return false;
    if (p->pos != fNu && ! p->canInvest)
        return false;
    if (p->investComplit)
        return false;
    CMezon *mz0 = &f->mz[0];
    if (mz0->type == 3) {
        if (p->money < mz0->invest)
            return false;
    } else {
        if (f->cur_mz == f->m_nu)
            return false;
        CMezon *mz = &f->mz[f->cur_mz];
        if (p->money < mz->invest)
            return false;
        if (mz->type == 1 && playerOwnerCount(player) == 0)
            return false;
        if (mz->type == 2 && playerMonCount(player) == 0)
            return false;
    }

    return true;
}

bool CDoc::canTake(quint8 player, quint8 fNu)
{
    CPlayer *p = &m_p[player];
    CFirm *f = &m_f[fNu];

    if (f->m_type != F_Firm)
        return false;
    if (f->owner != player)
        return false;
    if (p->pos != fNu)
        return false;
    if (p->seq > 0)
        return false;
    if (p->investComplit)
        return false;

    return true;
}

bool CDoc::buyFirm(int newOwner, int fNu, int flNu)
{
    CPlayer *p = &m_p[newOwner];
    CFirm *f = &m_f[fNu];
    QString plName = p->name;
    QString fName = f->name;

    if (! canBuy(newOwner, fNu)) {
        emit sendLog(plName + tr(" не может купить ") + fName);
        return false;
    }
    if (f->type == 1 && (m_f[flNu].m_type != F_Firm || m_f[flNu].owner != newOwner)) {
        emit sendLog(plName + tr(" не может купить ") + fName);
        return false;
    }
    if (f->type == 2 && (m_f[flNu].m_type != F_Firm || m_f[flNu].owner != newOwner || m_f[flNu].GetMultiplicator() == 1)) {
        emit sendLog(plName + tr(" не может купить ") + fName);
        return false;
    }

    p->money -= f->price;
    f->owner = newOwner;
    p->investComplit = true;
    if (f->type == 1 || f->type == 2) {
        loseFirm(newOwner, flNu);
        emit sendLog(plName + tr(" покупает ") + fName + tr(" и отказывается от ") + m_f[flNu].name);
    } else
        emit sendLog(plName + tr(" покупает ") + fName);

    p->hash.insert(fNu, f);

    foreach (CMonopol *mn, f->listMon) {
        bool res = true;
        foreach (CFirm *fm, mn->list) {
            qDebug() << mn->name << fm->name <<fm->owner << newOwner << res;
            if (fm->owner != newOwner)
                res = false;
        }
        if (res) {
            mn->owner = newOwner;
            p->listMon.append(mn);
            emit sendLog(plName + tr(" приобретает контроль над монополией ") + mn->name);
        }
    }
    return true;
}

bool CDoc::sellFirm(int oldOwner, int fNu)
{
    CPlayer *p = &m_p[oldOwner];
    CFirm *f = &m_f[fNu];
    QString plName = p->name;
    QString fName = f->name;

    if (! canSell(oldOwner, fNu)) {
        emit sendLog(plName + tr(" не может продать ") + fName);
        return false;
    }

    p->money += f->price;
    loseFirm(oldOwner, fNu);
    emit sendLog(plName + tr(" продает ") + fName);

    return true;
}

bool CDoc::loseFirm(int oldOwner, int fNu)
{
    CPlayer *p = &m_p[oldOwner];
    CFirm *f = &m_f[fNu];

    if (f->m_type != F_Firm)
        return false;
    if (f->owner != oldOwner)
        return false;

    f->owner = 4;
    f->cur_mz = 0;

    p->hash.remove(fNu);

    foreach (CMonopol *mn, f->listMon) {
        if (mn->owner == oldOwner) {
            mn->owner = 4;
            p->listMon.removeAll(mn);
            emit sendLog(p->name + tr(" теряет контроль над монополией ") + mn->name);
        }
    }
    return true;
}

bool CDoc::investFirm(int owner, int fNu, int flNu)
{
    CPlayer *p = &m_p[owner];
    CFirm *f = &m_f[fNu];
    QString plName = p->name;
    QString fName = f->name;

    if (! canInvest(owner, fNu)) {
        emit sendLog(plName + tr(" не может поставить мезон на ") + fName);
        return false;
    }

    CMezon *mz0 = &f->mz[0];
    if (mz0->type == 3) {
        p->money -= mz0->invest;
        f->cur_mz++;
        p->investComplit = true;
        emit sendLog(plName + tr(" не ставит мезон на ") + fName + tr(" за ") + mz0->invest.toString());
    } else {
        CMezon *mz = &f->mz[f->cur_mz];
        if (mz->type == 1 && (m_f[flNu].m_type != F_Firm || m_f[flNu].owner != owner)) {
            emit sendLog(plName + tr(" не может поставить мезон на ") + fName);
            return false;
        }
        if (mz->type == 2 && (m_f[flNu].m_type != F_Firm || m_f[flNu].owner != owner || m_f[flNu].GetMultiplicator() == 1)) {
            emit sendLog(plName + tr(" не может поставить мезон на ") + fName);
            return false;
        }
        p->money -= mz->invest;
        f->cur_mz++;
        p->investComplit = true;
        if (mz->type == 1 || mz->type == 2) {
            loseFirm(owner, flNu);
            emit sendLog(plName + tr(" ставит мезон на ") + fName + tr(" за ") + mz->invest.toString() + tr(" и отказывается от ") + m_f[flNu].name);
        } else
            emit sendLog(plName + tr(" ставит мезон на ") + fName + tr(" за ") + mz->invest.toString());
    }
    return true;
}

bool CDoc::takeFirm(int owner, int fNu)
{
    CPlayer *p = &m_p[owner];
    CFirm *f = &m_f[fNu];
    QString plName = p->name;
    QString fName = f->name;

    if (! canTake(owner, fNu)) {
        emit sendLog(plName + tr(" не может получить деньги с ") + fName);
        return false;
    }

    CMoney inc = f->GetCurIncome();
    p->money += inc;
    p->investComplit = true;
    emit sendLog(plName + tr(" получает ") + inc.toString() + tr(" с ") + fName);

    return true;
}

void CDoc::FillFields()
{
    for (quint8 i=0; i<57; i++) {
        m_f[i].m_type = F_Firm;
        m_f[i].type = 0;
    }

    m_f[0].m_type = F_3Ques;
    m_f[3].m_type = F_Ques;
    m_f[13].m_type = F_Ques;
    m_f[23].m_type = F_Ques;
    m_f[33].m_type = F_Ques;
    m_f[8].m_type = F_Pip;
    m_f[18].m_type = F_Pip;
    m_f[28].m_type = F_Pip;
    m_f[38].m_type = F_Pip;
    m_f[10].m_type = F_Taxi;
    m_f[20].m_type = F_Birga;
    m_f[30].m_type = F_Turma;
    m_f[44].m_type = F_Start;

    m_f[1].name = tr("ESSO");
    m_f[1].m_pict = P_NEF;
    m_f[1].m_flag = F_USA;
    m_f[1].price = 20;
    m_f[1].binc = 2;
    m_f[1].m_nu = 4;
    m_f[1].mz[0].invest = 15;
    m_f[1].mz[0].income = 6;
    m_f[1].mz[1].invest = 30;
    m_f[1].mz[1].income = 20;
    m_f[1].mz[2].invest = 50;
    m_f[1].mz[2].income = 30;
    m_f[1].mz[3].invest = 35;
    m_f[1].mz[3].income = 40;

    m_f[2].name = tr("British Petrolium");
    m_f[2].m_pict = P_NEF;
    m_f[2].m_flag = F_ENG;
    m_f[2].price = 25;
    m_f[2].binc = 2.5;
    m_f[2].m_nu = 4;
    m_f[2].mz[0].invest = 20;
    m_f[2].mz[0].income = 8;
    m_f[2].mz[1].invest = 45;
    m_f[2].mz[1].income = 25;
    m_f[2].mz[2].invest = 50;
    m_f[2].mz[2].income = 35;
    m_f[2].mz[3].invest = 40;
    m_f[2].mz[3].income = 55;

    m_f[4].name = tr("Shell");
    m_f[4].m_pict = P_NEF;
    m_f[4].m_flag = F_HEL;
    m_f[4].price = 30;
    m_f[4].binc = 3;
    m_f[4].m_nu = 4;
    m_f[4].mz[0].invest = 25;
    m_f[4].mz[0].income = 10;
    m_f[4].mz[1].invest = 40;
    m_f[4].mz[1].income = 30;
    m_f[4].mz[2].invest = 60;
    m_f[4].mz[2].income = 40;
    m_f[4].mz[3].invest = 55;
    m_f[4].mz[3].income = 60;

    m_f[5].name = tr("Yokogama");
    m_f[5].m_pict = P_POR;
    m_f[5].m_flag = F_JAP;
    m_f[5].price = 20;
    m_f[5].binc = 5;
    m_f[5].m_nu = 1;
    m_f[5].mz[0].invest = 10;
    m_f[5].mz[0].income = 10;

    m_f[6].name = tr("Борба");
    m_f[6].m_pict = P_RAD;
    m_f[6].m_flag = F_BLK;
    m_f[6].price = 25;
    m_f[6].binc = 2.5;
    m_f[6].m_nu = 4;
    m_f[6].mz[0].invest = 13;
    m_f[6].mz[0].income = 7;
    m_f[6].mz[1].invest = 30;
    m_f[6].mz[1].income = 25;
    m_f[6].mz[2].invest = 40;
    m_f[6].mz[2].income = 35;
    m_f[6].mz[3].invest = 35;
    m_f[6].mz[3].income = 50;

    m_f[7].name = tr("BBC");
    m_f[7].m_pict = P_RAD;
    m_f[7].m_flag = F_ENG;
    m_f[7].price = 30;
    m_f[7].binc = 3;
    m_f[7].m_nu = 4;
    m_f[7].mz[0].invest = 17;
    m_f[7].mz[0].income = 10;
    m_f[7].mz[1].invest = 35;
    m_f[7].mz[1].income = 25;
    m_f[7].mz[2].invest = 50;
    m_f[7].mz[2].income = 40;
    m_f[7].mz[3].invest = 40;
    m_f[7].mz[3].income = 60;

    m_f[9].name = tr("Маяк");
    m_f[9].m_pict = P_RAD;
    m_f[9].m_flag = F_URS;
    m_f[9].price = 35;
    m_f[9].binc = 3.5;
    m_f[9].m_nu = 4;
    m_f[9].mz[0].invest = 21;
    m_f[9].mz[0].income = 16;
    m_f[9].mz[1].invest = 40;
    m_f[9].mz[1].income = 35;
    m_f[9].mz[2].invest = 60;
    m_f[9].mz[2].income = 45;
    m_f[9].mz[3].invest = 55;
    m_f[9].mz[3].income = 70;

    m_f[11].name = tr("Bulgarconserv");
    m_f[11].m_pict = P_PRD;
    m_f[11].m_flag = F_BLK;
    m_f[11].price = 8;
    m_f[11].binc = .8;
    m_f[11].m_nu = 4;
    m_f[11].mz[0].invest = 4;
    m_f[11].mz[0].income = 2;
    m_f[11].mz[1].invest = 4;
    m_f[11].mz[1].income = 4;
    m_f[11].mz[2].invest = 8;
    m_f[11].mz[2].income = 8;
    m_f[11].mz[3].invest = 8;
    m_f[11].mz[3].income = 16;

    m_f[12].name = tr("Coca-Cola");
    m_f[12].m_pict = P_PRD;
    m_f[12].m_flag = F_USA;
    m_f[12].price = 12;
    m_f[12].binc = 1.2;
    m_f[12].m_nu = 4;
    m_f[12].mz[0].invest = 6;
    m_f[12].mz[0].income = 3;
    m_f[12].mz[1].invest = 6;
    m_f[12].mz[1].income = 6;
    m_f[12].mz[2].invest = 12;
    m_f[12].mz[2].income = 12;
    m_f[12].mz[3].invest = 12;
    m_f[12].mz[3].income = 24;

    m_f[14].name = tr("Globus");
    m_f[14].m_pict = P_PRD;
    m_f[14].m_flag = F_HUN;
    m_f[14].price = 16;
    m_f[14].binc = 1.6;
    m_f[14].m_nu = 4;
    m_f[14].mz[0].invest = 8;
    m_f[14].mz[0].income = 4;
    m_f[14].mz[1].invest = 8;
    m_f[14].mz[1].income = 8;
    m_f[14].mz[2].invest = 16;
    m_f[14].mz[2].income = 16;
    m_f[14].mz[3].invest = 16;
    m_f[14].mz[3].income = 32;

    m_f[15].name = tr("Rostoc");
    m_f[15].m_pict = P_POR;
    m_f[15].m_flag = F_GER;
    m_f[15].price = 20;
    m_f[15].binc = 5;
    m_f[15].m_nu = 1;
    m_f[15].mz[0].invest = 10;
    m_f[15].mz[0].income = 10;

    m_f[16].name = tr("BEA");
    m_f[16].m_pict = P_AVI;
    m_f[16].m_flag = F_ENG;
    m_f[16].price = 50;
    m_f[16].binc = 5;
    m_f[16].m_nu = 4;
    m_f[16].mz[0].invest = 25;
    m_f[16].mz[0].income = 25;
    m_f[16].mz[1].invest = 25;
    m_f[16].mz[1].income = 50;
    m_f[16].mz[2].invest = 50;
    m_f[16].mz[2].income = 70;
    m_f[16].mz[3].invest = 50;
    m_f[16].mz[3].income = 100;

    m_f[17].name = tr("Lufthanza");
    m_f[17].m_pict = P_AVI;
    m_f[17].m_flag = F_GER;
    m_f[17].price = 60;
    m_f[17].binc = 6;
    m_f[17].m_nu = 4;
    m_f[17].mz[0].invest = 30;
    m_f[17].mz[0].income = 30;
    m_f[17].mz[1].invest = 35;
    m_f[17].mz[1].income = 70;
    m_f[17].mz[2].invest = 60;
    m_f[17].mz[2].income = 90;
    m_f[17].mz[3].invest = 60;
    m_f[17].mz[3].income = 120;

    m_f[19].name = tr("Air France");
    m_f[19].m_pict = P_AVI;
    m_f[19].m_flag = F_FRA;
    m_f[19].price = 70;
    m_f[19].binc = 7;
    m_f[19].m_nu = 4;
    m_f[19].mz[0].invest = 35;
    m_f[19].mz[0].income = 35;
    m_f[19].mz[1].invest = 35;
    m_f[19].mz[1].income = 70;
    m_f[19].mz[2].invest = 70;
    m_f[19].mz[2].income = 100;
    m_f[19].mz[3].invest = 70;
    m_f[19].mz[3].income = 140;

    m_f[21].name = tr("Omega");
    m_f[21].m_pict = P_ELK;
    m_f[21].m_flag = F_HLV;
    m_f[21].price = 6;
    m_f[21].binc = .6;
    m_f[21].m_nu = 4;
    m_f[21].mz[0].invest = 6;
    m_f[21].mz[0].income = 3;
    m_f[21].mz[1].invest = 6;
    m_f[21].mz[1].income = 6;
    m_f[21].mz[2].invest = 6;
    m_f[21].mz[2].income = 9;
    m_f[21].mz[3].invest = 6;
    m_f[21].mz[3].income = 15;

    m_f[22].name = tr("Grundic");
    m_f[22].m_pict = P_ELK;
    m_f[22].m_flag = F_GER;
    m_f[22].price = 8;
    m_f[22].binc = .8;
    m_f[22].m_nu = 4;
    m_f[22].mz[0].invest = 8;
    m_f[22].mz[0].income = 4;
    m_f[22].mz[1].invest = 8;
    m_f[22].mz[1].income = 8;
    m_f[22].mz[2].invest = 8;
    m_f[22].mz[2].income = 16;
    m_f[22].mz[3].invest = 8;
    m_f[22].mz[3].income = 20;

    m_f[24].name = tr("Bell");
    m_f[24].m_pict = P_ELK;
    m_f[24].m_flag = F_USA;
    m_f[24].price = 10;
    m_f[24].binc = 1;
    m_f[24].m_nu = 4;
    m_f[24].mz[0].invest = 10;
    m_f[24].mz[0].income = 5;
    m_f[24].mz[1].invest = 10;
    m_f[24].mz[1].income = 10;
    m_f[24].mz[2].invest = 10;
    m_f[24].mz[2].income = 15;
    m_f[24].mz[3].invest = 10;
    m_f[24].mz[3].income = 25;

    m_f[25].name = tr("Roterdam");
    m_f[25].m_pict = P_POR;
    m_f[25].m_flag = F_HEL;
    m_f[25].price = 20;
    m_f[25].binc = 5;
    m_f[25].m_nu = 1;
    m_f[25].mz[0].invest = 10;
    m_f[25].mz[0].income = 10;

    m_f[26].name = tr("NG");
    m_f[26].m_pict = P_RAZ;
    m_f[26].m_flag = F_FRA;
    m_f[26].price = 30;
    m_f[26].binc = 3;
    m_f[26].m_nu = 4;
    m_f[26].mz[0].invest = 15;
    m_f[26].mz[0].income = 15;
    m_f[26].mz[1].invest = 15;
    m_f[26].mz[1].income = 30;
    m_f[26].mz[2].invest = 50;
    m_f[26].mz[2].income = 50;
    m_f[26].mz[3].invest = 30;
    m_f[26].mz[3].income = 90;

    m_f[27].name = tr("IS");
    m_f[27].m_pict = P_RAZ;
    m_f[27].m_flag = F_ENG;
    m_f[27].price = 35;
    m_f[27].binc = 3.5;
    m_f[27].m_nu = 4;
    m_f[27].mz[0].invest = 20;
    m_f[27].mz[0].income = 20;
    m_f[27].mz[1].invest = 20;
    m_f[27].mz[1].income = 35;
    m_f[27].mz[2].invest = 55;
    m_f[27].mz[2].income = 55;
    m_f[27].mz[3].invest = 40;
    m_f[27].mz[3].income = 90;

    m_f[29].name = tr("CIA");
    m_f[29].m_pict = P_RAZ;
    m_f[29].m_flag = F_USA;
    m_f[29].price = 40;
    m_f[29].binc = 4;
    m_f[29].m_nu = 4;
    m_f[29].mz[0].invest = 25;
    m_f[29].mz[0].income = 25;
    m_f[29].mz[1].invest = 25;
    m_f[29].mz[1].income = 40;
    m_f[29].mz[2].invest = 40;
    m_f[29].mz[2].income = 60;
    m_f[29].mz[3].invest = 50;
    m_f[29].mz[3].income = 100;

    m_f[31].name = tr("Союзмультфильм");
    m_f[31].m_pict = P_KIN;
    m_f[31].m_flag = F_URS;
    m_f[31].price = 15;
    m_f[31].binc = 1.5;
    m_f[31].m_nu = 4;
    m_f[31].mz[0].invest = 8;
    m_f[31].mz[0].income = 8;
    m_f[31].mz[1].invest = 8;
    m_f[31].mz[1].income = 6;
    m_f[31].mz[2].invest = 30;
    m_f[31].mz[2].income = 17;
    m_f[31].mz[3].invest = 35;
    m_f[31].mz[3].income = 40;

    m_f[32].name = tr("Defa");
    m_f[32].m_pict = P_KIN;
    m_f[32].m_flag = F_GER;
    m_f[32].price = 10;
    m_f[32].binc = 1;
    m_f[32].m_nu = 4;
    m_f[32].mz[0].invest = 4;
    m_f[32].mz[0].income = 6;
    m_f[32].mz[1].invest = 6;
    m_f[32].mz[1].income = 4;
    m_f[32].mz[2].invest = 25;
    m_f[32].mz[2].income = 14;
    m_f[32].mz[3].invest = 30;
    m_f[32].mz[3].income = 35;

    m_f[34].name = tr("XX-th centure fox");
    m_f[34].m_pict = P_KIN;
    m_f[34].m_flag = F_USA;
    m_f[34].price = 20;
    m_f[34].binc = 1.5;
    m_f[34].m_nu = 4;
    m_f[34].mz[0].invest = 8;
    m_f[34].mz[0].income = 10;
    m_f[34].mz[1].invest = 10;
    m_f[34].mz[1].income = 8;
    m_f[34].mz[2].invest = 25;
    m_f[34].mz[2].income = 14;
    m_f[34].mz[3].invest = 30;
    m_f[34].mz[3].income = 35;

    m_f[35].name = tr("Genoa");
    m_f[35].m_pict = P_POR;
    m_f[35].m_flag = F_ITA;
    m_f[35].price = 20;
    m_f[35].binc = 5;
    m_f[35].m_nu = 1;
    m_f[35].mz[0].invest = 10;
    m_f[35].mz[0].income = 10;

    m_f[36].name = tr("Tanug");
    m_f[36].m_pict = P_INF;
    m_f[36].m_flag = F_BLK;
    m_f[36].price = 15;
    m_f[36].binc = 1.5;
    m_f[36].m_nu = 4;
    m_f[36].mz[0].invest = 10;
    m_f[36].mz[0].income = 7;
    m_f[36].mz[1].invest = 25;
    m_f[36].mz[1].income = 16;
    m_f[36].mz[2].invest = 35;
    m_f[36].mz[2].income = 20;
    m_f[36].mz[3].invest = 30;
    m_f[36].mz[3].income = 55;

    m_f[37].name = tr("Reuter");
    m_f[37].m_pict = P_INF;
    m_f[37].m_flag = F_ENG;
    m_f[37].price = 20;
    m_f[37].binc = 2;
    m_f[37].m_nu = 4;
    m_f[37].mz[0].invest = 13;
    m_f[37].mz[0].income = 10;
    m_f[37].mz[1].invest = 20;
    m_f[37].mz[1].income = 18;
    m_f[37].mz[2].invest = 40;
    m_f[37].mz[2].income = 30;
    m_f[37].mz[3].invest = 35;
    m_f[37].mz[3].income = 45;

    m_f[39].name = tr("France Press");
    m_f[39].m_pict = P_INF;
    m_f[39].m_flag = F_FRA;
    m_f[39].price = 25;
    m_f[39].binc = 2.5;
    m_f[39].m_nu = 4;
    m_f[39].mz[0].invest = 10;
    m_f[39].mz[0].income = 13;
    m_f[39].mz[1].invest = 25;
    m_f[39].mz[1].income = 24;
    m_f[39].mz[2].invest = 45;
    m_f[39].mz[2].income = 30;
    m_f[39].mz[3].invest = 40;
    m_f[39].mz[3].income = 45;

    m_f[40].name = tr("Toyota");
    m_f[40].m_pict = P_AVT;
    m_f[40].m_flag = F_JAP;
    m_f[40].price = 15;
    m_f[40].binc = 5;
    m_f[40].m_nu = 3;
    m_f[40].mz[0].invest = 5;
    m_f[40].mz[0].income = 10;
    m_f[40].mz[1].invest = 5;
    m_f[40].mz[1].income = 15;
    m_f[40].mz[2].invest = 5;
    m_f[40].mz[2].income = 30;

    m_f[41].name = tr("Weimar");
    m_f[41].m_pict = P_TUR;
    m_f[41].m_flag = F_GER;
    m_f[41].type = 1;
    m_f[41].price = 10;
    m_f[41].binc = 10;
    m_f[41].m_nu = 5;
    m_f[41].mz[0].invest = 10;
    m_f[41].mz[0].income = 20;
    m_f[41].mz[1].invest = 10;
    m_f[41].mz[1].income = 0;
    m_f[41].mz[2].invest = 10;
    m_f[41].mz[2].income = 0;
    m_f[41].mz[3].invest = 10;
    m_f[41].mz[3].income = 100;
    m_f[41].mz[4].type = 1;
    m_f[41].mz[4].invest = 0;
    m_f[41].mz[4].income = 200;

    m_f[42].name = tr("Zarichev Zaitung");
    m_f[42].m_pict = P_GAZ;
    m_f[42].m_flag = F_HLV;
    m_f[42].price = 20;
    m_f[42].binc = 3;
    m_f[42].m_nu = 1;
    m_f[42].mz[0].invest = 10;
    m_f[42].mz[0].income = 6;

    m_f[43].name = tr("Колыма");
    m_f[43].m_pict = P_MED;
    m_f[43].m_flag = F_URS;
    m_f[43].price = 20;
    m_f[43].binc = 2;
    m_f[43].m_nu = 4;
    m_f[43].mz[0].invest = 20;
    m_f[43].mz[0].income = 20;
    m_f[43].mz[1].invest = 20;
    m_f[43].mz[1].income = 40;
    m_f[43].mz[2].invest = 60;
    m_f[43].mz[2].income = 60;
    m_f[43].mz[3].invest = 60;
    m_f[43].mz[3].income = 80;

    m_f[45].name = tr("Icarus");
    m_f[45].m_pict = P_AVT;
    m_f[45].m_flag = F_HUN;
    m_f[45].type = 2;
    m_f[45].price = 20;
    m_f[45].binc = 10;
    m_f[45].m_nu = 6;
    m_f[45].mz[0].invest = 20;
    m_f[45].mz[0].income = 40;
    m_f[45].mz[1].invest = 20;
    m_f[45].mz[1].income = 40;
    m_f[45].mz[2].type = 1;
    m_f[45].mz[2].invest = 20;
    m_f[45].mz[2].income = 60;
    m_f[45].mz[3].type = 1;
    m_f[45].mz[3].invest = 20;
    m_f[45].mz[3].income = 100;
    m_f[45].mz[4].type = 1;
    m_f[45].mz[4].invest = 20;
    m_f[45].mz[4].income = 300;
    m_f[45].mz[5].type = 2;
    m_f[45].mz[5].invest = 0;
    m_f[45].mz[5].income = 500;

    m_f[46].name = tr("Volksvagen");
    m_f[46].m_pict = P_AVT;
    m_f[46].m_flag = F_GER;
    m_f[46].price = 15;
    m_f[46].binc = 5;
    m_f[46].m_nu = 1;
    m_f[46].mz[0].invest = 15;
    m_f[46].mz[0].income = 20;

    m_f[47].name = tr("Nizza");
    m_f[47].m_pict = P_MED;
    m_f[47].m_flag = F_FRA;
    m_f[47].price = 60;
    m_f[47].binc = 6;
    m_f[47].m_nu = 4;
    m_f[47].mz[0].invest = 60;
    m_f[47].mz[0].income = 30;
    m_f[47].mz[1].invest = 60;
    m_f[47].mz[1].income = 60;
    m_f[47].mz[2].invest = 120;
    m_f[47].mz[2].income = 90;
    m_f[47].mz[3].invest = 120;
    m_f[47].mz[3].income = 120;

    m_f[48].name = tr("Fiat");
    m_f[48].m_pict = P_AVT;
    m_f[48].m_flag = F_ITA;
    m_f[48].price = 20;
    m_f[48].binc = 5;
    m_f[48].m_nu = 1;
    m_f[48].mz[0].invest = 10;
    m_f[48].mz[0].income = 10;

    m_f[49].name = tr("Мурзилка");
    m_f[49].m_pict = P_GAZ;
    m_f[49].m_flag = F_URS;
    m_f[49].price = 15;
    m_f[49].binc = 5;
    m_f[49].m_nu = 1;
    m_f[49].mz[0].type = 3;
    m_f[49].mz[0].invest = 5;
    m_f[49].mz[0].income = 5;

    m_f[50].name = tr("Lozanna");
    m_f[50].m_pict = P_MED;
    m_f[50].m_flag = F_HLV;
    m_f[50].price = 80;
    m_f[50].binc = 8;
    m_f[50].m_nu = 4;
    m_f[50].mz[0].invest = 70;
    m_f[50].mz[0].income = 35;
    m_f[50].mz[1].invest = 70;
    m_f[50].mz[1].income = 70;
    m_f[50].mz[2].invest = 140;
    m_f[50].mz[2].income = 100;
    m_f[50].mz[3].invest = 140;
    m_f[50].mz[3].income = 140;

    m_f[51].name = tr("Fudziama");
    m_f[51].m_pict = P_TUR;
    m_f[51].m_flag = F_JAP;
    m_f[51].type = 1;
    m_f[51].price = 10;
    m_f[51].binc = 10;
    m_f[51].m_nu = 6;
    m_f[51].mz[0].invest = 10;
    m_f[51].mz[0].income = 20;
    m_f[51].mz[1].invest = 10;
    m_f[51].mz[1].income = 30;
    m_f[51].mz[2].invest = 10;
    m_f[51].mz[2].income = 40;
    m_f[51].mz[3].invest = 10;
    m_f[51].mz[3].income = 50;
    m_f[51].mz[4].invest = 10;
    m_f[51].mz[4].income = 100;
    m_f[51].mz[5].type = 1;
    m_f[51].mz[5].invest = 0;
    m_f[51].mz[5].income = 150;

    m_f[52].name = tr("Запорожец");
    m_f[52].m_pict = P_AVT;
    m_f[52].m_flag = F_URS;
    m_f[52].price = 15;
    m_f[52].binc = 3;
    m_f[52].m_nu = 4;
    m_f[52].mz[0].invest = 3;
    m_f[52].mz[0].income = 6;
    m_f[52].mz[1].invest = 3;
    m_f[52].mz[1].income = 9;
    m_f[52].mz[2].invest = 3;
    m_f[52].mz[2].income = 15;
    m_f[52].mz[3].invest = 3;
    m_f[52].mz[3].income = 30;

    m_f[53].name = tr("Unita");
    m_f[53].m_pict = P_GAZ;
    m_f[53].m_flag = F_ITA;
    m_f[53].price = 20;
    m_f[53].binc = 3;
    m_f[53].m_nu = 1;
    m_f[53].mz[0].invest = 10;
    m_f[53].mz[0].income = 6;

    m_f[54].name = tr("Playboy");
    m_f[54].m_pict = P_GAZ;
    m_f[54].m_flag = F_USA;
    m_f[54].type = 2;
    m_f[54].price = 40;
    m_f[54].binc = 20;
    m_f[54].m_nu = 6;
    m_f[54].mz[0].invest = 40;
    m_f[54].mz[0].income = 40;
    m_f[54].mz[1].type = 1;
    m_f[54].mz[1].invest = 40;
    m_f[54].mz[1].income = 50;
    m_f[54].mz[2].type = 1;
    m_f[54].mz[2].invest = 0;
    m_f[54].mz[2].income = 100;
    m_f[54].mz[3].type = 1;
    m_f[54].mz[3].invest = 0;
    m_f[54].mz[3].income = 200;
    m_f[54].mz[4].type = 1;
    m_f[54].mz[4].invest = 0;
    m_f[54].mz[4].income = 400;
    m_f[54].mz[5].type = 1;
    m_f[54].mz[5].invest = 0;
    m_f[54].mz[5].income = 500;

    m_f[55].name = tr("Renult");
    m_f[55].m_pict = P_AVT;
    m_f[55].m_flag = F_FRA;
    m_f[55].price = 15;
    m_f[55].binc = 3;
    m_f[55].m_nu = 2;
    m_f[55].mz[0].invest = 3;
    m_f[55].mz[0].income = 5;
    m_f[55].mz[1].invest = 30;
    m_f[55].mz[1].income = 15;

    m_f[56].name = tr("Venesia");
    m_f[56].m_pict = P_TUR;
    m_f[56].m_flag = F_ITA;
    m_f[56].price = 40;
    m_f[56].binc = 4;
    m_f[56].m_nu = 4;
    m_f[56].mz[0].invest = 40;
    m_f[56].mz[0].income = 20;
    m_f[56].mz[1].invest = 40;
    m_f[56].mz[1].income = 40;
    m_f[56].mz[2].invest = 80;
    m_f[56].mz[2].income = 60;
    m_f[56].mz[3].invest = 80;
    m_f[56].mz[3].income = 80;

    m_m[0].name = tr("Нефтяные компании");
    m_m[0].list.append(&m_f[1]);
    m_m[0].list.append(&m_f[2]);
    m_m[0].list.append(&m_f[4]);

    m_m[1].name = tr("Радиостанции");
    m_m[1].list.append(&m_f[6]);
    m_m[1].list.append(&m_f[7]);
    m_m[1].list.append(&m_f[9]);

    m_m[2].name = tr("Продукты");
    m_m[2].list.append(&m_f[11]);
    m_m[2].list.append(&m_f[12]);
    m_m[2].list.append(&m_f[14]);

    m_m[3].name = tr("Авиакомпании");
    m_m[3].list.append(&m_f[16]);
    m_m[3].list.append(&m_f[17]);
    m_m[3].list.append(&m_f[19]);

    m_m[4].name = tr("Электроника");
    m_m[4].list.append(&m_f[21]);
    m_m[4].list.append(&m_f[22]);
    m_m[4].list.append(&m_f[24]);

    m_m[5].name = tr("Разведка");
    m_m[5].list.append(&m_f[26]);
    m_m[5].list.append(&m_f[27]);
    m_m[5].list.append(&m_f[29]);

    m_m[6].name = tr("Киностудии");
    m_m[6].list.append(&m_f[31]);
    m_m[6].list.append(&m_f[32]);
    m_m[6].list.append(&m_f[34]);

    m_m[7].name = tr("Информагентства");
    m_m[7].list.append(&m_f[36]);
    m_m[7].list.append(&m_f[37]);
    m_m[7].list.append(&m_f[39]);

    m_m[8].name = tr("Пресса");
    m_m[8].list.append(&m_f[54]);
    m_m[8].list.append(&m_f[42]);
    m_m[8].list.append(&m_f[53]);
    m_m[8].list.append(&m_f[49]);

    m_m[9].name = tr("Автоконцерны");
    m_m[9].list.append(&m_f[45]);
    m_m[9].list.append(&m_f[55]);
    m_m[9].list.append(&m_f[48]);
    m_m[9].list.append(&m_f[40]);
    m_m[9].list.append(&m_f[46]);
    m_m[9].list.append(&m_f[52]);

    m_m[10].name = tr("Туризм");
    m_m[10].list.append(&m_f[51]);
    m_m[10].list.append(&m_f[41]);
    m_m[10].list.append(&m_f[56]);

    m_m[11].name = tr("Курорты");
    m_m[11].list.append(&m_f[47]);
    m_m[11].list.append(&m_f[50]);
    m_m[11].list.append(&m_f[43]);

    m_m[12].name = tr("HLV");
    m_m[12].list.append(&m_f[50]);
    m_m[12].list.append(&m_f[42]);
    m_m[12].list.append(&m_f[21]);

    m_m[13].name = tr("USA ind");
    m_m[13].list.append(&m_f[1]);
    m_m[13].list.append(&m_f[12]);
    m_m[13].list.append(&m_f[24]);

    m_m[14].name = tr("USA id");
    m_m[14].list.append(&m_f[29]);
    m_m[14].list.append(&m_f[34]);
    m_m[14].list.append(&m_f[54]);

    m_m[15].name = tr("USA");
    m_m[15].list.append(&m_f[1]);
    m_m[15].list.append(&m_f[12]);
    m_m[15].list.append(&m_f[24]);
    m_m[15].list.append(&m_f[29]);
    m_m[15].list.append(&m_f[34]);
    m_m[15].list.append(&m_f[54]);

    m_m[16].name = tr("DDR");
    m_m[16].list.append(&m_f[15]);
    m_m[16].list.append(&m_f[41]);
    m_m[16].list.append(&m_f[32]);

    m_m[17].name = tr("BRD");
    m_m[17].list.append(&m_f[17]);
    m_m[17].list.append(&m_f[46]);
    m_m[17].list.append(&m_f[22]);

    m_m[18].name = tr("Germany");
    m_m[18].list.append(&m_f[15]);
    m_m[18].list.append(&m_f[41]);
    m_m[18].list.append(&m_f[32]);
    m_m[18].list.append(&m_f[17]);
    m_m[18].list.append(&m_f[46]);
    m_m[18].list.append(&m_f[22]);

    m_m[19].name = tr("ENG");
    m_m[19].list.append(&m_f[7]);
    m_m[19].list.append(&m_f[16]);
    m_m[19].list.append(&m_f[27]);
    m_m[19].list.append(&m_f[2]);
    m_m[19].list.append(&m_f[37]);

    m_m[20].name = tr("FRA");
    m_m[20].list.append(&m_f[19]);
    m_m[20].list.append(&m_f[55]);
    m_m[20].list.append(&m_f[47]);
    m_m[20].list.append(&m_f[39]);
    m_m[20].list.append(&m_f[26]);

    m_m[21].name = tr("JAP");
    m_m[21].list.append(&m_f[40]);
    m_m[21].list.append(&m_f[5]);
    m_m[21].list.append(&m_f[51]);

    m_m[22].name = tr("ITA");
    m_m[22].list.append(&m_f[35]);
    m_m[22].list.append(&m_f[56]);
    m_m[22].list.append(&m_f[53]);
    m_m[22].list.append(&m_f[48]);

    m_m[23].name = tr("BLK");
    m_m[23].list.append(&m_f[6]);
    m_m[23].list.append(&m_f[36]);
    m_m[23].list.append(&m_f[11]);

    m_m[24].name = tr("HEL");
    m_m[24].list.append(&m_f[25]);
    m_m[24].list.append(&m_f[4]);

    m_m[25].name = tr("HUN");
    m_m[25].list.append(&m_f[45]);
    m_m[25].list.append(&m_f[14]);

    m_m[26].name = tr("RUS");
    m_m[26].list.append(&m_f[9]);
    m_m[26].list.append(&m_f[31]);
    m_m[26].list.append(&m_f[52]);
    m_m[26].list.append(&m_f[49]);
    m_m[26].list.append(&m_f[43]);

    m_m[27].name = tr("Порты");
    m_m[27].list.append(&m_f[5]);
    m_m[27].list.append(&m_f[15]);
    m_m[27].list.append(&m_f[25]);
    m_m[27].list.append(&m_f[35]);

    for (quint8 i=0; i<nu_Monopols; i++) {
        foreach (CFirm *f, m_m[i].list) {
            f->listMon.append(&m_m[i]);
        }
    }

    m_p[0].name = tr("Красный");
    m_p[1].name = tr("Синий");
    m_p[2].name = tr("Желтый");
    m_p[3].name = tr("Зеленый");
}
