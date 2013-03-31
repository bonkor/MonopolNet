#ifndef CSTARTOPTIONSDIALOG_H
#define CSTARTOPTIONSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include "cgameopt.h"

namespace Ui {
class CStartOptionsDialog;
}

class CStartOptionsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CStartOptionsDialog(QWidget *parent = 0);
    ~CStartOptionsDialog();
    CGameOpt getGameOpt(void);
    
private:
    Ui::CStartOptionsDialog *ui;
    QLineEdit *le[4];
    QComboBox *cb[4];
    QLabel *lb[4];
    QComboBox *cbc[4];

    bool chkStringReady(quint8 line);

private slots:
    bool chkReady(void);
    void changeSpin(int val);
};

#endif // CSTARTOPTIONSDIALOG_H
