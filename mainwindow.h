#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ccontrol.h"
#include "monscene.h"
#include "cgameopt.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setGameOpt(CGameOpt go);
    
private:
    Ui::MainWindow *ui;
    MonScene *scene;
    CControl *ctrl;
};

#endif // MAINWINDOW_H
