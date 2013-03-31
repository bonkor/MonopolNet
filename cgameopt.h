#ifndef CGAMEOPT_H
#define CGAMEOPT_H

#include <QDialog>

struct CPlOpt {
    QString name;
    QColor color;
};

struct CGameOpt {
    quint8 plNum;
    CPlOpt po[4];
};

#endif // CGAMEOPT_H
