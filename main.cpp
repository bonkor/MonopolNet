#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QTextCodec>
#include "mainwindow.h"
#include "cstartdialog.h"
#include "cstartoptionsdialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("KOI8-R"));

    int w = a.desktop()->screenGeometry().width();
    int h = a.desktop()->screenGeometry().height();
    w /= 2;
    h /= 2;

    CStartDialog sd;
    int wSd = w - sd.width()/2;
    int hSd = h - sd.height()/2;
    sd.move(wSd, hSd);

    MainWindow wnd;
    int wMw = w - wnd.width()/2;
    int hMw = h - wnd.height()/2;
    wnd.move(wMw, hMw);

    CStartOptionsDialog sod;
    int wSo = w - sod.width()/2;
    int hSo = h - sod.height()/2;
    sod.move(wSo, hSo);

    int res;
    do {
        res = sd.exec();
        switch (res) {
        case CSD_CREATE_GAME:
            if (sod.exec() == QDialog::Accepted) {

            } else
                res = CSD_BACK;
            break;
        case CSD_CONNECT_GAME:
            break;
        case CSD_QUIT:
            a.quit();
            return 0;
            break;
        }
    } while (res == CSD_BACK);

    wnd.setGameOpt(sod.getGameOpt());
    wnd.show();
    return a.exec();
}
