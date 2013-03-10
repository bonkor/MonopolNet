#ifndef CFIRMSPANE_H
#define CFIRMSPANE_H

#include <QWidget>

class CFirmsPane : public QWidget
{
    Q_OBJECT
public:
    explicit CFirmsPane(QWidget *parent = 0);
protected:
    void paintEvent(QPaintEvent *event);

signals:
    
public slots:
    
};

#endif // CFIRMSPANE_H
