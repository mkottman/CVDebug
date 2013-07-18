#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <opencv2/opencv.hpp>
#include <QMutex>
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <zmq.hpp>
#include <iostream>

class ReceiverThread : public QThread{
    Q_OBJECT
    bool stopped;

public:
    explicit ReceiverThread(QObject *parent = 0);
    void run();
    void decodeData(const QByteArray &inputData);

signals:
    void receivedImage(const QString name, const cv::Mat& image);
    void receivedKeypoints(const QString imageName, const std::vector<cv::KeyPoint>& points);

public slots:
    void stop() {
        stopped = true;
    }
};

#endif // MYTHREAD_H
