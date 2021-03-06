#ifndef MY_MAIN_QLABEL_H
#define MY_MAIN_QLABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QEvent>
#include <QDebug>
#include <opencv2/core/core.hpp>

class ImageDisplay : public QLabel{
    Q_OBJECT
public:
    explicit ImageDisplay(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *event);
    void zoom(double zoomfactor, int mouse_pos_x, int mouse_pos_y);

    QPoint toImagePosition(int x, int y);

    QPixmap original_pixmap;  //we store here maximum number of pixels from the biggest possible image

    double scaleFactor_x, scaleFactor_y;
    double zoom_of_picture;
    int offset_x;
    int offset_y;
    int end_x;
    int end_y;
    float range_x;
    float range_y;
    QPoint startPosition;
    QPoint startOffset;
    QPoint startEnd;

private:

signals:
    void Mouse_position(int pos_x, int pos_y);
    void zoomed();

public slots:
    void refreshImage();
};

#endif // MY_MAIN_QLABEL_H
