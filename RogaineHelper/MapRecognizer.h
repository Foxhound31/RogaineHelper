#ifndef MAPRECOGNIZER_H
#define MAPRECOGNIZER_H

#include "opencv2/opencv.hpp"

//-----------------------------------------------------------------------
// Returns average color value (BGR) for a given circle (x,y,radius) on the Mat frame (BGR, 8UC3)
//--------------------------------------------------------------------------------------
cv::Vec3b averageCircleColor(const cv::Mat& mat, const cv::Vec3f circle);





class MapRecognizer
{
public:
    MapRecognizer();
};

#endif // MAPRECOGNAIZER_H
