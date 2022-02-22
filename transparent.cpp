#include "transparent.h"

Transparent::Transparent(QLabel *label)
{
    rubberBand = nullptr;
    this->label = label;
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
    settings.setValue("capture_height", rubberBand->geometry().width());
    settings.setValue("capture_width", rubberBand->geometry().height());
    settings.setValue("start_x", origin.rx());
    settings.setValue("start_y",  origin.ry());

    QString x = "Area Selezionata: origine (" +
            QString::number(origin.rx()) +
            "," +
            QString::number(origin.ry()) +
            "), dimensione: " +
            QString::number(rubberBand->geometry().width()) +
            "x" +
            QString::number(rubberBand->geometry().height());

    label->setText(x);
}
