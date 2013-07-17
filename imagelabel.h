#ifndef MY_LABEL_H
#define MY_LABEL_H

#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QObject>
#include <QMouseEvent>
#include <QEvent>
#include <QImage>

class ImageLabel : public QLabel
{
    Q_OBJECT

public:
    ImageLabel(int picture_number, QWidget* parent = 0);
    ~ImageLabel();
    int get_identity();

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent*) ;

private:
    int identity;

    void set_identity(int picture_number);

public slots:
    void reload_picture(QImage new_image, int actual_picture_ID);

};
#endif // MY_LABEL_H
