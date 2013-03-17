#include <QApplication>
#include <QGraphicsSceneMouseEvent>

#include "monscene.h"

#define Field_Width 51
#define Field_Height 35

#define Flag_Width 24
#define Flag_Height 18
#define Flag_X_At_Field 9
#define Flag_Y_At_Field 5

#define Work_Type_Width 24
#define Work_Type_Height 24
#define Work_Type_X_At_Field 5
#define Work_Type_Y_At_Field 7

#define Firm_Width 157
#define Firm_Height 176

#define Player_Size 12

#define Center_W 27
#define Center_H 21

#define VArr_W 27
#define VArr_H 31

#define HArr_W 37
#define HArr_H 25

QImage FieldPict[8];
QImage FlagPict[11];
QImage WorkTypePict[13];
QImage FirmPict;
QImage BuyPct[2];
QImage SelPct[2];
QImage InvPct[2];
QImage ResPct[2];
QImage RightPict[4];
QImage P_RightPict[4];
QImage LeftPict[4];
QImage P_LeftPict[4];
QImage UpPict[4];
QImage P_UpPict[4];
QImage DownPict[4];
QImage P_DownPict[4];
QImage CenterPict[4];
QImage P_CenterPict[4];

QColor PlColor[5] = {
        QColor(255, 0, 0),
        QColor(0, 0, 255),
        QColor(255, 255, 0),
        QColor(0, 255, 0),
        QColor(12632256)};

QString Cuntry[11] = {
        "HLV",
        "HEL",
        "ITA",
        "HUN",
        "GER",
        "FRA",
        "BLK",
        "JAP",
        "ENG",
        "USA",
        "RUS"};

extern quint16 QPaneWidth;
extern quint16 QPaneHeight;

MonScene::MonScene(QObject *parent) :
    QGraphicsScene(parent)
{
    setBackgroundBrush(QBrush(QColor(255,255,255), QPixmap("bg.jpg")));
//    setBackgroundBrush(QBrush(QColor(0,128,0)));
    scenePlayer = 4;
    showFirmMode = MF_NO;
    sceneMoveMode = MCM_NO;
    lastCI = -1;
}

void MonScene::init(QGraphicsView *main, CDoc *d)
{
    mainW = main;
    doc = d;
    preparePics();

    for (int i=0; i<57; i++) {
        CFirm *pf = doc->getFirm(i);
        QImage img = QImage(FieldPict[pf->m_type]);
        if (pf->m_type == F_Firm) {
            QPainter p(&img); // ������� ��������� � ���������, ��� �������� ����� �� img
            p.drawImage(QPoint(Flag_X_At_Field, Flag_Y_At_Field), FlagPict[pf->m_flag]);
            p.drawImage(QPoint(Work_Type_X_At_Field, Work_Type_Y_At_Field), WorkTypePict[pf->m_pict]);
            if (pf->owner != 4) {
                p.setPen(PlColor[pf->owner]);
                p.drawRect(1, 1, Field_Width - 3, Field_Height - 3);
            }
            p.end();
        }

        field[i] = addPixmap(QPixmap::fromImage(img));
//        field[i]->setCursor(Qt::PointingHandCursor);
        QPointF np;
        QRect r = GetFieldRect(i);
        np.setX(r.x());
        np.setY(r.y());
        field[i]->setData(0, 1);
        field[i]->setData(1, i);

        field[i]->setPos(np);
//        field[i]->translate(np.x(), np.y());
    }
//field[36]->setPixmap(QPixmap::fromImage(FieldPict[F_Ques]));

    int x = 585;
    int y = 90;
    int w = 120;
    int h = 60;
    for (int i=0; i<doc->nu_Players; i++) {
        QImage img = QImage(w, h, QImage::Format_RGB32);
        img.fill(QColor(255, 255, 255));
        QPainter p(&img);
        p.setPen(QColor(0, 0, 0));
        p.drawRect(0, 0, w - 1, h - 1);
        p.end();
        score[i] = addPixmap(QPixmap::fromImage(img));
        QPointF np;
        np.setX(x);
        np.setY(y);
        score[i]->setPos(np);
        y += 70;
    }
    showScore();

    w = Player_Size;
    h = Player_Size;
    for (int i=0; i<doc->nu_Players; i++) {
        QImage img = QImage(w, h, QImage::Format_RGB32);
        img.fill(QColor(0, 255, 255));
        QPainter p(&img);
        p.setPen(QColor(0, 0, 0));
        p.setBrush(PlColor[i]);
        p.drawEllipse(0, 0, w, h);
        p.end();
        pl[i] = addPixmap(QPixmap::fromImage(makeTranparant(img)));
        pl[i]->setData(0, 2);
        pl[i]->setData(1, i);
        pl[i]->hide();
    }
    showPlayers();

    te = new QTextEdit(main);
    te->move(5, 403);
    te->setReadOnly(true);
    te->setFixedSize(775, 120);
    te->show();

    cbp = new CPictButton(main);
    cbp->initButton(CenterPict, P_CenterPict, Center_W, Center_H);
    cbp->setDisabled(true);
    cbp->hide();

    lbp = new CPictButton(main);
    lbp->initButton(LeftPict, P_LeftPict, HArr_W, HArr_H, true);
    lbp->setDisabled(true);
    lbp->hide();

    rbp = new CPictButton(main);
    rbp->initButton(RightPict, P_RightPict, HArr_W, HArr_H, true);
    rbp->setDisabled(true);
    rbp->hide();

    ubp = new CPictButton(main);
    ubp->initButton(UpPict, P_UpPict, VArr_W, VArr_H, true);
    ubp->setDisabled(true);
    ubp->hide();

    dbp = new CPictButton(main);
    dbp->initButton(DownPict, P_DownPict, VArr_W, VArr_H, true);
    dbp->setDisabled(true);
    dbp->hide();

    endTurn = new QPushButton(main);
    endTurn->setText(tr("End"));
    endTurn->move(590, 10);
    endTurn->setDisabled(true);

    endMode = new QPushButton(main);
    endMode->setText(tr("�������� �������"));
    endMode->move(590, 377);
    endMode->setDisabled(false);
    endMode->hide();

    pbp = new QPushButton(main);
    pbp->setText(tr("���������� �� �������"));
    pbp->move(590, 373);
    pbp->setDisabled(false);
    pbp->hide();

    cubik = new CCubik(main);
    cubik->setText(tr("Cub"));
    cubik->move(590, 50);
    cubik->setDisabled(true);

    fvp = new FirmView(main, Qt::FramelessWindowHint, doc);
    fvp->hide();

    qPane = new CQPane(main, Qt::FramelessWindowHint);
    qPane->hide();

    fPane = new CFirmsPane(main, Qt::FramelessWindowHint);
    fPane->init(doc);
    fPane->hide();

    QObject::connect(fPane, SIGNAL(viewFirm(int)),
                     this, SLOT(showFirm(int)));
    QObject::connect(fvp, SIGNAL(showMonPane()),
                     this, SLOT(showMonPane()));


    QObject::connect(fvp, SIGNAL(loseMezon(int)),
                     this, SLOT(loseMezon(int)));
    QObject::connect(fvp, SIGNAL(buyFirm(int)),
                     this, SLOT(buyFirm(int)));
    QObject::connect(fvp, SIGNAL(sellFirm(int)),
                     this, SLOT(sellFirm(int)));
    QObject::connect(fvp, SIGNAL(investFirm(int)),
                     this, SLOT(investFirm(int)));
    QObject::connect(fvp, SIGNAL(takeFirm(int)),
                     this, SLOT(takeFirm(int)));
    QObject::connect(cubik, SIGNAL(cubikClicked(int)),
                     this, SLOT(cubikDropped(int)));
    QObject::connect(endTurn, SIGNAL(clicked()),
                     this, SLOT(EOTPressed()));
    QObject::connect(endMode, SIGNAL(clicked()),
                     this, SLOT(EOMPressed()));
    QObject::connect(pbp, SIGNAL(clicked()),
                     this, SLOT(PBPPressed()));
//    updateInfo();
}

MonScene::~MonScene()
{
    delete doc;
    delete fvp;
    delete qPane;
    delete fPane;
    delete endTurn;
    delete endMode;
    delete pbp;
    delete cubik;
    delete te;
    delete lbp;
    delete rbp;
    delete ubp;
    delete dbp;
    delete cbp;
}

void MonScene::preparePics(void){
    QImage OriginalPicture = QImage("Main.bmp");
    for (int i=0; i<8; i++) {
        FieldPict[i] = OriginalPicture.copy(QRect(1, 289 - (i * (Field_Height + 1)), Field_Width, Field_Height));
    }
    for (int i=0; i<11; i++) {
        FlagPict[i] = OriginalPicture.copy(QRect(54, 307 - (i * (Flag_Height + 1)), Flag_Width, Flag_Height));
    }
    for (int i=0; i<13; i++) {
        WorkTypePict[i] = makeTranparant(OriginalPicture.copy(QRect(79, 301 - (i * (Work_Type_Height + 1)), Work_Type_Width, Work_Type_Height)));
    }
    FirmPict = OriginalPicture.copy(QRect(104, 149, Firm_Width, Firm_Height));
    for (int i=0; i<2; i++) {
        BuyPct[i] = makeTranparant(OriginalPicture.copy(QRect(355, 305 - (i * (20 + 1)), 20, 20)));
    }
    for (int i=0; i<2; i++) {
        SelPct[i] = makeTranparant(OriginalPicture.copy(QRect(376, 305 - (i * (20 + 1)), 20, 20)));
    }
    for (int i=0; i<2; i++) {
        InvPct[i] = makeTranparant(OriginalPicture.copy(QRect(334, 305 - (i * (20 + 1)), 20, 20)));
    }
    for (int i=0; i<2; i++) {
        ResPct[i] = makeTranparant(OriginalPicture.copy(QRect(313, 305 - (i * (20 + 1)), 20, 20)));
    }
    for (int i=0; i<4; i++) {
        RightPict[i] = makeTranparant(OriginalPicture.copy(QRect(397, 300 - (i * (HArr_H + 1)), HArr_W, HArr_H)));
    }
    for (int i=0; i<4; i++) {
        P_RightPict[i] = makeTranparant(OriginalPicture.copy(QRect(397, 196 - (i * (HArr_H + 1)), HArr_W, HArr_H)));
    }
    for (int i=0; i<4; i++) {
        LeftPict[i] = makeTranparant(OriginalPicture.copy(QRect(435, 300 - (i * (HArr_H + 1)), HArr_W, HArr_H)));
    }
    for (int i=0; i<4; i++) {
        P_LeftPict[i] = makeTranparant(OriginalPicture.copy(QRect(435, 196 - (i * (HArr_H + 1)), HArr_W, HArr_H)));
    }
    for (int i=0; i<4; i++) {
        DownPict[i] = makeTranparant(OriginalPicture.copy(QRect(473, 294 - (i * (VArr_H + 1)), VArr_W, VArr_H)));
    }
    for (int i=0; i<4; i++) {
        P_DownPict[i] = makeTranparant(OriginalPicture.copy(QRect(473, 166 - (i * (VArr_H + 1)), VArr_W, VArr_H)));
    }
    for (int i=0; i<4; i++) {
        UpPict[i] = makeTranparant(OriginalPicture.copy(QRect(501, 294 - (i * (VArr_H + 1)), VArr_W, VArr_H)));
    }
    for (int i=0; i<4; i++) {
        P_UpPict[i] = makeTranparant(OriginalPicture.copy(QRect(501, 166 - (i * (VArr_H + 1)), VArr_W, VArr_H)));
    }
    for (int i=0; i<4; i++) {
        CenterPict[i] = makeTranparant(OriginalPicture.copy(QRect(529, 304 - (i * (Center_H + 1)), Center_W, Center_H)));
    }
    for (int i=0; i<4; i++) {
        P_CenterPict[i] = makeTranparant(OriginalPicture.copy(QRect(529, 216 - (i * (Center_H + 1)), Center_W, Center_H)));
    }
}

QImage MonScene::makeTranparant(QImage src)
{
    QImage mask = src.createMaskFromColor(0xff00ffff, Qt::MaskOutColor);
    src.setAlphaChannel(mask);
    return src;
}

QRect MonScene::GetFieldRect(int pos)
{
    int x, y;
    if (pos < 11) {
        x = pos * (Field_Width + 1);
        y = 0;
    } else if (pos < 21) {
        x = 10 * (Field_Width + 1);
        y = (pos - 10) * (Field_Height + 1);
    } else if (pos < 31) {
        x = (30 - pos) * (Field_Width + 1);
        y = 10 * (Field_Height + 1);
    } else if (pos < 40) {
        x = 0;
        y = (40 - pos) * (Field_Height + 1);
    } else if (pos < 49) {
        x = (pos - 39) * (Field_Width + 1);
        y = 5 * (Field_Height + 1);
    } else if (pos < 53) {
        x = 5 * (Field_Width + 1);
        y = (pos - 48) * (Field_Height + 1);
    } else {
        x = 5 * (Field_Width + 1);
        y = (pos - 47) * (Field_Height + 1);
    }
    return QRect(x, y, Field_Width, Field_Height);
}

void MonScene::showPlayers(void)
{
    for (quint8 i=0; i<doc->nu_Players; i++)
        if (! doc->m_p[i].active)
            pl[i]->hide();

    QHash<int,int> hash;
    for (int i=0; i<doc->getActivePlayers(); i++) {
        quint8 cp = doc->getNextPlayer(i);
        quint8 pole = doc->m_p[cp].pos;
        quint8 others = hash.value(pole, 0);
        QRect rect = GetFieldRect(pole);
        int x = rect.right() - Player_Size;
        int y = rect.y();
        y += others * 5;
        if (others == 0) {
            hash.insert(pole, 1);
        } else {
            hash.insert(pole, ++others);
        }
        QPointF np;
        np.setX(x);
        np.setY(y);
        pl[cp]->setPos(np);
        pl[cp]->setZValue(10 - i);
        pl[cp]->show();
    }
}

void MonScene::showScore(void)
{
    for (int i=0; i<doc->nu_Players; i++) {
        QPixmap img = score[i]->pixmap();
        QPainter p(&img);
        int w = img.width();
        int h = img.height();
        CPlayer *pl = &doc->m_p[i];
        if (pl->active)
            p.fillRect(1, 1, w-2, h-2, QColor(255, 255, 255));
        else
            p.fillRect(1, 1, w-2, h-2, QColor(128, 128, 128));
        QFont fn = QFont();
        fn.setBold(1);
        p.setFont(fn);
        p.setPen(PlColor[i]);
        QString tmpStr, stateStr1, stateStr2, stateStr3;
        if (pl->seq > 0) {
            tmpStr.setNum(pl->seq);
            stateStr1 += tr(" ���: ") + tmpStr;
        }
        if (pl->pbp > 0) {
            tmpStr.setNum(pl->pbp);
            stateStr1 += tr(" ��: ") + tmpStr;
        }
        if (pl->pbq > 0) {
            tmpStr.setNum(pl->pbq);
            stateStr1 += tr(" ���: ") + tmpStr;
        }
        if (pl->turnToStart > 0) {
            tmpStr.setNum(pl->turnToStart);
            stateStr2 += tr(" ��.�.������: ") + tmpStr;
        }
        if (pl->plusStart != 0) {
            tmpStr.setNum(abs(pl->plusStart));
            if (pl->plusStart > 0)
                stateStr3 += tr(" +st: ") + tmpStr;
            else
                stateStr3 += tr(" -st: ") + tmpStr;
        }
        tmpStr = pl->money.toString();
        p.drawText(QRect(1, 1, w-2, h-2), Qt::AlignCenter, pl->name + tr(": ") + pl->money.toString()  + "\n" + stateStr1 + "\n" + stateStr2 + "\n" + stateStr3);
        p.end();
        score[i]->setPixmap(img);
    }
}

void MonScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        QGraphicsItem *item = itemAt(mouseEvent->scenePos());
        if (item && item->data(0).toInt() == 1) {
            int a = item->data(1).toInt();
            quint8 t = doc->m_f[a].m_type;
//            qDebug() << t << a << doc->m_p[scenePlayer].pos;
            if (t == F_Firm) {
                showFirm(a, mouseEvent->scenePos());
                qPane->hide();
            } else if ((t == F_3Ques || t == F_Ques) && doc->m_p[scenePlayer].pos == a) {
                fvp->hide();
                qPane->show();
            } else {
                fvp->hide();
            }
        } else if (item && item->data(0).toInt() == 2) {
            if (sceneMoveMode != MCM_NO) {
                int a = item->data(1).toInt();
                if (a == scenePlayer) {
                    mouseStartPoint = mouseEvent->scenePos();
                    fishkaStartPoint = item->pos();
                }
            }
        } else {
            fvp->hide();
            qPane->hide();
            fPane->hide();
        }
    }
    if (mouseEvent->button() == Qt::RightButton) {
        fvp->hide();
        qPane->hide();
        fPane->hide();
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void MonScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (sceneMoveMode != MCM_NO && mouseEvent->buttons() & Qt::LeftButton) {
        QGraphicsItem *item = itemAt(mouseEvent->scenePos());
        if (item && item->data(0).toInt() == 2) {
            int a = item->data(1).toInt();
            if (a == scenePlayer) {
                QPointF p = mouseEvent->scenePos() - mouseStartPoint;
                if (p.manhattanLength() < QApplication::startDragDistance())
                    return;
                fishkaMoving = true;
                qPane->hide();

                QPointF r = fishkaStartPoint + p;
                if (r.rx() < 0)
                    r.setX(0);
                if (r.ry() < 0)
                    r.setY(0);
                qreal maxW = GetFieldRect(20).right() - item->boundingRect().width();
                qreal maxH = GetFieldRect(20).bottom() - item->boundingRect().height();
                if (r.rx() > maxW)
                    r.setX(maxW);
                if (r.ry() > maxH)
                    r.setY(maxH);

                item->setPos(r);

                QList<QGraphicsItem *> cil = item->collidingItems();
                if (cil.size() == 1 && cil[0]->data(0).toInt() == 1 && isValidForMoving(cil[0]->data(1).toInt())) {
                    int ciNu = cil[0]->data(1).toInt();
                    if (lastCI != -1 && lastCI != ciNu) {
                        field[lastCI]->setOpacity(.5);
                        lastCI = ciNu;
                        field[ciNu]->setOpacity(1);
                    } else if (lastCI == -1) {
                        lastCI = ciNu;
                        field[ciNu]->setOpacity(1);
                    }
                } else {
                    if (lastCI != -1) {
                        field[lastCI]->setOpacity(.5);
                        lastCI = -1;
                    }
                }
            }
        }
    }
}

void MonScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton && fishkaMoving) {
        fishkaMoving = false;
        if (lastCI != -1) {
            emit movedToPole(scenePlayer, lastCI);
            for (quint8 i=0; i<57; i++)
                field[i]->setOpacity(1);
            pl[scenePlayer]->unsetCursor();
            sceneMoveMode = MCM_NO;
        } else {
            showPlayers();
        }
    }
}

bool MonScene::isValidForMoving(quint8 pos)
{
    switch (sceneMoveMode) {
    case MCM_NO:
        return false;
    case MCM_MOVE_PIREFERIC:
        return doc->inPireferic(pos);
    case MCM_MOVE_CREST:
        return doc->inCrest(pos);
    }
}

void MonScene::showFirm(int fNu, QPointF point)
{
    fPane->hide();

    if (point == QPointF(0,0)) {
        QPoint p = GetFieldRect(fNu).center();
        point = mainW->mapToScene(p);
    }

    if (doc->m_f[fNu].m_type == F_Firm) {
        qreal sceneWidth = width();
        qreal sceneHeight = height();
        qreal pointX = point.rx();
        qreal pointY = point.ry();
        if (pointX + Firm_Width >= sceneWidth)
            pointX -= Firm_Width;
        if (pointY + Firm_Height >= sceneHeight)
            pointY -= Firm_Height;
        fvp->move(QPointF(pointX, pointY).toPoint());
        fvp->showFirm(fNu, scenePlayer, showFirmMode);
    }
}

void MonScene::recivedShowFirm(int pl)
{
    if (pl == scenePlayer)
        showFirm(doc->m_p[pl].pos);
}

void MonScene::setScenePlayer(int pl)
{
    scenePlayer = pl;
    mode = 0;
    fvp->hide();
    pbp->hide();
    endTurn->setStyleSheet("background: " + PlColor[pl].name());
    updateInfo();
}

void MonScene::addToLog(QString str)
{
    te->append(str);
}

void MonScene::takeFirm(int fNu)
{
    emit pressedTakeFirm(scenePlayer, fNu);
}

void MonScene::investFirm(int fNu)
{
    CFirm *f = &doc->m_f[fNu];
    QString name = f->name;
    if (f->mz[0].type == 3) {
        emit pressedInvestFirm(scenePlayer, fNu, 0);
        return;
    }
    CMezon *mz = &f->mz[f->cur_mz];
    quint8 t = mz->type;
    showFirmType = 1;
    if (t == 0 || doc->m_p[scenePlayer].canInvest)
        emit pressedInvestFirm(scenePlayer, fNu, 0);
    else if (t == 1) {
        fvp->hide();
        addToLog(tr("�� ������ ���������������� ") + name + tr(". ���������� �� ����� �����."));
        showFirmMode = MF_LOSE_FIRM;
        buyingFirm = fNu;
        endMode->show();
    } else if (t == 2) {
        fvp->hide();
        addToLog(tr("�� ������ ���������������� ") + name + tr(". ���������� �� ����� ����� �� ���������."));
        showFirmMode = MF_LOSE_MON;
        buyingFirm = fNu;
        endMode->show();
    }
}

void MonScene::buyFirm(int fNu)
{
    quint8 t = doc->m_f[fNu].type;
    QString name = doc->m_f[fNu].name;
    showFirmType = 0;
    if (t == 0)
        emit pressedBuyFirm(scenePlayer, fNu, 0);
    else if (t == 1) {
        fvp->hide();
        addToLog(tr("�� ������ ������ ") + name + tr(". ���������� �� ����� �����."));
        showFirmMode = MF_LOSE_FIRM;
        buyingFirm = fNu;
        endMode->show();
    } else if (t == 2) {
        fvp->hide();
        addToLog(tr("�� ������ ������ ") + name + tr(". ���������� �� ����� ����� �� ���������."));
        showFirmMode = MF_LOSE_MON;
        buyingFirm = fNu;
        endMode->show();
    }
}

void MonScene::loseMezon(int fNu)
{
    qDebug() << tr("MonScene::loseMezon");
    emit pressedLoseMezon(scenePlayer, fNu);
}

void MonScene::sellFirm(int fNu)
{
    qDebug("showFirmMode: %d", showFirmMode);
    if (showFirmMode == MF_NO || showFirmMode == MF_NO_LM) {
        emit pressedSellFirm(scenePlayer, fNu);
    } else if (showFirmMode == MF_LOSE_FIRM || showFirmMode == MF_LOSE_MON) {
        if (showFirmType == 0)
            emit pressedBuyFirm(scenePlayer, buyingFirm, fNu);
        else if (showFirmType == 1)
            emit pressedInvestFirm(scenePlayer, buyingFirm, fNu);
    }
    hideEndMode();
}

void MonScene::updateAll(void)
{
    for (int i=0; i<57; i++) {
        if (doc->getFirm(i)->m_type == F_Firm)
            showFirmOwnership(i);
    }
    updateInfo();
}

void MonScene::updateFirm(int fNu)
{
    if (doc->getFirm(fNu)->m_type == F_Firm)
        showFirmOwnership(fNu);
    updateInfo();
}

void MonScene::updateInfo(void)
{
    showScore();
    showPlayers();
    fvp->update();
    CPlayer *p = &doc->m_p[scenePlayer];
    if (p->pbp > 0 && p->lastPay.exist())
        pbp->show();
    else
        pbp->hide();
    fPane->update();
}

void MonScene::showFirmOwnership(quint8 fNu)
{
    CFirm *pf = doc->getFirm(fNu);
    if (pf->m_type != F_Firm)
        return;
    QPixmap img = field[fNu]->pixmap();
    QPainter p(&img);
    p.setPen(PlColor[pf->owner]);
    p.drawRect(1, 1, Field_Width - 3, Field_Height - 3);
    p.end();
    field[fNu]->setPixmap(img);
}

void MonScene::askDirection(int pl)
{
    if (pl != scenePlayer)
        return;

    showFirmMode = MF_NO;
    showDirChoose(doc->m_p[pl].pos, 0);
    addToLog(tr("�������� �����������"));
}

void MonScene::askCubik(int pl)
{
    if (pl != scenePlayer)
        return;

    showFirmMode = MF_NO;
    cubik->setEnabled(true);

    if(mode == 0)
        addToLog(tr("������� �����"));
    else if (mode == 1) {
        qPane->show();
        addToLog(tr("������� ����� � ������ ���"));
    } else if (mode == 2) {
        qPane->show();
        addToLog(tr("������� ����� �� ������ ���"));
    }
}

void MonScene::askStay(int pl)
{
    if (pl != scenePlayer)
        return;

    showFirmMode = MF_NO;
    quint8 dir = doc->getDir();
    showDirChoose(doc->m_p[pl].pos, dir);
    addToLog(tr("��������� ��� �������?"));
}

void MonScene::askStayTT(int pl)
{
    if (pl != scenePlayer)
        return;

    showFirmMode = MF_NO;
    quint8 dir = doc->getDir();
    showDirChoose(doc->m_p[pl].pos, dir);
    addToLog(tr("�����������?"));
}

void MonScene::askSell(int pl)
{
    if (pl != scenePlayer)
        return;

    showFirmMode = MF_NO;
    cubik->setEnabled(false);
    endTurn->setEnabled(false);
    addToLog(tr("������� ����� �����"));
}

void MonScene::askLose(int pl)
{
    if (pl != scenePlayer)
        return;

    showFirmMode = MF_NO;
    cubik->setEnabled(false);
    endTurn->setEnabled(false);
    addToLog(tr("��������� ����� �����"));
}

void MonScene::askLoseMon(int pl)
{
    if (pl != scenePlayer)
        return;

    showFirmMode = MF_NO_LM;
    cubik->setEnabled(false);
    endTurn->setEnabled(false);
    addToLog(tr("��������� ����� ����� �� ���������"));
}

void MonScene::askLoseMezon(int pl)
{
    if (pl != scenePlayer)
        return;

    showFirmMode = MF_NO_MEZ;
    cubik->setEnabled(false);
    endTurn->setEnabled(false);
    addToLog(tr("������� ����� � ����� �����"));
}

void MonScene::askSellSomething(int pl)
{
    if (pl != scenePlayer)
        return;

    showFirmMode = MF_NO;
    cubik->setEnabled(false);
    endTurn->setEnabled(false);
    addToLog(tr("�������� ������ ��� ������ �� ������"));
}

void MonScene::askQuestion(int pl)
{
    if (pl != scenePlayer)
        return;

    showFirmMode = MF_NO;
    mode = 1;
    qPane->clear();
    qPane->show();
    askCubik(pl);
}

void MonScene::askMoveToPireferic(int player)
{
    if (player != scenePlayer)
        return;

    addToLog(tr("����������� ����� �� ����� ������ �� ���������"));
    sceneMoveMode = MCM_MOVE_PIREFERIC;
    for (quint8 i=0; i<57; i++)
        if (doc->inPireferic(i))
            field[i]->setOpacity(.5);
    pl[player]->setCursor(Qt::PointingHandCursor);
}

void MonScene::askMoveToCrest(int player)
{
    if (player != scenePlayer)
        return;

    addToLog(tr("����������� ����� �� ����� ������ �� ������"));
    sceneMoveMode = MCM_MOVE_CREST;
    for (quint8 i=0; i<57; i++)
        if (doc->inCrest(i))
            field[i]->setOpacity(.5);
    pl[player]->setCursor(Qt::PointingHandCursor);
}

#define HZ_SHIFT 2
void MonScene::showDirChoose(quint8 pos, quint8 dir)
{
    QRect rect = GetFieldRect(pos);
    QPoint CP = rect.center();

    int LX = CP.x() - Center_W/2 - 1 - HArr_W;
    int LY = CP.y() - HArr_H/2;

    int RX = CP.x() + Center_W/2 + 2;
    int RY = CP.y() - HArr_H/2;

    int UX = CP.x() - VArr_W/2;
    int UY = CP.y() - Center_H/2 - 1 - VArr_H;

    int DX = CP.x() - VArr_W/2;
    int DY = CP.y() + Center_H/2 + 2;

    int CX = CP.x() - Center_W/2;
    int CY = CP.y() - Center_H/2;

    if (dir == D_Center)
        cbp->setDisabled(true);
    else
        cbp->setDisabled(false);
    lbp->setDisabled(false);
    rbp->setDisabled(false);
    ubp->setDisabled(false);
    dbp->setDisabled(false);

    cbp->setColor(doc->curPl);
    lbp->setColor(doc->curPl);
    rbp->setColor(doc->curPl);
    ubp->setColor(doc->curPl);
    dbp->setColor(doc->curPl);

    QPoint pc = mainW->mapFromScene(CX+HZ_SHIFT, CY+HZ_SHIFT);
    QPoint pd = mainW->mapFromScene(DX+HZ_SHIFT, DY+HZ_SHIFT);
    QPoint pu = mainW->mapFromScene(UX+HZ_SHIFT, UY+HZ_SHIFT);
    QPoint pl = mainW->mapFromScene(LX+HZ_SHIFT, LY+HZ_SHIFT);
    QPoint pr = mainW->mapFromScene(RX+HZ_SHIFT, RY+HZ_SHIFT);
    cbp->move(pc);
    dbp->move(pd);
    lbp->move(pl);
    rbp->move(pr);
    ubp->move(pu);

    QObject::connect(lbp, SIGNAL(clicked()),
                     this, SLOT(leftPressed()));
    QObject::connect(rbp, SIGNAL(clicked()),
                     this, SLOT(rightPressed()));
    QObject::connect(ubp, SIGNAL(clicked()),
                     this, SLOT(upPressed()));
    QObject::connect(dbp, SIGNAL(clicked()),
                     this, SLOT(downPressed()));
    QObject::connect(cbp, SIGNAL(clicked()),
                     this, SLOT(centerPressed()));

    cbp->show();
    if (dir == D_Center || dir == D_Left)
        lbp->show();
    if (dir == D_Center || dir == D_Right)
        rbp->show();
    if (dir == D_Center || dir == D_Up)
        ubp->show();
    if (dir == D_Center || dir == D_Down)
        dbp->show();
}

/*
 * exept - ����� ������� ��������:
 * 0 - ��������
 * 1 - �����, 2 - ������, � �.�.
 * 5 - ��� �������
 */
void MonScene::clearDirBut(quint8 exept)
{
    hideEndMode();

    if (exept != D_Center)
        cbp->hide();
    if (exept != D_Left)
        lbp->hide();
    if (exept != D_Up)
        ubp->hide();
    if (exept != D_Right)
        rbp->hide();
    if (exept != D_Down)
        dbp->hide();

    cbp->setDisabled(true);
    lbp->setDisabled(true);
    rbp->setDisabled(true);
    ubp->setDisabled(true);
    dbp->setDisabled(true);

    QObject::disconnect(lbp, SIGNAL(clicked()), 0, 0);
    QObject::disconnect(rbp, SIGNAL(clicked()), 0, 0);
    QObject::disconnect(ubp, SIGNAL(clicked()), 0, 0);
    QObject::disconnect(dbp, SIGNAL(clicked()), 0, 0);
    QObject::disconnect(cbp, SIGNAL(clicked()), 0, 0);
}

void MonScene::hideEndMode(void)
{
    endMode->hide();
    showFirmMode = MF_NO;
}

void MonScene::leftPressed(void)
{
    CPlayer *cp = doc->getCurPlayer();
    if (cp->crestDir == 0) {
        clearDirBut(D_Left);
        emit replayDir(D_Left);
    } else {
        clearDirBut(5);
        emit replayStay(D_Left);
    }
}

void MonScene::rightPressed(void)
{
    CPlayer *cp = doc->getCurPlayer();
    if (cp->crestDir == 0) {
        clearDirBut(D_Right);
        emit replayDir(D_Right);
    } else {
        clearDirBut(5);
        emit replayStay(D_Right);
    }
}

void MonScene::upPressed(void)
{
    CPlayer *cp = doc->getCurPlayer();
    if (cp->crestDir == 0) {
        clearDirBut(D_Up);
        emit replayDir(D_Up);
    } else {
        clearDirBut(5);
        emit replayStay(D_Up);
    }
}

void MonScene::downPressed(void)
{
    CPlayer *cp = doc->getCurPlayer();
    if (cp->crestDir == 0) {
        clearDirBut(D_Down);
        emit replayDir(D_Down);
    } else {
        clearDirBut(5);
        emit replayStay(D_Down);
    }
}

void MonScene::centerPressed(void)
{
    clearDirBut(5);
    emit replayStay(D_Center);
}

void MonScene::cubikDropped(int rnd)
{
    fvp->hide();
    clearDirBut(D_Nothing);

    QString s;
    s.setNum(rnd);
    if (mode == 0) {
        qPane->hide();
        emit replayCubik(rnd);
    } else if (mode == 1) {
        addToLog(tr("������ ") + s);
        cubik1 = rnd;
        mode = 2;
        qPane->selectRow(rnd);
        askCubik(scenePlayer);
    } else if (mode == 2) {
        qPane->show();
        addToLog(tr("������ ") + s);
        mode = 0;
        qPane->selectPos(cubik1, rnd);
        QPair<quint8,quint8> pair;
        pair.first = cubik1;
        pair.second = rnd;
        emit droppedQuestion(scenePlayer, pair);
    }
}

void MonScene::EOTPressed(void)
{
    qPane->hide();
    fvp->hide();
    clearDirBut(D_Nothing);
    emit pressedEndOfTurn(scenePlayer);
}

void MonScene::EOMPressed(void)
{
    showFirmMode = MF_NO;
    endMode->hide();
}

void MonScene::PBPPressed(void)
{
    pbp->hide();
    emit pressedPBP(scenePlayer);
}

void MonScene::showMonPane(void)
{
    fPane->show();
}

void MonScene::enaEndOfTurn(int pl)
{
    if (pl != scenePlayer)
        return;

    endTurn->setEnabled(true);
}

void MonScene::disEndOfTurn(int pl)
{
    if (pl != scenePlayer)
        return;

    endTurn->setEnabled(false);
}

void MonScene::disCubik(int pl)
{
    if (pl != scenePlayer)
        return;

    cubik->setEnabled(false);
}
