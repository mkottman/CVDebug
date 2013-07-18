#include "imagedisplay.h"

ImageDisplay::ImageDisplay(QWidget *parent) : QLabel(parent)
{
    scaleFactor_x = 1;
    scalefactor_y = 1;
}

void ImageDisplay::mouseMoveEvent(QMouseEvent *ev)
{
    emit Mouse_position(ev->x(), ev->y());
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

    float mouse_ratio_x = (float) mouse_pos_x / width();
    float mouse_ratio_y = (float) mouse_pos_y / height();

    float global_x = mouse_pos_x / scaleFactor_x + offset_x;
    float global_y = mouse_pos_y / scalefactor_y + offset_y;

    float range_x = (int)((double) original_pixmap.width() / zoom_of_picture);
    float range_y = (int)((double) original_pixmap.height() / zoom_of_picture);

    if (range_y < 3 || range_x < 3) {
        zoom_of_picture *= (1 / 1.25);   //we need to set the zoom back
        return; //we are not going to zoom any more, it is not necessary
    }

    //now we try to fit the edges of the area to original pixmap
    float top_left_corner_x = global_x - range_x * mouse_ratio_x;
    float top_left_corner_y = global_y - range_y * mouse_ratio_y;
    float bottom_right_corner_x = global_x + range_x * (1 - mouse_ratio_x);
    float bottom_right_corner_y = global_y + range_y * (1 - mouse_ratio_y);

    // this should not happen, but just in case...
    // if the edges are out of the range, we need to set them back
    // so they fit into original pixmap
    if (top_left_corner_y < 0) {
        top_left_corner_y = 0;
        bottom_right_corner_y = range_y;
    }
    if (top_left_corner_x < 0) {
        top_left_corner_x = 0;
        bottom_right_corner_x = range_x;
    }
    if (bottom_right_corner_y > original_pixmap.height()) {
        bottom_right_corner_y = original_pixmap.height();
        top_left_corner_y = original_pixmap.height() - range_y;
    }
    if (bottom_right_corner_x > original_pixmap.width()) {
        bottom_right_corner_x = original_pixmap.width();
        top_left_corner_x = original_pixmap.width() - range_x;
    }

    //we need to set new scalefactor as precize as possible
    scaleFactor_x = width()  / (bottom_right_corner_x - top_left_corner_x);
    scalefactor_y = height() / (bottom_right_corner_y - top_left_corner_y);

    //new pixmap offset depends on the top left corner of the rectangle we are going to display
    offset_x = (int) top_left_corner_x;
    offset_y = (int) top_left_corner_y;

    end_x = (int) bottom_right_corner_x;
    end_y = (int) bottom_right_corner_y;

    refreshImage();
}

void ImageDisplay::refreshImage()
{
    QPixmap pixmap_new = this->original_pixmap.copy(
                             offset_x, offset_y,
                             end_x - offset_x, end_y - offset_y);
    this->setPixmap(pixmap_new.scaled(this->parentWidget()->width(), this->parentWidget()->height(),
                                      Qt::KeepAspectRatio));
    this->setGeometry(0, 0, this->pixmap()->width(), this->pixmap()->height());

}
