#ifndef TRANSPARENT_H
#define TRANSPARENT_H
#include "QWidget"
#include "QRubberBand"
#include "QMouseEvent"
#include "QSize"
#include "QRect"
#include "QSettings"
#include "QScreen"
#include "QGuiApplication"
#include "QLabel"

class Transparent: public QWidget
{
    Q_OBJECT

public:
    QRubberBand *rubberBand;
    QPoint origin;
    QLabel *label;

    Transparent(QLabel *label);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

};

#endif // TRANSPARENT_H
