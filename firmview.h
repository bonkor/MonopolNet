#ifndef FIRMVIEW_H
#define FIRMVIEW_H

#include <QWidget>
#include "cdoc.h"
#include "cpictbutton.h"

enum
{
    MF_NO,
    MF_NO_LM,       // выпало на вопросе потерять монополию
    MF_NO_MEZ,      // выпало на вопросе снять мезон
    MF_CHANGE_START,// выпало на вопросе поменять
    MF_CHANGE_END,  // выбор фирмы для отказа при обмене
    MF_LOSE_FIRM,
    MF_LOSE_MON     // потеря монополии для покупки type=2
};

class FirmView : public QWidget
{
    Q_OBJECT
public:
    explicit FirmView(QWidget *parent = 0, Qt::WindowFlags f = 0, CDoc *d = 0);
    void showFirm(int fNu, quint8 pl, quint8 m = 0, quint8 tFNu = 0);
    ~FirmView();
protected:
    void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *mouseEvent);
private:
    CDoc *doc;
    quint8 curPl;
    int nu;
    quint8 mode;
    quint8 toFirm;  // при обмене, на какую фирму меняем
    CFirm *fp;
    void RedrawMesons(QPainter *p);
    void setButtonsState(void);
    CPictButton *bl;
    CPictButton *blm;
    CPictButton *bc1;
    CPictButton *bc2;
    CPictButton *br;

private slots:
    void investClicked();
    void takeClicked();
    void buyClicked();
    void sellClicked();
    void loseMesonClicked();

signals:
    void loseMezon(int fNu);
    void investFirm(int fNu);
    void takeFirm(int fNu);
    void buyFirm(int fNu);
    void sellFirm(int fNu);
    void showMonPane(void);

public slots:
    
};

#endif // FIRMVIEW_H
