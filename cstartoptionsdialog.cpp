#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include "cstartoptionsdialog.h"
#include "ui_cstartoptionsdialog.h"

QColor VariantColor[11] = {
    QColor(255, 0, 0),
    QColor(0, 0, 255),
    QColor(255, 255, 0),
    QColor(0, 255, 0),
    QColor(255, 0, 255),
//    QColor(0, 255, 255),
    QColor(127, 0, 0),
    QColor(0, 0, 127),
    QColor(127, 127, 0),
    QColor(0, 127, 0),
    QColor(127, 0, 127),
    QColor(0, 127, 127)
};

CStartOptionsDialog::CStartOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CStartOptionsDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Монополия"));

    ui->spinBox->setValue(4);

    for (quint8 i=0; i<4; i++) {
//        qDebug() << i;
        cbc[i] = new QComboBox(this);
        ui->gridLayout->addWidget(cbc[i], i, 0);
        le[i] = new QLineEdit(this);
        ui->gridLayout->addWidget(le[i], i, 1);
        cb[i] = new QComboBox(this);
        ui->gridLayout->addWidget(cb[i], i, 2);
        lb[i] = new QLabel(this);
        ui->gridLayout->addWidget(lb[i], i, 3);

        int w = 20;
        int h = 20;
        cbc[i]->setIconSize(QSize(w, h));
        cbc[i]->setFrame(false);
//        cbc[i]->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
//        cbc[i]->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
        for (quint8 j=0; j<11; j++) {
            QImage img = QImage(w, h, QImage::Format_RGB32);
            img.fill(VariantColor[j]);
            QPainter p(&img);
            p.setPen(QColor(255, 255, 255));
            p.drawRect(0, 0, w - 1, h - 1);
            p.end();

            QPixmap pm = QPixmap::fromImage(img);
            cbc[i]->addItem(pm, tr(""));
        }
        cbc[i]->setCurrentIndex(i);

        cb[i]->addItem(tr("Игрок"));
        cb[i]->addItem(tr("Компьютер"));
        cb[i]->addItem(tr("Сеть"));

        lb[i]->setText(tr("Готов"));

        QObject::connect(cb[i], SIGNAL(currentIndexChanged(int)),
                         this, SLOT(chkReady()));
        QObject::connect(le[i], SIGNAL(textChanged(QString)),
                         this, SLOT(chkReady()));
    }

    le[0]->setText(tr("Красный"));
    le[1]->setText(tr("Синий"));
    le[2]->setText(tr("Желтый"));
    le[3]->setText(tr("Зеленый"));

    QObject::connect(ui->spinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(changeSpin(int)));
}

CStartOptionsDialog::~CStartOptionsDialog()
{
    for (quint8 i=0; i<4; i++) {
        delete le[i];
        delete cb[i];
        delete cbc[i];
        delete lb[i];
    }
    delete ui;
}

CGameOpt CStartOptionsDialog::getGameOpt(void)
{
    CGameOpt go;
    go.plNum = ui->spinBox->value();
    for (quint8 i=0; i<go.plNum; i++) {
        go.po[i].name = le[i]->text();
        go.po[i].color = VariantColor[cbc[i]->currentIndex()];
    }
    return go;
}

bool CStartOptionsDialog::chkReady(void)
{
    bool res = true;
    for (quint8 i=0; i<4; i++) {
        if (! chkStringReady(i))
            res = false;
    }
    if (res)
        ui->startButton->setEnabled(true);
    else
        ui->startButton->setEnabled(false);
    return res;
}

bool CStartOptionsDialog::chkStringReady(quint8 line)
{
    if (! cb[line]->isEnabled()) {
        cbc[line]->setDisabled(true);
        le[line]->setDisabled(true);
        lb[line]->setText("");
        return true;
    }
    cbc[line]->setDisabled(false);
    le[line]->setDisabled(false);

    if (le[line]->text() == tr("")) {
        lb[line]->setText(tr("Ошибка"));
        return false;
    }

    if (cb[line]->currentIndex() == 0 || cb[line]->currentIndex() == 1) {
        lb[line]->setText(tr("Готов"));
        return true;
    }

    if (cb[line]->currentIndex() == 2) {
        le[line]->setDisabled(true);
        lb[line]->setText(tr("Ждем"));
        return false;
    }
}

void CStartOptionsDialog::changeSpin(int val)
{
    for (quint8 i=0; i<4; i++) {
        if (i < val) {
            cb[i]->setEnabled(true);
        } else {
            cb[i]->setEnabled(false);
        }
    }
    chkReady();
}
