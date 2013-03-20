#include <QDebug>
#include <QApplication>
#include <QPainter>
#include <QTime>
#include <QMouseEvent>
#include "cqpane.h"

quint16 QPaneWidth  = 327;
quint16 QPaneHeight = 231;

#define FW 51
#define FH 35

#define PW 43
#define PH 27

CQField::CQField(QWidget *parent, Qt::WindowFlags f, quint8 r, quint8 c) :
    QWidget(parent, f)
{
    row = r;
    col = c;
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

void CQField::setToOrdinadyMode(void)
{
    mode = QP_ORD;
    selected = false;
}

void CQField::setToChooseMode(void)
{
    mode = QP_CHOOSE;
    selected = false;
}

void CQField::enterEvent(QEvent *event)
{
//    qDebug() << tr("CQField::enterEvent");
    if (mode == QP_CHOOSE) {
        selected = true;
        update();
    }
}

void CQField::leaveEvent(QEvent *event)
{
//    qDebug() << tr("CQField::leaveEvent");
    if (mode == QP_CHOOSE) {
        selected = false;
        update();
    }
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

void CQField::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && mode == QP_CHOOSE) {
        mousePressPoint = event->globalPos();
    }
    QWidget::mousePressEvent(event);
}

void CQField::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && mode == QP_CHOOSE) {
        QPoint p = event->globalPos() - mousePressPoint;
        if (p.manhattanLength() < QApplication::startDragDistance())
            emit choose(row, col);
    }
    QWidget::mouseReleaseEvent(event);
}

QImage CQField::makeTranparant(QImage src)
{
    QImage mask = src.createMaskFromColor(0xffc0c0c0, Qt::MaskOutColor);
    src.setAlphaChannel(mask);
    return src;
}


CQPane::CQPane(QWidget *parent, Qt::WindowFlags f) :
    CMoveWidget(parent, f)
{
    mode = QP_ORD;
    setFixedSize(QPaneWidth, QPaneHeight);
    move(50, 50);
    for (int i=0; i<6; i++)
        for (int j=0; j<6; j++) {
            Field[i][j] = new CQField(this, Qt::FramelessWindowHint, i + 1, j + 1);
            QObject::connect(Field[i][j], SIGNAL(choose(int,int)),
                             this, SLOT(choosePriv(int,int)));
        }
    preparePics();

    Field[0][0]->setToolTip(tr("+10"));
    Field[0][1]->setToolTip(tr("-10"));
    Field[0][2]->setToolTip(tr("�������� � ������"));
    Field[0][3]->setToolTip(tr("� �����"));
    Field[0][4]->setToolTip(tr("������"));
    Field[0][5]->setToolTip(tr("�� �� ������"));
    Field[1][0]->setToolTip(tr("������"));
    Field[1][1]->setToolTip(tr("+20"));
    Field[1][2]->setToolTip(tr("-20"));
    Field[1][3]->setToolTip(tr("������ �����<br> ������ � �������"));
    Field[1][4]->setToolTip(tr("� ������"));
    Field[1][5]->setToolTip(tr("3 �� �� ������"));
    Field[2][0]->setToolTip(tr("��������� ���������<br> �����"));
    Field[2][1]->setToolTip(tr("�������"));
    Field[2][2]->setToolTip(tr("+30"));
    Field[2][3]->setToolTip(tr("-30"));
    Field[2][4]->setToolTip(tr("������ �� �����<br> �� ������"));
    Field[2][5]->setToolTip(tr("�� �����"));
    Field[3][0]->setToolTip(tr("5 ����� ������"));
    Field[3][1]->setToolTip(tr("����� �����"));
    Field[3][2]->setToolTip(tr("�������� �����"));
    Field[3][3]->setToolTip(tr("+50"));
    Field[3][4]->setToolTip(tr("���� �� 10"));
    Field[3][5]->setToolTip(tr("������ �� �����<br> �� ���������"));
    Field[4][0]->setToolTip(tr("��� ������� ����� �����<br> �������� �������������� +25"));
    Field[4][1]->setToolTip(tr("������� �� ???"));
    Field[4][2]->setToolTip(tr("�������� �����<br> �� ���������"));
    Field[4][3]->setToolTip(tr("���� (����� ������������)<br> �������� �� �����"));
    Field[4][4]->setToolTip(tr("�� ���� �� 10"));
    Field[4][5]->setToolTip(tr("���� �� 15"));
    Field[5][0]->setToolTip(tr("������� �����<br> �������"));
    Field[5][1]->setToolTip(tr("��� ������� ����� �����<br> �� �������� +25"));
    Field[5][2]->setToolTip(tr("5 ����� �����"));
    Field[5][3]->setToolTip(tr("5 ����� ���������"));
    Field[5][4]->setToolTip(tr("������� ���� �����<br> �� ������<br> (� ������� ���������)"));
    Field[5][5]->setToolTip(tr("�� ���� �� 15"));

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

void CQPane::choosePriv(int r, int c)
{
    if (mode == QP_CHOOSE) {
        setToOrdinadyMode();
        emit choose(r, c);
        selectPos(r, c);
    }
}

void CQPane::setToOrdinadyMode(void)
{
    mode = QP_ORD;
    for (int i=0; i<6; i++)
        for (int j=0; j<6; j++)
            Field[i][j]->setToOrdinadyMode();
}

void CQPane::setToChooseMode(void)
{
    mode = QP_CHOOSE;
    for (int i=0; i<6; i++)
        for (int j=0; j<6; j++)
            Field[i][j]->setToChooseMode();
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

    QTime dieTime=QTime::currentTime().addMSecs(300);
    while(QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
}
