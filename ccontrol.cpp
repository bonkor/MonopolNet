#include "ccontrol.h"

CControl::CControl(QObject *parent) :
    QObject(parent)
{
// ��������
    QObject::connect(&doc, SIGNAL(sendLog(QString)),
                     this, SLOT(toLog(QString)));
}

CDoc *CControl::getDoc(void)
{
    return &doc;
}

void CControl::tryPBP(int pl)
{
    CPlayer *p = &doc.m_p[pl];
    CLP *lp = &p->lastPay;

    if (p->pbp == 0 || ! lp->exist()) {
        emit sendToLog(p->name + tr(" �� ������� ���������� �� �������"));
        return;
    }

    foreach (quint8 tpl, lp->pls) {
        QString s;
        p->money += lp->sum;
        if (tpl != 4) {
            CPlayer *lpp = &doc.m_p[tpl];
            s = tr(" � ������ ") + lpp->name;
            lpp->money -= lp->sum;
            if (! lpp->money.positive())
                lpp->mustSellMode = true;
        }
        emit sendToLog(p->name + tr(" ������������ �� ������ ") + lp->sum.toString() + s);
    }
    p->pbp--;
    doc.clearLastPay(pl);
    emit docInfoChanged();
    if (p->mustSellMode && p->money.positive()) {
        p->mustSellMode = false;
        startMove();
    }
}

void CControl::tryInvestFirm(int pl, int fNu, int flNu)
{
    doc.clearLastPay(pl);
    if (doc.investFirm(pl, fNu, flNu)) {
        if (flNu == 0)
            emit docFirmChanged(fNu);
        else {
            emit showNewPoleForPlayer(pl);
            emit docChanged();
        }
    } else {
//        emit sendToLog(doc.m_p[pl].name + tr(" �� ������� ��������������� ") + doc.m_f[fNu].name);
    }
}

void CControl::tryTakeFirm(int pl, int fNu)
{
    doc.clearLastPay(pl);
    if (doc.takeFirm(pl, fNu))
        emit docFirmChanged(fNu);
    CPlayer *plp = &doc.m_p[pl];
    if (plp->mustSellMode && plp->money.positive()) {
        plp->mustSellMode = false;
        startMove();
    }
}

void CControl::tryBuyFirm(int pl, int fNu, int flNu)
{
    doc.clearLastPay(pl);
    QString s;
//    if (flNu != 0)
//        s = tr(" � ������������ �� ") + doc.m_f[flNu].name;
    if (doc.buyFirm(pl, fNu, flNu)) {
//        emit sendToLog(doc.m_p[pl].name + tr(" �������� ") + doc.m_f[fNu].name + s);
        if (flNu == 0)
            emit docFirmChanged(fNu);
        else {
            emit showNewPoleForPlayer(pl);
            emit docChanged();
        }
    } else {
//        emit sendToLog(doc.m_p[pl].name + tr(" �� ������� ������ ") + doc.m_f[fNu].name);
    }
}

void CControl::tryChangeFirm(int pl, int fNu, int tNu)
{
    doc.clearLastPay(pl);
    CPlayer *plp = &doc.m_p[pl];

    if (plp->mustChangeQues) {
        if (doc.changeFirm(pl, fNu, tNu)) {
            plp->mustChangeQues = false;
            emit docChanged();
            startMove();
        }
    }
}

void CControl::trySellFirm(int pl, int fNu)
{
    CPlayer *plp = &doc.m_p[pl];
    if (plp->mustLoseQues) {
        tryLoseFirm(pl, fNu);
        return;
    }
    if (plp->mustLoseMonQues) {
        tryLoseMon(pl, fNu);
        return;
    }

    doc.clearLastPay(pl);
    bool res;
    if (res = doc.sellFirm(pl, fNu)) {
        emit sendToLog(doc.m_p[pl].name + tr(" ������� ") + doc.m_f[fNu].name);
        emit docFirmChanged(fNu);
    } else {
        emit sendToLog(doc.m_p[pl].name + tr(" �� ������� ������� ") + doc.m_f[fNu].name);
    }
    if (plp->mustSellMode && plp->money.positive()) {
        qDebug() << pl << "-" << plp->money.toString();
        plp->mustSellMode = false;
        startMove();
    }
    if (plp->mustSellQues && res) {
        plp->mustSellQues = false;
        startMove();
    }
}

void CControl::tryLoseFirm(int pl, int fNu)
{
    bool res;
    if (res = doc.loseFirm(pl, fNu)) {
        emit sendToLog(doc.m_p[pl].name + tr(" ����������� �� ") + doc.m_f[fNu].name);
        emit docFirmChanged(fNu);
    } else {
        emit sendToLog(doc.m_p[pl].name + tr(" �� ������� ���������� �� ") + doc.m_f[fNu].name);
    }
    CPlayer *plp = &doc.m_p[pl];
    if (plp->mustLoseQues && res) {
        plp->mustLoseQues = false;
        startMove();
    }
}

void CControl::tryLoseMon(int pl, int fNu)
{
    bool res;
    if (doc.m_f[fNu].GetMultiplicator() == 1) {
        emit sendToLog(doc.m_p[pl].name + tr(" �� ������� ���������� �� ") + doc.m_f[fNu].name);
        return;
    }

    if (res = doc.loseFirm(pl, fNu)) {
        emit sendToLog(doc.m_p[pl].name + tr(" ����������� �� ") + doc.m_f[fNu].name);
        emit docFirmChanged(fNu);
    } else {
        emit sendToLog(doc.m_p[pl].name + tr(" �� ������� ���������� �� ") + doc.m_f[fNu].name);
    }
    CPlayer *plp = &doc.m_p[pl];
    if (plp->mustLoseMonQues && res) {
        plp->mustLoseMonQues = false;
        startMove();
    }
}

void CControl::tryLoseMezon(int pl, int fNu)
{
    qDebug() << tr("CControl::tryLoseMezon");
    CFirm *f = &doc.m_f[fNu];
    CPlayer *plp = &doc.m_p[pl];
    if (f->owner != pl || f->cur_mz == 0) {
        emit sendToLog(plp->name + tr(" �� ������� ����� ����� � ") + f->name);
        return;
    }

    f->cur_mz--;
    emit sendToLog(plp->name + tr(" ������� ����� � ") + f->name);

    plp->mustLoseMeson = false;
    emit docFirmChanged(fNu);
    startMove();
}

void CControl::tryMove(int pl, int fNu)
{
    qDebug() << tr("CControl::tryMove");
    CPlayer *plp = &doc.m_p[pl];
    if (plp->mustGoPireferic) {
        if (doc.inPireferic(fNu)) {
            plp->mustGoPireferic = false;
            movePlayer(0, fNu);
        } else {
            emit sendToLog(plp->name + tr(" �� ����� ������� �� �������"));
        }
    } else if (plp->mustGoCrest) {
        if (doc.inCrest(fNu)) {
            plp->mustGoCrest = false;
            movePlayer(0, fNu);
        } else {
            emit sendToLog(plp->name + tr(" �� ����� ������� �� �������"));
        }
    } else if (plp->mustGoBetween) {
        if (doc.inBetween(fNu)) {
            plp->mustGoBetween = false;
            movePlayer(0, fNu);
        } else {
            emit sendToLog(plp->name + tr(" �� ����� ������� �� �������"));
        }
    } else {
        emit sendToLog(plp->name + tr(" �� ����� ������� �� �������"));
    }
}

void CControl::toLog(QString str)
{
    emit sendToLog(str);
}

void CControl::replayDir(int dir)
{
    doc.clearLastPay(doc.curPl);
    CPlayer *cplp = doc.getCurPlayer();
    if (cplp->pos == 44 && cplp->crestDir == 0) {
        cplp->crestDir = dir;
        QString s;
        switch (dir) {
        case D_Left:
            s = QString(tr("������"));
            break;
        case D_Right:
            s = QString(tr("�������"));
            break;
        case D_Up:
            s = QString(tr("�����"));
            break;
        case D_Down:
            s = QString(tr("����"));
            break;
        }

        emit sendToLog(cplp->name + tr(" ���� ") + s);
        emit askCubik(doc.curPl);
    }
}

void CControl::replayCubik(int rnd)
{
    doc.clearLastPay(doc.curPl);
    CPlayer *cplp = doc.getCurPlayer();
//    if (doc.canTake(doc.curPl, cplp->pos)) {
//        doc.takeFirm(doc.curPl, cplp->pos);
//        emit docInfoChanged();
//    }
    QString s;
    s.setNum(rnd);
    emit sendToLog(cplp->name + tr(" �������� ") + s);
    if (rnd == 6)
        emit askStay(doc.curPl);
    else
        movePlayer(rnd);
}

void CControl::droppedQuestion(int pl, QPair<quint8,quint8> pair)
{
    qDebug() << pair.first << pair.second;

    pair.first = 6; pair.second = 5;
//    pair.first = 4; pair.second = 1;

    if (pl != doc.curPl)
        return;

    CPlayer *cplp = doc.getCurPlayer();
    QString name = cplp->name;
    if (pair.first == 1 && pair.second == 1) {
        // +10
        emit sendToLog(name + tr(" �������� +10"));
        doc.takeFromBank(pl, 10);
    } else if (pair.first == 1 && pair.second == 2) {
        // -10
        emit sendToLog(name + tr(" �������� -10"));
        doc.giveToBank(pl, 10);
    } else if (pair.first == 1 && pair.second == 3) {
        // �������� � ������
        emit sendToLog(name + tr(" �������� '�������� � ������'"));
        cplp->turnToStart++;
    } else if (pair.first == 1 && pair.second == 4) {
        // �����
        emit sendToLog(name + tr(" �������� '�����'"));
        doc.go(pl, 0, 10);
    } else if (pair.first == 1 && pair.second == 5) {
        // ?
        emit sendToLog(name + tr(" �������� '?'"));
        cplp->insertToQueue(Q_Ques);
    } else if (pair.first == 1 && pair.second == 6) {
        // �� �� ����������
        emit sendToLog(name + tr(" �������� '�� �� ?'"));
        cplp->pbq++;
    } else if (pair.first == 2 && pair.second == 1) {
        // buy
        emit sendToLog(name + tr(" �������� '����'"));
        cplp->canBuy = true;
        cplp->investComplit = false;
    } else if (pair.first == 2 && pair.second == 2) {
        // +20
        emit sendToLog(name + tr(" �������� +20"));
        doc.takeFromBank(pl, 20);
    } else if (pair.first == 2 && pair.second == 3) {
        // -20
        emit sendToLog(name + tr(" �������� -20"));
        doc.giveToBank(pl, 20);
    } else if (pair.first == 2 && pair.second == 4) {
        // ����� ������ � �������
        emit sendToLog(name + tr(" �������� '������ ����� ������ � �������'"));
        cplp->insertToQueue(Q_Move_Between);
    } else if (pair.first == 2 && pair.second == 5) {
        // ������
        emit sendToLog(name + tr(" �������� '������'"));
        doc.go(pl, 0, 30);
    } else if (pair.first == 2 && pair.second == 6) {
        // �� �� ������
        emit sendToLog(name + tr(" �������� '�� �� ������'"));
        cplp->pbp += 3;
    } else if (pair.first == 3 && pair.second == 1) {
        // buy
        emit sendToLog(name + tr(" �������� '������� �����'"));
        cplp->canInvest = true;
        cplp->investComplit = false;
    } else if (pair.first == 3 && pair.second == 2) {
        // sell
        emit sendToLog(name + tr(" �������� '������'"));
        cplp->insertToQueue(Q_Sell);
    } else if (pair.first == 3 && pair.second == 3) {
        // +30
        emit sendToLog(name + tr(" �������� +30"));
        doc.takeFromBank(pl, 30);
    } else if (pair.first == 3 && pair.second == 4) {
        // -30
        emit sendToLog(name + tr(" �������� -30"));
        doc.giveToBank(pl, 30);
    } else if (pair.first == 3 && pair.second == 5) {
        // �� �����
        emit sendToLog(name + tr(" �������� '������ �� ����� �� ������'"));
        cplp->insertToQueue(Q_Move_Crest);
    } else if (pair.first == 3 && pair.second == 6) {
        // �����
        emit sendToLog(name + tr(" �������� '�����'"));
        doc.go(pl, 0, 20);
    } else if (pair.first == 4 && pair.second == 1) {
        // 5 ������
        emit sendToLog(name + tr(" �������� '5 ����� ������'"));
        cplp->addMove(5);
    } else if (pair.first == 4 && pair.second == 2) {
        // -*
        emit sendToLog(name + tr(" �������� '����� �����'"));
        cplp->insertToQueue(Q_LoseMez);
    } else if (pair.first == 4 && pair.second == 3) {
        // lose
        emit sendToLog(name + tr(" �������� '�������'"));
        cplp->insertToQueue(Q_Lose);
    } else if (pair.first == 4 && pair.second == 4) {
        // +50
        emit sendToLog(name + tr(" �������� +50"));
        doc.takeFromBank(pl, 50);
    } else if (pair.first == 4 && pair.second == 5) {
        // ���� �� 10
        emit sendToLog(name + tr(" �������� '���� �� 10'"));
        doc.transferMoneyToAll(pl, 10);
    } else if (pair.first == 4 && pair.second == 6) {
        // �� ���������
        emit sendToLog(name + tr(" �������� '������ �� ����� �� ���������'"));
        cplp->insertToQueue(Q_Move_Perefiric);
    } else if (pair.first == 5 && pair.second == 1) {
        // +st
        emit sendToLog(name + tr(" �������� '�������������� +25 ��� ������� ����� �����'"));
        cplp->plusStart++;
    } else if (pair.first == 5 && pair.second == 2) {
        // 3?
        emit sendToLog(name + tr(" �������� '???'"));
        cplp->insertToQueue(Q_Ques, 3);
    } else if (pair.first == 5 && pair.second == 3) {
        // loseMon
        emit sendToLog(name + tr(" �������� '������� ����� �� ���������'"));
        cplp->insertToQueue(Q_LoseMon);
    } else if (pair.first == 5 && pair.second == 4) {
        // lose
        emit sendToLog(name + tr(" �������� '��� ��������'"));
        for (quint8 i=0; i<doc.nu_Players; i++) {
            CPlayer *p = &doc.m_p[i];
            if (i != pl && p->active)
                p->insertToQueue(Q_Lose);
        }
    } else if (pair.first == 5 && pair.second == 5) {
        // �� ���� �� 10
        emit sendToLog(name + tr(" �������� '�� ���� �� 10'"));
        for (quint8 i=0; i<doc.nu_Players; i++) {
            CPlayer *p = &doc.m_p[i];
            if (i != pl && p->active)
                doc.transferMoney(i, pl, 10);
        }
    } else if (pair.first == 5 && pair.second == 6) {
        // ���� �� 15
        emit sendToLog(name + tr(" �������� '���� �� 15'"));
        doc.transferMoneyToAll(pl, 15);
    } else if (pair.first == 6 && pair.second == 1) {

    } else if (pair.first == 6 && pair.second == 2) {
        // -st
        emit sendToLog(name + tr(" �������� '�� �������� +25 ��� ������� ����� �����'"));
        cplp->plusStart--;
    } else if (pair.first == 6 && pair.second == 3) {
        // 5 �����
        emit sendToLog(name + tr(" �������� '5 ����� �����'"));
        cplp->addMove(-5);
    } else if (pair.first == 6 && pair.second == 4) {
        // seq
        emit sendToLog(name + tr(" �������� '5 ����� ���������'"));
        cplp->seq += 5;
    } else if (pair.first == 6 && pair.second == 5) {
        // change
        emit sendToLog(name + tr(" �������� '�������'"));
        cplp->insertToQueue(Q_Change);
    } else if (pair.first == 6 && pair.second == 6) {
        // �� ���� �� 15
        emit sendToLog(name + tr(" �������� '�� ���� �� 15'"));
        for (quint8 i=0; i<doc.nu_Players; i++) {
            CPlayer *p = &doc.m_p[i];
            if (i != pl && p->active)
                doc.transferMoney(i, pl, 15);
        }
    }
    docChanged();
    startMove();
}

void CControl::replayStay(int dir)
{
    doc.clearLastPay(doc.curPl);
    CPlayer *cplp = doc.getCurPlayer();
    if (cplp->stay) {
        QString s = QString(tr("������"));
        if (cplp->pos == 10)
            s = QString(tr("�����"));
        if (dir == 0) {
            emit sendToLog(cplp->name + tr(" �� ���������� �� ") + s);
            startMove();
        } else {
            cplp->stay = false;
            doc.giveToBank(doc.curPl, 10);
            emit sendToLog(cplp->name + tr(" ���������� �� ") + s + tr(" �� 10"));
            cplp->addMove(1);
            emit docInfoChanged();
            startMove();
        }

    } else {
        if (dir == 0)
            movePlayer(0);
        else
            movePlayer(6);
    }
}

void CControl::movePlayer(quint8 st, int pos)
{
    CPlayer *cplp = doc.getCurPlayer();

    cplp->canBuy = false;
    cplp->canInvest = false;

    quint8 oldPos = cplp->pos;
    quint8 newPos = cplp->pos;
    if (doc.go(doc.curPl, st, pos)) {
        newPos = cplp->pos;
        emit docFirmChanged(newPos);
    } else
        emit docChanged();
    newPos = cplp->pos;
    quint8 ft = doc.m_f[newPos].m_type;
    if (ft == F_Pip) {
        for (quint8 i=0; i<doc.nu_Players; i++) {
            CPlayer *p = &doc.m_p[i];
            if (i != doc.curPl && p->active) {
                if (p->pos == 10) {         // �����
                    p->stay = false;
                    emit sendToLog(p->name + tr(" ������������ �� �����"));
                    doc.go(i, 0, newPos - 3);
                    emit docChanged();
                } else if (p->pos == 30) {  // ������
                    p->stay = false;
                    emit sendToLog(p->name + tr(" ������� �� ������ ���������"));
                }
            }
        }
    } else if (ft == F_3Ques)
        cplp->insertToQueue(Q_Ques, 3);
    else if (ft == F_Ques)
        cplp->insertToQueue(Q_Ques);

    if (cplp->active) {
        emit showNewPoleForPlayer(doc.curPl);
        startMove();
    } else
        endOfTurn(doc.curPl);
}

void CControl::startGame(void)
{
    emit sendToLog(tr("���� ����������..."));
    emit changeScenePlayer(0);
    startTurn(0);
}

void CControl::endGame(quint8 pNu)
{
    emit sendToLog(doc.m_p[pNu].name + tr(" �������"));

}

void CControl::startTurn(quint8 pNu)
{
    if (!doc.setCurPlayer(pNu)) {
        emit sendToLog(tr("�� ���� ���������� �������� ������. ����."));
        return;
    }

    emit disEndOfTurn(pNu);
    emit disCubik(pNu);
    emit sendToLog(tr("����� ") + doc.m_p[pNu].name);
    CPlayer *cplp = doc.getCurPlayer();
/*    if (cplp->seq > 0) {
        cplp->seq--;
        emit docInfoChanged();
    }*/
    cplp->canBuy = false;
    cplp->canInvest = false;
    cplp->addMove(1);

    startMove();
}

void CControl::endOfTurn(int pNu)
{
    doc.clearLastPay(pNu);
    if (pNu != doc.curPl) {
        emit sendToLog(tr("����������� EOT"));
        return;
    }

    emit sendToLog(doc.m_p[pNu].name + tr(" �������� ���"));
    quint8 next = doc.getNextPlayer(1);
    emit changeScenePlayer(next);
    if (doc.getActivePlayers() == 1)
        endGame(next);
    else
        startTurn(next);
}

void CControl::startMove(void)
{
    CPlayer *cplp = doc.getCurPlayer();

    if (! cplp->money.positive()) {
        emit askSellSomething(doc.curPl);
        return;
    }

    quint8 act = cplp->getNextAction();
    switch (act) {
    case Q_No:
        emit enaEndOfTurn(doc.curPl);
        break;
    case Q_Move:
        if (cplp->pos == 44 && cplp->crestDir == 0) {   // Start
            emit askDirection(doc.curPl);
        } else if ((cplp->pos == 10 || cplp->pos == 30) && cplp->stay == true) {   // ����� ��� ������
            if (doc.playerCapital(doc.curPl) > 10 && cplp->seq == 0)
                emit askStayTT(doc.curPl);
            else {
                emit sendToLog(cplp->name + tr(" �� ����� ����������"));
                if (cplp->seq > 0)
                    cplp->seq--;
                startMove();
            }
        } else {
            emit askCubik(doc.curPl);
        }
        break;
    case Q_Ques:
        emit askQuestion(doc.curPl);
        break;
    case Q_Sell:
        if (doc.playerOwnerCount(doc.curPl) == 0) {
            emit sendToLog(cplp->name + tr(" �� ����� ����"));
            startMove();
            return;
        }
        if (cplp->seq > 0) {
            emit sendToLog(cplp->name + tr(" �� ����� ��������� ��-�� ���������"));
            startMove();
            return;
        }
        cplp->mustSellQues = true;
        emit askSell(doc.curPl);
        break;
    case Q_Lose:
        if (doc.playerOwnerCount(doc.curPl) == 0) {
            emit sendToLog(cplp->name + tr(" �� ����� ����"));
            startMove();
            return;
        }
        cplp->mustLoseQues = true;
        emit askLose(doc.curPl);
        break;
    case Q_LoseMon:
        if (doc.playerMonCount(doc.curPl) == 0) {
            emit sendToLog(cplp->name + tr(" �� ����� ���������"));
            startMove();
            return;
        }
        cplp->mustLoseMonQues = true;
        emit askLoseMon(doc.curPl);
        break;
    case Q_LoseMez:
        if (doc.playerMezonCount(doc.curPl) == 0) {
            emit sendToLog(cplp->name + tr(" �� ����� �������"));
            startMove();
            return;
        }
        cplp->mustLoseMeson = true;
        emit askLoseMezon(doc.curPl);
        break;
    case Q_Change:
        if (doc.playerExpensiveFirm(doc.curPl) == 0 || doc.cheapestFreeFirm(doc.curPl) == 0 ||
                doc.playerExpensiveFirm(doc.curPl)->price <= doc.cheapestFreeFirm(doc.curPl)->price) {
            emit sendToLog(cplp->name + tr(" �� ����� ���� ��� ������"));
            startMove();
            return;
        }
        cplp->mustChangeQues = true;
        emit askChange(doc.curPl);
        break;
    case Q_Move_Perefiric:
        cplp->mustGoPireferic = true;
        emit askMoveToPirefiric(doc.curPl);
        break;
    case Q_Move_Crest:
        cplp->mustGoCrest = true;
        emit askMoveToCrest(doc.curPl);
        break;
    case Q_Move_Between:
        cplp->mustGoBetween = true;
        emit askMoveBetween(doc.curPl);
        break;
    }
}
