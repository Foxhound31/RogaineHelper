#define _USE_MATH_DEFINES
#include <cmath>

#include "MainWindow.h"
#include <QFileDialog>
#include <QSettings>
#include <QFileInfo>
#include "matToPixmap.h"

int median(std::vector<int>& v) {
    size_t n = v.size() / 2;
    nth_element(v.begin(), v.begin() + n, v.end());
    return v[n];
}

// Функция возвращает среднее значение цвета (BGR) для заданной окружности (x,y,radius)
// Mat в формате BGR, 8UC3
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

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    ui.setupUi(this);
}

void MainWindow::on_selectMapButton_clicked() {
    QSettings settings("RogaineHelper", QSettings::IniFormat);
    const QString pathKeyName = "imagePath";
    QString path = settings.value(pathKeyName).toString();
    const auto fileName = QFileDialog::getOpenFileName(nullptr, "Выберите фото карты", path,
                                                 QString("Изображения (*.png *.jpg)"));
    if (!fileName.isEmpty()) {
        mapImage = cv::imread(fileName.toStdString().c_str(), cv::IMREAD_COLOR);
        ui.mapLabel->setPixmap(cvMatToQPixmap(mapImage));
        QFileInfo fileInfo(fileName);
        path = fileInfo.absoluteDir().absolutePath();
        settings.setValue(pathKeyName, path);
    }
}


void MainWindow::on_selectMarkButton_clicked() {
    const auto fileName = QFileDialog::getOpenFileName(nullptr, "Выберите фото метки", QString(),
                                                 QString("Изображения (*.png *.jpg)"));
    markImage = cv::imread(fileName.toStdString().c_str(), cv::IMREAD_COLOR);
    //ui.mapLabel->setPixmap(cvMatToQPixmap(image));

}

void MainWindow::on_findMarksButton_clicked() {
    cv::Mat result;
    cv::Mat resultNormalized;
    //cv::Mat channels[3];
    //cv::split(mapImage, channels);
    //mapImage = channels[1]; // B[G]R
    //cv::matchTemplate(mapImage, markImage, result, cv::TM_SQDIFF_NORMED);
    //cv::normalize(result, resultNormalized, 0, 255, cv::NORM_MINMAX);
    //ui.mapLabel->setPixmap(cvMatToQPixmap(result));
    //cv::Mat mat2;
    //resultNormalized.convertTo(mat2, CV_8UC1);

    cv::Mat channels[3];
    cv::split(markImage, channels);

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(channels[1], circles, cv::HOUGH_GRADIENT, 1, channels[1].rows/2, channels[1].rows/4, channels[1].rows/2);
    assert(!circles.empty());
    cv::Mat markImage2;
    cv::cvtColor(channels[1], markImage2, cv::COLOR_GRAY2BGR);

    // Draw the circles detected
    assert(circles.size() == 1);
    for (size_t i = 0; i < circles.size(); ++i) {
        cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        cv::circle(markImage2, center, radius, cv::Scalar(0,0,255), 2, 8, 0);// circle outline
    }
    const int radius = cvRound(circles[0][2]);
    //cv::imshow("", markImage2);
    const auto avgColor = averageCircleColor(markImage, circles[0]);

    QString text2 = QString("(") + QString::number(avgColor[0]) + QString(",") +
            QString::number(avgColor[1]) + QString(",") +
            QString::number(avgColor[2]) + QString(")");

    // find circles on map with Hough
    cv::split(mapImage, channels);
    circles.clear();
    cv::HoughCircles(channels[1], circles, cv::HOUGH_GRADIENT,
            1,          // Inverse ratio of the accumulator resolution to the image resolution
            radius*2,   // Minimum distance between the centers of the detected circles
            radius*0.8, // the higher threshold of the two passed to the Canny edge detector (the lower one is twice smaller)
            20,         // the accumulator threshold for the circle centers at the detection stage. The smaller it is,
                        // the more false circles may be detected. Circles,
                        // corresponding to the larger accumulator values, will be returned first
            radius*0.8, radius*1.2); // min and max radius
    cv::Mat mapImage2;
    cv::cvtColor(channels[1], mapImage2, cv::COLOR_GRAY2BGR);
    for (size_t i = 0; i < circles.size(); ++i) {
        const cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        const int radius = cvRound(circles[i][2]);
        //const auto avgMapColor = averageCircleColor(mapImage, circles[i]);
        cv::Vec3b avgMapColor;
        int d0 = static_cast<int>(avgColor[0]) - static_cast<int>(avgMapColor[0]);
        int d1 = static_cast<int>(avgColor[1]) - static_cast<int>(avgMapColor[1]);
        int d2 = static_cast<int>(avgColor[2]) - static_cast<int>(avgMapColor[2]);
        double result = std::max({d0, d1, d2});
        cv::Scalar scalar;
        if (result < 50)
            scalar = cv::Scalar(0,0,255);
        else
            scalar = cv::Scalar(255,0,0);
        cv::circle(mapImage2, center, radius, scalar, 1, 8, 0);// circle outline
    }
    QString text = QString("found ") + QString::number(circles.size()) + QString("circles");
    cv::putText(mapImage2, text.toStdString().c_str(),  cv::Point(10, 40), cv::FONT_HERSHEY_DUPLEX, 1.5, cv::Scalar(0));
    cv::putText(mapImage2, text2.toStdString().c_str(), cv::Point(10, 80), cv::FONT_HERSHEY_DUPLEX, 1.5, cv::Scalar(0));
    cv::imshow("", mapImage2);

    cv::Mat frameResult;
    cv::Canny(channels[1], frameResult, 5, 100);
    cv::imshow("2", frameResult);
}
