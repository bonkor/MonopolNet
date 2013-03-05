#ifndef CCUBIK_H
#define CCUBIK_H

#include <QPushButton>

class CCubik : public QPushButton
{
    Q_OBJECT
public:
    explicit CCubik(QWidget *parent = 0);
    
private:
    int randInt(int low, int high);

signals:
    void cubikClicked(int);

public slots:
    void makeRandom(void);

};

#endif // CCUBIK_H
