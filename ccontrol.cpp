#include "ccontrol.h"

CControl::CControl(QObject *parent) :
    QObject(parent)
{
// временно
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
        emit sendToLog(p->name + tr(" не удается отказаться от платежа"));
        return;
    }

    foreach (quint8 tpl, lp->pls) {
        QString s;
        p->money += lp->sum;
        if (tpl != 4) {
            CPlayer *lpp = &doc.m_p[tpl];
            s = tr(" в пользу ") + lpp->name;
            lpp->money -= lp->sum;
            if (! lpp->money.positive())
                lpp->mustSellMode = true;
        }
        emit sendToLog(p->name + tr(" отказывается от оплаты ") + lp->sum.toString() + s);
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
//        emit sendToLog(doc.m_p[pl].name + tr(" не удается промезонировать ") + doc.m_f[fNu].name);
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
//        s = tr(" и отказывается от ") + doc.m_f[flNu].name;
    if (doc.buyFirm(pl, fNu, flNu)) {
//        emit sendToLog(doc.m_p[pl].name + tr(" покупает ") + doc.m_f[fNu].name + s);
        if (flNu == 0)
            emit docFirmChanged(fNu);
        else {
            emit showNewPoleForPlayer(pl);
            emit docChanged();
        }
    } else {
//        emit sendToLog(doc.m_p[pl].name + tr(" не удается купить ") + doc.m_f[fNu].name);
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
//        emit sendToLog(doc.m_p[pl].name + tr(" продает ") + doc.m_f[fNu].name);
        emit docFirmChanged(fNu);
//    } else {
//        emit sendToLog(doc.m_p[pl].name + tr(" не удается продать ") + doc.m_f[fNu].name);
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
        emit sendToLog(doc.m_p[pl].name + tr(" отказываетя от ") + doc.m_f[fNu].name);
        emit docFirmChanged(fNu);
    } else {
        emit sendToLog(doc.m_p[pl].name + tr(" не удается отказаться от ") + doc.m_f[fNu].name);
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
        emit sendToLog(doc.m_p[pl].name + tr(" не удается отказаться от ") + doc.m_f[fNu].name);
        return;
    }

    if (res = doc.loseFirm(pl, fNu)) {
        emit sendToLog(doc.m_p[pl].name + tr(" отказываетя от ") + doc.m_f[fNu].name);
        emit docFirmChanged(fNu);
    } else {
        emit sendToLog(doc.m_p[pl].name + tr(" не удается отказаться от ") + doc.m_f[fNu].name);
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
        emit sendToLog(plp->name + tr(" не удается снять мезон с ") + f->name);
        return;
    }

    f->cur_mz--;
    emit sendToLog(plp->name + tr(" снимает мезон с ") + f->name);

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
            emit sendToLog(plp->name + tr(" не может перейти на позицию"));
        }
    } else if (plp->mustGoCrest) {
        if (doc.inCrest(fNu)) {
            plp->mustGoCrest = false;
            movePlayer(0, fNu);
        } else {
            emit sendToLog(plp->name + tr(" не может перейти на позицию"));
        }
    } else if (plp->mustGoBetween) {
        if (doc.inBetween(fNu)) {
            plp->mustGoBetween = false;
            movePlayer(0, fNu);
        } else {
            emit sendToLog(plp->name + tr(" не может перейти на позицию"));
        }
    } else {
        emit sendToLog(plp->name + tr(" не может перейти на позицию"));
    }
}

void CControl::tryChoose(int pl, int r, int c)
{
    qDebug() << tr("CControl::tryChoose");
    CPlayer *plp = &doc.m_p[pl];
    if (plp->mustChoose) {
        plp->mustChoose = false;
        QPair<quint8,quint8> pair;
        pair.first = r;
        pair.second = c;
        tryQuestion(pl, pair, true);
    } else {
        emit sendToLog(plp->name + tr(" не может выбрать"));
    }
}

void CControl::tryQuestionAccept(int pl, bool res)
{
    qDebug() << tr("CControl::tryQuestionAccept");
    CPlayer *plp = &doc.m_p[pl];
    if (plp->mustAcceptQues) {
        plp->mustAcceptQues = false;
        if (res)
            droppedQuestion(pl, lastQuestion);
        else {
            plp->pbq--;
            emit docInfoChanged();
            emit sendToLog(plp->name + tr(" отказывается от вопроса ") + questionName(lastQuestion));
            startMove();
        }
    } else {
        emit sendToLog(plp->name + tr(" не может согласиться с вопросом. ОШИБКА"));
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
            s = QString(tr("налево"));
            break;
        case D_Right:
            s = QString(tr("направо"));
            break;
        case D_Up:
            s = QString(tr("вверх"));
            break;
        case D_Down:
            s = QString(tr("вниз"));
            break;
        }

        emit sendToLog(cplp->name + tr(" идет ") + s);
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
    emit sendToLog(cplp->name + tr(" выбросил ") + s);
    if (rnd == 6)
        emit askStay(doc.curPl);
    else
        movePlayer(rnd);
}

void CControl::tryQuestion(int pl, QPair<quint8,quint8> pair, bool choose)
{
    qDebug() << pair.first << pair.second;

//    pair.first = 6; pair.second = 1;
//    pair.first = 4; pair.second = 1;

    if (pl != doc.curPl)
        return;

    CPlayer *cplp = doc.getCurPlayer();
    QString name = cplp->name;

    QString txt1;
    if (choose)
        txt1 = tr(" выбрал ");
    else
        txt1 = tr(" выбросил ");
    emit sendToLog(name + txt1 + questionName(pair));

    if (! choose && cplp->pbq > 0 && ! checkQuestionPositive(pair)) {
        lastQuestion = pair;
        cplp->mustAcceptQues = true;
        emit askQuestionPB(pl);
    } else
        droppedQuestion(pl, pair);
}

bool CControl::checkQuestionPositive(QPair<quint8,quint8> pair)
{
    quint8 r = pair.first;
    quint8 c = pair.second;

    if (r == 1 && c == 1)
        return true;
    else if (r == 1 && c == 6)
        return true;
    else if (r == 1 && c == 6)
        return true;
    else if (r == 2 && c == 1)
        return true;
    else if (r == 2 && c == 2)
        return true;
    else if (r == 2 && c == 6)
        return true;
    else if (r == 3 && c == 1)
        return true;
    else if (r == 3 && c == 3)
        return true;
    else if (r == 4 && c == 4)
        return true;
    else if (r == 5 && c == 1)
        return true;
    else if (r == 5 && c == 4)
        return true;
    else if (r == 5 && c == 5)
        return true;
    else if (r == 6 && c == 1)
        return true;
    else if (r == 6 && c == 6)
        return true;
    else
        return false;
}

QString CControl::questionName(QPair<quint8,quint8> pair)
{
    if (pair.first == 1 && pair.second == 1)
        return tr("+10");
    else if (pair.first == 1 && pair.second == 2)
        return tr("-10");
    else if (pair.first == 1 && pair.second == 3)
        return tr("'Свернуть к старту'");
    else if (pair.first == 1 && pair.second == 4)
        return tr("'Такси'");
    else if (pair.first == 1 && pair.second == 5)
        return tr("'?'");
    else if (pair.first == 1 && pair.second == 6)
        return tr("'ПБ на ?'");
    else if (pair.first == 2 && pair.second == 1)
        return tr("'Купи'");
    else if (pair.first == 2 && pair.second == 2)
        return tr("+20");
    else if (pair.first == 2 && pair.second == 3)
        return tr("-20");
    else if (pair.first == 2 && pair.second == 4)
        return tr("'Встань между фишкой и стартом'");
    else if (pair.first == 2 && pair.second == 5)
        return tr("'Тюрьма'");
    else if (pair.first == 2 && pair.second == 6)
        return tr("'ПБ на платеж'");
    else if (pair.first == 3 && pair.second == 1)
        return tr("'Поставь мезон'");
    else if (pair.first == 3 && pair.second == 2)
        return tr("'Продай'");
    else if (pair.first == 3 && pair.second == 3)
        return tr("+30");
    else if (pair.first == 3 && pair.second == 4)
        return tr("-30");
    else if (pair.first == 3 && pair.second == 5)
        return tr("'Встань на любую из креста'");
    else if (pair.first == 3 && pair.second == 6)
        return tr("'Биржа'");
    else if (pair.first == 4 && pair.second == 1)
        return tr("'5 ходов вперед'");
    else if (pair.first == 4 && pair.second == 2)
        return tr("'Сними мезон'");
    else if (pair.first == 4 && pair.second == 3)
        return tr("'Потеряй'");
    else if (pair.first == 4 && pair.second == 4)
        return tr("+50");
    else if (pair.first == 4 && pair.second == 5)
        return tr("'Всем по 10'");
    else if (pair.first == 4 && pair.second == 6)
        return tr("'Встань на любую из перефирии'");
    else if (pair.first == 5 && pair.second == 1)
        return tr("'Дополнительные +25 при проходе через СТАРТ'");
    else if (pair.first == 5 && pair.second == 2)
        return tr("'???'");
    else if (pair.first == 5 && pair.second == 3)
        return tr("'Потеряй фирму из монополии'");
    else if (pair.first == 5 && pair.second == 4)
        return tr("'Все потеряли'");
    else if (pair.first == 5 && pair.second == 5)
        return tr("'От всех по 10'");
    else if (pair.first == 5 && pair.second == 6)
        return tr("'Всем по 15'");
    else if (pair.first == 6 && pair.second == 1)
        return tr("'Выбери среди вопросов'");
    else if (pair.first == 6 && pair.second == 2)
        return tr("'Не получать +25 при проходе через СТАРТ'");
    else if (pair.first == 6 && pair.second == 3)
        return tr("'5 ходов назад'");
    else if (pair.first == 6 && pair.second == 4)
        return tr("'5 ходов сервестра'");
    else if (pair.first == 6 && pair.second == 5)
        return tr("'Поменяй'");
    else if (pair.first == 6 && pair.second == 6)
        return tr("'От всех по 15'");

    return tr("");
}

void CControl::droppedQuestion(int pl, QPair<quint8,quint8> pair)
    {
    if (pl != doc.curPl)
        return;

    CPlayer *cplp = doc.getCurPlayer();
    QString name = cplp->name;

    if (pair.first == 1 && pair.second == 1) {
        // +10
        doc.takeFromBank(pl, 10);
    } else if (pair.first == 1 && pair.second == 2) {
        // -10
        doc.giveToBank(pl, 10);
    } else if (pair.first == 1 && pair.second == 3) {
        // свернуть к старту
        cplp->turnToStart++;
    } else if (pair.first == 1 && pair.second == 4) {
        // такси
        doc.go(pl, 0, 10);
    } else if (pair.first == 1 && pair.second == 5) {
        // ?
        cplp->insertToQueue(Q_Ques);
    } else if (pair.first == 1 && pair.second == 6) {
        // ПБ на вопрошалку
        cplp->pbq++;
    } else if (pair.first == 2 && pair.second == 1) {
        // buy
        cplp->canBuy = true;
        cplp->investComplit = false;
    } else if (pair.first == 2 && pair.second == 2) {
        // +20
        doc.takeFromBank(pl, 20);
    } else if (pair.first == 2 && pair.second == 3) {
        // -20
        doc.giveToBank(pl, 20);
    } else if (pair.first == 2 && pair.second == 4) {
        // между фишкой и стартом
        cplp->insertToQueue(Q_Move_Between);
    } else if (pair.first == 2 && pair.second == 5) {
        // тюрьма
        doc.go(pl, 0, 30);
    } else if (pair.first == 2 && pair.second == 6) {
        // ПБ на оплату
        cplp->pbp += 3;
    } else if (pair.first == 3 && pair.second == 1) {
        // buy
        cplp->canInvest = true;
        cplp->investComplit = false;
    } else if (pair.first == 3 && pair.second == 2) {
        // sell
        cplp->insertToQueue(Q_Sell);
    } else if (pair.first == 3 && pair.second == 3) {
        // +30
        doc.takeFromBank(pl, 30);
    } else if (pair.first == 3 && pair.second == 4) {
        // -30
        doc.giveToBank(pl, 30);
    } else if (pair.first == 3 && pair.second == 5) {
        // на крест
        cplp->insertToQueue(Q_Move_Crest);
    } else if (pair.first == 3 && pair.second == 6) {
        // биржа
        doc.go(pl, 0, 20);
    } else if (pair.first == 4 && pair.second == 1) {
        // 5 вперед
        cplp->addMove(5);
    } else if (pair.first == 4 && pair.second == 2) {
        // -*
        cplp->insertToQueue(Q_LoseMez);
    } else if (pair.first == 4 && pair.second == 3) {
        // lose
        cplp->insertToQueue(Q_Lose);
    } else if (pair.first == 4 && pair.second == 4) {
        // +50
        doc.takeFromBank(pl, 50);
    } else if (pair.first == 4 && pair.second == 5) {
        // всем по 10
        doc.transferMoneyToAll(pl, 10);
    } else if (pair.first == 4 && pair.second == 6) {
        // на перефирию
        cplp->insertToQueue(Q_Move_Perefiric);
    } else if (pair.first == 5 && pair.second == 1) {
        // +st
        cplp->plusStart++;
    } else if (pair.first == 5 && pair.second == 2) {
        // 3?
        doc.go(pl, 0, 0);
        cplp->insertToQueue(Q_Ques, 3);
    } else if (pair.first == 5 && pair.second == 3) {
        // loseMon
        cplp->insertToQueue(Q_LoseMon);
    } else if (pair.first == 5 && pair.second == 4) {
        // lose
        for (quint8 i=0; i<doc.nu_Players; i++) {
            CPlayer *p = &doc.m_p[i];
            if (i != pl && p->active)
                p->insertToQueue(Q_Lose);
        }
    } else if (pair.first == 5 && pair.second == 5) {
        // от всех по 10
        for (quint8 i=0; i<doc.nu_Players; i++) {
            CPlayer *p = &doc.m_p[i];
            if (i != pl && p->active)
                doc.transferMoney(i, pl, 10);
        }
    } else if (pair.first == 5 && pair.second == 6) {
        // всем по 15
        doc.transferMoneyToAll(pl, 15);
    } else if (pair.first == 6 && pair.second == 1) {
        // choose
        cplp->insertToQueue(Q_Choose);
    } else if (pair.first == 6 && pair.second == 2) {
        // -st
        cplp->plusStart--;
    } else if (pair.first == 6 && pair.second == 3) {
        // 5 назад
        cplp->addMove(-5);
    } else if (pair.first == 6 && pair.second == 4) {
        // seq
        cplp->seq += 5;
    } else if (pair.first == 6 && pair.second == 5) {
        // change
        cplp->insertToQueue(Q_Change);
    } else if (pair.first == 6 && pair.second == 6) {
        // от всех по 15
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
        QString s = QString(tr("тюрьмы"));
        if (cplp->pos == 10)
            s = QString(tr("такси"));
        if (dir == 0) {
            emit sendToLog(cplp->name + tr(" не выкупается из ") + s);
            startMove();
        } else {
            cplp->stay = false;
            doc.giveToBank(doc.curPl, 10);
            emit sendToLog(cplp->name + tr(" выкупается из ") + s + tr(" за 10"));
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
                if (p->pos == 10) {         // такси
                    p->stay = false;
                    emit sendToLog(p->name + tr(" перемещается из такси"));
                    doc.go(i, 0, newPos - 3);
                    emit docChanged();
                } else if (p->pos == 30) {  // тюрьма
                    p->stay = false;
                    emit sendToLog(p->name + tr(" выходит из тюрьмы бесплатно"));
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
    emit sendToLog(tr("Игра начинается..."));
    emit changeScenePlayer(0);
    startTurn(0);
}

void CControl::endGame(quint8 pNu)
{
    emit sendToLog(doc.m_p[pNu].name + tr(" выиграл"));

}

void CControl::startTurn(quint8 pNu)
{
    if (!doc.setCurPlayer(pNu)) {
        emit sendToLog(tr("Не могу установить текущего игрока. СТОП."));
        return;
    }

    emit disEndOfTurn(pNu);
    emit disCubik(pNu);
    emit sendToLog(tr("Ходит ") + doc.m_p[pNu].name);
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
        emit sendToLog(tr("Невозможный EOT"));
        return;
    }

    emit sendToLog(doc.m_p[pNu].name + tr(" закончил ход"));
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
        } else if ((cplp->pos == 10 || cplp->pos == 30) && cplp->stay == true) {   // Такси или тюрьма
            if (doc.playerCapital(doc.curPl) > 10 && cplp->seq == 0)
                emit askStayTT(doc.curPl);
            else {
                emit sendToLog(cplp->name + tr(" не может выкупиться"));
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
            emit sendToLog(cplp->name + tr(" не имеет фирм"));
            startMove();
            return;
        }
        if (cplp->seq > 0) {
            emit sendToLog(cplp->name + tr(" не может продавать из-за секвестра"));
            startMove();
            return;
        }
        cplp->mustSellQues = true;
        emit askSell(doc.curPl);
        break;
    case Q_Lose:
        if (doc.playerOwnerCount(doc.curPl) == 0) {
            emit sendToLog(cplp->name + tr(" не имеет фирм"));
            startMove();
            return;
        }
        cplp->mustLoseQues = true;
        emit askLose(doc.curPl);
        break;
    case Q_LoseMon:
        if (doc.playerMonCount(doc.curPl) == 0) {
            emit sendToLog(cplp->name + tr(" не имеет монополий"));
            startMove();
            return;
        }
        cplp->mustLoseMonQues = true;
        emit askLoseMon(doc.curPl);
        break;
    case Q_LoseMez:
        if (doc.playerMezonCount(doc.curPl) == 0) {
            emit sendToLog(cplp->name + tr(" не имеет мезонов"));
            startMove();
            return;
        }
        cplp->mustLoseMeson = true;
        emit askLoseMezon(doc.curPl);
        break;
    case Q_Change:
        if (doc.playerExpensiveFirm(doc.curPl) == 0 || doc.cheapestFreeFirm(doc.curPl) == 0 ||
                doc.playerExpensiveFirm(doc.curPl)->price <= doc.cheapestFreeFirm(doc.curPl)->price) {
            emit sendToLog(cplp->name + tr(" не имеет фирм для обмена"));
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
    case Q_Choose:
        cplp->mustChoose = true;
        emit askChoose(doc.curPl);
        break;
    }
}
