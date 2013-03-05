#include <QTime>
#include "ccubik.h"

CCubik::CCubik(QWidget *parent) :
    QPushButton(parent)
{
    QObject::connect(this, SIGNAL(clicked()),
                     this, SLOT(makeRandom()));
}

void CCubik::makeRandom(void)
{
    // Create seed for the random
    // That is needed only once on application startup
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    int randomValue = randInt(1,6);
    QString tmpStr;
    tmpStr.setNum(randomValue);

    setText(tmpStr);
    setDisabled(true);

    emit cubikClicked(randomValue);
}

int CCubik::randInt(int low, int high)
{
    // Random number between low and high
//    return 6;
    return qrand() % ((high + 1) - low) + low;
}
