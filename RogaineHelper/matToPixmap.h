#ifndef MATTOPIXMAP
#define MATTOPIXMAP

#include <QPixmap>
#include <opencv2/opencv.hpp>

// Функция конвертирует cv::Mat в QPixmap
QPixmap cvMatToQPixmap(const cv::Mat& mat);

#endif // MATTOPIXMAP

