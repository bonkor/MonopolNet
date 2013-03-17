#ifndef CCONTROL_H
#define CCONTROL_H

#include <QObject>
#include "cdoc.h"

class CControl : public QObject
{
    Q_OBJECT
public:
    explicit CControl(QObject *parent = 0);
    CDoc *getDoc(void);

private:
    CDoc doc;

    void movePlayer(quint8 st, int pos = -1);
    
signals:
    void sendToLog(QString);
    void changeScenePlayer(int pl);
    void docChanged(void);
    void docInfoChanged(void);
    void docFirmChanged(int fNu);
    void showNewPoleForPlayer(int pl);
    void askDirection(int fPu);
    void askCubik(int fPu);
    void askStay(int fPu);
    void askSell(int fPu);
    void askLose(int fPu);
    void askLoseMon(int fPu);
    void askLoseMezon(int fPu);
    void askStayTT(int fPu);
    void askSellSomething(int fPu);
    void askQuestion(int fPu);
    void askMoveToPirefiric(int fPu);
    void askMoveToCrest(int fPu);
    void askMoveBetween(int fPu);
    void enaEndOfTurn(int fPu);
    void disEndOfTurn(int fPu);
    void disCubik(int fPu);
public slots:
    void startGame(void);
    void endGame(quint8 pNu);
    void startTurn(quint8 pNu);
    void startMove(void);
    void tryInvestFirm(int pl, int fNu, int flNu = 0);
    void tryTakeFirm(int pl, int fNu);
    void tryLoseMezon(int pl, int fNu);
    void tryBuyFirm(int pl, int fNu, int flNu = 0);
    void trySellFirm(int pl, int fNu);
    void tryLoseFirm(int pl, int fNu);
    void tryLoseMon(int pl, int fNu);
    void tryMove(int pl, int fNu);
    void tryPBP(int pl);
    void replayDir(int dir);
    void replayStay(int dir);
    void replayCubik(int rnd);
    void endOfTurn(int pNu);
    void droppedQuestion(int pl, QPair<quint8,quint8> pair);
private slots:
    void toLog(QString str);  // ��������
};

#endif // CCONTROL_H
