#include <QPainter>
#include <QMouseEvent>
#include "cqpane.h"

quint16 QPaneWidth  = 327;
quint16 QPaneHeight = 231;

#define FW 51
#define FH 35

#define PW 43
#define PH 27

CQField::CQField(QWidget *parent, Qt::WindowFlags f) :
    QWidget(parent, f)
{
    setFixedSize(FW, FH);
}

void CQField::setPict(QImage i)
{
    Pict = i;
    innerPict = makeTranparant(Pict.copy(QRect(4, 4, PW, PH)));
}

void CQField::setSelected(bool s)
{
    selected = s;
}

void CQField::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.drawImage(0, 0, Pict);
    QColor c;
    if (selected)
        c = QColor(128, 128, 128);
    else
        c = QColor(192, 192, 192);
    p.fillRect(4, 4, PW, PH, c);
    p.drawImage(4, 4, innerPict);
}

QImage CQField::makeTranparant(QImage src)
{
    QImage mask = src.createMaskFromColor(0xffc0c0c0, Qt::MaskOutColor);
    src.setAlphaChannel(mask);
    return src;
}


CQPane::CQPane(QWidget *parent, Qt::WindowFlags f) :
    QWidget(parent, f)
{
    setFixedSize(QPaneWidth, QPaneHeight);
    for (int i=0; i<6; i++)
        for (int j=0; j<6; j++)
            Field[i][j] = new CQField(this, Qt::FramelessWindowHint);
    preparePics();

    Field[0][0]->setToolTip(tr("+10"));
    Field[0][1]->setToolTip(tr("-10"));
    Field[0][2]->setToolTip(tr("Свернуть к старту"));
    Field[0][3]->setToolTip(tr("В такси"));
    Field[0][4]->setToolTip(tr("Вопрос"));
    Field[0][5]->setToolTip(tr("ПБ на вопрос"));
    Field[1][0]->setToolTip(tr("Купить"));
    Field[1][1]->setToolTip(tr("+20"));
    Field[1][2]->setToolTip(tr("-20"));
    Field[1][3]->setToolTip(tr("Встать между<br> фишкой и стартом"));
    Field[1][4]->setToolTip(tr("В тюрьму"));
    Field[1][5]->setToolTip(tr("3 ПБ на платеж"));
    Field[2][0]->setToolTip(tr("Бесплатно поставить<br> мезон"));
    Field[2][1]->setToolTip(tr("Продать"));
    Field[2][2]->setToolTip(tr("+30"));
    Field[2][3]->setToolTip(tr("-30"));
    Field[2][4]->setToolTip(tr("Встать на любую<br> из креста"));
    Field[2][5]->setToolTip(tr("На Биржу"));
    Field[3][0]->setToolTip(tr("5 ходов вперед"));
    Field[3][1]->setToolTip(tr("Снять мезон"));
    Field[3][2]->setToolTip(tr("Потерять фирму"));
    Field[3][3]->setToolTip(tr("+50"));
    Field[3][4]->setToolTip(tr("Всем по 10"));
    Field[3][5]->setToolTip(tr("Встать на любую<br> из перефирии"));
    Field[4][0]->setToolTip(tr("При проходе через СТАРТ<br> получить дополнительные +25"));
    Field[4][1]->setToolTip(tr("Перейти на ???"));
    Field[4][2]->setToolTip(tr("Потерять фирму<br> из монополии"));
    Field[4][3]->setToolTip(tr("Всем (кроме вопрошающего)<br> потерять по фирме"));
    Field[4][4]->setToolTip(tr("От всех по 10"));
    Field[4][5]->setToolTip(tr("Всем по 15"));
    Field[5][0]->setToolTip(tr("Выбрать любой<br> вариант"));
    Field[5][1]->setToolTip(tr("При проходе через СТАРТ<br> не получить +25"));
    Field[5][2]->setToolTip(tr("5 ходов назад"));
    Field[5][3]->setToolTip(tr("5 ходов секвестра"));
    Field[5][4]->setToolTip(tr("Сменять одну фирму<br> на другую<br> (с потерей стоимости)"));
    Field[5][5]->setToolTip(tr("От всех по 15"));

    clear();
    row = 2;
//    Field[3][2]->setSelected(true);
//    Field[3][4]->setSelected(true);
//    Field[3][5]->setSelected(true);
}

CQPane::~CQPane()
{
    for (int i=0; i<6; i++)
        for (int j=0; j<6; j++)
            delete Field[i][j];
}

void CQPane::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.drawImage(0, 0, MainPict);
}

void CQPane::preparePics(void){
    QImage OriginalPicture = QImage("Q.bmp");
    MainPict = OriginalPicture.copy(QRect(0, 0, QPaneWidth, QPaneHeight));
    for (int i=0; i<6; i++)
        for (int j=0; j<6; j++) {
            Field[i][j]->setPict(OriginalPicture.copy(QRect(8 + (FW + 1) * j, 8 + (FH + 1) * i, FW, FH)));
            Field[i][j]->move(8 + (FW + 1) * j, 8 + (FH + 1) * i);
            Field[i][j]->show();
        }
}

void CQPane::clear(void)
{
    for (int i=0; i<6; i++)
        for (int j=0; j<6; j++)
            Field[i][j]->setSelected(false);
    update();
}

void CQPane::selectRow(quint8 r)
{
    clear();
    for (int i=0; i<6; i++)
        Field[r - 1][i]->setSelected(true);
    update();
}

void CQPane::selectPos(quint8 r, quint8 c)
{
    clear();
    Field[r - 1][c - 1]->setSelected(true);
    update();
}
