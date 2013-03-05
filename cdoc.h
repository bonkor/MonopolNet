#ifndef CDOC_H
#define CDOC_H

#include <QObject>
#include "pole.h"
#include "cplayer.h"

class CDoc : public QObject
{
    Q_OBJECT
public:
    explicit CDoc(QObject *parent = 0);
    CFirm *getFirm(quint8 fNu);
    quint8 getActivePlayers(void);
    quint8 getNextPlayer(int skip);
    bool canBuy(quint8 player, quint8 fNu);
    bool canSell(quint8 player, quint8 fNu);
    bool canInvest(quint8 player, quint8 fNu);
    bool canTake(quint8 player, quint8 fNu);
    CPlayer *getCurPlayer(void);
    quint8 getDir(void);
    bool inCrest(quint8 pos);
    bool go(quint8 pNu, quint8 st, int pos = -1);
    quint8 calculateNextPos(quint8 pNu, quint8 st);
    bool plusStart(quint8 oldPos, quint8 newPos);
    quint8 playerOwnerCount(quint8 pNu);
    quint8 playerMonCount(quint8 pNu);
    quint8 playersAtPoleExept(quint8 pNu, quint8 pos);
    CMoney playerSumCount(quint8 pNu);
    CMoney playerCapital(quint8 pNu);
    CMoney giveToBank(quint8 pNu, CMoney sum);
    void takeFromBank(quint8 pNu, CMoney sum);
    CMoney transferMoney(quint8 fromPl, quint8 toPl, CMoney sum);
    void sellAll(quint8 pNu);
    void clearLastPay(quint8 pNu);

    CFirm m_f[57];
    CPlayer m_p[4];
    CMonopol m_m[28];
    quint8 nu_Monopols;
    quint8 curPl;
    quint8 nu_Players;

private:
    void FillFields();

signals:
    void sendLog(QString);  // временно
    
public slots:
    bool buyFirm(int newOwner, int fNu, int flNu);
    bool sellFirm(int oldOwner, int fNu);
    bool loseFirm(int oldOwner, int fNu);
    bool investFirm(int owner, int fNu, int flNu);
    bool takeFirm(int owner, int fNu);
    bool setCurPlayer(int pNu);
};

#endif // CDOC_H
