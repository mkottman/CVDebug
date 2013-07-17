#ifdef NDEBUG
#undef NDEBUG // always build
#endif

#ifndef CVDEBUG_LIBRARY
#define CVDEBUG_LIBRARY // always define this to export
#endif

#include "cvdebug.h"

#include <cassert>
#include <zmq.hpp>
#include <iostream>

class CvDebugState
{
public:
    CvDebugState() :
        initialized(false),
        context(new zmq::context_t(1)),
        socket(new zmq::socket_t(*context, ZMQ_PUB))
    {
        try {
            int few = 20;
            socket->setsockopt(ZMQ_SNDHWM, &few, sizeof(few));
            socket->connect("tcp://localhost:7139");
            initialized = true;
        } catch (std::exception &ex) {
            std::cerr << "Failed to connect to localhost:7139: " << ex.what() << "\n";
        }
    }

    ~CvDebugState() {
        // leak memory please
    }

    inline bool send(zmq::message_t &msg, int flags = 0) {
        if (initialized)
            return socket->send(msg, flags);
        else
            return false;
    }

    bool initialized;
    zmq::context_t *context;
    zmq::socket_t *socket;
};

static CvDebugState cvdebug;

class BufWriter
{
    char *start, *p;
    size_t curr, size;

public:

    BufWriter(void *data, int length) {
        start = p = (char *) data;
        size = length;
        curr = 0;
    }

    template<typename T>
    void write(const T &data) {
        CV_DbgAssert(this->curr + sizeof(T) <= this->size);
        *((T *)p) = data;
        p += sizeof(T);
        curr += sizeof(T);
    }

    void writeStr(const std::string &str) {
        CV_DbgAssert(this->curr + sizeof(int) + str.length() <= this->size);
        write((uint) str.length());
        memcpy(p, str.c_str(), str.length());
        p += str.length();
        curr += str.length();
    }

    void *currPtr() {
        return p;
    }
};

void cvdebug_image(const cv::Mat &image, const std::string &name)
{
    if (!cvdebug.initialized) {
        std::cerr << "Trying to send image " << name << " in uninitialized state\n";
        return;
    }

    const int header_length = 2 * sizeof(int) + name.length();
    const int imgheader_length = 3 * sizeof(int);
    const int data_length = image.dataend - image.datastart;

    const int total_length = header_length + imgheader_length + data_length;

    zmq::message_t message(total_length); //new message of header+data size

    BufWriter w(message.data(), total_length);

    // the type/name header
    w.write((int) CVDEBUG_IMAGE);
    w.writeStr(name);

    // the image header
    w.write(image.cols);
    w.write(image.rows);
    w.write(image.flags);

    // the image data
    memcpy(w.currPtr(), image.data, image.dataend - image.datastart);
    try {
        cvdebug.send(message);
    } catch (std::exception &ex) {
        std::cerr << "Failed to send " << name << ": " << ex.what() << "\n";
    }
}

void cvdebug_keypoints(const std::vector<cv::KeyPoint> &keyPoints,
                       const std::string &imageName)
{
    if (!cvdebug.initialized) {
        std::cerr << "Trying to send keypoints for " << imageName <<
                  " in uninitialized state\n";
        return;
    }

    const int nPoints = keyPoints.size();

    const int header_length = 2 * sizeof(int) + imageName.length();
    const int kpheader_length = 1 * sizeof(int);
    const int data_length = nPoints * 4 * sizeof(
                                float); // each point is: x, y, size, angle

    const int total_length = header_length + kpheader_length + data_length;

    zmq::message_t message(total_length);

    BufWriter w(message.data(), total_length);

    // the type/name header
    w.write((int) CVDEBUG_KEYPOINTS);
    w.writeStr(imageName);

    // the image header
    w.write(nPoints);

    float *points = static_cast<float *>(w.currPtr());
    for (int i = 0; i < nPoints; i++) {
        const cv::KeyPoint &kp = keyPoints.at(i);
        *points++ = kp.pt.x;
        *points++ = kp.pt.y;
        *points++ = kp.size;
        *points++ = kp.angle;
    }

    try {
        cvdebug.send(message);
    } catch (std::exception &ex) {
        std::cerr << "Failed to send keypoints for " << imageName << ": " << ex.what()
                  << "\n";
    }
}
