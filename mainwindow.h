#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <QtGui>
#include <QtCore>
#include <QMutex>
#include "receiver.h"
#include "imagelabel.h"
#include "imagedisplay.h"

using namespace std;

namespace Ui
{
class MainWindow;
}

struct ReceivedItem {
    enum Type {
        TYPE_IMAGE, TYPE_KEYPOINTS
    } type;
    QString name;
    cv::Mat thumbnail;
    cv::Mat displayableImage;
    cv::Mat originalImage;
    std::vector<cv::KeyPoint> points;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *);

    static cv::Mat rescale(const cv::Mat &image);

signals:
    void picture_changed(QImage image, int picture_ID);

private:
    void displayCurrentImage();
    void addLabelForCurrentItem();
    ReceivedItem &insertNewReceivedItem();
    int findImageByName(QString name);

private slots:
    void reload_picture();
    void mouseMoved(int pos_x, int pos_y);

    void receivedImage(QString name, cv::Mat image);
    void receivedKeypoints(QString name, std::vector<cv::KeyPoint> points);

private:
    Ui::MainWindow *ui;
    ReceiverThread *receiverThread;
    QVBoxLayout *selectorLayout;
    QMutex mutex;

    QList<ReceivedItem> received_items;
    QList<QLabel *> descriptions;
    QList<ImageLabel *> labels;

    int current_ID;
    int max_item_count;
    int item_counter;
    int label_description_height;
    int representative_label_height, representative_label_width;
};

#endif // MAINWINDOW_H
