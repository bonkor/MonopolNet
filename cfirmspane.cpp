#include <QApplication>
#include <QPainter>
#include "cfirmspane.h"

extern QColor PlColor[5];

CFLabel::CFLabel(QWidget *parent) :
    QLabel(parent)
{
    fNu = 0;
    setMargin(2);

    pal = palette();
    pal.setColor(QPalette::Base, QColor(192, 192, 192));
    setPalette(pal);
    setAutoFillBackground(true);
}

void CFLabel::init(QString s, quint8 nu, bool h)
{
    QFont font;
    font.setPointSize(8);
    font.setUnderline(h);
    setFont(font);

    setAlignment(Qt::AlignCenter);
    setText(s);
    header = h;

    fNu = nu;
    owner = 4;
}

void CFLabel::setOwner(quint8 o)
{
    if (owner == o)
        return;

    owner = o;

    if (owner == 4)
        pal.setColor(QPalette::Text, QColor(0, 0, 0));
    else
        pal.setColor(QPalette::Text, PlColor[owner]);
    setPalette(pal);
}

quint8 CFLabel::getFirmNu(void)
{
    return fNu;
}

void CFLabel::enterEvent(QEvent *event)
{
    if (header)
        return;

    pal.setColor(backgroundRole(), QColor(128, 128, 128));
    setPalette(pal);
}

void CFLabel::leaveEvent(QEvent *event)
{
    if (header)
        return;

    pal.setColor(backgroundRole(), QColor(192, 192, 192));
    setPalette(pal);
}

void CFLabel::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton) {
        mousePressPoint = event->globalPos();
    }
    QWidget::mousePressEvent(event);
}

void CFLabel::mouseReleaseEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton) {
        QPoint p = event->globalPos() - mousePressPoint;
        if (p.manhattanLength() < QApplication::startDragDistance())
            emit labClick(fNu);
    }
    QWidget::mouseReleaseEvent(event);
}

CFirmsPane::CFirmsPane(QWidget *parent, Qt::WindowFlags f) :
    CMoveWidget(parent, f)
{
    setFixedSize(550, 300);

    move(50, 50);

    mainLayout = new QGridLayout(this);
    mainLayout->setMargin(11);
    mainLayout->setSpacing(6);

    for (int i=0; i<17; i++) {
        vL[i].setMargin(0);
        vL[i].setSpacing(0);
    }

    for (int i=0; i<4; i++)
        mainLayout->addLayout(&vL[i], 0, i);
    mainLayout->addLayout(&vL[4], 0, 5);
    for (int i=5; i<11; i++)
        mainLayout->addLayout(&vL[i], 1, i - 5);
    for (int i=11; i<17; i++)
        mainLayout->addLayout(&vL[i], 2, i - 11);
}

CFirmsPane::~CFirmsPane()
{
    for (int i=0; i<17; i++) {
        vL[i].setParent(0);
    }
    delete mainLayout;
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
    m[lNu].init(doc->m_m[mNu].name, mNu, true);
    vL[lNu].addWidget(&m[lNu]);
    for (int j=0; j<doc->m_m[mNu].list.size(); j++) {
        CFirm *fp = doc->m_m[mNu].list[j];
        int i = 0;
        do i++; while (fp != &doc->m_f[i]);
        f[lNu][j].init(fp->name, i);
        vL[lNu].addWidget(&f[lNu][j]);
        QObject::connect(&f[lNu][j], SIGNAL(labClick(int)),
                         this, SLOT(labClick(int)));
    }
    vL[lNu].addStretch(1);
}

void CFirmsPane::update(void)
{
    for (quint8 i=0; i<17; i++) {
        quint8 nu = m[i].getFirmNu();
        m[i].setOwner(doc->m_m[nu].owner);
    }
    for (quint8 i=0; i<17; i++) {
        for (quint8 j=0; j<5; j++) {
            quint8 nu = f[i][j].getFirmNu();
            if (nu != 0)
                f[i][j].setOwner(doc->m_f[nu].owner);
        }
    }
}

void CFirmsPane::labClick(int nu)
{
    emit viewFirm(nu);
}

void CFirmsPane::paintEvent(QPaintEvent *event)
{
    int w = width();
    int h = height();
    QPainter p(this);
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

    QWidget::paintEvent(event);
}
