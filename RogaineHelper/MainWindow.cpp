#define _USE_MATH_DEFINES
#include <cmath>

#include "MainWindow.h"
#include <QFileDialog>
#include <QSettings>
#include <QFileInfo>
#include <QGraphicsItem>
#include <QDir>
#include "MapRecognizer.h"
#include "OpencvQtTools.h"

const QString settingsFileName = "RogaineHelper.ini";
const QString settingsKeyPath = "filesPath";


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    // Connect view and scene
    ui.view->setScene(&scene);

    // Configure scene and connect singnals-slots for it
    scene.setSceneRect(QRectF(0, 0, 5000, 5000));
    //connect(nodeEditorScene, SIGNAL(itemInserted(NodeItem*)), this, SLOT(itemInserted(NodeItem*)));
    //connect(nodeEditorScene, SIGNAL(itemSelected(QGraphicsItem*)), this, SLOT(itemSelected(QGraphicsItem*)));


    // Check settings
    QSettings settings(settingsFileName, QSettings::IniFormat);
    ui.textBrowser->append("Application settings file: " + settings.fileName());
    if (settings.allKeys().count() == 0) {
        ui.textBrowser->append("No settings in file");
    }
}

//--------------------------------------------------------------------------------------
// Open Map image file
//--------------------------------------------------------------------------------------
void MainWindow::on_openMapButton_clicked()
{
    // Open settings to read path to files
    QSettings settings(settingsFileName, QSettings::IniFormat);
    QString path = settings.value(settingsKeyPath).toString();
    if (path.isEmpty()) path = QDir::currentPath();

    // Open map file
    const auto fileName = QFileDialog::getOpenFileName(this, tr("Open map image"), path, QString("Pictures (*.png *.jpg); All files (*.*)"));
    if (fileName.isEmpty()) {
        ui.textBrowser->append("Operation cancelled");
        return;
    }

    // Create Mat frame
    mapImage = cv::imread(fileName.toStdString().c_str(), cv::IMREAD_COLOR);

    // Add map on the scene
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(cvMatToQPixmap(mapImage));
    scene.addItem(item);
    ui.view->show();

    // Save path into settings
    settings.setValue(settingsKeyPath, QFileInfo(fileName).absoluteDir().absolutePath());
}

//--------------------------------------------------------------------------------------
// Set map scale
//--------------------------------------------------------------------------------------
void MainWindow::on_setScaleButton_clicked()
{

}



//--------------------------------------------------------------------------------------
// Open Nodes file
//--------------------------------------------------------------------------------------
void MainWindow::on_openNodesButton_clicked()
{
    // Open settings to read path to files
    QSettings settings(settingsFileName, QSettings::IniFormat);
    QString path = settings.value(settingsKeyPath).toString();
    if (path.isEmpty()) path = QDir::currentPath();

    // Open map file
    const auto fileName = QFileDialog::getOpenFileName(this, tr("Open nodes file"), path);
    if (fileName.isEmpty()) {
        ui.textBrowser->append("Operation cancelled");
        return;
    }

    // TODO Nodes file

    // Save path into settings
    settings.setValue(settingsKeyPath, QFileInfo(fileName).absoluteDir().absolutePath());
}







//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
void MainWindow::on_selectNodeButton_clicked()
{


}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
void MainWindow::on_findNodeButton_clicked()
{
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



