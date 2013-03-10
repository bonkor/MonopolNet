#include <QPainter>
#include <QMouseEvent>

#include "firmview.h"

#define Flag_Width 24
#define Flag_Height 18

#define Work_Type_Width 24
#define Work_Type_Height 24

#define Firm_Width 157
#define Firm_Height 176

extern QImage FlagPict[11];
extern QImage WorkTypePict[13];
extern QImage FirmPict;
extern QColor PlColor[5];
extern QString Cuntry[11];
extern QImage BuyPct[2];
extern QImage SelPct[2];
extern QImage InvPct[2];
extern QImage ResPct[2];

FirmView::FirmView(QWidget *parent, Qt::WindowFlags f, CDoc *d) :
    QWidget(parent, f)
{
    doc = d;

    setFixedSize(Firm_Width, Firm_Height);
    bl = new CPictButton(this);
    bl->initButton(InvPct[0], InvPct[1], 43, 24);
    bl->move(8, 144);
    bl->setDisabled(true);
    bl->show();

    bc1 = new CPictButton(this);
    bc1->initButton(BuyPct[0], BuyPct[1], 43, 24);
    bc1->move(57, 144);
    bc1->setDisabled(true);
    bc1->show();

    bc2 = new CPictButton(this);
    bc2->initButton(SelPct[0], SelPct[1], 43, 24);
    bc2->move(57, 144);
    bc2->setDisabled(true);
    bc2->hide();

    br = new CPictButton(this);
    br->initButton(ResPct[0], ResPct[1], 43, 24);
    br->move(106, 144);
    br->setDisabled(true);
    br->show();

    QObject::connect(bc1, SIGNAL(clicked()), this, SLOT(buyClicked()));
    QObject::connect(bc2, SIGNAL(clicked()), this, SLOT(sellClicked()));
    QObject::connect(bl, SIGNAL(clicked()), this, SLOT(investClicked()));
    QObject::connect(br, SIGNAL(clicked()), this, SLOT(takeClicked()));
}

FirmView::~FirmView()
{
    delete bl;
    delete bc1;
    delete bc2;
    delete br;
}

void FirmView::showFirm(int fNu, quint8 pl, quint8 m)
{
    nu = fNu;
    curPl = pl;
    mode = m;
    fp = doc->getFirm(nu);
    if (fp->m_type != F_Firm)
        return;
    show();
}

void FirmView::mousePressEvent(QMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::RightButton) {
        hide();
    }
}

void FirmView::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.drawImage(0, 0, FirmPict);
    p.drawImage(17, 11, FlagPict[fp->m_flag]);
    p.drawImage(117, 6, WorkTypePict[fp->m_pict]);
    p.fillRect(9, 38, 139, 3, PlColor[fp->owner]);
    p.fillRect(9, 69, 139, 3, PlColor[fp->owner]);
    QFont fn = QFont();
    fn.setBold(1);
    p.setFont(fn);
    p.drawText(QRect(60, 11, 39, 20), Qt::AlignCenter, Cuntry[fp->m_flag]);
    p.drawText(QRect(8, 43, 141, 26), Qt::AlignCenter, fp->name);
    QString tmpStr;
    tmpStr = fp->price.toString();
    if (fp->type == 1)
        tmpStr += '*';
    if (fp->type == 2)
        tmpStr += "**";
    p.drawText(QRect(10, 80, 39, 20), Qt::AlignCenter, tmpStr);
    RedrawMesons(&p);
    setButtonsState();
}

void FirmView::setButtonsState(void)
{
    if (mode == MF_NO) {
        if (doc->canInvest(curPl, nu))
            bl->setDisabled(false);
        else
            bl->setDisabled(true);
        if (doc->canTake(curPl, nu))
            br->setDisabled(false);
        else
            br->setDisabled(true);
        if (doc->canSell(curPl, nu)) {
            bc1->hide();
            bc2->setDisabled(false);
            bc2->show();
        } else if (doc->canBuy(curPl, nu)) {
            bc2->hide();
            bc1->setDisabled(false);
            bc1->show();
        } else {
            bc2->hide();
            bc1->setDisabled(true);
            bc1->show();
        }
    } else if  (mode == MF_LOSE_FIRM) {
        bl->setDisabled(true);
        br->setDisabled(true);
        bc1->setDisabled(true);
        if (fp->owner == curPl) {
            bc1->hide();
            bc2->show();
            bc2->setDisabled(false);
        } else  {
            bc2->hide();
            bc1->show();
            bc1->setDisabled(true);
        }
    } else if  (mode == MF_LOSE_MON) {
        bl->setDisabled(true);
        br->setDisabled(true);
        bc1->setDisabled(true);
        if (fp->owner == curPl && fp->GetMultiplicator() > 1) {
            bc1->hide();
            bc2->show();
            bc2->setDisabled(false);
        } else  {
            bc2->hide();
            bc1->show();
            bc1->setDisabled(true);
        }
    }
}

void FirmView::RedrawMesons(QPainter *p)
{
    p->fillRect(108, 80, 39, 20, PlColor[4]);
    p->fillRect(59, 80, 39, 20, PlColor[4]);
    p->fillRect(2, 102, 153, 42, PlColor[4]);

    CMoney inc = fp->GetCurIncome();
    QString tmpStr = inc.toString();
    p->drawText(QRect(108, 80, 39, 20), Qt::AlignCenter, tmpStr);

    quint8 multi = fp->GetMultiplicator();
    tmpStr.setNum(multi);
    p->drawText(QRect(59, 80, 39, 20), Qt::AlignCenter, "x" + tmpStr);

    QRect Pos[6][6] = {
            QRect(57, 114, 49, 20), QRect(), QRect(), QRect(), QRect(), QRect(),
            QRect(32, 114, 49, 20), QRect(82, 114, 49, 20), QRect(), QRect(), QRect(), QRect(),
            QRect(8, 114, 49, 20), QRect(57, 114, 49, 20), QRect(106, 114, 49, 20), QRect(), QRect(), QRect(),
            QRect(32, 105, 49, 20), QRect(82, 105, 49, 20), QRect(22, 124, 59, 20), QRect(82, 124, 59, 20), QRect(), QRect(),
            QRect(8, 105, 49, 20), QRect(57, 105, 49, 20), QRect(106, 105, 49, 20), QRect(32, 124, 49, 20), QRect(82, 124, 49, 20), QRect(),
            QRect(8, 105, 49, 20), QRect(57, 105, 49, 20), QRect(106, 105, 49, 20), QRect(4, 124, 49, 20), QRect(57, 124, 49, 20), QRect(106, 124, 49, 20)
    };
    for (quint32 i=0; i<fp->m_nu; i++) {
        if (fp->mz[i].invest != 0)
            tmpStr = fp->mz[i].invest.toString();
        else
            tmpStr = "";
        switch (fp->mz[i].type) {
        case 0:
            tmpStr += "-";
            break;
        case 1:
            tmpStr += "*-";
            break;
        case 2:
            tmpStr += "**-";
            break;
        case 3:
            tmpStr += "- +";
        }
        QString tmpStr1;
        tmpStr1 = fp->mz[i].income.toString();
        tmpStr += tmpStr1;
        if (i == fp->cur_mz - 1) {
            p->setPen(QColor(255, 0, 0));
        } else {
            p->setPen(QColor(0, 0, 0));
        }
        p->drawText(Pos[fp->m_nu-1][i], Qt::AlignCenter, tmpStr);
    }
}

void FirmView::buyClicked()
{
    emit buyFirm(nu);
}

void FirmView::sellClicked()
{
    emit sellFirm(nu);
}

void FirmView::investClicked()
{
    emit investFirm(nu);
}

void FirmView::takeClicked()
{
    emit takeFirm(nu);
}
