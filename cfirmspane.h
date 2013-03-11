#ifndef CFIRMSPANE_H
#define CFIRMSPANE_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include "cdoc.h"

class CFirmsPane : public QWidget
{
    Q_OBJECT
public:
    explicit CFirmsPane(QWidget *parent = 0);
    void init(CDoc * d);

protected:
    void paintEvent(QPaintEvent *event);

private:
    CDoc *doc;
    QGridLayout *mainLayout;
    QHBoxLayout hL[4];
    QVBoxLayout vL[17];
    QLabel f[17][5];
    QLabel m[17];

    void addLabels(quint8 lNu, quint8 mNu);
signals:
    
public slots:
    
};

#endif // CFIRMSPANE_H
