#define _USE_MATH_DEFINES
#include <cmath>

#include "MapRecognizer.h"
#include "opencv2/opencv.hpp"

MapRecognizer::MapRecognizer()
{
    cv::Mat m;
    auto m2 = m.clone();
}



//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
int median(std::vector<int>& v) {
    size_t n = v.size() / 2;
    nth_element(v.begin(), v.begin() + n, v.end());
    return v[n];
}

//-----------------------------------------------------------------------
// Returns average color value (BGR) for a given circle (x,y,radius) on the Mat frame (BGR, 8UC3)
//--------------------------------------------------------------------------------------
cv::Vec3b averageCircleColor(const cv::Mat& mat, const cv::Vec3f circle) {
    auto mat2 = mat.clone();

    assert(mat.channels() == 3);
    const float x = circle[0];
    const float y = circle[1];
    const float r = circle[2];
    const double angleStep = 0.1;
    const size_t amount =  (2*M_PI/angleStep + 1)*3;
    double avgB = 0;
    double avgG = 0;
    double avgR = 0;

    std::vector<int> bVec(amount);
    std::vector<int> gVec(amount);
    std::vector<int> rVec(amount);

    size_t index = 0;
    for (double angle = 0; angle < 2*M_PI; angle += angleStep) {
        auto getColorLambda = [&](double r) -> cv::Vec3b {
            int row = y+r*sin(angle);
            int col = x+r*cos(angle);
            if (row < 0)
                row = 0;
            if (col < 0)
                col = 0;
            if (row >= mat.rows)
                row = mat.rows - 1;
            if (col >= mat.cols)
                col = mat.cols - 1;
            assert(row >= 0 && row < mat.rows);
            assert(col >= 0 && col < mat.cols);
            mat2.at<cv::Vec3b>(row, col) = cv::Vec3b(0,0,0);
            return mat.at<cv::Vec3b>(row, col);
        };
        auto color = getColorLambda(r);
        bVec[index] = color[0];
        gVec[index] = color[1];
        rVec[index] = color[2];
        color = getColorLambda(r+1);
        ++index;
        bVec[index] = color[0];
        gVec[index] = color[1];
        rVec[index] = color[2];
        color = getColorLambda(r-1);
        ++index;
        bVec[index] = color[0];
        gVec[index] = color[1];
        rVec[index] = color[2];
        ++index;
    }
    int medB = median(bVec);
    int medG = median(gVec);
    int medR = median(rVec);
    cv::imshow("1", mat2);
    return cv::Vec3b(medB, medG, medR);
}
