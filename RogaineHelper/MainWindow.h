#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"
#include <opencv2/opencv.hpp>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private slots:
    void on_openMapButton_clicked();
    void on_selectNodeButton_clicked();
    void on_findNodeButton_clicked();

    void on_openNodesButton_clicked();

    void on_setScaleButton_clicked();

private:
    Ui::MainWindow ui;

    QGraphicsScene scene;

    // OpenCv data for automatic node finder
    cv::Mat mapImage;
    cv::Mat markImage;
};

#endif // MAINWINDOW_H
