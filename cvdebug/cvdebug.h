#ifndef CVDEBUG_H
#define CVDEBUG_H

#ifndef NDEBUG

// simplified from Qt
#if defined(_WIN32)
#  define DECL_EXPORT __declspec(dllexport)
#else
#  define DECL_EXPORT
#endif
#if defined(_WIN32)
#  define DECL_IMPORT __declspec(dllimport)
#else
#  define DECL_IMPORT
#endif

#if defined(CVDEBUG_LIBRARY)
#  define EXPORTED DECL_EXPORT
#else
#  define EXPORTED DECL_IMPORT
#endif

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

EXPORTED void cvdebug_image(const cv::Mat &image, const std::string &name);
EXPORTED void cvdebug_keypoints(const std::vector<cv::KeyPoint> &keyPoints, const std::string &imageName);

#undef EXPORTED
#undef DECL_IMPORT
#undef DECL_EXPORT

#else // ifndef NDEBUG

#define cvdebug_image(...)
#define cvdebug_keypoints(...)

#endif

#endif // CVDEBUG_H
