#ifndef CSTARTDIALOG_H
#define CSTARTDIALOG_H

#include <QDialog>

enum {
    CSD_QUIT,
    CSD_BACK,
    CSD_CREATE_GAME,
    CSD_CONNECT_GAME
};

namespace Ui {
class CStartDialog;
}

class CStartDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CStartDialog(QWidget *parent = 0);
    ~CStartDialog();
    
private:
    Ui::CStartDialog *ui;

signals:

private slots:
    void create(void);
    void connect(void);
    void quit(void);
};

#endif // CSTARTDIALOG_H
