#include "transparent.h"

Transparent::Transparent(QLabel *label, QLabel *label1)
{
    rubberBand = nullptr;
    this->label = label;
    this->label1 = label1;
}

void Transparent::mousePressEvent(QMouseEvent *event)
{
    origin = event->pos();
    if (!rubberBand)
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    rubberBand->setGeometry(QRect(origin, QSize()));
    rubberBand->show();
}

void Transparent::mouseMoveEvent(QMouseEvent *event)
{
    rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
}

void Transparent::mouseReleaseEvent(QMouseEvent *event)
{
    rubberBand->hide();
    this->hide();


    QSettings settings("HKEY_CURRENT_USER\\Software\\screen-capture-recorder",
                       QSettings::NativeFormat);
    settings.setValue("capture_height", rubberBand->geometry().height());
    settings.setValue("capture_width", rubberBand->geometry().width());
    settings.setValue("start_x", rubberBand->geometry().topLeft().rx());
    settings.setValue("start_y",  rubberBand->geometry().topLeft().ry());

    QString x = "Area Selezionata: origine (" +
            QString::number(rubberBand->geometry().topLeft().rx()) +
            "," +
            QString::number(rubberBand->geometry().topLeft().ry()) +
            "), dimensione: " +
            QString::number(rubberBand->geometry().width()) +
            "x" +
            QString::number(rubberBand->geometry().height());

    label->setText(x);
    label1->setText(x);
}
