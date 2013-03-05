#ifndef POLE_H
#define POLE_H

#include <QDebug>

#include <QObject>
#include <QList>
#include "cmoney.h"

enum
{
    F_HLV,
    F_HEL,
    F_ITA,
    F_HUN,
    F_GER,
    F_FRA,
    F_BLK,
    F_JAP,
    F_ENG,
    F_USA,
    F_URS
};

enum
{
    P_POR,
    P_RAZ,
    P_MED,
    P_TUR,
    P_RAD,
    P_PRD,
    P_INF,
    P_AVT,
    P_GAZ,
    P_ELK,
    P_NEF,
    P_KIN,
    P_AVI
};

enum
{
    D_Center,
    D_Left,
    D_Up,
    D_Right,
    D_Down,
    D_Nothing
};

enum FieldType
{
    F_3Ques = 7,
    F_Ques = 6,
    F_Pip = 5,
    F_Start = 4,
    F_Taxi = 3,
    F_Turma = 2,
    F_Birga = 1,
    F_Firm = 0
};

class CMezon : public QObject
{
    Q_OBJECT

public:
    quint8 type;             // ��� ������
    CMoney invest;           // ������ �������������
    CMoney income;           // ������� ��������
public:
    CMezon(QObject *parent = 0);
};

class CField : public QObject
{
    Q_OBJECT

public:
    quint8 m_type;
    CField(QObject *parent = 0);
protected:
};

class CMonopol;

class CFirm : public CField
{
public:
    quint8 m_pict;
    quint8 m_flag;    // ���� ��� �����
    quint8 owner;     // ������� ��������
    quint32 cur_mz;   // ���-�� ������������ �������
    quint8 type;      // ��� ����� �� �������
    QString name;   // ��������
    CMoney price;    // ����
    CMoney binc;     // ��������� �����
    quint32 m_nu;     // ����� ���-�� �������
    CMezon mz[6];
    QList<CMonopol*> listMon;  // ������ ���������, � ������� ������
public:
    CFirm(CField *parent = 0);
    quint8 GetMultiplicator(void);
    CMoney GetCurIncome(void);
    void clear(void);
};

class CMonopol : public QObject
{
    Q_OBJECT

public:
    QString name;
    quint8 owner;
    QList<CFirm*> list;

    CMonopol(QObject *parent = 0);
protected:
};

#endif // POLE_H
