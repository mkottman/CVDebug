#include "receiver.h"

#include "cvdebug/cvdebug.h"

using namespace std;

ReceiverThread::ReceiverThread(QObject *parent) : QThread(parent)
{}

void ReceiverThread::run()
{
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_SUB);
    socket.bind("tcp://*:7139");
    socket.setsockopt(ZMQ_SUBSCRIBE, NULL, 0);
    stop = false;

    while (!stop) {
        zmq::message_t request;

        // Wait for next request from client
        socket.recv(&request);

        // Decode the request
        QByteArray data((const char *) request.data(), request.size());
        decodeData(data);
    }
}

void ReceiverThread::decodeData(const QByteArray &inputData)
{
    const int *header = (const int *) inputData.constData();
    int type = header[0];
    uint nameLength = header[1];
    QString name = QString::fromUtf8((const char *) &header[2], nameLength);

    const void *dataHeader = (const void *)((const char *)&header[2] + nameLength);

    if (type == CVDEBUG_IMAGE) {
        const int *imgHeader = (const int *) dataHeader;
        int sirka = imgHeader[0];
        int vyska = imgHeader[1];
        int flags = imgHeader[2];

        const void *data = &imgHeader[3];

        cv::Mat image(vyska, sirka, flags, (void *) data);
        cv::Mat result = image.clone();

        emit receivedImage(name, result);
    } else if (type == CVDEBUG_KEYPOINTS) {
        const int *kpHeader = (const int *) dataHeader;
        const float *kpData = (const float *) &kpHeader[1];

        int nPoints = kpHeader[0];
        std::vector<cv::KeyPoint> result(nPoints);

        for (int i = 0; i < nPoints; i++) {
            cv::KeyPoint &kp = result.at(i);
            kp.pt.x = *kpData++;
            kp.pt.y = *kpData++;
            kp.size = *kpData++;
            kp.angle = *kpData++;
        }

        emit receivedKeypoints(name, result);
    } else {
        std::cerr << "Unknown type: " << type << std::endl;
    }
}

/*
    |||              Generic header               ||     Specific header     |   Data...  |||
    ||| type | picture_name_length | picture_name || width | height | flags  |   Data...  |||
 */
