#ifndef CPICTBUTTON_H
#define CPICTBUTTON_H

#include <QPushButton>
#include <QImage>

class CPictButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CPictButton(QWidget *parent = 0);
    void initButton(QImage i1, QImage i2, int w1, int h1);
    void initButton(QImage *pi1, QImage *pi2, int w1, int h1, bool disPressed = false);
    void setColor(quint8 cl);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QImage enImage;
    QImage disImage;
    QImage upImage[4];
    QImage downImage[4];
    bool useArr;
    int color;
    int w;
    int h;
    bool dP;
    void drawBorder(QPainter *p);
    void drawArr(QPainter *p);

signals:
    
public slots:
    
};

#endif // CPICTBUTTON_H
