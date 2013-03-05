#include <QPainter>

#include "cpictbutton.h"

extern QColor PlColor[5];

CPictButton::CPictButton(QWidget *parent) :
    QPushButton(parent)
{
}

void CPictButton::initButton(QImage i1, QImage i2, int w1, int h1)
{
    useArr = false;
    enImage = i1;
    disImage = i2;
    w = w1;
    h = h1;
    setFixedSize(w, h);
}

void CPictButton::initButton(QImage *pi1, QImage *pi2, int w1, int h1, bool disPressed)
{
    useArr = true;
    for (int i=0; i<4; i++) {
        upImage[i] = pi1[i];
        downImage[i] = pi2[i];
    }
    w = w1;
    h = h1;
    dP = disPressed;
    dP = false;
    setFixedSize(w, h);
}

void CPictButton::setColor(quint8 cl)
{
    color = cl;
}

void CPictButton::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    if (useArr)
        drawArr(&p);
    else
        drawBorder(&p);
}

void CPictButton::drawArr(QPainter *p)
{
    if (isDown() || (dP && !isEnabled()))
        p->drawImage(0, 0, downImage[color]);
    else
        p->drawImage(0, 0, upImage[color]);

}

void CPictButton::drawBorder(QPainter *p)
{
    p->fillRect(0, 0, w, h, QBrush(PlColor[4]));
    if (isEnabled()) {
        int imgX = (w - enImage.width()) / 2;
        int imgY = (h - enImage.height()) / 2;
        if (isDown()) {
            imgX++;
            imgY++;
        }
        p->drawImage(imgX, imgY, enImage);
    } else {
        int imgX = (w - disImage.width()) / 2;
        int imgY = (h - disImage.height()) / 2;
        p->drawImage(imgX, imgY, disImage);
    }
    if (isDown()) {
        p->setPen(QColor(0, 0, 0));
        p->drawLine(0, 0, w-1, 0);
        p->drawLine(0, 0, 0, h-1);
        p->setPen(QColor(128, 128, 128));
        p->drawLine(1, h-2, 1, 1);
        p->drawLine(1, 1, 1, w-2);
        p->setPen(QColor(255, 255, 255));
        p->drawLine(1, h-1, w-1, h-1);
        p->drawLine(w-1, 1, w-1, h-1);
    } else {
        p->setPen(QColor(255, 255, 255));
        p->drawLine(0, 0, w-2, 0);
        p->drawLine(0, 0, 0, h-2);
        p->setPen(QColor(128, 128, 128));
        p->drawLine(1, h-2, w-2, h-2);
        p->drawLine(w-2, 1, w-2, h-2);
        p->setPen(QColor(0, 0, 0));
        p->drawLine(0, h-1, w-1, h-1);
        p->drawLine(w-1, 0, w-1, h-1);
    }

}
