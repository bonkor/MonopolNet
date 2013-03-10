#include <QPainter>
#include "cfirmspane.h"

CFirmsPane::CFirmsPane(QWidget *parent) :
    QWidget(parent)
{
    setFixedSize(500, 300);
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
}
