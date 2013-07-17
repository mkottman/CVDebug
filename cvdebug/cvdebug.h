#ifndef CVDEBUG_H
#define CVDEBUG_H

enum CVDEBUG_TYPE {
    CVDEBUG_IMAGE,
    CVDEBUG_KEYPOINTS,
    CVDEBUG_LAST
};

#ifdef _DEBUG

#include <QtCore/qglobal.h>

#if defined(CVDEBUG_LIBRARY)
#  define EXPORTED Q_DECL_EXPORT
#else
#  define EXPORTED Q_DECL_IMPORT
#endif

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

EXPORTED void cvdebug_image(const cv::Mat &image, const std::string &name);
EXPORTED void cvdebug_keypoints(const std::vector<cv::KeyPoint> &keyPoints, const std::string &imageName);

#undef EXPORTED

#else // _DEBUG

#define cvdebug_image(...)
#define cvdebug_keypoints(...)

#endif

#endif // CVDEBUG_H
