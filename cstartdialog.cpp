#include <QApplication>
#include "cstartdialog.h"
#include "ui_cstartdialog.h"

CStartDialog::CStartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CStartDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Монополия"));

    QObject::connect(ui->createButton, SIGNAL(clicked()),
                    this , SLOT(create()));
    QObject::connect(ui->connectButton, SIGNAL(clicked()),
                    this , SLOT(connect()));
    QObject::connect(ui->quitButton, SIGNAL(clicked()),
                    this , SLOT(quit()));

}

CStartDialog::~CStartDialog()
{
    delete ui;
}

void CStartDialog::create(void)
{
    done(CSD_CREATE_GAME);
}

void CStartDialog::connect(void)
{
    done(CSD_CONNECT_GAME);
}

void CStartDialog::quit(void)
{
    done(CSD_QUIT);
}
