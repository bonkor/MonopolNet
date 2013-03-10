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
    Q_Lose      // ������ ����� �������
};

class CLP
{
public:
    int pl;         // -1 ���� �������
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
    void addMove(int mv);   // �������� mv ����� ������
    void addQues(int q);    // �������� q �������� (����������)
    void addSell(void);     // �������� ������������� ������� (����������)
    void addLose(void);     // �������� ������������� ������ (����������)

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
    quint8 pbp;         // ���-�� ��-��� �� ������
    quint8 pbq;         // ���-�� ��-��� �� ����������
    quint8 turnToStart; // �������� � ������
    QString name;
    CLP lastPay;        // ��������� ������
    QHash<quint8, CFirm*> hash;     // ������ ����
    QList<CMonopol*> listMon;       // ������ ���������
    QList<quint8> queue;   // ������� ����������� ��������
};

#endif // CPLAYER_H
