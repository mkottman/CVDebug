#include "imagedisplay.h"

ImageDisplay::ImageDisplay(QWidget *parent) : QLabel(parent)
{
    scaleFactor_x = 1;
    scaleFactor_y = 1;
}

void ImageDisplay::mousePressEvent(QMouseEvent *ev)
{
    startPosition = ev->globalPos();
    startOffset = QPoint(offset_x, offset_y);
    startEnd = QPoint(end_x, end_y);
}

void ImageDisplay::mouseMoveEvent(QMouseEvent *ev)
{
    if (ev->buttons() & Qt::LeftButton) {
        QPoint diff = startPosition - ev->globalPos();
        offset_x = startOffset.x() + diff.x() / scaleFactor_x;
        offset_y = startOffset.y() + diff.y() / scaleFactor_y;
        end_x = startEnd.x() + diff.x() / scaleFactor_x;
        end_y = startEnd.y() + diff.y() / scaleFactor_y;
        refreshImage();
    } else {
        if (ev->x() >=0 && ev->y() >= 0)
            emit Mouse_position(ev->x(), ev->y());
    }
}

void ImageDisplay::wheelEvent(QWheelEvent *event)
{
    double numSteps = (event->delta() / 8) / 15.0f;
    zoom(numSteps, event->x(), event->y());
}

void ImageDisplay::zoom(double zoomfactor, int mouse_pos_x, int mouse_pos_y)
{

    //we will not zoom out to make picture disappear, there is no point to do that
    if (zoomfactor < 0 && zoom_of_picture <= 1) { return; }

    if (zoomfactor > 0) { zoomfactor = 1.25; }
    else { zoomfactor = 1 / 1.25; }
    zoom_of_picture *= zoomfactor;

    if (zoom_of_picture < 1) { zoom_of_picture = 1; }

    range_x = (int)((double) original_pixmap.width() / zoom_of_picture);
    range_y = (int)((double) original_pixmap.height() / zoom_of_picture);

    if (range_y < 3 || range_x < 3) {
        zoom_of_picture *= (1 / 1.25);   //we need to set the zoom back
        return; //we are not going to zoom any more, it is not necessary
    }

    float mouse_ratio_x = (float) mouse_pos_x / width();
    float mouse_ratio_y = (float) mouse_pos_y / height();

    float global_x = mouse_pos_x / scaleFactor_x + offset_x;
    float global_y = mouse_pos_y / scaleFactor_y + offset_y;

    //now we try to fit the edges of the area to original pixmap
    float top_left_corner_x = global_x - range_x * mouse_ratio_x;
    float top_left_corner_y = global_y - range_y * mouse_ratio_y;
    float bottom_right_corner_x = global_x + range_x * (1 - mouse_ratio_x);
    float bottom_right_corner_y = global_y + range_y * (1 - mouse_ratio_y);

    //new pixmap offset depends on the top left corner of the rectangle we are going to display
    offset_x = (int) top_left_corner_x;
    offset_y = (int) top_left_corner_y;

    end_x = (int) bottom_right_corner_x;
    end_y = (int) bottom_right_corner_y;

    refreshImage();
}

QPoint ImageDisplay::toImagePosition(int x, int y)
{
    //counting accurate possition of mouse cursor on displayed image
    x = (int)((x / scaleFactor_x) + offset_x);
    y = (int)((y / scaleFactor_y) + offset_y);

    if (x<0 || y<0) {
        x = y = 0;
    }

    return QPoint(x, y);
}

void ImageDisplay::refreshImage()
{
    // if the edges are out of the range, we need to set them back
    // so they fit into original pixmap
    if (offset_x < 0) {
        offset_x = 0;
        end_x = range_x;
    }
    if (offset_y < 0) {
        offset_y = 0;
        end_y = range_y;
    }
    if (end_x > original_pixmap.width()) {
        end_x = original_pixmap.width();
        offset_x = original_pixmap.width() - range_x;
    }
    if (end_y > original_pixmap.height()) {
        end_y = original_pixmap.height();
        offset_y = original_pixmap.height() - range_y;
    }

    //we need to set new scalefactor as precize as possible
    if (end_x == offset_x)
        scaleFactor_x = 1;
    else
        scaleFactor_x = (double) width()  / (end_x - offset_x);

    if (end_y == offset_y)
        scaleFactor_y = 1;
    else
        scaleFactor_y = (double) height() / (end_y - offset_y);

    QPixmap pixmap_new = this->original_pixmap.copy(
                             offset_x, offset_y,
                             end_x - offset_x, end_y - offset_y);
    this->setPixmap(pixmap_new.scaled(this->parentWidget()->width(), this->parentWidget()->height(),
                                      Qt::KeepAspectRatio));
    this->setGeometry(0, 0, this->pixmap()->width(), this->pixmap()->height());

}
