#include "imagelabel.h"

ImageLabel::ImageLabel(int picture_number, QWidget* parent):QLabel(parent)
{
    set_identity(picture_number);
}

ImageLabel::~ImageLabel()
{
}

void ImageLabel::mousePressEvent(QMouseEvent*)
{
    emit clicked();
}

int ImageLabel::get_identity()
{
    return identity;
}

void ImageLabel::set_identity(int picture_number)
{
    identity = picture_number;
}

void ImageLabel::reload_picture(QImage new_image, int actual_picture_ID)
{
    if (this->get_identity() == actual_picture_ID)
        this->setPixmap(QPixmap::fromImage(new_image).scaled(90,90,Qt::KeepAspectRatio));
}
