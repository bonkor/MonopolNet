#ifndef CMOVEWIDGET_H
#define CMOVEWIDGET_H

#include <QWidget>

class CMoveWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CMoveWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);

protected:
    virtual void mousePressEvent(QMouseEvent * mouseEvent);
    virtual void mouseMoveEvent(QMouseEvent * mouseEvent);

private:
    QPoint dragStartMousePosition;
    QPoint dragStartWidgetPosition;

signals:
    
public slots:
    
};

#endif // CMOVEWIDGET_H
