#ifndef CQPANE_H
#define CQPANE_H

#include <QWidget>
#include <QImage>

class CQField : public QWidget
{
    Q_OBJECT
public:
    explicit CQField(QWidget *parent = 0, Qt::WindowFlags f = 0);
    void setPict(QImage i);
    void setSelected(bool s);
protected:
    void paintEvent(QPaintEvent *event);
private:
    QImage Pict;
    QImage innerPict;
    bool selected;

    QImage makeTranparant(QImage src);
signals:

public slots:

};

class CQPane : public QWidget
{
    Q_OBJECT
public:
    explicit CQPane(QWidget *parent = 0, Qt::WindowFlags f = 0);
    void clear(void);
    void selectRow(quint8 r);
    void selectPos(quint8 r, quint8 c);
    ~CQPane();
protected:
    void paintEvent(QPaintEvent *event);
private:
    QImage MainPict;
    CQField *Field[6][6];
    quint8 row;
    quint8 col;

    void preparePics(void);

signals:
    
public slots:
    
};

#endif // CQPANE_H
