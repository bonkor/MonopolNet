#include <QApplication>
#include <QMouseEvent>
#include "cmovewidget.h"

CMoveWidget::CMoveWidget(QWidget *parent, Qt::WindowFlags f) :
    QWidget(parent, f)
{
}

void CMoveWidget::mousePressEvent(QMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        dragStartMousePosition = mouseEvent->globalPos();
        dragStartWidgetPosition = this->pos();
    }
    if (mouseEvent->button() == Qt::RightButton) {
        hide();
    }
    QWidget::mousePressEvent(mouseEvent);
}

void CMoveWidget::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    QPoint p = mouseEvent->globalPos() - dragStartMousePosition;
    if (p.manhattanLength() < QApplication::startDragDistance())
        return;
    QPoint r = dragStartWidgetPosition + p;
    if (r.x() < 0)
        r.setX(0);
    if (r.y() < 0)
    r.setY(0);
    int maxW = this->parentWidget()->geometry().width() - this->geometry().width();
    int maxH = this->parentWidget()->geometry().height() - this->geometry().height();
    if (r.rx() > maxW)
        r.setX(maxW);
    if (r.ry() > maxH)
        r.setY(maxH);

    move(r);

    QWidget::mouseMoveEvent(mouseEvent);
}

void CMoveWidget::checkWidgetFitted(void)
{
    QPoint r = this->pos();

    int maxW = this->parentWidget()->geometry().width() - this->geometry().width();
    int maxH = this->parentWidget()->geometry().height() - this->geometry().height();
    if (r.rx() > maxW)
        r.setX(maxW);
    if (r.ry() > maxH)
        r.setY(maxH);

    move(r);
}
