#ifndef CQPANE_H
#define CQPANE_H

#include <QWidget>
#include <QImage>
#include <QMouseEvent>
#include "cmovewidget.h"
#include "cpictbutton.h"

enum {
    QP_ORD,     // ������� ����
    QP_CHOOSE,  // ����� ������
    QP_PB       // ����� �� �����
};

class CQField : public QWidget
{
    Q_OBJECT
public:
    explicit CQField(QWidget *parent = 0, Qt::WindowFlags f = 0, quint8 r = 0, quint8 c = 0);
    void setPict(QImage i);
    void setSelected(bool s);
    void setToOrdinadyMode(void);
    void setToChooseMode(void);
protected:
    void paintEvent(QPaintEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    quint8 row;
    quint8 col;
    QImage Pict;
    QImage innerPict;
    bool selected;
    quint8 mode;
    QPoint mousePressPoint;

    QImage makeTranparant(QImage src);
    bool test(quint8 r, quint8 c);
signals:
    void choose(int r, int c);
public slots:

};

class CQPane : public CMoveWidget
{
    Q_OBJECT
public:
    explicit CQPane(QWidget *parent = 0, Qt::WindowFlags f = 0);
    void clear(void);
    void selectRow(quint8 r);
    void selectPos(quint8 r, quint8 c);
    void setToOrdinadyMode(void);
    void setToChooseMode(void);
    void setToPBMode(void);
    ~CQPane();
protected:
    void paintEvent(QPaintEvent *event);
private:
    QImage MainPict;
    QImage BottomPict;
    CQField *Field[6][6];
    CPictButton *reject;
    CPictButton *accept;
    quint8 row;
    quint8 col;
    quint8 mode;

    void preparePics(void);

signals:
    void choose(int r, int c);
    void qAccept(bool res);

private slots:
    void choosePriv(int r, int c);
    void acceptButton(void);
    void rejectButton(void);

public slots:
    
};

#endif // CQPANE_H
