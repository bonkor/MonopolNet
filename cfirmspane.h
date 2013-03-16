#ifndef CFIRMSPANE_H
#define CFIRMSPANE_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QMouseEvent>
#include "cdoc.h"
#include "cmovewidget.h"

class CFLabel : public QLabel
{
    Q_OBJECT
public:
    explicit CFLabel(QWidget *parent = 0);
    void init(QString s, quint8 nu, bool h = false);
    void setOwner(quint8 o);
    quint8 getFirmNu(void);

protected:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);

private:
    QPalette pal;   // текущая палитра
    bool header;
    quint8 fNu;
    quint8 owner;

protected:
    virtual void mouseReleaseEvent(QMouseEvent * event);

signals:
    void labClick(int nu);

public slots:

};

class CFirmsPane : public CMoveWidget
{
    Q_OBJECT
public:
    explicit CFirmsPane(QWidget *parent = 0, Qt::WindowFlags f = 0);
    void init(CDoc * d);
    void update(void);

protected:
    void paintEvent(QPaintEvent *event);

private:
    CDoc *doc;
    QGridLayout *mainLayout;
    QVBoxLayout vL[17];
    CFLabel f[17][5];
    CFLabel m[17];

    void addLabels(quint8 lNu, quint8 mNu);
signals:
    void viewFirm(int nu);

private slots:
    void labClick(int nu);

public slots:

};

#endif // CFIRMSPANE_H
