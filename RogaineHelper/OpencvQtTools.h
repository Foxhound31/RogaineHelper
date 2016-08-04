#ifndef MATTOPIXMAP
#define MATTOPIXMAP

#include <QPixmap>
#include "opencv2/opencv.hpp"

//--------------------------------------------------------------------------------------
// Convert OpenCV Mat frame to QT image format
//--------------------------------------------------------------------------------------
QImage cvMatToQImage(const cv::Mat& mat);

//--------------------------------------------------------------------------------------
// Convert OpenCV Mat frame to QT pixmap format
//--------------------------------------------------------------------------------------
QPixmap cvMatToQPixmap(const cv::Mat& mat);



#endif // MATTOPIXMAP

