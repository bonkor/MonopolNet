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
    Q_LoseMon,  // потеря моноп. после вопроса
    Q_Lose,     // потеря после вопроса
    Q_LoseMez   // снять мезон после вопроса
};

class CLP
{
public:
    QList<quint8> pls;
    CMoney sum;

    CLP();
    void clear(void);
    bool exist(void);
    void set(quint8 p, CMoney s);
    void set(QList<quint8> &l, CMoney s);
};

class CPlayer : public QObject
{
public:
    explicit CPlayer(QObject *parent = 0);
    quint8 getNextAction(void);
    void insertToQueue(QueueType q, quint8 nu = 1); // добавить nu действий (немедленно)
    void addMove(int mv);   // добавить mv ходов вперед/назад

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
    bool mustLoseMonQues;  // должен потерять монополию после вопроса
    bool mustLoseMeson; // должен снять мезон после вопроса
    quint8 pbp;         // кол-во ПБ-шек на платеж
    quint8 pbq;         // кол-во ПБ-шек на вопрошалку
    quint8 turnToStart; // свернуть к старту
    qint8 plusStart;    // +st/-st
    QString name;
    CLP lastPay;        // последний платеж
    QHash<quint8, CFirm*> hash;     // список фирм
    QList<CMonopol*> listMon;       // список монополий
    QList<quint8> queue;   // очередь предстоящих действий
};

#endif // CPLAYER_H
