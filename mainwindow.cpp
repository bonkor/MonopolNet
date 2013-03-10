#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Монополия"));
    ctrl = new CControl;
    scene = new MonScene;
    scene->setSceneRect(0,0,780,530);
//    scene->setSceneRect(0,0,791,541);
    ui->graphicsView->setScene(scene);
    scene->init(ui->graphicsView, ctrl->getDoc());

    QObject::connect(ctrl, SIGNAL(changeScenePlayer(int)),
                     scene, SLOT(setScenePlayer(int)));
    QObject::connect(ctrl, SIGNAL(sendToLog(QString)),
                     scene, SLOT(addToLog(QString)));
    QObject::connect(ctrl, SIGNAL(docChanged()),
                     scene, SLOT(updateAll()));
    QObject::connect(ctrl, SIGNAL(docInfoChanged()),
                     scene, SLOT(updateInfo()));
    QObject::connect(ctrl, SIGNAL(docFirmChanged(int)),
                     scene, SLOT(updateFirm(int)));
    QObject::connect(ctrl, SIGNAL(askDirection(int)),
                     scene, SLOT(askDirection(int)));
    QObject::connect(ctrl, SIGNAL(askCubik(int)),
                     scene, SLOT(askCubik(int)));
    QObject::connect(ctrl, SIGNAL(askStay(int)),
                     scene, SLOT(askStay(int)));
    QObject::connect(ctrl, SIGNAL(showNewPoleForPlayer(int)),
                     scene, SLOT(recivedShowFirm(int)));
    QObject::connect(ctrl, SIGNAL(enaEndOfTurn(int)),
                     scene, SLOT(enaEndOfTurn(int)));
    QObject::connect(ctrl, SIGNAL(disEndOfTurn(int)),
                     scene, SLOT(disEndOfTurn(int)));
    QObject::connect(ctrl, SIGNAL(disCubik(int)),
                     scene, SLOT(disCubik(int)));
    QObject::connect(ctrl, SIGNAL(askSellSomething(int)),
                     scene, SLOT(askSellSomething(int)));
    QObject::connect(ctrl, SIGNAL(askStayTT(int)),
                     scene, SLOT(askStayTT(int)));
    QObject::connect(ctrl, SIGNAL(askQuestion(int)),
                     scene, SLOT(askQuestion(int)));
    QObject::connect(ctrl, SIGNAL(askSell(int)),
                     scene, SLOT(askSell(int)));
    QObject::connect(ctrl, SIGNAL(askLose(int)),
                     scene, SLOT(askLose(int)));
    QObject::connect(ctrl, SIGNAL(askLoseMon(int)),
                     scene, SLOT(askLoseMon(int)));

    QObject::connect(scene, SIGNAL(pressedInvestFirm(int,int,int)),
                     ctrl, SLOT(tryInvestFirm(int,int,int)));
    QObject::connect(scene, SIGNAL(pressedTakeFirm(int,int)),
                     ctrl, SLOT(tryTakeFirm(int,int)));
    QObject::connect(scene, SIGNAL(pressedBuyFirm(int,int,int)),
                     ctrl, SLOT(tryBuyFirm(int,int,int)));
    QObject::connect(scene, SIGNAL(pressedSellFirm(int,int)),
                     ctrl, SLOT(trySellFirm(int,int)));
    QObject::connect(scene, SIGNAL(replayDir(int)),
                     ctrl, SLOT(replayDir(int)));
    QObject::connect(scene, SIGNAL(replayCubik(int)),
                     ctrl, SLOT(replayCubik(int)));
    QObject::connect(scene, SIGNAL(replayStay(int)),
                     ctrl, SLOT(replayStay(int)));
    QObject::connect(scene, SIGNAL(pressedEndOfTurn(int)),
                     ctrl, SLOT(endOfTurn(int)));
    QObject::connect(scene, SIGNAL(pressedPBP(int)),
                     ctrl, SLOT(tryPBP(int)));
    QObject::connect(scene, SIGNAL(droppedQuestion(int,QPair<quint8,quint8>)),
                     ctrl, SLOT(droppedQuestion(int,QPair<quint8,quint8>)));

    ctrl->startGame();
}

MainWindow::~MainWindow()
{
    delete scene;
    delete ctrl;
    delete ui;
}
