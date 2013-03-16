#ifndef MONSCENE_H
#define MONSCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QPushButton>
#include <QTextEdit>

#include "cdoc.h"
#include "firmview.h"
#include "cpictbutton.h"
#include "ccubik.h"
#include "cqpane.h"
#include "cfirmspane.h"

class MonScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit MonScene(QObject *parent = 0);
    ~MonScene();
    void init(QGraphicsView *main, CDoc *d);
private:
    QGraphicsView *mainW;
    CDoc *doc;
    QGraphicsPixmapItem *field[57];
    QGraphicsPixmapItem *score[4];
    QGraphicsPixmapItem *pl[4];
    FirmView *fvp;
    CQPane *qPane;
    CFirmsPane *fPane;
    QPushButton *endTurn;
    QPushButton *endMode;
    QPushButton *pbp;
    CCubik *cubik;
    CPictButton *lbp;
    CPictButton *rbp;
    CPictButton *ubp;
    CPictButton *dbp;
    CPictButton *cbp;
    QTextEdit *te;
    quint8 scenePlayer;
    quint8 showFirmMode;
    quint8 showFirmType;    // 0 - buy 1 - invest
    quint8 buyingFirm;      // фирма, с типом != 0 которую собираемся купить или инвестировать
    quint8 mode;            // мода. 0 - обычная 1 - вопрос (перывый бросок кубика) 2 - второй бросок
    quint8 cubik1;          // значение первого броска после моды 1

    void preparePics(void);
    QImage makeTranparant(QImage src);
    QRect GetFieldRect(int pos);
    void showScore(void);
    void showPlayers(void);
    void showFirmOwnership(quint8 fNu);
    void showDirChoose(quint8 pos, quint8 dir);
    void showStay(void);
    void clearDirBut(quint8 exept);
    void hideEndMode(void);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);

signals:
    void pressedInvestFirm(int pl, int fNu, int flNu);
    void pressedTakeFirm(int pl, int fNu);
    void pressedLoseMezon(int pl, int fNu);
    void pressedBuyFirm(int pl, int fNu, int flNu);
    void pressedSellFirm(int pl, int fNu);
    void replayDir(int dir);
    void replayStay(int dir);
    void replayCubik(int rnd);
    void pressedEndOfTurn(int pl);
    void pressedPBP(int pl);
    void droppedQuestion(int pl, QPair<quint8,quint8>);
public slots:
    void updateAll(void);
    void updateFirm(int fNu);
    void updateInfo(void);
    void recivedShowFirm(int pl);
    void showFirm(int fNu, QPointF point = QPointF(0,0));
    void setScenePlayer(int pl);
    void addToLog(QString str);
    void askDirection(int pl);
    void askCubik(int pl);
    void askStay(int pl);
    void askStayTT(int pl);
    void askSell(int pl);
    void askLose(int pl);
    void askLoseMon(int pl);
    void askLoseMezon(int pl);
    void askSellSomething(int pl);
    void askQuestion(int pl);
    void enaEndOfTurn(int pl);
    void disEndOfTurn(int pl);
    void disCubik(int pl);

private slots:
    void investFirm(int fNu);
    void loseMezon(int fNu);
    void takeFirm(int fNu);
    void buyFirm(int fNu);
    void sellFirm(int fNu);
    void leftPressed(void);
    void rightPressed(void);
    void downPressed(void);
    void upPressed(void);
    void centerPressed(void);
    void cubikDropped(int rnd);
    void EOTPressed(void);
    void EOMPressed(void);
    void PBPPressed(void);
    void showMonPane(void);
};

#endif // MONSCENE_H
