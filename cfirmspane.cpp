#include <QPainter>
#include "cfirmspane.h"

CFirmsPane::CFirmsPane(QWidget *parent) :
    QWidget(parent)
{
    setFixedSize(500, 300);
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setMargin(11);
    mainLayout->setSpacing(6);

    for (int i=0; i<17; i++) {
        vL[i].setMargin(0);
        vL[i].setSpacing(0);
    }
    for (int i=0; i<4; i++) {
        hL[i].setMargin(0);
        hL[i].setSpacing(0);
    }

    QFont font;
    font.setPointSize(8);
    QFont fontMon;
    fontMon = font;
    fontMon.setUnderline(true);


    for (int j=0; j<17; j++) {
        m[j].setFont(fontMon);
        m[j].setAlignment(Qt::AlignCenter);
        for (int i=0; i<5; i++) {
            f[j][i].setFont(font);
            f[j][i].setMargin(2);
        }
    }

/*
    mainLayout->addLayout(&hL[0], 0, 0);
    mainLayout->addLayout(&hL[1], 1, 0);
    mainLayout->addLayout(&hL[2], 0, 1);
    mainLayout->addLayout(&hL[3], 1, 1);

    for (int i=0; i<4; i++)
        hL[0].addLayout(&vL[i]);
    for (int i=4; i<9; i++)
        hL[1].addLayout(&vL[i]);
    for (int i=9; i<13; i++)
        hL[2].addLayout(&vL[i]);
    for (int i=13; i<17; i++)
        hL[3].addLayout(&vL[i]);
*/

    for (int i=0; i<4; i++)
        mainLayout->addLayout(&vL[i], 0, i);
    mainLayout->addLayout(&vL[4], 0, 5);
    for (int i=5; i<11; i++)
        mainLayout->addLayout(&vL[i], 1, i - 5);
    for (int i=11; i<17; i++)
        mainLayout->addLayout(&vL[i], 2, i - 11);
}

void CFirmsPane::init(CDoc * d)
{
    doc = d;

    addLabels(0, 8);
    addLabels(1, 9);
    addLabels(2, 10);
    addLabels(3, 11);
    addLabels(4, 12);
    addLabels(5, 13);
    addLabels(6, 14);
    addLabels(7, 16);
    addLabels(8, 17);
    addLabels(9, 19);
    addLabels(10, 20);
    addLabels(11, 21);
    addLabels(12, 22);
    addLabels(13, 23);
    addLabels(14, 24);
    addLabels(15, 25);
    addLabels(16, 26);

}

void CFirmsPane::addLabels(quint8 lNu, quint8 mNu)
{
    m[lNu].setText(doc->m_m[mNu].name);
    vL[lNu].addWidget(&m[lNu]);
    for (int j=0; j<doc->m_m[mNu].list.size(); j++) {
        f[lNu][j].setText(doc->m_m[mNu].list[j]->name);
        vL[lNu].addWidget(&f[lNu][j]);
    }
    vL[lNu].addStretch(1);
}

void CFirmsPane::paintEvent(QPaintEvent *event)
{

    int w = width();
    int h = height();
    QPainter p(this);
//    p.drawImage(0, 0, Pict);
    QColor c = QColor(192, 192, 192);
    p.fillRect(0, 0, w, h, c);
    p.setPen(QColor(222, 222, 222));
    p.drawLine(0, 0, 0, h - 2);
    p.drawLine(0, 0, w - 2, 0);
    p.setPen(QColor(255, 255, 255));
    p.drawLine(1, 1, 1, h - 3);
    p.drawLine(1, 1, w - 3, 1);
    p.setPen(QColor(128, 128, 128));
    p.drawLine(1, h - 2, w - 2, h - 2);
    p.drawLine(w - 2, 1, w - 2, h - 2);
    p.setPen(QColor(0, 0, 0));
    p.drawLine(w - 1, 0, w - 1, h - 1);
    p.drawLine(0, h - 1, w - 1, h - 1);
//    p.drawImage(4, 4, innerPict);

    QWidget::paintEvent(event);
}
