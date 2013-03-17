#ifndef CPLAYER_H
#define CPLAYER_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QString>
#include "pole.h"

enum QueueType
{
    Q_No,       // ������
    Q_Move,     // ���
    Q_Ques,     // ������
    Q_Sell,     // ������� ����� �������
    Q_LoseMon,  // ������ �����. ����� �������
    Q_Lose,     // ������ ����� �������
    Q_LoseMez   // ����� ����� ����� �������
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
    void insertToQueue(QueueType q, quint8 nu = 1); // �������� nu �������� (����������)
    void addMove(int mv);   // �������� mv ����� ������/�����

    bool active;        // ����� ������
    int pos;            // ��������� ������
    int stepsBack;      // ���-�� ����� �����
    CMoney money;       // ���-�� �����
    bool investComplit; // ������������� ������ ��� ��� ������ � �����
    int seq;            // ����� (������) ���������
    int crestDir;       // ����������� �� ������  1 - �����, 2 - �����, 3 - ������, 4 - ����, 0 - ���������
    bool canBuy;        // � ������� ������ "������"
    bool canInvest;     // � ������� ������ "��������� �����"
    bool stay;          // ����� � ������ ��� �����
    bool mustSellMode;  // ���� � �����. ������ ��� �� �������
    bool mustSellQues;  // ������ ��� �� ������� ����� �������
    bool mustLoseQues;  // ������ ��� �� �������� ����� �������
    bool mustLoseMonQues;  // ������ �������� ��������� ����� �������
    bool mustLoseMeson; // ������ ����� ����� ����� �������
    quint8 pbp;         // ���-�� ��-��� �� ������
    quint8 pbq;         // ���-�� ��-��� �� ����������
    quint8 turnToStart; // �������� � ������
    qint8 plusStart;    // +st/-st
    QString name;
    CLP lastPay;        // ��������� ������
    QHash<quint8, CFirm*> hash;     // ������ ����
    QList<CMonopol*> listMon;       // ������ ���������
    QList<quint8> queue;   // ������� ����������� ��������
};

#endif // CPLAYER_H
