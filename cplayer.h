#ifndef CPLAYER_H
#define CPLAYER_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QString>
#include "pole.h"

enum QueueType
{
    Q_No,       // ничего
    Q_Move,     // ход
    Q_Ques,     // вопрос
    Q_Sell,     // продажа после вопроса
    Q_Lose      // потеря после вопроса
};

class CLP
{
public:
    int pl;         // -1 если сброшен
    CMoney sum;

    CLP() {pl = -1;};
    void clear(void) {pl = -1;};
    bool exist(void) {if (pl == -1) return false; else return true;};
    void set(quint8 p, CMoney s) {pl = p; sum = s;};
};

class CPlayer : public QObject
{
public:
    explicit CPlayer(QObject *parent = 0);
    quint8 getNextAction(void);
    void addMove(int mv);   // добавить mv ходов вперед
    void addQues(int q);    // добавить q вопросов (немедленно)
    void addSell(void);     // добавить необходимость продажи (немедленно)
    void addLose(void);     // добавить необходимость потери (немедленно)

    bool active;        // игрок играет
    int pos;            // положение игрока
    int stepsBack;      // кол-во ходов назад
    CMoney money;       // кол-во денег
    bool investComplit; // инвестировать нельзя или уже купили с биржи
    int seq;            // ходов (турнов) секвестра
    int crestDir;       // направление на кресте  1 - влево, 2 - вверх, 3 - вправо, 4 - вниз, 0 - невыбрано
    bool canBuy;        // в вопросе выпало "Купить"
    bool canInvest;     // в вопросе выпало "Поставить мезон"
    bool stay;          // попал в тюрьму или такси
    bool mustSellMode;  // ушел в минус. должен что то продать
    bool mustSellQues;  // должен что то продать после вопроса
    bool mustLoseQues;  // должен что то потерять после вопроса
    quint8 pbp;         // кол-во ПБ-шек на платеж
    quint8 pbq;         // кол-во ПБ-шек на вопрошалку
    quint8 turnToStart; // свернуть к старту
    QString name;
    CLP lastPay;        // последний платеж
    QHash<quint8, CFirm*> hash;     // список фирм
    QList<CMonopol*> listMon;       // список монополий
    QList<quint8> queue;   // очередь предстоящих действий
};

#endif // CPLAYER_H
